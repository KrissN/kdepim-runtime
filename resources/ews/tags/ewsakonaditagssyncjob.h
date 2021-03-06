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

#ifndef EWSAKONADITAGSSYNCJOB_H
#define EWSAKONADITAGSSYNCJOB_H

#include "ewsjob.h"

class EwsTagStore;
class EwsClient;
namespace Akonadi
{
class Collection;
}

class EwsAkonadiTagsSyncJob : public EwsJob
{
    Q_OBJECT
public:
    EwsAkonadiTagsSyncJob(EwsTagStore *tagStore, EwsClient &client,
                          const Akonadi::Collection &rootCollection, QObject *parent);
    ~EwsAkonadiTagsSyncJob();

    void start() override;

private Q_SLOTS:
    void tagFetchFinished(KJob *job);
    void tagWriteFinished(KJob *job);
private:
    EwsTagStore *mTagStore = nullptr;
    EwsClient &mClient;
    const Akonadi::Collection &mRootCollection;
};

#endif
