/*
 * Copyright 2013  Christian Mollekopf <mollekopf@kolabsys.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "dummymigrator.h"
#include <QTimer>
#include <QDebug>

DummyMigrator::DummyMigrator(const QString &identifier)
:   MigratorBase(QLatin1String("dummymigrator") + identifier, QString(), QString())
{}

QString DummyMigrator::displayName() const
{
    return QLatin1String("dummymigrator");
}

void DummyMigrator::startWork()
{
    qDebug();
    QTimer::singleShot(10000, this, SLOT(onTimerElapsed()));
}

void DummyMigrator::onTimerElapsed()
{
    qDebug();
    setMigrationState(Complete);
}

bool DummyMigrator::shouldAutostart() const
{
    return true;
}

bool DummyMigrator::canStart()
{
    return true;
}

void DummyMigrator::pause()
{
    qDebug();
    MigratorBase::pause();
}

void DummyMigrator::abort()
{
    qDebug();
    MigratorBase::abort();
}
