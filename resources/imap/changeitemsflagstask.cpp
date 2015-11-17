/*
    Copyright (c) 2013 Daniel Vrátil <dvratil@redhat.com>

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

#include "changeitemsflagstask.h"

#include <kimap/session.h>
#include <kimap/selectjob.h>
#include <kimap/storejob.h>
#include "imapresource_debug.h"
#include "imapresource_debug.h"

ChangeItemsFlagsTask::ChangeItemsFlagsTask(const ResourceStateInterface::Ptr &resource, QObject *parent):
    ResourceTask(ResourceTask::DeferIfNoSession, resource, parent),
    m_processedItems(0)
{

}

ChangeItemsFlagsTask::~ChangeItemsFlagsTask()
{
}

void ChangeItemsFlagsTask::doStart(KIMAP::Session *session)
{
    const QString mailBox = mailBoxForCollection(items().at(0).parentCollection());
    qCDebug(IMAPRESOURCE_LOG) << mailBox;

    if (session->selectedMailBox() != mailBox) {
        KIMAP::SelectJob *select = new KIMAP::SelectJob(session);
        select->setMailBox(mailBox);

        connect(select, &KJob::result,
                this, &ChangeItemsFlagsTask::onSelectDone);

        select->start();

    } else {
        if (!addedFlags().isEmpty()) {
            triggerAppendFlagsJob(session);
        } else if (!removedFlags().isEmpty()) {
            triggerRemoveFlagsJob(session);
        } else {
            qCDebug(IMAPRESOURCE_LOG) << "nothing to do";
            changeProcessed();
        }
    }
}

void ChangeItemsFlagsTask::onSelectDone(KJob *job)
{
    if (job->error()) {
        qCWarning(IMAPRESOURCE_LOG) << "Select failed: " << job->errorString();
        cancelTask(job->errorString());
    } else {
        KIMAP::SelectJob *select = static_cast<KIMAP::SelectJob *>(job);
        qCDebug(IMAPRESOURCE_LOG) << addedFlags();
        if (!addedFlags().isEmpty()) {
            triggerAppendFlagsJob(select->session());
        } else if (!removedFlags().isEmpty()) {
            triggerRemoveFlagsJob(select->session());
        } else {
            qCDebug(IMAPRESOURCE_LOG) << "nothing to do";
            changeProcessed();
        }
    }
}

KIMAP::StoreJob *ChangeItemsFlagsTask::prepareJob(KIMAP::Session *session)
{
    KIMAP::ImapSet set;
    const Akonadi::Item::List &allItems = items();
    // Split the request to multiple smaller requests of 2000 UIDs each - various IMAP
    // servers have various limits on maximum size of a request
    // 2000 is a random number that sounds like a good compromise between performance
    // and functionality (i.e. 2000 UIDs should be supported by any server out there)
    for (int i = 0, count = qMin(2000, allItems.count() - m_processedItems); i < count; ++i) {
        set.add(allItems[m_processedItems].remoteId().toLong());
        ++m_processedItems;
    }

    KIMAP::StoreJob *store = new KIMAP::StoreJob(session);
    store->setUidBased(true);
    store->setSequenceSet(set);

    return store;
}

void ChangeItemsFlagsTask::triggerAppendFlagsJob(KIMAP::Session *session)
{
    KIMAP::StoreJob *store = prepareJob(session);
    store->setFlags(fromAkonadiToSupportedImapFlags(addedFlags().toList(), items().at(0).parentCollection()));
    store->setMode(KIMAP::StoreJob::AppendFlags);
    connect(store, &KIMAP::StoreJob::result, this, &ChangeItemsFlagsTask::onAppendFlagsDone);
    store->start();
}

void ChangeItemsFlagsTask::triggerRemoveFlagsJob(KIMAP::Session *session)
{
    KIMAP::StoreJob *store = prepareJob(session);
    store->setFlags(fromAkonadiToSupportedImapFlags(removedFlags().toList(), items().at(0).parentCollection()));
    store->setMode(KIMAP::StoreJob::RemoveFlags);
    connect(store, &KIMAP::StoreJob::result, this, &ChangeItemsFlagsTask::onRemoveFlagsDone);
    store->start();
}

void ChangeItemsFlagsTask::onAppendFlagsDone(KJob *job)
{
    if (job->error()) {
        qCWarning(IMAPRESOURCE_LOG) << "Flag append failed: " << job->errorString();
        cancelTask(job->errorString());
    } else {
        KIMAP::Session *session = qobject_cast<KIMAP::Job *>(job)->session();
        if (m_processedItems < items().count()) {
            triggerAppendFlagsJob(session);
        } else if (removedFlags().isEmpty()) {
            changeProcessed();
        } else {
            qCDebug(IMAPRESOURCE_LOG) << removedFlags();
            m_processedItems = 0;
            triggerRemoveFlagsJob(session);
        }
    }
}

void ChangeItemsFlagsTask::onRemoveFlagsDone(KJob *job)
{
    if (job->error()) {
        qCWarning(IMAPRESOURCE_LOG) << "Flag remove failed: " << job->errorString();
        cancelTask(job->errorString());
    } else {
        if (m_processedItems < items().count()) {
            triggerRemoveFlagsJob(qobject_cast<KIMAP::Job *>(job)->session());
        } else {
            changeProcessed();
        }
    }
}

