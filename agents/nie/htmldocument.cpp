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
#include "htmldocument.h"


Nepomuk::HtmlDocument::HtmlDocument()
  : PlainTextDocument( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#HtmlDocument") )
{
}


Nepomuk::HtmlDocument::HtmlDocument( const HtmlDocument& res )
  : PlainTextDocument( res )
{
}


Nepomuk::HtmlDocument::HtmlDocument( const Nepomuk::Resource& res )
  : PlainTextDocument( res )
{
}


Nepomuk::HtmlDocument::HtmlDocument( const QString& uri )
  : PlainTextDocument( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#HtmlDocument") )
{
}

Nepomuk::HtmlDocument::HtmlDocument( const QUrl& uri )
  : PlainTextDocument( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#HtmlDocument") )
{
}

Nepomuk::HtmlDocument::HtmlDocument( const QString& uri, const QUrl& type )
  : PlainTextDocument( uri, type )
{
}

Nepomuk::HtmlDocument::HtmlDocument( const QUrl& uri, const QUrl& type )
  : PlainTextDocument( uri, type )
{
}

Nepomuk::HtmlDocument::~HtmlDocument()
{
}


Nepomuk::HtmlDocument& Nepomuk::HtmlDocument::operator=( const HtmlDocument& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::HtmlDocument::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#HtmlDocument";
}

QList<Nepomuk::HtmlDocument> Nepomuk::HtmlDocument::allHtmlDocuments()
{
    return Nepomuk::convertResourceList<HtmlDocument>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#HtmlDocument") ) );
}


