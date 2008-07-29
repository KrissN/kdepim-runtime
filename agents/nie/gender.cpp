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
#include "gender.h"

#include "personcontact.h"
Nepomuk::Gender::Gender()
  : Resource( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Gender") )
{
}


Nepomuk::Gender::Gender( const Gender& res )
  : Resource( res )
{
}


Nepomuk::Gender::Gender( const Nepomuk::Resource& res )
  : Resource( res )
{
}


Nepomuk::Gender::Gender( const QString& uri )
  : Resource( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Gender") )
{
}

Nepomuk::Gender::Gender( const QUrl& uri )
  : Resource( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Gender") )
{
}

Nepomuk::Gender::Gender( const QString& uri, const QUrl& type )
  : Resource( uri, type )
{
}

Nepomuk::Gender::Gender( const QUrl& uri, const QUrl& type )
  : Resource( uri, type )
{
}

Nepomuk::Gender::~Gender()
{
}


Nepomuk::Gender& Nepomuk::Gender::operator=( const Gender& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::Gender::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Gender";
}

QList<Nepomuk::PersonContact> Nepomuk::Gender::genderOf() const
{
    return convertResourceList<PersonContact>( ResourceManager::instance()->allResourcesWithProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#gender"), *this ) );
}

QList<Nepomuk::Gender> Nepomuk::Gender::allGenders()
{
    return Nepomuk::convertResourceList<Gender>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Gender") ) );
}


