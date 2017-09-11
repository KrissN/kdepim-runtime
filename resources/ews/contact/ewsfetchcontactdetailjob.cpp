/*
    Copyright (C) 2015-2016 Krzysztof Nowicki <krissn@op.pl>

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

#include "ewsfetchcontactdetailjob.h"
#include "ewsitemshape.h"
#include "ewsgetitemrequest.h"
#include "ewsmailbox.h"
#include "ewsresource_debug.h"

using namespace Akonadi;

EwsFetchContactDetailJob::EwsFetchContactDetailJob(EwsClient &client, QObject *parent, const Akonadi::Collection &collection)
    : EwsFetchItemDetailJob(client, parent, collection)
{
    EwsItemShape shape(EwsShapeIdOnly);
    mRequest->setItemShape(shape);
}


EwsFetchContactDetailJob::~EwsFetchContactDetailJob()
{
}

void EwsFetchContactDetailJob::processItems(const QList<EwsGetItemRequest::Response> &responses)
{
    Item::List::iterator it = mChangedItems.begin();

    Q_FOREACH (const EwsGetItemRequest::Response &resp, responses) {
        Item &item = *it;

        if (!resp.isSuccess()) {
            qCWarningNC(EWSRES_LOG) << QStringLiteral("Failed to fetch item %1").arg(item.remoteId());
            continue;
        }

        //const EwsItem &ewsItem = resp.item();

        // TODO: Implement

        ++it;
    }

    emitResult();
}