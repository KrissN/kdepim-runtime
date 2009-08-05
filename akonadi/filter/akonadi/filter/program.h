/****************************************************************************** *
 *
 *  File : program.h
 *  Created on Thu 07 May 2009 13:30:16 by Szymon Tomasz Stefanek
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
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *******************************************************************************/

#ifndef _AKONADI_FILTER_PROGRAM_H_
#define _AKONADI_FILTER_PROGRAM_H_

#include <akonadi/filter/config-akonadi-filter.h>

#include <QtCore/QList>
#include <QtCore/QString>

#include <akonadi/filter/component.h>
#include <akonadi/filter/data.h>
#include <akonadi/filter/propertybag.h>
#include <akonadi/filter/rulelist.h>

namespace Akonadi
{
namespace Filter
{

/**
 * A complete filtering program.
 *
 * The program is made up of a list of rules that are applied in sequence.
 */
class AKONADI_FILTER_EXPORT Program : public Action::RuleList, public PropertyBag
{
public:

  /**
   * Creates an empty filtering program.
   */
  Program();

  /**
   * Destroys the filtering program including any included rules.
   */
  virtual ~Program();

public:

  /**
   * Returns the name of this program.
   * This is equivalent to property( QString::fromAscii( "name" ) ).
   */
  QString name() const;

  /**
   * Sets the user supplied name of this filtering program.
   * This is equivalent to setProperty( QString::fromAscii( "name" ), name ).
   */
  void setName( const QString &name );

  /**
   * Reimplemented from Component. Returns true.
   */
  virtual bool isProgram() const;

  /**
   * Reimplemented from Component: dumps the program on the console.
   */
  virtual void dump( const QString &prefix );

}; // class Program

} // namespace Filter

} // namespace Akonadi

#endif //!_AKONADI_FILTER_PROGRAM_H_
