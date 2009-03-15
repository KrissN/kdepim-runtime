/*
    Copyright (c) 2009 Volker Krause <vkrause@kde.org>
    Copyright (c) 2009 Igor Trindade Oliveira <igor_trindade@yahoo.com.br>

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

#ifndef RESOURCE_H
#define RESOURCE_H

#include <akonadi/agentinstance.h>

#include <QObject>
#include <QHash>
#include <QVariant>

class Resource: public QObject
{
  Q_OBJECT
  public:
    Resource( QObject *parent = 0 );

  public slots:
    void setType( const QString &type );
    QString identifier() const;

    void setOption( const QString &key, const QVariant &value );

    bool create();
    void destroy();

  private:
    QString mTypeIdentifier;
    Akonadi::AgentInstance mInstance;
    QHash<QString, QVariant> mSettings;
};   

#endif
