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

#ifndef MOVECOLLECTIONTASK_H
#define MOVECOLLECTIONTASK_H

#include "resourcetask.h"

class MoveCollectionTask : public ResourceTask
{
    Q_OBJECT

public:
    explicit MoveCollectionTask(const ResourceStateInterface::Ptr &resource, QObject *parent = nullptr);
    ~MoveCollectionTask() override;

private Q_SLOTS:
    void onExamineDone(KJob *job);
    void onRenameDone(KJob *job);
    void onSubscribeDone(KJob *job);

protected:
    void doStart(KIMAP::Session *session) override;

private:
    void doRename(KIMAP::Session *session);
    QString mailBoxForCollections(const Akonadi::Collection &parent, const Akonadi::Collection &child) const;

    Akonadi::Collection m_collection;
};

#endif
