/*
    Copyright (c) 2007 Tobias Koenig <tokoe@kde.org>

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

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include "vcardresource.h"

static KCmdLineOptions options[] =
{
  { "identifier <argument>", "Resource identifier", 0 },
  KCmdLineLastOption
};

int main( int argc, char **argv )
{
  KCmdLineArgs::init( argc, argv, argv[0], I18N_NOOP("VCard Resource"),
                      I18N_NOOP("VCard Resource for Akonadi") ,"0.1" );
  KCmdLineArgs::addCmdLineOptions( options );
  KApplication app;

  Akonadi::ResourceBase::init<VCardResource>();

  return app.exec();
}
