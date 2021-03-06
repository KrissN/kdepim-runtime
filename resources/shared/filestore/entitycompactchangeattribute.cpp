/*  This file is part of the KDE project
    Copyright (C) 2010 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    Author: Kevin Krammer, krake@kdab.com

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

#include "entitycompactchangeattribute.h"

#include <QDataStream>

using namespace Akonadi;

class FileStore::EntityCompactChangeAttribute::Private
{
public:
    Private &operator=(const Private &other)
    {
        if (&other == this) {
            return *this;
        }

        mRemoteId = other.mRemoteId;
        mRemoteRev = other.mRemoteRev;
        return *this;
    }

public:
    QString mRemoteId;
    QString mRemoteRev;
};

FileStore::EntityCompactChangeAttribute::EntityCompactChangeAttribute()
    : Attribute()
    , d(new Private())
{
}

FileStore::EntityCompactChangeAttribute::~EntityCompactChangeAttribute()
{
    delete d;
}

void FileStore::EntityCompactChangeAttribute::setRemoteId(const QString &remoteId)
{
    d->mRemoteId = remoteId;
}

QString FileStore::EntityCompactChangeAttribute::remoteId() const
{
    return d->mRemoteId;
}

void FileStore::EntityCompactChangeAttribute::setRemoteRevision(const QString &remoteRev)
{
    d->mRemoteRev = remoteRev;
}

QString FileStore::EntityCompactChangeAttribute::remoteRevision() const
{
    return d->mRemoteRev;
}

QByteArray FileStore::EntityCompactChangeAttribute::type() const
{
    static const QByteArray sType("ENTITYCOMPACTCHANGE");
    return sType;
}

FileStore::EntityCompactChangeAttribute *FileStore::EntityCompactChangeAttribute::clone() const
{
    FileStore::EntityCompactChangeAttribute *copy = new FileStore::EntityCompactChangeAttribute();
    *(copy->d) = *d;
    return copy;
}

QByteArray FileStore::EntityCompactChangeAttribute::serialized() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << d->mRemoteId;
    stream << d->mRemoteRev;

    return data;
}

void FileStore::EntityCompactChangeAttribute::deserialize(const QByteArray &data)
{
    QDataStream stream(data);
    stream >> d->mRemoteId;
    stream >> d->mRemoteRev;
}
