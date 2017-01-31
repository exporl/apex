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
 
#ifndef __SYNTHGUICOLORS_H__
#define __SYNTHGUICOLORS_H__

#if QT_VERSION < 0x040000
#include <qcolor.h>
#else
#include <QColor>
#endif

namespace synthgui
{

  const QColor gc_Socket( 55, 160, 180 );
  const QColor gc_SocketBG( 192, 192, 193 );
  const QColor gc_SocketFG( 180, 180, 180 );
  const QColor gc_SocketUFG( 0, 0, 0 );
  const QColor gc_SocketCFG( 50, 190, 60 );
  const QColor gc_SocketText( 180, 50, 60 );

  const QColor gc_ItemBG( 46, 136, 148 );
  const QColor gc_ItemLabelBG( 54, 158, 172 );

  const QColor gc_CableColor( 20, 190, 40 );

}

#endif //#ifndef __SYNTHGUICOLORS_H__
