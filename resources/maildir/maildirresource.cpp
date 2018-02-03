/*
    Copyright (c) 2007 Till Adam <adam@kde.org>

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

#include "maildirresource.h"
#include "settings.h"
#include "maildirsettingsadaptor.h"
#include "configdialog.h"
#include "retrieveitemsjob.h"

#include <QDir>
#include <QDBusConnection>

#include <Akonadi/KMime/MessageParts>
#include <changerecorder.h>
#include <itemfetchscope.h>
#include <itemfetchjob.h>
#include <itemmodifyjob.h>
#include <collectionfetchscope.h>
#include <cachepolicy.h>
#include <collectionfetchjob.h>
#include <kdbusconnectionpool.h>
#include <Akonadi/KMime/MessageFlags>
#include <kmime/kmime_message.h>

#include "maildirresource_debug.h"
#include <kdirwatch.h>
#include <KLocalizedString>
#include <kwindowsystem.h>

#include "libmaildir/maildir.h"
#include <QStandardPaths>

using namespace Akonadi;
using KPIM::Maildir;
using namespace Akonadi_Maildir_Resource;

#define CLEANER_TIMEOUT 2*6000

Maildir MaildirResource::maildirForCollection(const Collection &col)
{
    const QString path = maildirPathForCollection(col);
    if (mMaildirsForCollection.contains(path)) {
        return mMaildirsForCollection.value(path);
    }

    if (col.remoteId().isEmpty()) {
        qCWarning(MAILDIRRESOURCE_LOG) << "Got incomplete ancestor chain:" << col;
        return Maildir();
    }

    if (col.parentCollection() == Collection::root()) {
        if (col.remoteId() != mSettings->path()) {
            qCWarning(MAILDIRRESOURCE_LOG) << "RID mismatch, is " << col.remoteId() << " expected " << mSettings->path();
        }
        Maildir maildir(col.remoteId(), mSettings->topLevelIsContainer());
        mMaildirsForCollection.insert(path, maildir);
        return maildir;
    }
    Maildir parentMd = maildirForCollection(col.parentCollection());
    Maildir maildir = parentMd.subFolder(col.remoteId());
    mMaildirsForCollection.insert(path, maildir);
    return maildir;
}

Collection MaildirResource::collectionForMaildir(const Maildir &md) const
{
    if (!md.isValid()) {
        return Collection();
    }

    Collection col;
    if (md.path() == mSettings->path()) {
        col.setRemoteId(md.path());
        col.setParentCollection(Collection::root());
    } else {
        const Collection parent = collectionForMaildir(md.parent());
        col.setRemoteId(md.name());
        col.setParentCollection(parent);
    }

    return col;
}

MaildirResource::MaildirResource(const QString &id)
    : ResourceBase(id)
    , mSettings(new MaildirSettings(config()))
    , mFsWatcher(new KDirWatch(this))
{
    // we cannot be sure that a config file is existing
    // the MaildirResource will always be build
    // look for a resource of this name
    QString configFile = QStandardPaths::locate(QStandardPaths::ConfigLocation, id + QLatin1String("rc"));
    // if not present, create it
    if (configFile.isEmpty()) {
        // check if the resource was used before
        CollectionFetchJob *job = new CollectionFetchJob(Collection::root(), Akonadi::CollectionFetchJob::FirstLevel, this);
        job->fetchScope().setResource(id);
        connect(job, &CollectionFetchJob::result, this, &MaildirResource::attemptConfigRestoring);
        job->start();
    }
    new MaildirSettingsAdaptor(mSettings);
    KDBusConnectionPool::threadConnection().registerObject(QStringLiteral("/Settings"),
                                                           mSettings, QDBusConnection::ExportAdaptors);
    connect(this, &MaildirResource::reloadConfiguration, this, &MaildirResource::configurationChanged);

    // We need to enable this here, otherwise we neither get the remote ID of the
    // parent collection when a collection changes, nor the full item when an item
    // is added.
    changeRecorder()->fetchCollection(true);
    changeRecorder()->itemFetchScope().fetchFullPayload(true);
    changeRecorder()->itemFetchScope().setAncestorRetrieval(ItemFetchScope::All);
    changeRecorder()->itemFetchScope().setFetchModificationTime(false);
    changeRecorder()->collectionFetchScope().setAncestorRetrieval(CollectionFetchScope::All);
    changeRecorder()->fetchChangedOnly(true);

    setHierarchicalRemoteIdentifiersEnabled(true);

    ItemFetchScope scope(changeRecorder()->itemFetchScope());
    scope.fetchFullPayload(false);
    scope.fetchPayloadPart(MessagePart::Header);
    scope.setAncestorRetrieval(ItemFetchScope::None);
    setItemSynchronizationFetchScope(scope);

    connect(mFsWatcher, &KDirWatch::dirty, this, &MaildirResource::slotDirChanged);
    if (!ensureSaneConfiguration()) {
        Q_EMIT error(i18n("Unusable configuration."));
    } else {
        synchronizeCollectionTree();
    }

    mChangedCleanerTimer = new QTimer(this);
    connect(mChangedCleanerTimer, &QTimer::timeout, this, &MaildirResource::changedCleaner);
}

void MaildirResource::attemptConfigRestoring(KJob *job)
{
    if (job->error()) {
        qCDebug(MAILDIRRESOURCE_LOG) << job->errorString();
        return;
    }
    // we cannot be sure that a config file is existing
    const QString id = identifier();
    const QString configFile = QStandardPaths::locate(QStandardPaths::ConfigLocation, id + QLatin1String("rc"));
    // we test it again, to be sure
    if (configFile.isEmpty()) {
        // it is still empty, create it
        qCWarning(MAILDIRRESOURCE_LOG) << "the resource is not properly configured: there is no config file for the resource. We create a new one.";
        const Collection::List cols = qobject_cast<CollectionFetchJob *>(job)->collections();
        QString path;
        if (!cols.isEmpty()) {
            qCDebug(MAILDIRRESOURCE_LOG) << "the collections list is not empty";
            Collection col = cols.first();
            // get the path of the collection
            path = col.remoteId();
        }
        // test the path
        if (path.isEmpty()) {
            qCDebug(MAILDIRRESOURCE_LOG) << "build a new path";
            const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1Char('/');
            // we use "id" to get an unique path
            path = dataDir;
            if (!defaultResourceType().isEmpty()) {
                path += defaultResourceType()  + QLatin1Char('/');
            }
            path += id;
            qCDebug(MAILDIRRESOURCE_LOG) << "set the path" << path;
            mSettings->setPath(path);
            // set the resource into container mode for its top level
            mSettings->setTopLevelIsContainer(true);
        } else {
            // check how the directory looks like the actual check is missing.
            Maildir root(mSettings->path(), true);
            mSettings->setTopLevelIsContainer(root.isValid());
        }
        qCDebug(MAILDIRRESOURCE_LOG) << "synchronize";
        configurationChanged();
    }
}

MaildirResource::~MaildirResource()
{
    delete mSettings;
}

bool MaildirResource::retrieveItems(const Akonadi::Item::List &items, const QSet<QByteArray> &parts)
{
    Q_UNUSED(parts);

    const Maildir md = maildirForCollection(items.at(0).parentCollection());
    if (!md.isValid()) {
        cancelTask(i18n("Unable to fetch item: The maildir folder \"%1\" is not valid.",
                        md.path()));
        return false;
    }

    Akonadi::Item::List rv;
    rv.reserve(items.count());
    for (const Akonadi::Item &item : items) {
        const QByteArray data = md.readEntry(item.remoteId());

        KMime::Message *mail = new KMime::Message();
        mail->setContent(KMime::CRLFtoLF(data));
        mail->parse();
        // Some messages may have an empty body
        if (mail->body().isEmpty()) {
            if (parts.contains("PLD:BODY") || parts.contains("PLD:RFC822")) {
                // In that case put a space in as body so that it gets cached
                // otherwise we'll wrongly believe the body part is missing from the cache
                mail->setBody(" ");
            }
        }

        Item i(item);
        i.setPayload(KMime::Message::Ptr(mail));
        Akonadi::MessageFlags::copyMessageFlags(*mail, i);
        rv.push_back(i);
    }

    itemsRetrieved(rv);
    return true;
}

QString MaildirResource::itemMimeType() const
{
    return KMime::Message::mimeType();
}

void MaildirResource::configurationChanged()
{
    mSettings->save();
    bool configValid = ensureSaneConfiguration();
    configValid = configValid && ensureDirExists();
    if (configValid) {
        Q_EMIT status(Idle);
        setOnline(true);
    }
}

void MaildirResource::aboutToQuit()
{
    // The settings may not have been saved if e.g. they have been modified via
    // DBus instead of the config dialog.
    mSettings->save();
}

QString MaildirResource::defaultResourceType()
{
    return QString();
}

void MaildirResource::configure(WId windowId)
{
    ConfigDialog dlg(mSettings, identifier());
    if (windowId) {
        KWindowSystem::setMainWindow(&dlg, windowId);
    }
    dlg.setWindowIcon(QIcon::fromTheme(QStringLiteral("message-rfc822")));
    if (dlg.exec()) {
        // if we have no name, or the default one,
        // better use the name of the top level collection
        // that looks nicer
        if (name().isEmpty() || name() == identifier()) {
            Maildir md(mSettings->path());
            setName(md.name());
        }
        Q_EMIT configurationDialogAccepted();
    } else {
        Q_EMIT configurationDialogRejected();
    }

    configurationChanged();
    synchronizeCollectionTree();
}

void MaildirResource::itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection)
{
    if (!ensureSaneConfiguration()) {
        cancelTask(i18n("Unusable configuration."));
        return;
    }
    Maildir dir = maildirForCollection(collection);
    if (mSettings->readOnly() || !dir.isValid()) {
        cancelTask(dir.lastError());
        return;
    }

    // we can only deal with mail
    if (!item.hasPayload<KMime::Message::Ptr>()) {
        cancelTask(i18n("Error: Unsupported type."));
        return;
    }
    const KMime::Message::Ptr mail = item.payload<KMime::Message::Ptr>();

    stopMaildirScan(dir);

    const QString rid = dir.addEntry(mail->encodedContent());

    if (rid.isEmpty()) {
        restartMaildirScan(dir);
        cancelTask(dir.lastError());
        return;
    }

    mChangedFiles.insert(rid);
    mChangedCleanerTimer->start(CLEANER_TIMEOUT);

    restartMaildirScan(dir);

    Item i(item);
    i.setRemoteId(rid);
    changeCommitted(i);
}

void MaildirResource::itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    if (!ensureSaneConfiguration()) {
        cancelTask(i18n("Unusable configuration."));
        return;
    }

    bool bodyChanged = false;
    bool flagsChanged = false;
    bool headChanged = false;
    for (const QByteArray &part : parts) {
        if (part.startsWith("PLD:RFC822")) {
            bodyChanged = true;
        } else if (part.startsWith("PLD:HEAD")) {
            headChanged = true;
        }
        if (part.contains("FLAGS")) {
            flagsChanged = true;
        }
    }

    if (mSettings->readOnly() || (!bodyChanged && !flagsChanged && !headChanged)) {
        changeProcessed();
        return;
    }

    Maildir dir = maildirForCollection(item.parentCollection());
    if (!dir.isValid()) {
        cancelTask(dir.lastError());
        return;
    }

    Item newItem(item);

    if (flagsChanged || bodyChanged || headChanged) {   //something has changed that we can deal with
        stopMaildirScan(dir);

        if (flagsChanged) {   //flags changed, store in file name and get back the new filename (id)
            const QString newKey = dir.changeEntryFlags(item.remoteId(), item.flags());
            if (newKey.isEmpty()) {
                restartMaildirScan(dir);
                cancelTask(i18n("Failed to change the flags for the mail. %1", dir.lastError()));
                return;
            }
            newItem.setRemoteId(newKey);
        }

        if (bodyChanged || headChanged) {   //head or body changed
            // we can only deal with mail
            if (item.hasPayload<KMime::Message::Ptr>()) {
                const KMime::Message::Ptr mail = item.payload<KMime::Message::Ptr>();
                QByteArray data = mail->encodedContent();
                if (headChanged && !bodyChanged) {
                    //only the head has changed, get the current version of the mail
                    //replace the head and store the new mail in the file
                    const QByteArray currentData = dir.readEntry(newItem.remoteId());
                    if (currentData.isEmpty() && !dir.lastError().isEmpty()) {
                        restartMaildirScan(dir);
                        cancelTask(dir.lastError());
                        return;
                    }
                    const QByteArray newHead = mail->head();
                    mail->setContent(currentData);
                    mail->setHead(newHead);
                    mail->parse();
                    data = mail->encodedContent();
                }
                if (!dir.writeEntry(newItem.remoteId(), data)) {
                    restartMaildirScan(dir);
                    cancelTask(dir.lastError());
                    return;
                }
                mChangedFiles.insert(newItem.remoteId());
                mChangedCleanerTimer->start(CLEANER_TIMEOUT);
            } else {
                restartMaildirScan(dir);
                cancelTask(i18n("Maildir resource got a non-mail content."));
                return;
            }
        }

        restartMaildirScan(dir);

        changeCommitted(newItem);
    } else {
        Q_EMIT changeProcessed();
    }
}

void MaildirResource::itemMoved(const Item &item, const Collection &source, const Collection &destination)
{
    if (source == destination) {   // should not happen but would confuse Maildir::moveEntryTo
        changeProcessed();
        return;
    }

    if (!ensureSaneConfiguration()) {
        cancelTask(i18n("Unusable configuration."));
        return;
    }

    Maildir sourceDir = maildirForCollection(source);
    if (!sourceDir.isValid()) {
        cancelTask(i18n("Source folder is invalid: '%1'.", sourceDir.lastError()));
        return;
    }

    Maildir destDir = maildirForCollection(destination);
    if (!destDir.isValid()) {
        cancelTask(i18n("Destination folder is invalid: '%1'.", destDir.lastError()));
        return;
    }

    stopMaildirScan(sourceDir);
    stopMaildirScan(destDir);

    const QString newRid = sourceDir.moveEntryTo(item.remoteId(), destDir);

    mChangedFiles.insert(newRid);
    mChangedCleanerTimer->start(CLEANER_TIMEOUT);

    restartMaildirScan(sourceDir);
    restartMaildirScan(destDir);

    if (newRid.isEmpty()) {
        cancelTask(i18n("Could not move message '%1' from '%2' to '%3'. The error was %4.", item.remoteId(), sourceDir.path(), destDir.path(), sourceDir.lastError()));
        return;
    }

    Item i(item);
    i.setRemoteId(newRid);
    changeCommitted(i);
}

void MaildirResource::itemRemoved(const Akonadi::Item &item)
{
    if (!ensureSaneConfiguration()) {
        cancelTask(i18n("Unusable configuration."));
        return;
    }

    if (!mSettings->readOnly()) {
        Maildir dir = maildirForCollection(item.parentCollection());
        // !dir.isValid() means that our parent folder has been deleted already,
        // so we don't care at all as that one will be recursive anyway
        stopMaildirScan(dir);
        if (dir.isValid() && !dir.removeEntry(item.remoteId())) {
            Q_EMIT error(i18n("Failed to delete message: %1", item.remoteId()));
        }
        restartMaildirScan(dir);
    }
    qCDebug(MAILDIRRESOURCE_LOG) << "Item removed" << item.id() << " in collection :" << item.parentCollection().id();
    changeProcessed();
}

Collection::List MaildirResource::listRecursive(const Collection &root, const Maildir &dir)
{
    if (mSettings->monitorFilesystem()) {
        mFsWatcher->addDir(dir.path() + QDir::separator() + QLatin1String("new"));
        mFsWatcher->addDir(dir.path() + QDir::separator() + QLatin1String("cur"));
        mFsWatcher->addDir(dir.subDirPath());
        if (dir.isRoot()) {
            mFsWatcher->addDir(dir.path());
        }
    }

    Collection::List list;
    const QStringList mimeTypes = QStringList() << itemMimeType() << Collection::mimeType();
    const QStringList lstDir = dir.subFolderList();
    for (const QString &sub : lstDir) {
        Collection c;
        c.setName(sub);
        c.setRemoteId(sub);
        c.setParentCollection(root);
        c.setContentMimeTypes(mimeTypes);

        const Maildir md = maildirForCollection(c);
        if (!md.isValid()) {
            continue;
        }

        list << c;
        list += listRecursive(c, md);
    }
    return list;
}

void MaildirResource::retrieveCollections()
{
    Maildir dir(mSettings->path(), mSettings->topLevelIsContainer());
    if (!dir.isValid()) {
        Q_EMIT error(dir.lastError());
        collectionsRetrieved(Collection::List());
        return;
    }

    Collection root;
    root.setParentCollection(Collection::root());
    root.setRemoteId(mSettings->path());
    root.setName(name());
    if (mSettings->readOnly()) {
        root.setRights(Collection::ReadOnly);
    } else {
        if (mSettings->topLevelIsContainer()) {
            root.setRights(Collection::ReadOnly | Collection::CanCreateCollection);
        } else {
            root.setRights(Collection::CanChangeItem | Collection::CanCreateItem | Collection::CanDeleteItem
                           | Collection::CanCreateCollection);
        }
    }

    CachePolicy policy;
    policy.setInheritFromParent(false);
    policy.setSyncOnDemand(true);
    policy.setLocalParts(QStringList() << QLatin1String(MessagePart::Envelope));
    policy.setCacheTimeout(1);
    policy.setIntervalCheckTime(-1);
    root.setCachePolicy(policy);

    QStringList mimeTypes;
    mimeTypes << Collection::mimeType();
    mimeTypes << itemMimeType();
    root.setContentMimeTypes(mimeTypes);

    Collection::List list;
    list << root;
    list += listRecursive(root, dir);
    collectionsRetrieved(list);
}

void MaildirResource::retrieveItems(const Akonadi::Collection &col)
{
    const Maildir md = maildirForCollection(col);
    if (!md.isValid()) {
        cancelTask(i18n("Maildir '%1' for collection '%2' is invalid.", md.path(), col.remoteId()));
        return;
    }

    RetrieveItemsJob *job = new RetrieveItemsJob(col, md, this);
    job->setMimeType(itemMimeType());
    connect(job, &RetrieveItemsJob::result, this, &MaildirResource::slotItemsRetrievalResult);
}

void MaildirResource::slotItemsRetrievalResult(KJob *job)
{
    if (job->error()) {
        cancelTask(job->errorString());
    } else {
        itemsRetrievalDone();
    }
}

void MaildirResource::collectionAdded(const Collection &collection, const Collection &parent)
{
    if (!ensureSaneConfiguration()) {
        Q_EMIT error(i18n("Unusable configuration."));
        changeProcessed();
        return;
    }

    Maildir md = maildirForCollection(parent);
    qCDebug(MAILDIRRESOURCE_LOG) << md.subFolderList();
    if (mSettings->readOnly() || !md.isValid()) {
        changeProcessed();
        return;
    } else {
        const QString collectionName(collection.name().remove(QDir::separator()));
        const QString newFolderPath = md.addSubFolder(collectionName);
        if (newFolderPath.isEmpty()) {
            changeProcessed();
            return;
        }

        qCDebug(MAILDIRRESOURCE_LOG) << md.subFolderList();

        Collection col = collection;
        col.setRemoteId(collectionName);
        col.setName(collectionName);
        changeCommitted(col);
    }
}

void MaildirResource::collectionChanged(const Collection &collection)
{
    if (!ensureSaneConfiguration()) {
        Q_EMIT error(i18n("Unusable configuration."));
        changeProcessed();
        return;
    }

    if (collection.parentCollection() == Collection::root()) {
        if (collection.name() != name()) {
            setName(collection.name());
        }
        changeProcessed();
        return;
    }

    if (collection.remoteId() == collection.name()) {
        changeProcessed();
        return;
    }

    Maildir md = maildirForCollection(collection);
    if (!md.isValid()) {
        assert(!collection.remoteId().isEmpty());   // caught in resourcebase
        // we don't have a maildir for this collection yet, probably due to a race
        // make one, otherwise the rename below will fail
        md.create();
    }

    const QString collectionName(collection.name().remove(QDir::separator()));
    if (!md.rename(collectionName)) {
        Q_EMIT error(i18n("Unable to rename maildir folder '%1'.", collection.name()));
        changeProcessed();
        return;
    }
    Collection c(collection);
    c.setRemoteId(collectionName);
    c.setName(collectionName);
    changeCommitted(c);
}

void MaildirResource::collectionMoved(const Collection &collection, const Collection &source, const Collection &dest)
{
    qCDebug(MAILDIRRESOURCE_LOG) << collection << source << dest;

    if (!ensureSaneConfiguration()) {
        Q_EMIT error(i18n("Unusable configuration."));
        changeProcessed();
        return;
    }

    if (collection.parentCollection() == Collection::root()) {
        Q_EMIT error(i18n("Cannot move root maildir folder '%1'.", collection.remoteId()));
        changeProcessed();
        return;
    }

    if (source == dest) {   // should not happen, but who knows...
        changeProcessed();
        return;
    }

    Collection c(collection);
    c.setParentCollection(source);
    Maildir md = maildirForCollection(c);
    Maildir destMd = maildirForCollection(dest);
    if (!md.moveTo(destMd)) {
        Q_EMIT error(i18n("Unable to move maildir folder '%1' from '%2' to '%3'.", collection.remoteId(), source.remoteId(), dest.remoteId()));
        changeProcessed();
    } else {
        const QString path = maildirPathForCollection(c);
        mMaildirsForCollection.remove(path);
        changeCommitted(collection);
    }
}

void MaildirResource::collectionRemoved(const Akonadi::Collection &collection)
{
    if (!ensureSaneConfiguration()) {
        Q_EMIT error(i18n("Unusable configuration."));
        changeProcessed();
        return;
    }

    if (collection.parentCollection() == Collection::root()) {
        Q_EMIT error(i18n("Cannot delete top-level maildir folder '%1'.", mSettings->path()));
        changeProcessed();
        return;
    }

    Maildir md = maildirForCollection(collection.parentCollection());
    // !md.isValid() means that our parent folder has been deleted already,
    // so we don't care at all as that one will be recursive anyway
    if (md.isValid() && !md.removeSubFolder(collection.remoteId())) {
        Q_EMIT error(i18n("Failed to delete sub-folder '%1'.", collection.remoteId()));
    }

    const QString path = maildirPathForCollection(collection);
    mMaildirsForCollection.remove(path);

    changeProcessed();
}

bool MaildirResource::ensureDirExists()
{
    Maildir root(mSettings->path());
    if (!root.isValid(false) && !mSettings->topLevelIsContainer()) {
        if (!root.create()) {
            Q_EMIT status(Broken, i18n("Unable to create maildir '%1'.", mSettings->path()));
        }
        return false;
    }
    return true;
}

bool MaildirResource::ensureSaneConfiguration()
{
    if (mSettings->path().isEmpty()) {
        Q_EMIT status(NotConfigured, i18n("No usable storage location configured."));
        setOnline(false);
        return false;
    }
    return true;
}

void MaildirResource::slotDirChanged(const QString &dir)
{
    QFileInfo fileInfo(dir);
    if (fileInfo.isFile()) {
        slotFileChanged(fileInfo);
        return;
    }

    if (dir == mSettings->path()) {
        synchronizeCollectionTree();
        synchronizeCollection(Collection::root().id());
        return;
    }

    if (dir.endsWith(QLatin1String(".directory"))) {
        synchronizeCollectionTree(); //might be too much, but this is not a common case anyway
        return;
    }

    QDir d(dir);
    if (!d.cdUp()) {
        return;
    }

    Maildir md(d.path());
    if (!md.isValid()) {
        return;
    }

    md.refreshKeyCache();

    const Collection col = collectionForMaildir(md);
    if (col.remoteId().isEmpty()) {
        qCDebug(MAILDIRRESOURCE_LOG) << "unable to find collection for path" << dir;
        return;
    }

    CollectionFetchJob *job = new CollectionFetchJob(col, Akonadi::CollectionFetchJob::Base, this);
    connect(job, &CollectionFetchJob::result, this, &MaildirResource::fsWatchDirFetchResult);
}

void MaildirResource::fsWatchDirFetchResult(KJob *job)
{
    if (job->error()) {
        qCDebug(MAILDIRRESOURCE_LOG) << job->errorString();
        return;
    }
    const Collection::List cols = qobject_cast<CollectionFetchJob *>(job)->collections();
    if (cols.isEmpty()) {
        return;
    }

    synchronizeCollection(cols.first().id());
}

void MaildirResource::slotFileChanged(const QFileInfo &fileInfo)
{
    const QString key = fileInfo.fileName();
    if (mChangedFiles.remove(key) > 0) {
        return;
    }

    QString path = fileInfo.path();
    if (path.endsWith(QLatin1String("/new"))) {
        path.remove(path.length() - 4, 4);
    } else if (path.endsWith(QLatin1String("/cur"))) {
        path.remove(path.length() - 4, 4);
    }

    const Maildir md(path);
    if (!md.isValid()) {
        return;
    }

    const Collection col = collectionForMaildir(md);
    if (col.remoteId().isEmpty()) {
        qCDebug(MAILDIRRESOURCE_LOG) << "unable to find collection for path" << fileInfo.path();
        return;
    }

    Item item;
    item.setRemoteId(key);
    item.setParentCollection(col);

    ItemFetchJob *job = new ItemFetchJob(item, this);
    job->setProperty("entry", key);
    job->setProperty("dir", path);
    connect(job, &ItemFetchJob::result, this, &MaildirResource::fsWatchFileFetchResult);
}

void MaildirResource::fsWatchFileFetchResult(KJob *job)
{
    if (job->error()) {
        qCDebug(MAILDIRRESOURCE_LOG) << job->errorString();
        return;
    }
    Item::List items = qobject_cast<ItemFetchJob *>(job)->items();
    if (items.isEmpty()) {
        return;
    }

    const QString fileName = job->property("entry").toString();
    const QString path = job->property("dir").toString();

    const Maildir md(path);

    Item item(items.at(0));
    const qint64 entrySize = md.size(fileName);
    if (entrySize >= 0) {
        item.setSize(entrySize);
    }

    const Item::Flags flags = md.readEntryFlags(fileName);
    for (const Item::Flag &flag : flags) {
        item.setFlag(flag);
    }

    const QByteArray data = md.readEntry(fileName);
    KMime::Message *mail = new KMime::Message();
    mail->setContent(KMime::CRLFtoLF(data));
    mail->parse();

    item.setPayload(KMime::Message::Ptr(mail));
    Akonadi::MessageFlags::copyMessageFlags(*mail, item);

    ItemModifyJob *mjob = new ItemModifyJob(item);
    connect(mjob, &ItemModifyJob::result, this, &MaildirResource::fsWatchFileModifyResult);
}

void MaildirResource::fsWatchFileModifyResult(KJob *job)
{
    if (job->error()) {
        qCDebug(MAILDIRRESOURCE_LOG) << " MaildirResource::fsWatchFileModifyResult error: " << job->errorString();
        return;
    }
}

QString MaildirResource::maildirPathForCollection(const Collection &collection) const
{
    QString path = collection.remoteId();
    Akonadi::Collection parent = collection.parentCollection();
    while (!parent.remoteId().isEmpty()) {
        path.prepend(parent.remoteId() + QLatin1Char('/'));
        parent = parent.parentCollection();
    }

    return path;
}

void MaildirResource::stopMaildirScan(const Maildir &maildir)
{
    const QString path = maildir.path();
    mFsWatcher->removeDir(path + QLatin1Literal("/new"));
    mFsWatcher->removeDir(path + QLatin1Literal("/cur"));
}

void MaildirResource::restartMaildirScan(const Maildir &maildir)
{
    const QString path = maildir.path();
    mFsWatcher->addDir(path + QLatin1Literal("/new"));
    mFsWatcher->addDir(path + QLatin1Literal("/cur"));
}

void MaildirResource::changedCleaner()
{
    mChangedFiles.clear();
}
