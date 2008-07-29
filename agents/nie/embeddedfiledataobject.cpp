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
#include "embeddedfiledataobject.h"


Nepomuk::EmbeddedFileDataObject::EmbeddedFileDataObject()
  : FileDataObject( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#EmbeddedFileDataObject") )
{
}


Nepomuk::EmbeddedFileDataObject::EmbeddedFileDataObject( const EmbeddedFileDataObject& res )
  : FileDataObject( res )
{
}


Nepomuk::EmbeddedFileDataObject::EmbeddedFileDataObject( const Nepomuk::Resource& res )
  : FileDataObject( res )
{
}


Nepomuk::EmbeddedFileDataObject::EmbeddedFileDataObject( const QString& uri )
  : FileDataObject( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#EmbeddedFileDataObject") )
{
}

Nepomuk::EmbeddedFileDataObject::EmbeddedFileDataObject( const QUrl& uri )
  : FileDataObject( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#EmbeddedFileDataObject") )
{
}

Nepomuk::EmbeddedFileDataObject::EmbeddedFileDataObject( const QString& uri, const QUrl& type )
  : FileDataObject( uri, type )
{
}

Nepomuk::EmbeddedFileDataObject::EmbeddedFileDataObject( const QUrl& uri, const QUrl& type )
  : FileDataObject( uri, type )
{
}

Nepomuk::EmbeddedFileDataObject::~EmbeddedFileDataObject()
{
}


Nepomuk::EmbeddedFileDataObject& Nepomuk::EmbeddedFileDataObject::operator=( const EmbeddedFileDataObject& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::EmbeddedFileDataObject::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#EmbeddedFileDataObject";
}

QStringList Nepomuk::EmbeddedFileDataObject::encodings() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#encoding") ).toStringList());
}

void Nepomuk::EmbeddedFileDataObject::setEncodings( const QStringList& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#encoding"), Variant( value ) );
}

void Nepomuk::EmbeddedFileDataObject::addEncoding( const QString& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#encoding") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#encoding"), v );
}

QUrl Nepomuk::EmbeddedFileDataObject::encodingUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#encoding");
}

QList<Nepomuk::EmbeddedFileDataObject> Nepomuk::EmbeddedFileDataObject::allEmbeddedFileDataObjects()
{
    return Nepomuk::convertResourceList<EmbeddedFileDataObject>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#EmbeddedFileDataObject") ) );
}


