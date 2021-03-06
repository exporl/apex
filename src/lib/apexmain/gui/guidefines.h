/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_GUIDEFINES_H_
#define _EXPORL_SRC_LIB_APEXMAIN_GUI_GUIDEFINES_H_

#include <QColor>
#include <QString>

namespace apex
{
namespace gui
{
// const QColor      sc_DefaultBGColor( 32, 27, 122 );
const QColor sc_DefaultBGColor(0x13, 0xa8, 0x9e);
const QColor sc_DefaultPanelColor(212, 208, 200);
const QString sc_sDefaultFont("Tahoma" /*"Helvetica"*/);
const QString sc_sDefaultElementFont("Helvetica");
const unsigned sc_nDefaultFontSize = 20;
const unsigned sc_nMaxElementsInRow = 30;
const unsigned sc_nMaxElementsInCol = 30;
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_GUIDEFINES_H_
