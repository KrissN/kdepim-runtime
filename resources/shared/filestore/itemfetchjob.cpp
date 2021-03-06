/*  This file is part of the KDE project
    Copyright (C) 2009 Kevin Krammer <kevin.krammer@gmx.at>

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

#include "itemfetchjob.h"

#include "session_p.h"

#include <itemfetchscope.h>

using namespace Akonadi;

class FileStore::ItemFetchJob::Private
{
public:
    ItemFetchScope mFetchScope;

    Item::List mRequestedItems;
    Item::List mResultItems;

    Collection mCollection;
    Item mItem;
};

FileStore::ItemFetchJob::ItemFetchJob(const Collection &collection, FileStore::AbstractJobSession *session)
    : FileStore::Job(session)
    , d(new Private())
{
    d->mCollection = collection;

    session->addJob(this);
}

FileStore::ItemFetchJob::ItemFetchJob(const Item &item, FileStore::AbstractJobSession *session)
    : FileStore::Job(session)
    , d(new Private())
{
    d->mItem = item;

    session->addJob(this);
}

FileStore::ItemFetchJob::ItemFetchJob(const Item::List &items, FileStore::AbstractJobSession *session)
    : FileStore::Job(session)
    , d(new Private())
{
    d->mRequestedItems = items;

    session->addJob(this);
}

FileStore::ItemFetchJob::~ItemFetchJob()
{
    delete d;
}

Collection FileStore::ItemFetchJob::collection() const
{
    return d->mCollection;
}

Item FileStore::ItemFetchJob::item() const
{
    return d->mItem;
}

void FileStore::ItemFetchJob::setFetchScope(const ItemFetchScope &fetchScope)
{
    d->mFetchScope = fetchScope;
}

ItemFetchScope &FileStore::ItemFetchJob::fetchScope()
{
    return d->mFetchScope;
}

Item::List FileStore::ItemFetchJob::requestedItems() const
{
    return d->mRequestedItems;
}

Item::List FileStore::ItemFetchJob::items() const
{
    return d->mResultItems;
}

bool FileStore::ItemFetchJob::accept(FileStore::Job::Visitor *visitor)
{
    return visitor->visit(this);
}

void FileStore::ItemFetchJob::handleItemsReceived(const Item::List &items)
{
    d->mResultItems << items;

    Q_EMIT itemsReceived(items);
}
