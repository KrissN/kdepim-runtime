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
#include "archive.h"


Nepomuk::Archive::Archive()
  : DataContainer( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Archive") )
{
}


Nepomuk::Archive::Archive( const Archive& res )
  : DataContainer( res )
{
}


Nepomuk::Archive::Archive( const Nepomuk::Resource& res )
  : DataContainer( res )
{
}


Nepomuk::Archive::Archive( const QString& uri )
  : DataContainer( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Archive") )
{
}

Nepomuk::Archive::Archive( const QUrl& uri )
  : DataContainer( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Archive") )
{
}

Nepomuk::Archive::Archive( const QString& uri, const QUrl& type )
  : DataContainer( uri, type )
{
}

Nepomuk::Archive::Archive( const QUrl& uri, const QUrl& type )
  : DataContainer( uri, type )
{
}

Nepomuk::Archive::~Archive()
{
}


Nepomuk::Archive& Nepomuk::Archive::operator=( const Archive& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::Archive::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Archive";
}

QList<qint64> Nepomuk::Archive::uncompressedSizes() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#uncompressedSize") ).toInt64List());
}

void Nepomuk::Archive::setUncompressedSizes( const QList<qint64>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#uncompressedSize"), Variant( value ) );
}

void Nepomuk::Archive::addUncompressedSize( const qint64& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#uncompressedSize") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#uncompressedSize"), v );
}

QUrl Nepomuk::Archive::uncompressedSizeUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#uncompressedSize");
}

QList<Nepomuk::Archive> Nepomuk::Archive::allArchives()
{
    return Nepomuk::convertResourceList<Archive>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Archive") ) );
}


