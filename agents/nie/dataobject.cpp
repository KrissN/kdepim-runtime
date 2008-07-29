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
#include "dataobject.h"

#include "datasource.h"
#include "media.h"
#include "datacontainer.h"
#include "informationelement.h"
#include "contact.h"
#include "organizationcontact.h"
Nepomuk::DataObject::DataObject()
  : Resource( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#DataObject") )
{
}


Nepomuk::DataObject::DataObject( const DataObject& res )
  : Resource( res )
{
}


Nepomuk::DataObject::DataObject( const Nepomuk::Resource& res )
  : Resource( res )
{
}


Nepomuk::DataObject::DataObject( const QString& uri )
  : Resource( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#DataObject") )
{
}

Nepomuk::DataObject::DataObject( const QUrl& uri )
  : Resource( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#DataObject") )
{
}

Nepomuk::DataObject::DataObject( const QString& uri, const QUrl& type )
  : Resource( uri, type )
{
}

Nepomuk::DataObject::DataObject( const QUrl& uri, const QUrl& type )
  : Resource( uri, type )
{
}

Nepomuk::DataObject::~DataObject()
{
}


Nepomuk::DataObject& Nepomuk::DataObject::operator=( const DataObject& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::DataObject::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#DataObject";
}

QList<QDateTime> Nepomuk::DataObject::createds() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#created") ).toDateTimeList());
}

void Nepomuk::DataObject::setCreateds( const QList<QDateTime>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#created"), Variant( value ) );
}

void Nepomuk::DataObject::addCreated( const QDateTime& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#created") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#created"), v );
}

QUrl Nepomuk::DataObject::createdUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#created");
}

QList<Nepomuk::InformationElement> Nepomuk::DataObject::interpretedAses() const
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    return convertResourceList<InformationElement>( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#interpretedAs") ).toResourceList() );
}

void Nepomuk::DataObject::setInterpretedAses( const QList<Nepomuk::InformationElement>& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    QList<Resource> l;
    for( QList<InformationElement>::const_iterator it = value.constBegin();
         it != value.constEnd(); ++it ) {
        l.append( Resource( (*it) ) );
    }
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#interpretedAs"), Variant( l ) );
}

void Nepomuk::DataObject::addInterpretedAs( const Nepomuk::InformationElement& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#interpretedAs") );
    v.append( Resource( value ) );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#interpretedAs"), v );
}

QUrl Nepomuk::DataObject::interpretedAsUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#interpretedAs");
}

QList<Nepomuk::InformationElement> Nepomuk::DataObject::isPartOfs() const
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    return convertResourceList<InformationElement>( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#isPartOf") ).toResourceList() );
}

void Nepomuk::DataObject::setIsPartOfs( const QList<Nepomuk::InformationElement>& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    QList<Resource> l;
    for( QList<InformationElement>::const_iterator it = value.constBegin();
         it != value.constEnd(); ++it ) {
        l.append( Resource( (*it) ) );
    }
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#isPartOf"), Variant( l ) );
}

void Nepomuk::DataObject::addIsPartOf( const Nepomuk::InformationElement& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#isPartOf") );
    v.append( Resource( value ) );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#isPartOf"), v );
}

QUrl Nepomuk::DataObject::isPartOfUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#isPartOf");
}

QDateTime Nepomuk::DataObject::lastRefreshed() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#lastRefreshed") ).toDateTimeList() << QDateTime() ).first();
}

void Nepomuk::DataObject::setLastRefreshed( const QDateTime& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#lastRefreshed"), Variant( value ) );
}

QUrl Nepomuk::DataObject::lastRefreshedUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#lastRefreshed");
}

QList<Nepomuk::DataSource> Nepomuk::DataObject::dataSources() const
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    return convertResourceList<DataSource>( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#dataSource") ).toResourceList() );
}

void Nepomuk::DataObject::setDataSources( const QList<Nepomuk::DataSource>& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    QList<Resource> l;
    for( QList<DataSource>::const_iterator it = value.constBegin();
         it != value.constEnd(); ++it ) {
        l.append( Resource( (*it) ) );
    }
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#dataSource"), Variant( l ) );
}

void Nepomuk::DataObject::addDataSource( const Nepomuk::DataSource& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#dataSource") );
    v.append( Resource( value ) );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#dataSource"), v );
}

QUrl Nepomuk::DataObject::dataSourceUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#dataSource");
}

qint64 Nepomuk::DataObject::byteSize() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#byteSize") ).toInt64List() << 0 ).first();
}

void Nepomuk::DataObject::setByteSize( const qint64& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#byteSize"), Variant( value ) );
}

QUrl Nepomuk::DataObject::byteSizeUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#byteSize");
}

QList<Nepomuk::DataContainer> Nepomuk::DataObject::belongsToContainers() const
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    return convertResourceList<DataContainer>( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#belongsToContainer") ).toResourceList() );
}

void Nepomuk::DataObject::setBelongsToContainers( const QList<Nepomuk::DataContainer>& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    QList<Resource> l;
    for( QList<DataContainer>::const_iterator it = value.constBegin();
         it != value.constEnd(); ++it ) {
        l.append( Resource( (*it) ) );
    }
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#belongsToContainer"), Variant( l ) );
}

void Nepomuk::DataObject::addBelongsToContainer( const Nepomuk::DataContainer& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#belongsToContainer") );
    v.append( Resource( value ) );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#belongsToContainer"), v );
}

QUrl Nepomuk::DataObject::belongsToContainerUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#belongsToContainer");
}

QList<Nepomuk::Resource> Nepomuk::DataObject::dependsOf() const
{
    return convertResourceList<Resource>( ResourceManager::instance()->allResourcesWithProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#depends"), *this ) );
}

QList<Nepomuk::InformationElement> Nepomuk::DataObject::linksOf() const
{
    return convertResourceList<InformationElement>( ResourceManager::instance()->allResourcesWithProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#links"), *this ) );
}

QList<Nepomuk::InformationElement> Nepomuk::DataObject::relatedToOf() const
{
    return convertResourceList<InformationElement>( ResourceManager::instance()->allResourcesWithProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#relatedTo"), *this ) );
}

QList<Nepomuk::Contact> Nepomuk::DataObject::keyOf() const
{
    return convertResourceList<Contact>( ResourceManager::instance()->allResourcesWithProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#key"), *this ) );
}

QList<Nepomuk::OrganizationContact> Nepomuk::DataObject::logoOf() const
{
    return convertResourceList<OrganizationContact>( ResourceManager::instance()->allResourcesWithProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#logo"), *this ) );
}

QList<Nepomuk::Contact> Nepomuk::DataObject::photoOf() const
{
    return convertResourceList<Contact>( ResourceManager::instance()->allResourcesWithProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#photo"), *this ) );
}

QList<Nepomuk::Contact> Nepomuk::DataObject::soundOf() const
{
    return convertResourceList<Contact>( ResourceManager::instance()->allResourcesWithProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#sound"), *this ) );
}

QList<Nepomuk::Media> Nepomuk::DataObject::mediaStreamOf() const
{
    return convertResourceList<Media>( ResourceManager::instance()->allResourcesWithProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasMediaStream"), *this ) );
}

QList<Nepomuk::DataObject> Nepomuk::DataObject::allDataObjects()
{
    return Nepomuk::convertResourceList<DataObject>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#DataObject") ) );
}


