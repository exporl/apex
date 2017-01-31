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
 
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "apextypedefs.h"

class QString;

namespace apex
{
  namespace editor
  {
    using gui::tScreenElementMap;

    /**
     * Find an id for a ScreenElement.  The given base string will be
     * used, stripped of all spaces, and then a number will be added
     * at the end, starting with 1.  The first id that is not present
     * in the given map takenIDs will be returned.
     */
    QString findFreeID( const QString& base, const tScreenElementMap& takenIDs );

    /**
     * Shorten a given string, and return a string of length smaller
     * than the given length.  If the given string is too long,
     * characters will be removed from it at the start, and replaced
     * by three dots ( "..." ).
     */
    QString shortenString( const QString& s, int length );

  }
}
#endif
