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
 
  /**
    * @file guicore/colors.h
    * Specifies some colors for using with Qt.
    */

#ifndef __QT_COLORS_H__
#define __QT_COLORS_H__

#if QT_VERSION < 0x040000
#include <qcolor.h>
#else
#include <QColor>
#endif

namespace guicore
{
  const QColor gc_Green( 18, 111, 10 );
  const QColor gc_Orange( 245, 169, 54 );
  const QColor gc_LightGrey( 232, 232, 232 );
  const QColor gc_MidGrey( 211, 211, 211 );
}

#endif //#ifndef __QT_COLORS_H__
