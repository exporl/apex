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
 
#ifndef __SCREENDEFINES_H__
#define __SCREENDEFINES_H__

#if QT_VERSION < 0x040000
#include <qcolor.h>
#include <qstring.h>
#else
#include <QColor>
#include <QString>
#endif

namespace apex
{
  namespace gui
  {
    const QColor      sc_DefaultBGColor( 32, 27, 122 );
    const QColor      sc_DefaultPanelColor( 212, 208, 200 );
    const QString     sc_sDefaultFont( "Tahoma"/*"Helvetica"*/ );
    const QString     sc_sDefaultElementFont( "Helvetica" );
    const unsigned    sc_nDefaultFontSize   = 20;
    const unsigned    sc_nMaxElementsInRow  = 30;
    const unsigned    sc_nMaxElementsInCol  = 30;
  }
}

#endif //#ifndef __SCREENDEFINES_H__
