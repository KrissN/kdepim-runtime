/*
 *  kalarmresourcecommon.h  -  common functions for KAlarm Akonadi resources
 *  Program:  kalarm
 *  Copyright © 2011-2014 by David Jarvie <djarvie@kde.org>
 *
 *  This library is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This library is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301, USA.
 */

#ifndef KALARMRESOURCECOMMON_H
#define KALARMRESOURCECOMMON_H

#include <kalarmcal/kacalendar.h>
#include <kalarmcal/kaevent.h>

#include <QObject>

namespace KCalCore {
class FileStorage;
}
namespace Akonadi {
class Collection;
class Item;
}
using namespace KAlarmCal;

namespace KAlarmResourceCommon {
void          initialise(QObject *parent);
//    void          customizeConfigDialog(SingleFileResourceConfigDialog<Settings>*);
KACalendar::Compat getCompatibility(const KCalCore::FileStorage::Ptr &, int &version);
Akonadi::Item retrieveItem(const Akonadi::Item &, KAEvent &);
KAEvent       checkItemChanged(const Akonadi::Item &, QString &errorMsg);
void          setCollectionCompatibility(const Akonadi::Collection &, KACalendar::Compat, int version);

enum ErrorCode {
    UidNotFound,
    NotCurrentFormat,
    EventNotCurrentFormat,
    EventNoAlarms,
    EventReadOnly,
    CalendarAdd
};
QString errorMessage(ErrorCode, const QString &param = QString());
}

#endif // KALARMRESOURCECOMMON_H
