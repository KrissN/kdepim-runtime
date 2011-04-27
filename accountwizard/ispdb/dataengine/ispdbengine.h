/*
    Copyright 2011 Sebastian Kügler <sebas@kde.org>

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


#ifndef ISPDBENGINE_H
#define ISPDBENGINE_H

#include <plasma/dataengine.h>


class IspdbEnginePrivate;
struct server;

class IspdbEngine : public Plasma::DataEngine
{
    Q_OBJECT

    public:
        IspdbEngine(QObject* parent, const QVariantList& args);
        ~IspdbEngine();
        QStringList sources() const;
        virtual void init();


    protected:
        bool sourceRequestEvent(const QString &name);

    private:
        QString populateSource(server s, const QString &protocol);
        IspdbEnginePrivate* d;
public slots:
    void onIspDbRequestFinished(bool);
};

K_EXPORT_PLASMA_DATAENGINE(metadataengine, IspdbEngine)

#endif
