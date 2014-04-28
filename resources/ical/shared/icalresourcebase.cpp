/*
    Copyright (c) 2006 Till Adam <adam@kde.org>
    Copyright (c) 2009 David Jarvie <djarvie@kde.org>

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

#include "icalresourcebase.h"
#include "icalsettingsadaptor.h"
#include "singlefileresourceconfigdialog.h"

#include <dbusconnectionpool.h>

#include <KCalCore/filestorage.h>
#include <KCalCore/memorycalendar.h>
#include <KCalCore/incidence.h>
#include <KCalCore/icalformat.h>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

using namespace Akonadi;
using namespace KCalCore;
using namespace SETTINGS_NAMESPACE;

ICalResourceBase::ICalResourceBase( const QString &id )
  : SingleFileResource<Settings>( id )
{
  KGlobal::locale()->insertCatalog( QLatin1String("akonadi_ical_resource") );
}

void ICalResourceBase::initialise( const QStringList &mimeTypes, const QString &icon )
{
  setSupportedMimetypes( mimeTypes, icon );
  new ICalSettingsAdaptor( mSettings );
  DBusConnectionPool::threadConnection().registerObject( QLatin1String( "/Settings" ),
                                                         mSettings, QDBusConnection::ExportAdaptors );
}

ICalResourceBase::~ICalResourceBase()
{
}

bool ICalResourceBase::retrieveItem( const Akonadi::Item &item,
                                     const QSet<QByteArray> &parts )
{
  kDebug( 5251 ) << "Item:" << item.url();

  if ( !mCalendar ) {
    kError() << "akonadi_ical_resource: Calendar not loaded";
    emit error( i18n( "Calendar not loaded.") );
    return false;
  }

  return doRetrieveItem( item, parts );
}

void ICalResourceBase::aboutToQuit()
{
  if ( !mSettings->readOnly() ) {
    writeFile();
  }
  mSettings->writeConfig();
}

void ICalResourceBase::customizeConfigDialog( SingleFileResourceConfigDialog<Settings> *dlg )
{
#ifndef KDEPIM_MOBILE_UI
  dlg->setFilter( QLatin1String("text/calendar") );
#else
  dlg->setFilter( QLatin1String("*.ics *.vcs") );
#endif
  dlg->setCaption( i18n( "Select Calendar" ) );
}

bool ICalResourceBase::readFromFile( const QString &fileName )
{
  mCalendar = KCalCore::MemoryCalendar::Ptr( new KCalCore::MemoryCalendar( QLatin1String( "UTC" ) ) );
  mFileStorage = KCalCore::FileStorage::Ptr( new KCalCore::FileStorage( mCalendar, fileName,
                                                                        new KCalCore::ICalFormat() ) );
  const bool result = mFileStorage->load();
  if ( !result ) {
    kError() << "akonadi_ical_resource: Error loading file " << fileName;
  }

  return result;
}

void ICalResourceBase::itemRemoved( const Akonadi::Item &item )
{
  if ( !mCalendar ) {
    kError() << "akonadi_ical_resource: mCalendar is 0!";
    cancelTask( i18n( "Calendar not loaded." ) );
    return;
  }

  Incidence::Ptr i = mCalendar->instance( item.remoteId() );
  if ( i ) {
    if ( !mCalendar->deleteIncidence( i ) ) {
      kError() << "akonadi_ical_resource: Can't delete incidence with instance identifier "
               << item.remoteId() << "; item.id() = " << item.id();
      cancelTask();
      return;
    }
  } else {
    kError() << "akonadi_ical_resource: itemRemoved(): Can't find incidence with instance identifier "
             << item.remoteId() << "; item.id() = " << item.id();
  }
  scheduleWrite();
  changeProcessed();
}

void ICalResourceBase::retrieveItems( const Akonadi::Collection &col )
{
  reloadFile();
  if ( mCalendar ) {
    doRetrieveItems( col );
  } else {
    kError() << "akonadi_ical_resource: retrieveItems(): mCalendar is 0!";
  }
}

bool ICalResourceBase::writeToFile( const QString &fileName )
{
  if ( !mCalendar ) {
    kError() << "akonadi_ical_resource: writeToFile() mCalendar is 0!";
    return false;
  }

  KCalCore::FileStorage *fileStorage = mFileStorage.data();
  if ( fileName != mFileStorage->fileName() ) {
    fileStorage = new KCalCore::FileStorage( mCalendar,
                                             fileName,
                                             new KCalCore::ICalFormat() );
  }

  bool success = true;
  if ( !fileStorage->save() ) {
    kError() << QLatin1String("akonadi_ical_resource: Failed to save calendar to file ") + fileName;
    emit error( i18n( "Failed to save calendar file to %1", fileName ) );
    success = false;
  }

  if ( fileStorage != mFileStorage.data() ) {
    delete fileStorage;
  }

  return success;
}

KCalCore::MemoryCalendar::Ptr ICalResourceBase::calendar() const
{
  return mCalendar;
}

KCalCore::FileStorage::Ptr ICalResourceBase::fileStorage() const
{
  return mFileStorage;
}


