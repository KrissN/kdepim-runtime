/****************************************************************************** *
 *
 *  File : decoder.h
 *  Created on Sun 03 May 2009 12:10:16 by Szymon Tomasz Stefanek
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

#ifndef _AKONADI_FILTER_IO_DECODER_H_
#define _AKONADI_FILTER_IO_DECODER_H_

#include "config-akonadi-filter.h"

#include <QString>

namespace Akonadi
{
namespace Filter
{

class Factory;

namespace IO
{

class AKONADI_FILTER_EXPORT Decoder
{
public:
  Decoder( Factory * componentFactory );
  virtual ~Decoder();
protected:
  Factory * mFactory;
  QString mLastError;

public:

  /**
   * Returns the last error occured in this component execution run.
   */
  const QString & lastError() const
  {
    return mLastError;
  }

  void setLastError( const QString &error )
  {
    mLastError = error;
  }

  Factory * factory() const
  {
    return mFactory;
  }
};

} // namespace IO

} // namespace Filter

} // namespace Akonadi

#endif //!_AKONADI_FILTER_IO_DECODER_H_
