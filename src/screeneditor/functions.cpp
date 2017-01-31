/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/
 
#include "functions.h"

namespace apex
{
  namespace editor
  {
    QString findFreeID( const QString& base, const tScreenElementMap& takenIDs )
    {
      QString s = base;
      s.replace( ' ', QString() );
      int i = 1;
      while ( true )
      {
        QString testId = s + QString::number( i++ );
        if ( takenIDs.find( testId ) == takenIDs.end() )
          return testId;
      }
    }

    QString shortenString( const QString& s, int l )
    {
      int oldlen = s.length();
      if ( oldlen <= l )
        return s;
      else
      {
        QString ret = s.right( l - 3 );
        return "..." + ret;
      }
    }
  }
}
