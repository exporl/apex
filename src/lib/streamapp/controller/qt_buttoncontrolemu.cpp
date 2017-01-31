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
 
#include "qt_buttoncontrolemu.h"

#ifdef _MSC_VER
#pragma warning ( disable : 4512 )  //assignment operator in qt files
#endif

#include <qglobal.h>
#ifdef USESQT
#if QT_VERSION < 0x040000
#include <qt.h>
#else
#include <Qt>
#endif
#endif

using namespace controller;

QtButtonControlEmu::QtButtonControlEmu() :
  mv_nNewKey( mc_eNoButton )
{
}

QtButtonControlEmu::~QtButtonControlEmu()
{
}

bool QtButtonControlEmu::mf_bPoll( mt_eControlCode& a_Code )
{
  a_Code = mv_nNewKey;
  mv_nNewKey = mc_eNoButton;
  return true;
}

void QtButtonControlEmu::mf_KeyPressed( const int ac_nKey )
{
  switch( ac_nKey )
  {
    case Qt::Key_Left : mv_nNewKey = mc_eLeft; break;
    case Qt::Key_Right : mv_nNewKey = mc_eRight; break;
    case Qt::Key_Up : mv_nNewKey = mc_eDown; break;
    case Qt::Key_Down : mv_nNewKey = mc_eUp; break;
    case Qt::Key_Escape : mv_nNewKey = mc_ePower; break;
    case Qt::Key_Enter : mv_nNewKey = mc_eSelect; break;
    case Qt::Key_Return : mv_nNewKey = mc_eSelect; break;
    default: break;
  };
}
