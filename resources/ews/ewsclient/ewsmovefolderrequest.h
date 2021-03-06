/*
    Copyright (C) 2015-2017 Krzysztof Nowicki <krissn@op.pl>

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

#ifndef EWSMOVEFOLDERREQUEST_H
#define EWSMOVEFOLDERREQUEST_H

#include <QList>
#include <QSharedPointer>

#include "ewsfolder.h"
#include "ewsrequest.h"
#include "ewstypes.h"

class QXmlStreamReader;

class EwsMoveFolderRequest : public EwsRequest
{
    Q_OBJECT
public:
    class Response : public EwsRequest::Response
    {
    public:
        const EwsId &folderId() const
        {
            return mId;
        }
    protected:
        Response(QXmlStreamReader &reader);

        EwsId mId;

        friend class EwsMoveFolderRequest;
    };

    EwsMoveFolderRequest(EwsClient &client, QObject *parent);
    ~EwsMoveFolderRequest() override;

    void setFolderIds(const EwsId::List &ids)
    {
        mIds = ids;
    }

    void setDestinationFolderId(const EwsId &id)
    {
        mDestFolderId = id;
    }

    void start() override;

    const QList<Response> &responses() const
    {
        return mResponses;
    }
protected:
    bool parseResult(QXmlStreamReader &reader) override;
    bool parseItemsResponse(QXmlStreamReader &reader);
private:
    EwsId::List mIds;
    EwsId mDestFolderId;
    QList<Response> mResponses;
};

#endif
