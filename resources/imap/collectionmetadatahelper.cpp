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

#include "collectionmetadatahelper.h"
#include <imapaclattribute.h>

Akonadi::Collection::Rights CollectionMetadataHelper::convertRights(const KIMAP::Acl::Rights imapRights, KIMAP::Acl::Rights parentRights)
{
    Akonadi::Collection::Rights newRights = Akonadi::Collection::ReadOnly;

    // For renaming, the parent folder needs to have the CreateMailbox or Create permission.
    // We map renaming to CanChangeCollection here, which is not entirely correct, but we have no
    // CanRenameCollection flag.
    if (parentRights & KIMAP::Acl::CreateMailbox ||
            parentRights & KIMAP::Acl::Create) {
        newRights |= Akonadi::Collection::CanChangeCollection;
    }

    if (imapRights & KIMAP::Acl::Write) {
        newRights |= Akonadi::Collection::CanChangeItem;
    }

    if (imapRights & KIMAP::Acl::Insert) {
        newRights |= Akonadi::Collection::CanCreateItem;
    }

    if (imapRights & (KIMAP::Acl::DeleteMessage | KIMAP::Acl::Delete)) {
        newRights |= Akonadi::Collection::CanDeleteItem;
    }

    if (imapRights & (KIMAP::Acl::CreateMailbox | KIMAP::Acl::Create)) {
        newRights |= Akonadi::Collection::CanCreateCollection;
    }

    if (imapRights & (KIMAP::Acl::DeleteMailbox | KIMAP::Acl::Delete)) {
        newRights |= Akonadi::Collection::CanDeleteCollection;
    }
    return newRights;
}

bool CollectionMetadataHelper::applyRights(Akonadi::Collection &collection, const KIMAP::Acl::Rights imapRights, KIMAP::Acl::Rights parentRights)
{
    Akonadi::Collection::Rights newRights = convertRights(imapRights, parentRights);

    if (collection.hasAttribute("noinferiors")) {
        newRights &= ~Akonadi::Collection::CanCreateCollection;
    }

    if (collection.parentCollection().hasAttribute("noselect")) {
        newRights &= ~Akonadi::Collection::CanChangeCollection;
    }

    //This can result in false positives for new collections with defaults access rights.
    //The caller needs to handles those.
    bool accessRevoked = false;
    if ((collection.rights() & Akonadi::Collection::CanCreateItem) &&
            !(newRights & Akonadi::Collection::CanCreateItem)) {
        // write access revoked
        accessRevoked = true;
    }

    // qDebug() << collection.remoteId()
    //             << "imapRights:" << imapRights
    //             << "newRights:" << newRights
    //             << "oldRights:" << collection.rights();

    if (newRights != collection.rights()) {
        collection.setRights(newRights);
    }
    return accessRevoked;
}

