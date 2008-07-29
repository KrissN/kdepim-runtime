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
#include "presentation.h"


Nepomuk::Presentation::Presentation()
  : Document( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Presentation") )
{
}


Nepomuk::Presentation::Presentation( const Presentation& res )
  : Document( res )
{
}


Nepomuk::Presentation::Presentation( const Nepomuk::Resource& res )
  : Document( res )
{
}


Nepomuk::Presentation::Presentation( const QString& uri )
  : Document( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Presentation") )
{
}

Nepomuk::Presentation::Presentation( const QUrl& uri )
  : Document( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Presentation") )
{
}

Nepomuk::Presentation::Presentation( const QString& uri, const QUrl& type )
  : Document( uri, type )
{
}

Nepomuk::Presentation::Presentation( const QUrl& uri, const QUrl& type )
  : Document( uri, type )
{
}

Nepomuk::Presentation::~Presentation()
{
}


Nepomuk::Presentation& Nepomuk::Presentation::operator=( const Presentation& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::Presentation::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Presentation";
}

QList<Nepomuk::Presentation> Nepomuk::Presentation::allPresentations()
{
    return Nepomuk::convertResourceList<Presentation>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Presentation") ) );
}


