/*
    Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>

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

#ifndef STORERESULTJOB_H
#define STORERESULTJOB_H

#include <TransactionSequence>

#include <QString>
#include <AkonadiCore/Item>
/**
 * This class stores the result of a StoreResultJob in an item.
 * First, it removes the 'queued' flag.
 * After that, if the result was success, it stores the 'sent' flag.
 * If the result was failure, it stores the 'error' flag and an ErrorAttribute.
 */
class StoreResultJob : public Akonadi::TransactionSequence
{
    Q_OBJECT

public:
    /**
     * Creates a new store result job.
     *
     * @param item The item to store.
     * @param success Whether the mail could be dispatched or not.
     * @param message An error message in case the mail could not be dispatched.
     * @param parent The parent object.
     */
    explicit StoreResultJob(const Akonadi::Item &item, bool success, const QString &message, QObject *parent = nullptr);

    /**
     * Destroys the store result job.
     */
    ~StoreResultJob() override;

    bool success() const;
    QString message() const;

protected:
    // reimpl from TransactionSequence
    void doStart() override;

private:
    // Q_SLOTS:
    void fetchDone(KJob *job);
    void modifyDone(KJob *job);

    Akonadi::Item mItem;
    bool mSuccess = false;
    QString mMessage;
};

#endif
