/*
 *
 * $Id: $
 *
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

/*
 * This file has been generated by the Nepomuk Resource class generator.
 * DO NOT EDIT THIS FILE.
 * ANY CHANGES WILL BE LOST.
 */

#include <nepomuk/tools.h>
#include <nepomuk/variant.h>
#include <nepomuk/resourcemanager.h>
#include "filedataobject.h"

#include "filehash.h"
#include "contact.h"
Nepomuk::FileDataObject::FileDataObject()
  : DataObject( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileDataObject") )
{
}


Nepomuk::FileDataObject::FileDataObject( const FileDataObject& res )
  : DataObject( res )
{
}


Nepomuk::FileDataObject::FileDataObject( const Nepomuk::Resource& res )
  : DataObject( res )
{
}


Nepomuk::FileDataObject::FileDataObject( const QString& uri )
  : DataObject( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileDataObject") )
{
}

Nepomuk::FileDataObject::FileDataObject( const QUrl& uri )
  : DataObject( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileDataObject") )
{
}

Nepomuk::FileDataObject::FileDataObject( const QString& uri, const QUrl& type )
  : DataObject( uri, type )
{
}

Nepomuk::FileDataObject::FileDataObject( const QUrl& uri, const QUrl& type )
  : DataObject( uri, type )
{
}

Nepomuk::FileDataObject::~FileDataObject()
{
}


Nepomuk::FileDataObject& Nepomuk::FileDataObject::operator=( const FileDataObject& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::FileDataObject::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileDataObject";
}

QList<QDateTime> Nepomuk::FileDataObject::fileLastAccesseds() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileLastAccessed") ).toDateTimeList());
}

void Nepomuk::FileDataObject::setFileLastAccesseds( const QList<QDateTime>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileLastAccessed"), Variant( value ) );
}

void Nepomuk::FileDataObject::addFileLastAccessed( const QDateTime& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileLastAccessed") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileLastAccessed"), v );
}

QUrl Nepomuk::FileDataObject::fileLastAccessedUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileLastAccessed");
}

QList<QDateTime> Nepomuk::FileDataObject::fileCreateds() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileCreated") ).toDateTimeList());
}

void Nepomuk::FileDataObject::setFileCreateds( const QList<QDateTime>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileCreated"), Variant( value ) );
}

void Nepomuk::FileDataObject::addFileCreated( const QDateTime& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileCreated") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileCreated"), v );
}

QUrl Nepomuk::FileDataObject::fileCreatedUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileCreated");
}

QList<qint64> Nepomuk::FileDataObject::fileSizes() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileSize") ).toInt64List());
}

void Nepomuk::FileDataObject::setFileSizes( const QList<qint64>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileSize"), Variant( value ) );
}

void Nepomuk::FileDataObject::addFileSize( const qint64& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileSize") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileSize"), v );
}

QUrl Nepomuk::FileDataObject::fileSizeUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileSize");
}

QStringList Nepomuk::FileDataObject::permissionses() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#permissions") ).toStringList());
}

void Nepomuk::FileDataObject::setPermissionses( const QStringList& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#permissions"), Variant( value ) );
}

void Nepomuk::FileDataObject::addPermissions( const QString& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#permissions") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#permissions"), v );
}

QUrl Nepomuk::FileDataObject::permissionsUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#permissions");
}

QStringList Nepomuk::FileDataObject::fileNames() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileName") ).toStringList());
}

void Nepomuk::FileDataObject::setFileNames( const QStringList& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileName"), Variant( value ) );
}

void Nepomuk::FileDataObject::addFileName( const QString& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileName") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileName"), v );
}

QUrl Nepomuk::FileDataObject::fileNameUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileName");
}

QList<Nepomuk::Resource> Nepomuk::FileDataObject::fileUrls() const
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    return convertResourceList<Resource>( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileUrl") ).toResourceList() );
}

void Nepomuk::FileDataObject::setFileUrls( const QList<Nepomuk::Resource>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileUrl"), Variant( value ) );
}

void Nepomuk::FileDataObject::addFileUrl( const Nepomuk::Resource& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileUrl") );
    v.append( Resource( value ) );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileUrl"), v );
}

QUrl Nepomuk::FileDataObject::fileUrlUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileUrl");
}

QList<Nepomuk::FileHash> Nepomuk::FileDataObject::hashs() const
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    return convertResourceList<FileHash>( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasHash") ).toResourceList() );
}

void Nepomuk::FileDataObject::setHashs( const QList<Nepomuk::FileHash>& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    QList<Resource> l;
    for( QList<FileHash>::const_iterator it = value.constBegin();
         it != value.constEnd(); ++it ) {
        l.append( Resource( (*it) ) );
    }
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasHash"), Variant( l ) );
}

void Nepomuk::FileDataObject::addHash( const Nepomuk::FileHash& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasHash") );
    v.append( Resource( value ) );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasHash"), v );
}

QUrl Nepomuk::FileDataObject::hashUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasHash");
}

QList<Nepomuk::Contact> Nepomuk::FileDataObject::fileOwners() const
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    return convertResourceList<Contact>( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileOwner") ).toResourceList() );
}

void Nepomuk::FileDataObject::setFileOwners( const QList<Nepomuk::Contact>& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    QList<Resource> l;
    for( QList<Contact>::const_iterator it = value.constBegin();
         it != value.constEnd(); ++it ) {
        l.append( Resource( (*it) ) );
    }
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileOwner"), Variant( l ) );
}

void Nepomuk::FileDataObject::addFileOwner( const Nepomuk::Contact& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileOwner") );
    v.append( Resource( value ) );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileOwner"), v );
}

QUrl Nepomuk::FileDataObject::fileOwnerUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileOwner");
}

QList<QDateTime> Nepomuk::FileDataObject::fileLastModifieds() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileLastModified") ).toDateTimeList());
}

void Nepomuk::FileDataObject::setFileLastModifieds( const QList<QDateTime>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileLastModified"), Variant( value ) );
}

void Nepomuk::FileDataObject::addFileLastModified( const QDateTime& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileLastModified") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileLastModified"), v );
}

QUrl Nepomuk::FileDataObject::fileLastModifiedUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileLastModified");
}

QList<Nepomuk::FileDataObject> Nepomuk::FileDataObject::allFileDataObjects()
{
    return Nepomuk::convertResourceList<FileDataObject>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileDataObject") ) );
}


