/****************************************************************************** * *
 *
 *  File : extensionlabel.h
 *  Created on Fri 15 May 2009 04:53:16 by Szymon Tomasz Stefanek
 *
 *  This file is part of the Akonadi Filtering Framework
 *
 *  Copyright 2009 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the editoried warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *******************************************************************************/

#ifndef _AKONADI_FILTER_UI_PRIVATE_EXTENSIONLABEL_H_
#define _AKONADI_FILTER_UI_PRIVATE_EXTENSIONLABEL_H_

#include "config-akonadi-filter-ui.h"

#include <QWidget>
#include <QColor>

class QPaintEvent;

namespace Akonadi
{
namespace Filter
{
namespace UI
{
namespace Private
{

class ExtensionLabel : public QWidget
{
  Q_OBJECT
public:
  ExtensionLabel( QWidget * parent );
protected:
  qreal mOpacity;
  QColor mOverlayColor;
  int mFixedHeight;
public:
  void setFixedHeight( int h );
  int fixedHeight()
  {
    return mFixedHeight;
  }
  void setOpacity( qreal opacity );
  qreal opacity()
  {
    return mOpacity;
  }
  void setOverlayColor( const QColor &clr );
  QColor overlayColor()
  {
    return mOverlayColor;
  }
  virtual void paintEvent( QPaintEvent * e );
}; // class ExtensionLabel

} // namespace Private

} // namespace UI

} // namespace Filter

} // namespace Akonadi

#endif //!_AKONADI_FILTER_UI_PRIVATE_EXTENSIONLABEL_H_
