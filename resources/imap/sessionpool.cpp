/*
    Copyright (c) 2010 Klarälvdalens Datakonsult AB,
                       a KDAB Group company <info@kdab.com>
    Author: Kevin Ottens <kevin@kdab.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "sessionpool.h"

#include <QtCore/QTimer>
#include <QtNetwork/QSslSocket>

#include "imapresource_debug.h"
#include <KLocalizedString>

#include <kimap/capabilitiesjob.h>
#include <kimap/logoutjob.h>
#include <kimap/namespacejob.h>

#include "imapaccount.h"
#include "passwordrequesterinterface.h"

qint64 SessionPool::m_requestCounter = 0;

SessionPool::SessionPool(int maxPoolSize, QObject *parent)
    : QObject(parent),
      m_maxPoolSize(maxPoolSize),
      m_account(Q_NULLPTR),
      m_passwordRequester(Q_NULLPTR),
      m_initialConnectDone(false),
      m_pendingInitialSession(Q_NULLPTR)
{
}

SessionPool::~SessionPool()
{
    disconnect(CloseSession);
}

PasswordRequesterInterface *SessionPool::passwordRequester() const
{
    return m_passwordRequester;
}

void SessionPool::setPasswordRequester(PasswordRequesterInterface *requester)
{
    delete m_passwordRequester;

    m_passwordRequester = requester;
    m_passwordRequester->setParent(this);
    QObject::connect(m_passwordRequester, SIGNAL(done(int,QString)),
                     this, SLOT(onPasswordRequestDone(int,QString)));
}

void SessionPool::cancelPasswordRequests()
{
    m_passwordRequester->cancelPasswordRequests();
}

KIMAP::SessionUiProxy::Ptr SessionPool::sessionUiProxy() const
{
    return m_sessionUiProxy;
}

void SessionPool::setSessionUiProxy(KIMAP::SessionUiProxy::Ptr proxy)
{
    m_sessionUiProxy = proxy;
}

bool SessionPool::isConnected() const
{
    return m_initialConnectDone;
}

bool SessionPool::connect(ImapAccount *account)
{
    if (m_account) {
        return false;
    }

    m_account = account;
    if (m_account->authenticationMode() == KIMAP::LoginJob::GSSAPI) {
        // for GSSAPI we don't have to ask for username/password, because it uses session wide tickets
        QMetaObject::invokeMethod(this, "onPasswordRequestDone", Qt::QueuedConnection,
                                  Q_ARG(int, PasswordRequesterInterface::PasswordRetrieved),
                                  Q_ARG(QString, QString()));
    } else {
        m_passwordRequester->requestPassword();
    }

    return true;
}

void SessionPool::disconnect(SessionTermination termination)
{
    if (!m_account) {
        return;
    }

    foreach (KIMAP::Session *s, m_unusedPool + m_reservedPool + m_connectingPool) {
        killSession(s, termination);
    }
    m_unusedPool.clear();
    m_reservedPool.clear();
    m_connectingPool.clear();
    m_pendingInitialSession = Q_NULLPTR;
    m_passwordRequester->cancelPasswordRequests();

    delete m_account;
    m_account = Q_NULLPTR;
    m_namespaces.clear();
    m_capabilities.clear();

    m_initialConnectDone = false;
    Q_EMIT disconnectDone();
}

qint64 SessionPool::requestSession()
{
    if (!m_initialConnectDone) {
        return -1;
    }

    qint64 requestNumber = ++m_requestCounter;

    // The queue was empty, so trigger the processing
    if (m_pendingRequests.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(processPendingRequests()));
    }

    m_pendingRequests << requestNumber;

    return requestNumber;
}

void SessionPool::cancelSessionRequest(qint64 id)
{
    Q_ASSERT(id > 0);
    m_pendingRequests.removeAll(id);
}

void SessionPool::releaseSession(KIMAP::Session *session)
{
    if (m_reservedPool.contains(session)) {
        m_reservedPool.removeAll(session);
        m_unusedPool << session;
    }
}

ImapAccount *SessionPool::account() const
{
    return m_account;
}

QStringList SessionPool::serverCapabilities() const
{
    return m_capabilities;
}

QList<KIMAP::MailBoxDescriptor> SessionPool::serverNamespaces() const
{
    return m_namespaces;
}

QList<KIMAP::MailBoxDescriptor> SessionPool::serverNamespaces(Namespace ns) const
{
    switch (ns) {
    case Personal:
        return m_personalNamespaces;
    case User:
        return m_userNamespaces;
    case Shared:
        return m_sharedNamespaces;
    default:
        break;
    }
    Q_ASSERT(false);
    return QList<KIMAP::MailBoxDescriptor>();
}

void SessionPool::killSession(KIMAP::Session *session, SessionTermination termination)
{
    if (!m_unusedPool.contains(session) && !m_reservedPool.contains(session) && !m_connectingPool.contains(session)) {
        qCWarning(IMAPRESOURCE_LOG) << "Unmanaged session" << session;
        Q_ASSERT(false);
        return;
    }
    QObject::disconnect(session, SIGNAL(stateChanged(KIMAP::Session::State,KIMAP::Session::State)),
                        this, SLOT(onSessionStateChanged(KIMAP::Session::State,KIMAP::Session::State)));
    m_unusedPool.removeAll(session);
    m_reservedPool.removeAll(session);
    m_connectingPool.removeAll(session);

    if (session->state() != KIMAP::Session::Disconnected && termination == LogoutSession) {
        KIMAP::LogoutJob *logout = new KIMAP::LogoutJob(session);
        QObject::connect(logout, &KJob::result,
                         session, &QObject::deleteLater);
        logout->start();
    } else {
        session->close();
        session->deleteLater();
    }
}

void SessionPool::declareSessionReady(KIMAP::Session *session)
{
    //This can happen if we happen to disconnect while capabilities and namespace are being retrieved,
    //resulting in us keeping a dangling pointer to a deleted session
    if (!m_connectingPool.contains(session)) {
        qCWarning(IMAPRESOURCE_LOG) << "Tried to declare a removed session ready";
        return;
    }

    m_pendingInitialSession = Q_NULLPTR;

    if (!m_initialConnectDone) {
        m_initialConnectDone = true;
        Q_EMIT connectDone();
    }

    m_connectingPool.removeAll(session);

    if (m_pendingRequests.isEmpty()) {
        m_unusedPool << session;
    } else {
        m_reservedPool << session;
        Q_EMIT sessionRequestDone(m_pendingRequests.takeFirst(), session);

        if (!m_pendingRequests.isEmpty()) {
            QTimer::singleShot(0, this, SLOT(processPendingRequests()));
        }
    }
}

void SessionPool::cancelSessionCreation(KIMAP::Session *session, int errorCode,
                                        const QString &errorMessage)
{
    m_pendingInitialSession = Q_NULLPTR;

    QString msg;
    if (m_account) {
        msg = i18n("Could not connect to the IMAP-server %1.\n%2", m_account->server(), errorMessage);
    } else {
        // Can happen when we lose all ready connections while trying to establish
        // a new connection, for example.
        msg = i18n("Could not connect to the IMAP server.\n%1", errorMessage);
    }

    if (!m_initialConnectDone) {
        disconnect(); // kills all sessions, including \a session
    } else {
        killSession(session, LogoutSession);
        if (!m_pendingRequests.isEmpty()) {
            Q_EMIT sessionRequestDone(m_pendingRequests.takeFirst(), Q_NULLPTR, errorCode, errorMessage);
            if (!m_pendingRequests.isEmpty()) {
                QTimer::singleShot(0, this, SLOT(processPendingRequests()));
            }
        }
    }
    //AlwaysQ_EMIT this at the end. This can call SessionPool::disconnect via ImapResource.
    Q_EMIT connectDone(errorCode, msg);
}

void SessionPool::processPendingRequests()
{
    if (!m_unusedPool.isEmpty()) {
        // We have a session ready to give out
        KIMAP::Session *session = m_unusedPool.takeFirst();
        m_reservedPool << session;
        if (!m_pendingRequests.isEmpty()) {
            Q_EMIT sessionRequestDone(m_pendingRequests.takeFirst(), session);
            if (!m_pendingRequests.isEmpty()) {
                QTimer::singleShot(0, this, SLOT(processPendingRequests()));
            }
        }
    } else if (m_unusedPool.size() + m_reservedPool.size() < m_maxPoolSize) {
        // We didn't reach the max pool size yet so create a new one
        m_passwordRequester->requestPassword();

    } else {
        // No session available, and max pool size reached
        if (!m_pendingRequests.isEmpty()) {
            Q_EMIT sessionRequestDone(
                m_pendingRequests.takeFirst(), Q_NULLPTR, NoAvailableSessionError,
                i18n("Could not create another extra connection to the IMAP-server %1.",
                     m_account->server()));
            if (!m_pendingRequests.isEmpty()) {
                QTimer::singleShot(0, this, SLOT(processPendingRequests()));
            }
        }
    }
}

void SessionPool::onPasswordRequestDone(int resultType, const QString &password)
{
    QString errorMessage;

    if (!m_account) {
        // it looks like the connection was lost while we were waiting
        // for the password, we should fail all the pending requests and stop there
        foreach (int request, m_pendingRequests) {
            Q_EMIT sessionRequestDone(request, Q_NULLPTR,
                                      LoginFailError, i18n("Disconnected from server during login."));
        }
        return;
    }

    switch (resultType) {
    case PasswordRequesterInterface::PasswordRetrieved:
        // All is fine
        break;
    case PasswordRequesterInterface::ReconnectNeeded:
        Q_ASSERT(m_pendingInitialSession != 0);
        cancelSessionCreation(m_pendingInitialSession, ReconnectNeededError, errorMessage);
        return;
    case PasswordRequesterInterface::UserRejected:
        errorMessage = i18n("Could not read the password: user rejected wallet access");
        if (m_pendingInitialSession) {
            cancelSessionCreation(m_pendingInitialSession, LoginFailError, errorMessage);
        } else {
            Q_EMIT connectDone(PasswordRequestError, errorMessage);
        }
        return;
    case PasswordRequesterInterface::EmptyPasswordEntered:
        errorMessage = i18n("Empty password");
        if (m_pendingInitialSession) {
            cancelSessionCreation(m_pendingInitialSession, LoginFailError, errorMessage);
        } else {
            Q_EMIT connectDone(PasswordRequestError, errorMessage);
        }
        return;
    }

    if (m_account->encryptionMode() != KIMAP::LoginJob::Unencrypted && !QSslSocket::supportsSsl()) {
        qCWarning(IMAPRESOURCE_LOG) << "Crypto not supported!";
        Q_EMIT connectDone(EncryptionError,
                           i18n("You requested TLS/SSL to connect to %1, but your "
                                "system does not seem to be set up for that.", m_account->server()));
        disconnect();
        return;
    }

    KIMAP::Session *session = Q_NULLPTR;
    if (m_pendingInitialSession) {
        session = m_pendingInitialSession;
    } else {
        session = new KIMAP::Session(m_account->server(), m_account->port(), this);
        QObject::connect(session, SIGNAL(destroyed(QObject*)), this, SLOT(onSessionDestroyed(QObject*)));
        session->setUiProxy(m_sessionUiProxy);
        session->setTimeout(m_account->timeout());
        m_connectingPool << session;
    }

    QObject::connect(session, SIGNAL(stateChanged(KIMAP::Session::State,KIMAP::Session::State)),
                     this, SLOT(onSessionStateChanged(KIMAP::Session::State,KIMAP::Session::State)));

    KIMAP::LoginJob *loginJob = new KIMAP::LoginJob(session);
    loginJob->setUserName(m_account->userName());
    loginJob->setPassword(password);
    loginJob->setEncryptionMode(m_account->encryptionMode());
    loginJob->setAuthenticationMode(m_account->authenticationMode());

    QObject::connect(loginJob, SIGNAL(result(KJob*)),
                     this, SLOT(onLoginDone(KJob*)));
    loginJob->start();
}

void SessionPool::onLoginDone(KJob *job)
{
    KIMAP::LoginJob *login = static_cast<KIMAP::LoginJob *>(job);
    //Can happen if we disonnected meanwhile
    if (!m_connectingPool.contains(login->session())) {
        Q_EMIT connectDone(CancelledError, i18n("Disconnected from server during login."));
        return;
    }

    if (job->error() == 0) {
        if (m_initialConnectDone) {
            declareSessionReady(login->session());
        } else {
            // On initial connection we ask for capabilities
            KIMAP::CapabilitiesJob *capJob = new KIMAP::CapabilitiesJob(login->session());
            QObject::connect(capJob, &KIMAP::CapabilitiesJob::result, this, &SessionPool::onCapabilitiesTestDone);
            capJob->start();
        }
    } else {
        if (job->error() == KIMAP::LoginJob::ERR_COULD_NOT_CONNECT) {
            if (m_account) {
                cancelSessionCreation(login->session(),
                                      CouldNotConnectError,
                                      i18n("Could not connect to the IMAP-server %1.\n%2",
                                           m_account->server(), job->errorString()));
            } else {
                // Can happen when we loose all ready connections while trying to login.
                cancelSessionCreation(login->session(),
                                      CouldNotConnectError,
                                      i18n("Could not connect to the IMAP-server.\n%1",
                                           job->errorString()));
            }
        } else {
            // Connection worked, but login failed -> ask for a different password or ssl settings.
            m_pendingInitialSession = login->session();
            m_passwordRequester->requestPassword(PasswordRequesterInterface::WrongPasswordRequest,
                                                 job->errorString());
        }
    }
}

void SessionPool::onCapabilitiesTestDone(KJob *job)
{
    KIMAP::CapabilitiesJob *capJob = qobject_cast<KIMAP::CapabilitiesJob *>(job);
    //Can happen if we disonnected meanwhile
    if (!m_connectingPool.contains(capJob->session())) {
        Q_EMIT connectDone(CancelledError, i18n("Disconnected from server during login."));
        return;
    }

    if (job->error()) {
        if (m_account) {
            cancelSessionCreation(capJob->session(),
                                  CapabilitiesTestError,
                                  i18n("Could not test the capabilities supported by the "
                                       "IMAP server %1.\n%2",
                                       m_account->server(), job->errorString()));
        } else {
            // Can happen when we loose all ready connections while trying to check capabilities.
            cancelSessionCreation(capJob->session(),
                                  CapabilitiesTestError,
                                  i18n("Could not test the capabilities supported by the "
                                       "IMAP server.\n%1", job->errorString()));
        }
        return;
    }

    m_capabilities = capJob->capabilities();
    QStringList expected;
    expected << QStringLiteral("IMAP4REV1");

    QStringList missing;
    foreach (const QString &capability, expected) {
        if (!m_capabilities.contains(capability)) {
            missing << capability;
        }
    }

    if (!missing.isEmpty()) {
        cancelSessionCreation(capJob->session(),
                              IncompatibleServerError,
                              i18n("Cannot use the IMAP server %1, "
                                   "some mandatory capabilities are missing: %2. "
                                   "Please ask your sysadmin to upgrade the server.",
                                   m_account->server(),
                                   missing.join(QLatin1String(", "))));
        return;
    }

    // If the extension is supported, grab the namespaces from the server
    if (m_capabilities.contains(QLatin1String("NAMESPACE"))) {
        KIMAP::NamespaceJob *nsJob = new KIMAP::NamespaceJob(capJob->session());
        QObject::connect(nsJob, &KIMAP::NamespaceJob::result, this, &SessionPool::onNamespacesTestDone);
        nsJob->start();
        return;
    } else {
        declareSessionReady(capJob->session());
    }
}

void SessionPool::onNamespacesTestDone(KJob *job)
{
    KIMAP::NamespaceJob *nsJob = qobject_cast<KIMAP::NamespaceJob *>(job);
    // Can happen if we disconnect meanwhile
    if (!m_connectingPool.contains(nsJob->session())) {
        Q_EMIT connectDone(CancelledError, i18n("Disconnected from server during login."));
        return;
    }

    if (nsJob->containsEmptyNamespace()) {
        // When we got the empty namespace here, we assume that the other
        // ones can be freely ignored and that the server will give us all
        // the mailboxes if we list from the empty namespace itself...

        m_namespaces.clear();

    } else {
        // ... otherwise we assume that we have to list explicitly each
        // namespace

        m_namespaces = nsJob->personalNamespaces() +
                       nsJob->userNamespaces() +
                       nsJob->sharedNamespaces();
    }

    declareSessionReady(nsJob->session());
}

void SessionPool::onSessionStateChanged(KIMAP::Session::State newState, KIMAP::Session::State oldState)
{
    if (newState == KIMAP::Session::Disconnected && oldState != KIMAP::Session::Disconnected) {
        onConnectionLost();
    }
}

void SessionPool::onConnectionLost()
{
    KIMAP::Session *session = static_cast<KIMAP::Session *>(sender());

    m_unusedPool.removeAll(session);
    m_reservedPool.removeAll(session);
    m_connectingPool.removeAll(session);

    if (m_unusedPool.isEmpty() && m_reservedPool.isEmpty()) {
        m_passwordRequester->cancelPasswordRequests();
        delete m_account;
        m_account = Q_NULLPTR;
        m_namespaces.clear();
        m_capabilities.clear();

        m_initialConnectDone = false;
    }

    Q_EMIT connectionLost(session);

    session->deleteLater();
    if (session == m_pendingInitialSession) {
        m_pendingInitialSession = Q_NULLPTR;
    }
}

void SessionPool::onSessionDestroyed(QObject *object)
{
    //Safety net for bugs that cause dangling session pointers
    KIMAP::Session *session = static_cast<KIMAP::Session *>(object);
    if (m_unusedPool.contains(session) || m_reservedPool.contains(session) || m_connectingPool.contains(session)) {
        qCWarning(IMAPRESOURCE_LOG) << "Session destroyed while still in pool" << session;
        m_unusedPool.removeAll(session);
        m_reservedPool.removeAll(session);
        m_connectingPool.removeAll(session);
        Q_ASSERT(false);
    }
}

