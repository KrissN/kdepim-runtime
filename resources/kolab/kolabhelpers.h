/*
    Copyright (c) 2014 Christian Mollekopf <mollekopf@kolabsys.com>

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

#ifndef KOLABHELPERS_H
#define KOLABHELPERS_H

#include <AkonadiCore/Item>
#include "pimkolab/kolabformat/kolabdefinitions.h"//libkolab
#include "pimkolab/kolabformat/formathelpers.h" //libkolab

#define KOLAB_COLOR_ANNOTATION "/vendor/kolab/color"

class QColor;

class KolabHelpers
{
public:
    static bool checkForErrors(const Akonadi::Item &affectedItem);
    static Akonadi::Item translateFromImap(Kolab::FolderType folderType, const Akonadi::Item &item, bool &ok);
    static Akonadi::Item::List translateToImap(const Akonadi::Item::List &items, bool &ok);
    static Akonadi::Item translateToImap(const Akonadi::Item &item, bool &ok);
    static Kolab::FolderType folderTypeFromString(const QByteArray &folderTypeName);
    static QByteArray getFolderTypeAnnotation(const QMap<QByteArray, QByteArray> &annotations);
    static void setFolderTypeAnnotation(QMap<QByteArray, QByteArray> &annotations, const QByteArray &value);
    static QColor getFolderColor(const QMap<QByteArray, QByteArray> &annotations);
    static void setFolderColor(QMap<QByteArray, QByteArray> &annotations, const QColor &color);
    static Kolab::ObjectType getKolabTypeFromMimeType(const QString &type);
    static QByteArray kolabTypeForMimeType(const QStringList &contentMimeTypes);
    static QStringList getContentMimeTypes(Kolab::FolderType type);
    static QString getIcon(Kolab::FolderType type);
    //Returns true if the folder type shouldn't be ignored
    static bool isHandledType(Kolab::FolderType type);
    static QString getMimeType(Kolab::FolderType type);
    static QList<QByteArray> ancestorChain(const Akonadi::Collection &col);
    static QString createMemberUrl(const Akonadi::Item &item, const QString &user);
};

#endif
