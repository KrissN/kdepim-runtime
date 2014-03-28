/*
    Copyright (c) 2007 Till Adam <adam@kde.org>
    Copyright (C) 2008 Omat Holding B.V. <info@omat.nl>
    Copyright (C) 2009 Kevin Ottens <ervin@kde.org>

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

#ifndef RESOURCES_IMAP_IMAPRESOURCE_H
#define RESOURCES_IMAP_IMAPRESOURCE_H

#include <akonadi/resourcebase.h>
#include <akonadi/agentsearchinterface.h>
#include <KDialog>
#include <QPointer>

class QTimer;

class ResourceTask;
namespace KIMAP
{
  class Session;
}

class ImapIdleManager;
class SessionPool;
class ResourceState;
class SubscriptionDialog;

class ImapResource : public Akonadi::ResourceBase,
                     public Akonadi::AgentBase::ObserverV3,
                     public Akonadi::AgentSearchInterface
{
  Q_OBJECT
  Q_CLASSINFO( "D-Bus Interface", "org.kde.Akonadi.Imap.Resource" )

  using Akonadi::AgentBase::Observer::collectionChanged;

public:
  explicit ImapResource( const QString &id );
  ~ImapResource();


  KDialog *createConfigureDialog( WId windowId );
  QStringList serverCapabilities() const;

public Q_SLOTS:
  virtual void configure( WId windowId );

  // DBus methods
  Q_SCRIPTABLE void requestManualExpunge( qint64 collectionId );
  Q_SCRIPTABLE int configureSubscription( qlonglong windowId = 0 );

  // pseudo-virtual called by ResourceBase
  QString dumpResourceToString() const;

protected Q_SLOTS:
  void startIdleIfNeeded();
  void startIdle();

  void abortActivity();

  void retrieveCollections();

  void retrieveItems( const Akonadi::Collection &col );
  bool retrieveItem( const Akonadi::Item &item, const QSet<QByteArray> &parts );

protected:
  virtual void itemAdded( const Akonadi::Item &item, const Akonadi::Collection &collection );
  virtual void itemChanged( const Akonadi::Item &item, const QSet<QByteArray> &parts );
  virtual void itemsFlagsChanged( const Akonadi::Item::List &items, const QSet<QByteArray> &addedFlags, const QSet<QByteArray> &removedFlags );
  virtual void itemsRemoved( const Akonadi::Item::List &items );
  virtual void itemsMoved( const Akonadi::Item::List &item, const Akonadi::Collection &source,
                           const Akonadi::Collection &destination );


  virtual void collectionAdded( const Akonadi::Collection &collection, const Akonadi::Collection &parent );
  virtual void collectionChanged( const Akonadi::Collection &collection, const QSet<QByteArray> &parts );
  virtual void collectionRemoved( const Akonadi::Collection &collection );
  virtual void collectionMoved( const Akonadi::Collection &collection, const Akonadi::Collection &source,
                                const Akonadi::Collection &destination );

  virtual void addSearch( const QString &query, const QString &queryLanguage, const Akonadi::Collection &resultCollection );
  virtual void removeSearch( const Akonadi::Collection &resultCollection );
  virtual void search( const QString &query, const Akonadi::Collection &collection );

  virtual void doSetOnline(bool online);

  QChar separatorCharacter() const;
  void setSeparatorCharacter( const QChar &separator );

  virtual void aboutToQuit();

private Q_SLOTS:
  void doSearch( const QVariant &arg );

  void reconnect();

  void scheduleConnectionAttempt();
  void startConnect( const QVariant & ); // the parameter is necessary, since this method is used by the task scheduler
  void onConnectDone( int errorCode, const QString &errorMessage );
  void onConnectionLost( KIMAP::Session *session );


  void onIdleCollectionFetchDone( KJob *job );


  void onExpungeCollectionFetchDone( KJob *job );
  void triggerCollectionExpunge( const QVariant &collectionVariant );


  void triggerCollectionExtraInfoJobs( const QVariant &collection );

  void taskDestroyed( QObject *task );

  void showError( const QString &message );
  void clearStatusMessage();

  void onConfigurationDone( int result );

private:
  friend class ResourceState;

  void queueTask( ResourceTask *task );
  bool needsNetwork() const;

  friend class ImapIdleManager;

  SessionPool *m_pool;
  QList<ResourceTask*> m_taskList;
  QPointer<SubscriptionDialog> mSubscriptions;
  ImapIdleManager *m_idle;
  QTimer *m_statusMessageTimer;
  QChar m_separatorCharacter;
};

#endif
