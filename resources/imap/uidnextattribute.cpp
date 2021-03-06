/*
    Copyright (C) 2008 Omat Holding B.V. <info@omat.nl>

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

#include "uidnextattribute.h"

#include <QByteArray>

#include <attribute.h>

UidNextAttribute::UidNextAttribute(int uidnext)
    : mUidNext(uidnext)
{
}

void UidNextAttribute::setUidNext(int uidnext)
{
    mUidNext = uidnext;
}

int UidNextAttribute::uidNext() const
{
    return mUidNext;
}

QByteArray UidNextAttribute::type() const
{
    static const QByteArray sType("uidnext");
    return sType;
}

Akonadi::Attribute *UidNextAttribute::clone() const
{
    return new UidNextAttribute(mUidNext);
}

QByteArray UidNextAttribute::serialized() const
{
    return QByteArray::number(mUidNext);
}

void UidNextAttribute::deserialize(const QByteArray &data)
{
    mUidNext = data.toInt();
}
