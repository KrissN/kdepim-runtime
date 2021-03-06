/*
    Copyright (c) 2014 Klarälvdalens Datakonsult AB,
                       a KDAB Group company <info@kdab.com>
    Author: Kevin Krammer <kevin.krammer@kdab.com>

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

#include "kolabchangetagtask.h"
#include "kolabresource_debug.h"
#include "tagchangehelper.h"

#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemFetchScope>

KolabChangeTagTask::KolabChangeTagTask(const ResourceStateInterface::Ptr &resource, const QSharedPointer<TagConverter> &tagConverter, QObject *parent)
    : KolabRelationResourceTask(resource, parent)
    , mSession(nullptr)
    , mTagConverter(tagConverter)
{
}

void KolabChangeTagTask::startRelationTask(KIMAP::Session *session)
{
    mSession = session;

    Akonadi::ItemFetchJob *fetch = new Akonadi::ItemFetchJob(resourceState()->tag());
    fetch->fetchScope().setCacheOnly(true);
    fetch->fetchScope().setFetchGid(true);
    fetch->fetchScope().setAncestorRetrieval(Akonadi::ItemFetchScope::All);
    fetch->fetchScope().fetchFullPayload(true);
    connect(fetch, &KJob::result, this, &KolabChangeTagTask::onItemsFetchDone);
}

void KolabChangeTagTask::onItemsFetchDone(KJob *job)
{
    if (job->error()) {
        qCWarning(KOLABRESOURCE_LOG) << "ItemFetch failed: " << job->errorString();
        cancelTask(job->errorString());
        return;
    }

    const Akonadi::Item::List items = static_cast<Akonadi::ItemFetchJob *>(job)->items();

    TagChangeHelper *changeHelper = new TagChangeHelper(this);

    connect(changeHelper, &TagChangeHelper::applyCollectionChanges,
            this, &KolabChangeTagTask::onApplyCollectionChanged);
    connect(changeHelper, &TagChangeHelper::cancelTask, this, &KolabChangeTagTask::onCancelTask);
    connect(changeHelper, &TagChangeHelper::changeCommitted, this, &KolabChangeTagTask::onChangeCommitted);

    changeHelper->start(resourceState()->tag(), mTagConverter->createMessage(resourceState()->tag(), items, resourceState()->userName()), mSession);
}

void KolabChangeTagTask::onApplyCollectionChanged(const Akonadi::Collection &collection)
{
    mRelationCollection = collection;
    applyCollectionChanges(collection);
}

void KolabChangeTagTask::onCancelTask(const QString &errorText)
{
    cancelTask(errorText);
}

void KolabChangeTagTask::onChangeCommitted()
{
    changeProcessed();
}
