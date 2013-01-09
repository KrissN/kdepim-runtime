/*
 *   Copyright (C) 2012  Christian Mollekopf <chrigi_1@fastmail.fm>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FINDUNINDEXEDITEMSJOB_H
#define FINDUNINDEXEDITEMSJOB_H
#include <KJob>
#include <QTime>
#include <akonadi/item.h>
#include <Akonadi/Collection>

class FindUnindexedItemsJob: public KJob
{
    Q_OBJECT
public:
    explicit FindUnindexedItemsJob(int compatLevel, QObject* parent = 0);
    virtual void start();
    const QHash<Akonadi::Item::Id, Akonadi::Collection::Id> &getUnindexed() const;
private slots:
    void itemsRetrieved(KJob*);
    void retrieveIndexedNepomukResources();
private:
    void retrieveAkonadiItems();
    QHash<Akonadi::Item::Id, Akonadi::Collection::Id> mAkonadiItems;
    QTime mTime;
    const int mCompatLevel;
};

#endif // FINDUNINDEXEDITEMSJOB_H
