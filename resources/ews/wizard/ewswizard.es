/*
    Copyright (c) 2017 Montel Laurent <montel@kde.org>

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

var page = Dialog.addPage( "ewswizard.ui", qsTr("Personal Settings") );

//function validateInput()
//{
//  if ( page.widget().maildirPath.text == "" ) {
//    page.setValid( false );
//  } else {
//    page.setValid( true );
//  }
//}

function setup()
{
  var maildirRes = SetupManager.createResource( "akonadi_ews_resource" );
  //maildirRes.setOption( "Path", page.widget().maildirPath.text );

  SetupManager.execute();
}

//page.widget().maildirPath.textChanged.connect( validateInput );
page.pageLeftNext.connect( setup );
validateInput();
