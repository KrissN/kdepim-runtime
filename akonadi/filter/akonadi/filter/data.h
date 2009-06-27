/****************************************************************************** *
 *
 *  File : data.h
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

#ifndef _AKONADI_FILTER_DATA_H_
#define _AKONADI_FILTER_DATA_H_

#include <akonadi/filter/config-akonadi-filter.h>

#include <QtCore/QVariant>

#include <akonadi/filter/datatype.h>

namespace Akonadi
{
namespace Filter
{

class FunctionDescriptor;
class DataMemberDescriptor;

class AKONADI_FILTER_EXPORT Data
{
public:
  Data();
  virtual ~Data();

public:
  virtual QVariant getPropertyValue( const FunctionDescriptor * function, const DataMemberDescriptor * dataMember );

protected:
  virtual QVariant getDataMemberValue( const DataMemberDescriptor * dataMember );
};

} // namespace Filter

} // namespace Akonadi

#endif //!_AKONADI_FILTER_DATA_H_
