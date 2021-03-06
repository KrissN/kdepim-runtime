/*
    This file is part of oxaccess.

    Copyright (c) 2009 Tobias Koenig <tokoe@kde.org>

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

#ifndef OXA_USER_H
#define OXA_USER_H

#include <QVector>
#include <QString>

namespace OXA {
class User
{
public:
    typedef QVector<User> List;

    User();

    bool isValid() const;

    void setUid(qlonglong uid);
    qlonglong uid() const;

    void setEmail(const QString &email);
    QString email() const;

    void setName(const QString &name);
    QString name() const;

private:
    qlonglong mUid;
    QString mEmail;
    QString mName;
};
}

Q_DECLARE_TYPEINFO(OXA::User, Q_MOVABLE_TYPE);
#endif
