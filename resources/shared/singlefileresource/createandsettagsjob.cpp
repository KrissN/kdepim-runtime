/*
 *  Copyright (c) 2014 Christian Mollekopf <mollekopf@kolabsys.com>
 *
 *  This library is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This library is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301, USA.
 */
#include "createandsettagsjob.h"

#include <AkonadiCore/tagcreatejob.h>
#include <AkonadiCore/itemmodifyjob.h>

#include <QDebug>

using namespace Akonadi;

CreateAndSetTagsJob::CreateAndSetTagsJob(const Item &item, const Akonadi::Tag::List &tags, QObject *parent)
    : KJob(parent)
    , mItem(item)
    , mTags(tags)
    , mCount(0)
{
}

void CreateAndSetTagsJob::start()
{
    if (mTags.isEmpty()) {
        emitResult();
    }
    for (const Akonadi::Tag &tag : qAsConst(mTags)) {
        Akonadi::TagCreateJob *createJob = new Akonadi::TagCreateJob(tag, this);
        createJob->setMergeIfExisting(true);
        connect(createJob, &Akonadi::TagCreateJob::result, this, &CreateAndSetTagsJob::onCreateDone);
    }
}

void CreateAndSetTagsJob::onCreateDone(KJob *job)
{
    mCount++;
    if (job->error()) {
        qWarning() << "Failed to create tag " << job->errorString();
    } else {
        Akonadi::TagCreateJob *createJob = static_cast<Akonadi::TagCreateJob *>(job);
        mCreatedTags << createJob->tag();
    }
    if (mCount == mTags.size()) {
        for (const Akonadi::Tag &tag : qAsConst(mCreatedTags)) {
            mItem.setTag(tag);
        }
        Akonadi::ItemModifyJob *modJob = new Akonadi::ItemModifyJob(mItem, this);
        connect(modJob, &Akonadi::ItemModifyJob::result, this, &CreateAndSetTagsJob::onModifyDone);
    }
}

void CreateAndSetTagsJob::onModifyDone(KJob *job)
{
    if (job->error()) {
        qWarning() << "Failed to modify item " << job->errorString();
        setError(KJob::UserDefinedError);
    }
    emitResult();
}
