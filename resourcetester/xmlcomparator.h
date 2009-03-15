/*
    Copyright (c) 2009 Volker Krause <vkrause@kde.org>

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

#ifndef AKONADI_XMLCOMPARATOR_H
#define AKONADI_XMLCOMPARATOR_H

#include <akonadi/collection.h>
#include <akonadi/item.h>
#include <akonadi/xml/xmldocument.h>

#include <QtCore/QObject>
#include <QtCore/QTextStream>

/**
  Compares a Akonadi collection sub-tree with reference data supplied in an XML file.
*/
class XmlComparator : public QObject
{
  Q_OBJECT
  public:
    XmlComparator( QObject *parent = 0 );
    ~XmlComparator();

    void setRootCollections( const QString &resourceId );
    void setRootCollections( const Akonadi::Collection::List &roots );
    void setXmlFile( const QString &fileName );

    bool compare();

    QString lastError() const;

  private:
    bool compareCollections( const Akonadi::Collection::List &cols, const Akonadi::Collection::List &refCols );
    bool compareCollection( const Akonadi::Collection &col, const Akonadi::Collection &refCol );
    bool compareItems( const Akonadi::Item::List &items, const Akonadi::Item::List &refItems );
    bool compareItem( const Akonadi::Item &item, const Akonadi::Item &refItem );
    bool compareAttributes( const Akonadi::Entity &entity, const Akonadi::Entity &refEntity );

    template <typename T> bool compareValue( const Akonadi::Collection &col, const Akonadi::Collection &refCol,
                                             T (Akonadi::Collection::*property)() const,
                                             const char* propertyName );
    template <typename T> bool compareValue( const Akonadi::Item& item, const Akonadi::Item& refItem,
                                             T (Akonadi::Item::*property)() const,
                                             const char* propertyName );
    template <typename T> bool compareValue( const T& value, const T& refValue );

  private:
    Akonadi::Collection::List mRoots;
    Akonadi::XmlDocument mDocument;
    QString mErrorMsg;
};


template <typename T>
bool XmlComparator::compareValue( const Akonadi::Collection& col, const Akonadi::Collection& refCol,
                                  T (Akonadi::Collection::*property)() const,
                                  const char* propertyName )
{
  const bool result = compareValue<T>( ((col).*(property))(), ((refCol).*(property))() );
  if ( !result ) {
    mErrorMsg.prepend( QString::fromLatin1( "Collection with remote id '%1' differs in property '%2':\n" )
    .arg( col.remoteId() ).arg( propertyName ) );
  }
  return result;
}

template <typename T>
bool XmlComparator::compareValue( const Akonadi::Item& item, const Akonadi::Item& refItem,
                                  T (Akonadi::Item::*property)() const,
                                  const char* propertyName )
{
  const bool result = compareValue<T>( ((item).*(property))(), ((refItem).*(property))() );
  if ( !result ) {
    mErrorMsg.prepend( QString::fromLatin1( "Item with remote id '%1' differs in property '%2':\n" )
      .arg( item.remoteId() ).arg( propertyName ) );
  }
  return result;
}

template <typename T>
bool XmlComparator::compareValue(const T& value, const T& refValue )
{
  if ( value == refValue )
    return true;
  QTextStream ts( &mErrorMsg );
  ts << " Actual: " << value << endl << " Expected: " << refValue;
  return false;
}

#endif
