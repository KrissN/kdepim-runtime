/*
    Copyright (C) 2015-2017 Krzysztof Nowicki <krissn@op.pl>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "ewsresource.h"

#include <QDebug>

#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/CollectionFetchScope>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/CollectionFetchJob>
#include <AkonadiCore/CollectionModifyJob>
#include <AkonadiCore/EntityDisplayAttribute>
#include <Akonadi/KMime/SpecialMailCollections>
#include <KMime/Message>
#include <KWallet/KWallet>

#include <KLocalizedString>

#include "ewsfetchitemsjob.h"
#include "ewsfetchfoldersjob.h"
#include "ewsfetchfoldersincrjob.h"
#include "ewsgetitemrequest.h"
#include "ewsupdateitemrequest.h"
#include "ewsmodifyitemflagsjob.h"
#include "ewsmoveitemrequest.h"
#include "ewsdeleteitemrequest.h"
#include "ewscreatefolderrequest.h"
#include "ewsmovefolderrequest.h"
#include "ewsupdatefolderrequest.h"
#include "ewsdeletefolderrequest.h"
#include "ewssubscriptionmanager.h"
#include "ewsgetfolderrequest.h"
#include "ewsitemhandler.h"
#include "ewsmodifyitemjob.h"
#include "ewscreateitemjob.h"
#include "ewsconfigdialog.h"
#include "ewssettings.h"
#ifdef HAVE_SEPARATE_MTA_RESOURCE
#include "ewscreateitemrequest.h"
#endif
#include "tags/ewstagstore.h"
#include "tags/ewsupdateitemstagsjob.h"
#include "tags/ewsglobaltagswritejob.h"
#include "tags/ewsglobaltagsreadjob.h"
#include "ewsresource_debug.h"

#include "ewsresourceadaptor.h"
#include "ewssettingsadaptor.h"
#include "ewswalletadaptor.h"

using namespace Akonadi;

struct SpecialFolders {
    EwsDistinguishedId did;
    SpecialMailCollections::Type type;
    QString iconName;
};

static const QVector<SpecialFolders> specialFolderList = {
    {EwsDIdInbox, SpecialMailCollections::Inbox, QStringLiteral("mail-folder-inbox")},
    {EwsDIdOutbox, SpecialMailCollections::Outbox, QStringLiteral("mail-folder-outbox")},
    {EwsDIdSentItems, SpecialMailCollections::SentMail, QStringLiteral("mail-folder-sent")},
    {EwsDIdDeletedItems, SpecialMailCollections::Trash, QStringLiteral("user-trash")},
    {EwsDIdDrafts, SpecialMailCollections::Drafts, QStringLiteral("document-properties")}
};

const QString EwsResource::akonadiEwsPropsetUuid = QStringLiteral("9bf757ae-69b5-4d8a-bf1d-2dd0c0871a28");

const EwsPropertyField EwsResource::globalTagsProperty(EwsResource::akonadiEwsPropsetUuid,
        QStringLiteral("GlobalTags"), EwsPropTypeStringArray);
const EwsPropertyField EwsResource::globalTagsVersionProperty(EwsResource::akonadiEwsPropsetUuid,
        QStringLiteral("GlobalTagsVersion"), EwsPropTypeInteger);
const EwsPropertyField EwsResource::tagsProperty(EwsResource::akonadiEwsPropsetUuid,
        QStringLiteral("Tags"), EwsPropTypeStringArray);
const EwsPropertyField EwsResource::flagsProperty(EwsResource::akonadiEwsPropsetUuid,
        QStringLiteral("Flags"), EwsPropTypeStringArray);

static Q_CONSTEXPR int InitialReconnectTimeout = 60;
static Q_CONSTEXPR int ReconnectTimeout = 300;

EwsResource::EwsResource(const QString &id)
    : Akonadi::ResourceBase(id), mTagsRetrieved(false), mReconnectTimeout(InitialReconnectTimeout),
      mSettings(new EwsSettings(winIdForDialogs()))
{
    mEwsClient.setUserAgent(mSettings->userAgent());
    mEwsClient.setEnableNTLMv2(mSettings->enableNTLMv2());

    changeRecorder()->fetchCollection(true);
    changeRecorder()->collectionFetchScope().setAncestorRetrieval(CollectionFetchScope::Parent);
    changeRecorder()->itemFetchScope().fetchFullPayload(true);
    changeRecorder()->itemFetchScope().setAncestorRetrieval(ItemFetchScope::Parent);
    changeRecorder()->itemFetchScope().setFetchModificationTime(false);
    changeRecorder()->itemFetchScope().setFetchTags(true);

    mRootCollection.setParentCollection(Collection::root());
    mRootCollection.setName(name());
    mRootCollection.setContentMimeTypes(QStringList() << Collection::mimeType() << KMime::Message::mimeType());
    mRootCollection.setRights(Collection::ReadOnly);

    setScheduleAttributeSyncBeforeItemSync(true);

    // Load the sync state
    QByteArray data = QByteArray::fromBase64(mSettings->syncState().toAscii());
    if (!data.isEmpty()) {
        data = qUncompress(data);
        if (!data.isEmpty()) {
            QDataStream stream(data);
            stream >> mSyncState;
        }
    }
    data = QByteArray::fromBase64(mSettings->folderSyncState().toAscii());
    if (!data.isEmpty()) {
        data = qUncompress(data);
        if (!data.isEmpty()) {
            mFolderSyncState = QString::fromAscii(data);
        }
    }

    setHierarchicalRemoteIdentifiersEnabled(true);

    mTagStore = new EwsTagStore(this);

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    QMetaObject::invokeMethod(this, &EwsResource::delayedInit, Qt::QueuedConnection);
#else
    QMetaObject::invokeMethod(this, "delayedInit", Qt::QueuedConnection);
#endif

    connect(this, &AgentBase::reloadConfiguration, this, &EwsResource::reloadConfig);

    connect(mSettings.data(), &EwsSettings::passwordRequestFinished, this,
            &EwsResource::passwordRequestFinished);
}

EwsResource::~EwsResource()
{
}

void EwsResource::delayedInit()
{
    new EwsResourceAdaptor(this);
    new EwsSettingsAdaptor(mSettings.data());
    new EwsWalletAdaptor(mSettings.data());
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Settings"),
            mSettings.data(), QDBusConnection::ExportAdaptors);
}

void EwsResource::resetUrl()
{
    Q_EMIT status(Running, i18nc("@info:status", "Connecting to Exchange server"));

    EwsGetFolderRequest *req = new EwsGetFolderRequest(mEwsClient, this);
    const EwsId::List folders {EwsId(EwsDIdMsgFolderRoot), EwsId(EwsDIdInbox)};
    req->setFolderIds(folders);
    EwsFolderShape shape(EwsShapeIdOnly);
    shape << EwsPropertyField(QStringLiteral("folder:DisplayName"));
    // Use the opportunity of reading the root folder to read the tag data.
    shape << globalTagsProperty << globalTagsVersionProperty;
    req->setFolderShape(shape);
    connect(req, &EwsRequest::result, this, &EwsResource::rootFolderFetchFinished);
    req->start();
}

void EwsResource::rootFolderFetchFinished(KJob *job)
{
    EwsGetFolderRequest *req = qobject_cast<EwsGetFolderRequest*>(job);
    if (!req) {
        Q_EMIT status(Idle, i18nc("@info:status", "Unable to connect to Exchange server"));
        setTemporaryOffline(reconnectTimeout());
        qCWarning(EWSRES_LOG) << QStringLiteral("Invalid EwsGetFolderRequest job object");
        return;
    }

    if (req->error()) {
        Q_EMIT status(Idle, i18nc("@info:status", "Unable to connect to Exchange server"));
        setTemporaryOffline(reconnectTimeout());
        qWarning() << "ERROR" << req->errorString();
        return;
    }

    if (req->responses().size() != 2) {
        Q_EMIT status(Idle, i18nc("@info:status", "Unable to connect to Exchange server"));
        setTemporaryOffline(reconnectTimeout());
        qCWarning(EWSRES_LOG) << QStringLiteral("Invalid number of responses received");
        return;
    }

    EwsFolder folder = req->responses()[1].folder();
    EwsId id = folder[EwsFolderFieldFolderId].value<EwsId>();
    if (id.type() == EwsId::Real) {
        /* Since KDE PIM is heavily based on IMAP philosophy it would only consider for filtering
         * folders with the remote identifier set to "INBOX". While this is true for IMAP/POP3, Exchange
         * uses Base64-encoded strings with data private to the server. In order for mail filtering to work
         * the EWS resource has pretended that the inbox folder's remote name is "INBOX". Since KDE Applications
         * 17.12 this workaround is no longer needed, however in order to clean-up after old Akonadi EWS
         * installations the code below sets the correct Exchange id for the Inbox folder.
         *
         * At some day in the future this part of code can be removed too. */
        Collection c;
        c.setRemoteId(QStringLiteral("INBOX"));
        CollectionFetchJob *job = new CollectionFetchJob(c, CollectionFetchJob::Base, this);
        job->setFetchScope(changeRecorder()->collectionFetchScope());
        job->fetchScope().setResource(identifier());
        job->fetchScope().setListFilter(CollectionFetchScope::Sync);
        job->setProperty("inboxId", id.id());
        connect(job, &CollectionFetchJob::result, this, &EwsResource::adjustInboxRemoteIdFetchFinished);

        int inboxIdx = mSettings->serverSubscriptionList().indexOf(QStringLiteral("INBOX"));
        if (inboxIdx >= 0) {
            QStringList subList = mSettings->serverSubscriptionList();
            subList[inboxIdx] = id.id();
            mSettings->setServerSubscriptionList(subList);
        }
    }

    folder = req->responses().first().folder();
    id = folder[EwsFolderFieldFolderId].value<EwsId>();
    if (id.type() == EwsId::Real) {
        mRootCollection.setRemoteId(id.id());
        mRootCollection.setRemoteRevision(id.changeKey());
        qCDebug(EWSRES_LOG) << "Root folder is " << id;
        Q_EMIT status(Idle, i18nc("@info:status Resource is ready", "Ready"));

        if (mSettings->serverSubscription()) {
            mSubManager.reset(new EwsSubscriptionManager(mEwsClient, id, mSettings.data(), this));
            connect(mSubManager.data(), &EwsSubscriptionManager::foldersModified, this, &EwsResource::foldersModifiedEvent);
            connect(mSubManager.data(), &EwsSubscriptionManager::folderTreeModified, this, &EwsResource::folderTreeModifiedEvent);
            connect(mSubManager.data(), &EwsSubscriptionManager::fullSyncRequested, this, &EwsResource::fullSyncRequestedEvent);

            /* Use a queued connection here as the connectionError() method will actually destroy the subscription manager. If this
             * was done with a direct connection this would have ended up with destroying the caller object followed by a crash. */
            connect(mSubManager.data(), &EwsSubscriptionManager::connectionError, this, &EwsResource::connectionError,
                    Qt::QueuedConnection);
            mSubManager->start();
        }

        synchronizeCollectionTree();

        mTagStore->readTags(folder[globalTagsProperty].toStringList(), folder[globalTagsVersionProperty].toInt());
    }
}

void EwsResource::adjustInboxRemoteIdFetchFinished(KJob *job)
{
    if (!job->error()) {
        CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob*>(job);
        Q_ASSERT(fetchJob);
        if (!fetchJob->collections().isEmpty()) {
            Collection c = fetchJob->collections()[0];
            c.setRemoteId(fetchJob->property("inboxId").toString());
            CollectionModifyJob *modifyJob = new CollectionModifyJob(c, this);
            modifyJob->start();
        }
    }
}

void EwsResource::retrieveCollections()
{
    if (mRootCollection.remoteId().isNull()) {
        cancelTask(i18nc("@info:status", "Root folder id not known."));
        return;
    }

    Q_EMIT status(Running, i18nc("@info:status", "Retrieving collection tree"));

    if (!mFolderSyncState.isEmpty() && !mRootCollection.isValid()) {
        /* When doing an incremental sync the real Akonadi identifier of the root collection must
         * be known, because the retrieved list of changes needs to include all parent folders up
         * to the root. None of the child collections are required to be valid, but the root must
         * be, as it needs to be the anchor point.
         */
        CollectionFetchJob *fetchJob = new CollectionFetchJob(mRootCollection,
                CollectionFetchJob::Base);
        connect(fetchJob, &CollectionFetchJob::result, this, &EwsResource::rootCollectionFetched);
        fetchJob->start();
    } else {
        doRetrieveCollections();
    }
    synchronizeTags();
}

void EwsResource::rootCollectionFetched(KJob *job)
{
    if (job->error()) {
        qCWarning(EWSRES_LOG) << "ERROR" << job->errorString();
    } else {
        CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob*>(job);
        if (fetchJob && !fetchJob->collections().isEmpty()) {
            mRootCollection = fetchJob->collections().first();
            qCDebugNC(EWSRES_LOG) << QStringLiteral("Root collection fetched: ") << mRootCollection;
        }
    }

    /* If the fetch failed for whatever reason force a full sync, which doesn't require the root
     * collection to be valid. */
    if (!mRootCollection.isValid()) {
        mFolderSyncState.clear();
    }

    doRetrieveCollections();
}

void EwsResource::doRetrieveCollections()
{
    if (mFolderSyncState.isEmpty()) {
        EwsFetchFoldersJob *job = new EwsFetchFoldersJob(mEwsClient, mRootCollection, this);
        connect(job, &EwsFetchFoldersJob::result, this, &EwsResource::fetchFoldersJobFinished);
        job->start();
    } else {
        EwsFetchFoldersIncrJob *job = new EwsFetchFoldersIncrJob(mEwsClient, mFolderSyncState,
                mRootCollection, this);
        connect(job, &EwsFetchFoldersIncrJob::result, this, &EwsResource::fetchFoldersIncrJobFinished);
        job->start();
    }
}

void EwsResource::connectionError()
{
    Q_EMIT status(Broken, i18nc("@info:status", "Unable to connect to Exchange server"));
    setTemporaryOffline(reconnectTimeout());
}

void EwsResource::retrieveItems(const Collection &collection)
{
    Q_EMIT status(1, QStringLiteral("Retrieving item list"));

    Q_EMIT status(Running, i18nc("@info:status", "Retrieving %1 items", collection.name()));

    QString rid = collection.remoteId();
    EwsFetchItemsJob *job = new EwsFetchItemsJob(collection, mEwsClient,
            mSyncState.value(rid), mItemsToCheck.value(rid), mTagStore, this);
    job->setQueuedUpdates(mQueuedUpdates.value(collection.remoteId()));
    mQueuedUpdates.remove(collection.remoteId());
    connect(job, &EwsFetchItemsJob::result, this, &EwsResource::itemFetchJobFinished);
    connect(job, &EwsFetchItemsJob::status, this, [this](int s, const QString &message) {
        Q_EMIT status(s, message);
    });
    connect(job, &EwsFetchItemsJob::percent, this, [this](int p) {
        Q_EMIT percent(p);
    });
    job->start();
}

bool EwsResource::retrieveItems(const Item::List &items, const QSet<QByteArray> &parts)
{
    qCDebugNC(EWSRES_AGENTIF_LOG) << "retrieveItems: start " << items << parts;

    EwsGetItemRequest *req = new EwsGetItemRequest(mEwsClient, this);
    EwsId::List ids;
    ids.reserve(items.count());
    for (const Item &item : items) {
        ids << EwsId(item.remoteId(), item.remoteRevision());
    }
    req->setItemIds(ids);
    EwsItemShape shape(EwsShapeIdOnly);
    shape << EwsPropertyField(QStringLiteral("item:MimeContent"));
    req->setItemShape(shape);
    req->setProperty("items", QVariant::fromValue<Item::List>(items));
    connect(req, &EwsGetItemRequest::result, this, &EwsResource::getItemsRequestFinished);
    req->start();

    return true;
}

void EwsResource::getItemsRequestFinished(KJob *job)
{
    if (job->error()) {
        qWarning() << "ERROR" << job->errorString();
        cancelTask(i18nc("@info:status", "Failed to process items retrieval request"));
        return;
    }
    EwsGetItemRequest *req = qobject_cast<EwsGetItemRequest*>(job);
    if (!req) {
        qCWarning(EWSRES_LOG) << QStringLiteral("Invalid EwsGetItemRequest job object");
        cancelTask(i18nc("@info:status", "Failed to retrieve items - internal error"));
        return;
    }

    const Item::List items = req->property("items").value<Item::List>();

    QHash<QString, Item> itemHash;
    itemHash.reserve(items.count());
    for (const Item &item : items) {
        itemHash.insert(item.remoteId(), item);
    }

    const EwsGetItemRequest::Response &resp = req->responses()[0];
    if (!resp.isSuccess()) {
        qCWarningNC(EWSRES_AGENTIF_LOG) << QStringLiteral("retrieveItems: Item fetch failed.");
        cancelTask(i18nc("@info:status", "Failed to retrieve items"));
        return;
    }

    if (items.size() != req->responses().size()) {
        qCWarningNC(EWSRES_AGENTIF_LOG) << QStringLiteral("retrieveItems: incorrect number of responses.");
        cancelTask(i18nc("@info:status", "Failed to retrieve items - incorrect number of responses"));
        return;

    }

    Q_FOREACH (const EwsGetItemRequest::Response &resp, req->responses()) {
        const EwsItem &ewsItem = resp.item();
        EwsId id = ewsItem[EwsItemFieldItemId].value<EwsId>();
        auto it = itemHash.find(id.id());
        if (it == itemHash.end()) {
            qCWarningNC(EWSRES_AGENTIF_LOG) << QStringLiteral("retrieveItems: Akonadi item not found for item %1.").arg(id.id());
            cancelTask(i18nc("@info:status", "Failed to retrieve items - Akonadi item not found for item %1", id.id()));
            return;
        }
        EwsItemType type = ewsItem.internalType();
        if (type == EwsItemTypeUnknown) {
            qCWarningNC(EWSRES_AGENTIF_LOG) << QStringLiteral("retrieveItems: Unknown item type for item %1!").arg(id.id());
            cancelTask(i18nc("@info:status", "Failed to retrieve items - Unknown item type for item %1", id.id()));
            return;
        }
        if (!EwsItemHandler::itemHandler(type)->setItemPayload(*it, ewsItem)) {
            qCWarningNC(EWSRES_AGENTIF_LOG) << "retrieveItems: Failed to fetch item payload";
            cancelTask(i18nc("@info:status", "Failed to fetch item payload"));
            return;
        }
    }

    qCDebugNC(EWSRES_AGENTIF_LOG) << "retrieveItems: done";
    itemsRetrieved(itemHash.values().toVector());
}

void EwsResource::reloadConfig()
{
    mSubManager.reset(nullptr);
    mEwsClient.setUrl(mSettings->baseUrl());
    mSettings->requestPassword(true);
}

void EwsResource::passwordRequestFinished(const QString &password)
{
    mPassword = password;
    if (mPassword.isNull()) {
        setOnline(false);
        Q_EMIT status(NotConfigured, i18nc("@info:status", "No password configured."));
    } else {
        if (mSettings->domain().isEmpty()) {
            mEwsClient.setCredentials(mSettings->username(), mPassword);
        } else {
            mEwsClient.setCredentials(mSettings->domain() + QLatin1Char('\\') + mSettings->username(), mPassword);
        }
        mSettings->save();
        if (mSettings->baseUrl().isEmpty()) {
            setOnline(false);
            Q_EMIT status(NotConfigured, i18nc("@info:status", "No server configured yet."));
        } else {
            resetUrl();
        }
    }
}

void EwsResource::configure(WId windowId)
{
    QPointer<EwsConfigDialog> dlg = new EwsConfigDialog(this, mEwsClient, windowId);
    if (dlg->exec()) {
        reloadConfig();
        Q_EMIT configurationDialogAccepted();
    } else {
        Q_EMIT configurationDialogRejected();
    }
    delete dlg;
}

void EwsResource::fetchFoldersJobFinished(KJob *job)
{
    Q_EMIT status(Idle, i18nc("@info:status The resource is ready", "Ready"));
    EwsFetchFoldersJob *req = qobject_cast<EwsFetchFoldersJob*>(job);
    if (!req) {
        qCWarning(EWSRES_LOG) << QStringLiteral("Invalid EwsFetchFoldersJob job object");
        cancelTask(i18nc("@info:status", "Failed to retrieve folders - internal error"));
        return;
    }

    if (req->error()) {
        qWarning() << "ERROR" << req->errorString();
        cancelTask(i18nc("@info:status", "Failed to process folders retrieval request"));
        return;
    }

    mFolderSyncState = req->syncState();
    saveState();
    collectionsRetrieved(req->folders());

    fetchSpecialFolders();
}

void EwsResource::fetchFoldersIncrJobFinished(KJob *job)
{
    Q_EMIT status(Idle, i18nc("@info:status The resource is ready", "Ready"));
    EwsFetchFoldersIncrJob *req = qobject_cast<EwsFetchFoldersIncrJob*>(job);
    if (!req) {
        qCWarning(EWSRES_LOG) << QStringLiteral("Invalid EwsFetchFoldersIncrJob job object");
        cancelTask(i18nc("@info:status", "Invalid incremental folders retrieval request job object"));
        return;
    }

    if (req->error()) {
        qCWarningNC(EWSRES_LOG) << QStringLiteral("ERROR") << req->errorString();

        /* Retry with a full sync. */
        qCWarningNC(EWSRES_LOG) << QStringLiteral("Retrying with a full sync.");
        mFolderSyncState.clear();
        doRetrieveCollections();
        return;
    }

    mFolderSyncState = req->syncState();
    saveState();
    collectionsRetrievedIncremental(req->changedFolders(), req->deletedFolders());

    fetchSpecialFolders();
}

void EwsResource::itemFetchJobFinished(KJob *job)
{
    EwsFetchItemsJob *fetchJob = qobject_cast<EwsFetchItemsJob*>(job);

    if (!fetchJob) {
        qCWarningNC(EWSRES_LOG) << QStringLiteral("Invalid EwsFetchItemsJobjob object");
        cancelTask(i18nc("@info:status", "Failed to retrieve items - internal error"));
        return;
    }
    if (job->error()) {
        qCWarningNC(EWSRES_LOG) << QStringLiteral("Item fetch error:") << job->errorString();
        if (mSyncState.contains(fetchJob->collection().remoteId())) {
            qCDebugNC(EWSRES_LOG) << QStringLiteral("Retrying with empty state.");
            // Retry with a clear sync state.
            mSyncState.remove(fetchJob->collection().remoteId());
            retrieveItems(fetchJob->collection());
        } else {
            qCDebugNC(EWSRES_LOG) << QStringLiteral("Clean sync failed.");
            // No more hope
            cancelTask(i18nc("@info:status", "Failed to retrieve items"));
            return;
        }
    } else {
        mSyncState[fetchJob->collection().remoteId()] = fetchJob->syncState();
        itemsRetrievedIncremental(fetchJob->changedItems(), fetchJob->deletedItems());
    }
    saveState();
    mItemsToCheck.remove(fetchJob->collection().remoteId());
    Q_EMIT status(Idle, i18nc("@info:status The resource is ready", "Ready"));
}

void EwsResource::itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &partIdentifiers)
{
    qCDebugNC(EWSRES_AGENTIF_LOG) << "itemChanged: start " << item << partIdentifiers;

    EwsItemType type = EwsItemHandler::mimeToItemType(item.mimeType());
    if (isEwsMessageItemType(type)) {
        qCWarningNC(EWSRES_AGENTIF_LOG) << "itemChanged: Item type not supported for changing";
        cancelTask(i18nc("@info:status", "Item type not supported for changing"));
    } else {
        EwsModifyItemJob *job = EwsItemHandler::itemHandler(type)->modifyItemJob(mEwsClient,
                                Item::List() << item, partIdentifiers, this);
        connect(job, &KJob::result, this, &EwsResource::itemChangeRequestFinished);
        job->start();
    }
}

void EwsResource::itemsFlagsChanged(const Akonadi::Item::List &items, const QSet<QByteArray> &addedFlags,
                                    const QSet<QByteArray> &removedFlags)
{
    qCDebug(EWSRES_AGENTIF_LOG) << "itemsFlagsChanged: start" << items << addedFlags << removedFlags;

    EwsModifyItemFlagsJob *job = new EwsModifyItemFlagsJob(mEwsClient, this, items, addedFlags, removedFlags);
    connect(job, &EwsModifyItemFlagsJob::result, this, &EwsResource::itemModifyFlagsRequestFinished);
    job->start();
}

void EwsResource::itemModifyFlagsRequestFinished(KJob *job)
{
    if (job->error()) {
        qCWarning(EWSRES_AGENTIF_LOG) << "itemsFlagsChanged:" << job->errorString();
        cancelTask(i18nc("@info:status", "Failed to process item flags update request"));
        return;
    }

    EwsModifyItemFlagsJob *req = qobject_cast<EwsModifyItemFlagsJob*>(job);
    if (!req) {
        qCWarning(EWSRES_AGENTIF_LOG) << "itemsFlagsChanged: Invalid EwsModifyItemFlagsJob job object";
        cancelTask(i18nc("@info:status", "Failed to update item flags - internal error"));
        return;
    }

    qCDebug(EWSRES_AGENTIF_LOG) << "itemsFlagsChanged: done";
    changesCommitted(req->items());
}

void EwsResource::itemChangeRequestFinished(KJob *job)
{
    if (job->error()) {
        qCWarningNC(EWSRES_AGENTIF_LOG) << "itemChanged: " << job->errorString();
        cancelTask(i18nc("@info:status", "Failed to process item update request"));
        return;
    }

    EwsModifyItemJob *req = qobject_cast<EwsModifyItemJob*>(job);
    if (!req) {
        qCWarningNC(EWSRES_AGENTIF_LOG) << "itemChanged: Invalid EwsModifyItemJob job object";
        cancelTask(i18nc("@info:status", "Failed to update item - internal error"));
        return;
    }

    qCDebugNC(EWSRES_AGENTIF_LOG) << "itemChanged: done";
    changesCommitted(req->items());
}

void EwsResource::itemsMoved(const Item::List &items, const Collection &sourceCollection,
                             const Collection &destinationCollection)
{
    qCDebug(EWSRES_AGENTIF_LOG) << "itemsMoved: start" << items << sourceCollection << destinationCollection;

    EwsId::List ids;

    ids.reserve(items.count());
    for (const Item &item : items) {
        EwsId id(item.remoteId(), item.remoteRevision());
        ids.append(id);
    }

    EwsMoveItemRequest *req = new EwsMoveItemRequest(mEwsClient, this);
    req->setItemIds(ids);
    EwsId destId(destinationCollection.remoteId(), QString());
    req->setDestinationFolderId(destId);
    req->setProperty("items", QVariant::fromValue<Item::List>(items));
    req->setProperty("sourceCollection", QVariant::fromValue<Collection>(sourceCollection));
    req->setProperty("destinationCollection", QVariant::fromValue<Collection>(destinationCollection));
    connect(req, &KJob::result, this, &EwsResource::itemMoveRequestFinished);
    req->start();
}

void EwsResource::itemMoveRequestFinished(KJob *job)
{
    if (job->error()) {
        qCWarningNC(EWSRES_AGENTIF_LOG) << "itemsMoved: " << job->errorString();
        cancelTask(i18nc("@info:status", "Failed to process item move request"));
        return;
    }

    EwsMoveItemRequest *req = qobject_cast<EwsMoveItemRequest*>(job);
    if (!req) {
        qCWarningNC(EWSRES_AGENTIF_LOG) << "itemsMoved: Invalid EwsMoveItemRequest job object";
        cancelTask(i18nc("@info:status", "Failed to move item - internal error"));
        return;
    }
    Item::List items = job->property("items").value<Item::List>();

    if (items.count() != req->responses().count()) {
        qCWarningNC(EWSRES_AGENTIF_LOG) << "itemsMoved: Invalid number of responses received from server";
        cancelTask(i18nc("@info:status", "Failed to move item - invalid number of responses received from server"));
        return;
    }

    /* When moving a batch of items it is possible that the operation will fail for some of them.
     * Unfortunately Akonadi doesn't provide a way to report such partial success/failure. In order
     * to work around this in case of partial failure the source and destination folders will be
     * resynchronised. In order to avoid doing a full sync a hint will be provided in order to
     * indicate the item(s) to check.
     */

    Item::List movedItems;
    EwsId::List failedIds;

    Collection srcCol = req->property("sourceCollection").value<Collection>();
    Collection dstCol = req->property("destinationCollection").value<Collection>();
    Item::List::iterator it = items.begin();
    Q_FOREACH (const EwsMoveItemRequest::Response &resp, req->responses()) {
        Item &item = *it;
        if (resp.isSuccess()) {
            qCDebugNC(EWSRES_AGENTIF_LOG) << QStringLiteral("itemsMoved: succeeded for item %1 (new id: %2)")
                                          .arg(ewsHash(item.remoteId())).arg(ewsHash(resp.itemId().id()));
            if (item.isValid()) {
                /* Log item deletion in the source folder so that the next sync doesn't trip over
                 * non-existent items. Use old remote ids for that. */
                if (mSubManager) {
                    mSubManager->queueUpdate(EwsDeletedEvent, item.remoteId(), QString());
                }
                mQueuedUpdates[srcCol.remoteId()].append({item.remoteId(), QString(), EwsDeletedEvent});

                item.setRemoteId(resp.itemId().id());
                item.setRemoteRevision(resp.itemId().changeKey());
                movedItems.append(item);
            }
        } else {
            Q_EMIT warning(QStringLiteral("Move failed for item %1").arg(item.remoteId()));
            qCDebugNC(EWSRES_AGENTIF_LOG) << QStringLiteral("itemsMoved: failed for item %1").arg(ewsHash(item.remoteId()));
            failedIds.append(EwsId(item.remoteId(), QString()));
        }
        ++it;
    }

    if (!failedIds.isEmpty()) {
        qCWarningNC(EWSRES_LOG) << QStringLiteral("Failed to move %1 items. Forcing src & dst folder sync.")
                                .arg(failedIds.size());
        mItemsToCheck[srcCol.remoteId()] += failedIds;
        foldersModifiedEvent(EwsId::List({EwsId(srcCol.remoteId(), QString())}));
        mItemsToCheck[dstCol.remoteId()] += failedIds;
        foldersModifiedEvent(EwsId::List({EwsId(dstCol.remoteId(), QString())}));
    }

    qCDebugNC(EWSRES_AGENTIF_LOG) << "itemsMoved: done";
    changesCommitted(movedItems);
}

void EwsResource::itemsRemoved(const Item::List &items)
{
    qCDebugNC(EWSRES_AGENTIF_LOG) << "itemsRemoved: start" << items;

    EwsId::List ids;
    ids.reserve(items.count());
    for (const Item &item : items) {
        EwsId id(item.remoteId(), item.remoteRevision());
        ids.append(id);
    }

    EwsDeleteItemRequest *req = new EwsDeleteItemRequest(mEwsClient, this);
    req->setItemIds(ids);
    req->setProperty("items", QVariant::fromValue<Item::List>(items));
    connect(req, &EwsDeleteItemRequest::result, this, &EwsResource::itemDeleteRequestFinished);
    req->start();

}

void EwsResource::itemDeleteRequestFinished(KJob *job)
{
    if (job->error()) {
        qCWarningNC(EWSRES_AGENTIF_LOG) << "itemsRemoved: " << job->errorString();
        cancelTask(i18nc("@info:status", "Failed to process item delete request"));
        return;
    }

    EwsDeleteItemRequest *req = qobject_cast<EwsDeleteItemRequest*>(job);
    if (!req) {
        qCWarningNC(EWSRES_AGENTIF_LOG) << "itemsRemoved: Invalid EwsDeleteItemRequest job object";
        cancelTask(i18nc("@info:status", "Failed to delete item - internal error"));
        return;
    }
    Item::List items = job->property("items").value<Item::List>();

    if (items.count() != req->responses().count()) {
        qCWarningNC(EWSRES_AGENTIF_LOG) << "itemsRemoved: Invalid number of responses received from server";
        cancelTask(i18nc("@info:status", "Failed to delete item - invalid number of responses received from server"));
        return;
    }

    /* When removing a batch of items it is possible that the operation will fail for some of them.
     * Unfortunately Akonadi doesn't provide a way to report such partial success/failure. In order
     * to work around this in case of partial failure the original folder(s) will be resynchronised.
     * In order to avoid doing a full sync a hint will be provided in order to indicate the item(s)
     * to check.
     */

    EwsId::List foldersToSync;

    Item::List::iterator it = items.begin();
    Q_FOREACH (const EwsDeleteItemRequest::Response &resp, req->responses()) {
        Item &item = *it;
        if (resp.isSuccess()) {
            qCDebugNC(EWSRES_AGENTIF_LOG) << QStringLiteral("itemsRemoved: succeeded for item %1").arg(ewsHash(item.remoteId()));
            if (mSubManager) {
                mSubManager->queueUpdate(EwsDeletedEvent, item.remoteId(), QString());
            }
            mQueuedUpdates[item.parentCollection().remoteId()].append({item.remoteId(), QString(), EwsDeletedEvent});
        } else {
            Q_EMIT warning(QStringLiteral("Delete failed for item %1").arg(item.remoteId()));
            qCWarningNC(EWSRES_AGENTIF_LOG) << QStringLiteral("itemsRemoved: failed for item %1").arg(ewsHash(item.remoteId()));
            EwsId colId = EwsId(item.parentCollection().remoteId(), QString());
            mItemsToCheck[colId.id()].append(EwsId(item.remoteId(), QString()));
            if (!foldersToSync.contains(colId)) {
                foldersToSync.append(colId);
            }
        }
        ++it;
    }

    if (!foldersToSync.isEmpty()) {
        qCWarningNC(EWSRES_LOG) << QStringLiteral("Need to force sync for %1 folders.")
                                .arg(foldersToSync.size());
        foldersModifiedEvent(foldersToSync);
    }

    qCDebug(EWSRES_AGENTIF_LOG) << "itemsRemoved: done";
    changeProcessed();
}

void EwsResource::itemAdded(const Item &item, const Collection &collection)
{
    EwsItemType type = EwsItemHandler::mimeToItemType(item.mimeType());
    if (isEwsMessageItemType(type)) {
        cancelTask(i18nc("@info:status", "Item type not supported for creation"));
    } else {
        EwsCreateItemJob *job = EwsItemHandler::itemHandler(type)->createItemJob(mEwsClient, item,
                                collection, mTagStore, this);
        connect(job, &EwsCreateItemJob::result, this, &EwsResource::itemCreateRequestFinished);
        job->start();
    }
}

void EwsResource::itemCreateRequestFinished(KJob *job)
{
    if (job->error()) {
        cancelTask(i18nc("@info:status", "Failed to process item create request"));
        return;
    }

    EwsCreateItemJob *req = qobject_cast<EwsCreateItemJob*>(job);
    if (!req) {
        cancelTask(i18nc("@info:status", "Failed to create item - internal error"));
        return;
    }

    changeCommitted(req->item());
}

void EwsResource::collectionAdded(const Collection &collection, const Collection &parent)
{
    EwsFolderType type;
    QStringList mimeTypes = collection.contentMimeTypes();
    if (mimeTypes.contains(EwsItemHandler::itemHandler(EwsItemTypeCalendarItem)->mimeType())) {
        type = EwsFolderTypeCalendar;
    } else if (mimeTypes.contains(EwsItemHandler::itemHandler(EwsItemTypeContact)->mimeType())) {
        type = EwsFolderTypeContacts;
    } else if (mimeTypes.contains(EwsItemHandler::itemHandler(EwsItemTypeTask)->mimeType())) {
        type = EwsFolderTypeTasks;
    } else if (mimeTypes.contains(EwsItemHandler::itemHandler(EwsItemTypeMessage)->mimeType())) {
        type = EwsFolderTypeMail;
    } else {
        qCWarningNC(EWSRES_LOG) << QStringLiteral("Cannot determine EWS folder type.");
        cancelTask(i18nc("@info:status", "Failed to add collection - cannot determine EWS folder type"));
        return;
    }

    EwsFolder folder;
    folder.setType(type);
    folder.setField(EwsFolderFieldDisplayName, collection.name());

    EwsCreateFolderRequest *req = new EwsCreateFolderRequest(mEwsClient, this);
    req->setParentFolderId(EwsId(parent.remoteId()));
    req->setFolders(EwsFolder::List() << folder);
    req->setProperty("collection", QVariant::fromValue<Collection>(collection));
    connect(req, &EwsCreateFolderRequest::result, this, &EwsResource::folderCreateRequestFinished);
    req->start();
}

void EwsResource::folderCreateRequestFinished(KJob *job)
{
    if (job->error()) {
        cancelTask(i18nc("@info:status", "Failed to process folder create request"));
        return;
    }

    EwsCreateFolderRequest *req = qobject_cast<EwsCreateFolderRequest*>(job);
    if (!req) {
        cancelTask(i18nc("@info:status", "Failed to create folder - internal error"));
        return;
    }
    Collection col = job->property("collection").value<Collection>();

    EwsCreateFolderRequest::Response resp = req->responses().first();
    if (resp.isSuccess()) {
        const EwsId &id = resp.folderId();
        col.setRemoteId(id.id());
        col.setRemoteRevision(id.changeKey());
        changeCommitted(col);
    } else {
        cancelTask(i18nc("@info:status", "Failed to create folder"));
    }
}

void EwsResource::collectionMoved(const Collection &collection, const Collection &collectionSource,
                                  const Collection &collectionDestination)
{
    Q_UNUSED(collectionSource)

    EwsId::List ids;
    ids.append(EwsId(collection.remoteId(), collection.remoteRevision()));

    EwsMoveFolderRequest *req = new EwsMoveFolderRequest(mEwsClient, this);
    req->setFolderIds(ids);
    EwsId destId(collectionDestination.remoteId());
    req->setDestinationFolderId(destId);
    req->setProperty("collection", QVariant::fromValue<Collection>(collection));
    connect(req, &EwsMoveFolderRequest::result, this, &EwsResource::folderMoveRequestFinished);
    req->start();
}

void EwsResource::folderMoveRequestFinished(KJob *job)
{
    if (job->error()) {
        cancelTask(i18nc("@info:status", "Failed to process folder move request"));
        return;
    }

    EwsMoveFolderRequest *req = qobject_cast<EwsMoveFolderRequest*>(job);
    if (!req) {
        cancelTask(i18nc("@info:status", "Failed to move folder - internal error"));
        return;
    }
    Collection col = job->property("collection").value<Collection>();

    if (req->responses().count() != 1) {
        cancelTask(i18nc("@info:status", "Failed to move folder - invalid number of responses received from server"));
        return;
    }

    EwsMoveFolderRequest::Response resp = req->responses().first();
    if (resp.isSuccess()) {
        const EwsId &id = resp.folderId();
        col.setRemoteId(id.id());
        col.setRemoteRevision(id.changeKey());
        changeCommitted(col);
    } else {
        cancelTask(i18nc("@info:status", "Failed to move folder"));
    }
}

void EwsResource::collectionChanged(const Collection &collection,
                                    const QSet<QByteArray> &changedAttributes)
{
    if (changedAttributes.contains("NAME")) {
        EwsUpdateFolderRequest *req = new EwsUpdateFolderRequest(mEwsClient, this);
        EwsUpdateFolderRequest::FolderChange fc(EwsId(collection.remoteId(), collection.remoteRevision()),
                                                EwsFolderTypeMail);
        EwsUpdateFolderRequest::Update *upd
            = new EwsUpdateFolderRequest::SetUpdate(EwsPropertyField(QStringLiteral("folder:DisplayName")),
                    collection.name());
        fc.addUpdate(upd);
        req->addFolderChange(fc);
        req->setProperty("collection", QVariant::fromValue<Collection>(collection));
        connect(req, &EwsUpdateFolderRequest::finished, this, &EwsResource::folderUpdateRequestFinished);
        req->start();
    } else {
        changeCommitted(collection);
    }
}

void EwsResource::collectionChanged(const Akonadi::Collection &collection)
{
    Q_UNUSED(collection)
}

void EwsResource::folderUpdateRequestFinished(KJob *job)
{
    if (job->error()) {
        cancelTask(i18nc("@info:status", "Failed to process folder update request"));
        return;
    }

    EwsUpdateFolderRequest *req = qobject_cast<EwsUpdateFolderRequest*>(job);
    if (!req) {
        cancelTask(i18nc("@info:status", "Failed to update folder - internal error"));
        return;
    }
    Collection col = job->property("collection").value<Collection>();

    if (req->responses().count() != 1) {
        cancelTask(i18nc("@info:status", "Failed to update folder - invalid number of responses received from server"));
        return;
    }

    EwsUpdateFolderRequest::Response resp = req->responses().first();
    if (resp.isSuccess()) {
        const EwsId &id = resp.folderId();
        col.setRemoteId(id.id());
        col.setRemoteRevision(id.changeKey());
        changeCommitted(col);
    } else {
        cancelTask(i18nc("@info:status", "Failed to update folder"));
    }
}

void EwsResource::collectionRemoved(const Collection &collection)
{
    EwsDeleteFolderRequest *req = new EwsDeleteFolderRequest(mEwsClient, this);
    EwsId::List ids;
    ids.append(EwsId(collection.remoteId(), collection.remoteRevision()));
    req->setFolderIds(ids);
    connect(req, &EwsDeleteFolderRequest::result, this, &EwsResource::folderDeleteRequestFinished);
    req->start();

}

void EwsResource::folderDeleteRequestFinished(KJob *job)
{
    if (job->error()) {
        cancelTask(i18nc("@info:status", "Failed to process folder delete request"));
        return;
    }

    EwsDeleteFolderRequest *req = qobject_cast<EwsDeleteFolderRequest*>(job);
    if (!req) {
        cancelTask(i18nc("@info:status", "Failed to delete folder - internal error"));
        return;
    }

    EwsDeleteFolderRequest::Response resp = req->responses().first();
    if (resp.isSuccess()) {
        changeProcessed();
    } else {
        cancelTask(i18nc("@info:status", "Failed to delete folder"));
        mFolderSyncState.clear();
        synchronizeCollectionTree();
    }
}

void EwsResource::sendItem(const Akonadi::Item &item)
{
    EwsItemType type = EwsItemHandler::mimeToItemType(item.mimeType());
    if (isEwsMessageItemType(type)) {
        itemSent(item, TransportFailed, i18nc("@info:status", "Item type not supported for creation"));
    } else {
        EwsCreateItemJob *job = EwsItemHandler::itemHandler(type)->createItemJob(mEwsClient, item,
                                Collection(), mTagStore, this);
        job->setSend(true);
        job->setProperty("item", QVariant::fromValue<Item>(item));
        connect(job, &EwsCreateItemJob::result, this, &EwsResource::itemSendRequestFinished);
        job->start();
    }
}

void EwsResource::itemSendRequestFinished(KJob *job)
{
    Item item = job->property("item").value<Item>();
    if (job->error()) {
        itemSent(item, TransportFailed, i18nc("@info:status", "Failed to process item send request"));
        return;
    }

    EwsCreateItemJob *req = qobject_cast<EwsCreateItemJob*>(job);
    if (!req) {
        itemSent(item, TransportFailed, i18nc("@info:status", "Failed to send item - internal error"));
        return;
    }

    itemSent(item, TransportSucceeded);
}

void EwsResource::sendMessage(const QString &id, const QByteArray &content)
{
#ifdef HAVE_SEPARATE_MTA_RESOURCE
    EwsCreateItemRequest *req = new EwsCreateItemRequest(mEwsClient, this);

    EwsItem item;
    item.setType(EwsItemTypeMessage);
    item.setField(EwsItemFieldMimeContent, content);
    req->setItems(EwsItem::List() << item);
    req->setMessageDisposition(EwsDispSendOnly);
    req->setProperty("requestId", id);
    connect(req, &EwsCreateItemRequest::finished, this, &EwsResource::messageSendRequestFinished);
    req->start();
#endif
}

#ifdef HAVE_SEPARATE_MTA_RESOURCE
void EwsResource::messageSendRequestFinished(KJob *job)
{
    QString id = job->property("requestId").toString();
    if (job->error()) {
        Q_EMIT messageSent(id, i18nc("@info:status", "Failed to process item send request"));
        return;
    }

    EwsCreateItemRequest *req = qobject_cast<EwsCreateItemRequest*>(job);
    if (!req) {
        Q_EMIT messageSent(id, i18nc("@info:status", "Failed to send item - internal error"));
        return;
    }

    if (req->responses().count() != 1) {
        Q_EMIT messageSent(id, i18nc("@info:status", "Invalid number of responses received from server"));
        return;
    }

    EwsCreateItemRequest::Response resp = req->responses().first();
    if (resp.isSuccess()) {
        Q_EMIT messageSent(id, QString());
    } else {
        Q_EMIT messageSent(id, resp.responseMessage());
    }
}
#endif

void EwsResource::foldersModifiedEvent(const EwsId::List &folders)
{
    Q_FOREACH (const EwsId &id, folders) {
        Collection c;
        c.setRemoteId(id.id());
        CollectionFetchJob *job = new CollectionFetchJob(c, CollectionFetchJob::Base);
        job->setFetchScope(changeRecorder()->collectionFetchScope());
        job->fetchScope().setResource(identifier());
        job->fetchScope().setListFilter(CollectionFetchScope::Sync);
        connect(job, &KJob::result, this, &EwsResource::foldersModifiedCollectionSyncFinished);
    }

}

void EwsResource::foldersModifiedCollectionSyncFinished(KJob *job)
{
    if (job->error()) {
        qCDebug(EWSRES_LOG) << QStringLiteral("Failed to fetch collection tree for sync.");
        return;
    }

    CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob*>(job);
    synchronizeCollection(fetchJob->collections()[0].id());
}

void EwsResource::folderTreeModifiedEvent()
{
    synchronizeCollectionTree();
}

void EwsResource::fullSyncRequestedEvent()
{
    synchronize();
}

void EwsResource::clearFolderSyncState()
{
    mSyncState.clear();
    saveState();
}

void EwsResource::clearFolderSyncState(const QString &folderId)
{
    mSyncState.remove(folderId);
    saveState();
}

void EwsResource::clearFolderTreeSyncState()
{
    mFolderSyncState.clear();
    saveState();
}

void EwsResource::fetchSpecialFolders()
{
    CollectionFetchJob *job = new CollectionFetchJob(mRootCollection, CollectionFetchJob::Recursive, this);
    connect(job, &CollectionFetchJob::collectionsReceived, this, &EwsResource::specialFoldersCollectionsRetrieved);
    connect(job, &CollectionFetchJob::result, this, [this](KJob * job) {
        if (job->error()) {
            qCWarningNC(EWSRES_LOG) << "Special folders fetch failed:" << job->errorString();
        }
    });
    job->start();
}

void EwsResource::specialFoldersCollectionsRetrieved(const Collection::List &folders)
{
    EwsId::List queryItems;

    queryItems.reserve(specialFolderList.count());
    Q_FOREACH (const SpecialFolders &sf, specialFolderList) {
        queryItems.append(EwsId(sf.did));
    }

    if (!queryItems.isEmpty()) {
        EwsGetFolderRequest *req = new EwsGetFolderRequest(mEwsClient, this);
        req->setFolderShape(EwsFolderShape(EwsShapeIdOnly));
        req->setFolderIds(queryItems);
        req->setProperty("collections", QVariant::fromValue<Collection::List>(folders));
        connect(req, &EwsGetFolderRequest::finished, this, &EwsResource::specialFoldersFetchFinished);
        req->start();
    }
}

void EwsResource::specialFoldersFetchFinished(KJob *job)
{
    if (job->error()) {
        qCWarningNC(EWSRES_LOG) << QStringLiteral("Special collection fetch failed:") << job->errorString();
        return;
    }

    EwsGetFolderRequest *req = qobject_cast<EwsGetFolderRequest*>(job);
    if (!req) {
        qCWarningNC(EWSRES_LOG) << QStringLiteral("Special collection fetch failed:")
                                << QStringLiteral("Invalid EwsGetFolderRequest job object");
        return;
    }

    const Collection::List collections = req->property("collections").value<Collection::List>();

    if (req->responses().size() != specialFolderList.size()) {
        qCWarningNC(EWSRES_LOG) << QStringLiteral("Special collection fetch failed:")
                                << QStringLiteral("Invalid number of responses received");
        return;
    }

    QMap<QString, Collection> map;
    for (const Collection &col : collections) {
        map.insert(col.remoteId(), col);
    }

    auto it = specialFolderList.cbegin();
    Q_FOREACH (const EwsGetFolderRequest::Response &resp, req->responses()) {
        if (resp.isSuccess()) {
            EwsId fid = resp.folder()[EwsFolderFieldFolderId].value<EwsId>();
            QMap<QString, Collection>::iterator mapIt = map.find(fid.id());
            if (mapIt != map.end()) {
                qCDebugNC(EWSRES_LOG) << QStringLiteral("Registering folder %1(%2) as special collection %3")
                                      .arg(ewsHash(mapIt->remoteId())).arg(mapIt->id()).arg(it->type);
                SpecialMailCollections::self()->registerCollection(it->type, *mapIt);
                if (!mapIt->hasAttribute<EntityDisplayAttribute>()) {
                    EntityDisplayAttribute *attr = mapIt->attribute<EntityDisplayAttribute>(Collection::AddIfMissing);
                    attr->setIconName(it->iconName);
                    CollectionModifyJob *modJob = new CollectionModifyJob(*mapIt, this);
                    modJob->start();
                }
            }
        }
        it++;
    }
}

void EwsResource::saveState()
{
    QByteArray str;
    QDataStream dataStream(&str, QIODevice::WriteOnly);
    dataStream << mSyncState;
    mSettings->setSyncState(QString::fromLatin1(qCompress(str, 9).toBase64()));
    mSettings->setFolderSyncState(QString::fromLatin1(qCompress(mFolderSyncState.toAscii(), 9).toBase64()));
    mSettings->save();
}

void EwsResource::doSetOnline(bool online)
{
    if (online) {
        reloadConfig();
    } else {
        mSubManager.reset(nullptr);
    }
}

int EwsResource::reconnectTimeout()
{
    // Return InitialReconnectTimeout for the first time, then ReconnectTimeout.
    int timeout = mReconnectTimeout;
    mReconnectTimeout = ReconnectTimeout;
    return timeout;
}

void EwsResource::itemsTagsChanged(const Item::List &items, const QSet<Tag> &addedTags,
                                   const QSet<Tag> &removedTags)
{
    Q_UNUSED(addedTags)
    Q_UNUSED(removedTags)

    EwsUpdateItemsTagsJob *job = new EwsUpdateItemsTagsJob(items, mTagStore, mEwsClient, this);
    connect(job, &EwsUpdateItemsTagsJob::result, this, &EwsResource::itemsTagChangeFinished);
    job->start();
}

void EwsResource::itemsTagChangeFinished(KJob *job)
{
    if (job->error()) {
        cancelTask(i18nc("@info:status", "Failed to process item tags update request"));
        return;
    }

    EwsUpdateItemsTagsJob *updJob = qobject_cast<EwsUpdateItemsTagsJob*>(job);
    if (!updJob) {
        cancelTask(i18nc("@info:status", "Failed to update item tags - internal error"));
        return;
    }

    changesCommitted(updJob->items());
}

void EwsResource::tagAdded(const Tag &tag)
{
    mTagStore->addTag(tag);

    EwsGlobalTagsWriteJob *job = new EwsGlobalTagsWriteJob(mTagStore, mEwsClient, mRootCollection, this);
    connect(job, &EwsGlobalTagsWriteJob::result, this, &EwsResource::globalTagChangeFinished);
    job->start();
}

void EwsResource::tagChanged(const Tag &tag)
{
    mTagStore->addTag(tag);

    EwsGlobalTagsWriteJob *job = new EwsGlobalTagsWriteJob(mTagStore, mEwsClient, mRootCollection, this);
    connect(job, &EwsGlobalTagsWriteJob::result, this, &EwsResource::globalTagChangeFinished);
    job->start();
}


void EwsResource::tagRemoved(const Tag &tag)
{
    mTagStore->removeTag(tag);

    EwsGlobalTagsWriteJob *job = new EwsGlobalTagsWriteJob(mTagStore, mEwsClient, mRootCollection, this);
    connect(job, &EwsGlobalTagsWriteJob::result, this, &EwsResource::globalTagChangeFinished);
    job->start();
}

void EwsResource::globalTagChangeFinished(KJob *job)
{
    if (job->error()) {
        cancelTask(i18nc("@info:status", "Failed to process global tag update request"));
    } else {
        changeProcessed();
    }
}


void EwsResource::retrieveTags()
{
    EwsGlobalTagsReadJob *job = new EwsGlobalTagsReadJob(mTagStore, mEwsClient, mRootCollection, this);
    connect(job, &EwsGlobalTagsReadJob::result, this, &EwsResource::globalTagsRetrievalFinished);
    job->start();
}

void EwsResource::globalTagsRetrievalFinished(KJob *job)
{
    if (job->error()) {
        cancelTask(i18nc("@info:status", "Failed to process global tags retrieval request"));
    } else {
        EwsGlobalTagsReadJob *readJob = qobject_cast<EwsGlobalTagsReadJob*>(job);
        Q_ASSERT(readJob);
        tagsRetrieved(readJob->tags(), QHash<QString, Item::List>());
    }
}

AKONADI_RESOURCE_MAIN(EwsResource)
