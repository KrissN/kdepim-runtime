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

#ifndef EWSCREATEMAILJOB_H
#define EWSCREATEMAILJOB_H

#include "ewscreateitemjob.h"

class EwsCreateMailJob : public EwsCreateItemJob
{
    Q_OBJECT
public:
    EwsCreateMailJob(EwsClient &client, const Akonadi::Item &item,
                     const Akonadi::Collection &collection, EwsTagStore *tagStore, EwsResource *parent);
    ~EwsCreateMailJob() override;
    bool setSend(bool send = true) override;
protected:
    void doStart() override;
private Q_SLOTS:
    void mailCreateFinished(KJob *job);
    void mailCreateWorkaroundFinished(KJob *job);
    void mailMoveWorkaroundFinished(KJob *job);
private:
    bool mSend;
};

#endif
