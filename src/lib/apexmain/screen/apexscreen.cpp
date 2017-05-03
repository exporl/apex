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

#include "apexdata/screen/screenresult.h"

#include "gui/mainwindow.h"

#include "apexcontrol.h"
#include "apexscreen.h"
#include "experimentparser.h"

using namespace apex;
using namespace apex::gui;

void ApexScreen::iAnswered(const ScreenResult* result) {
    qCDebug(APEX_RS, "***iAnswered");
    m_lastResult = *result;
    ApexControl::Get().mainWindow()->EnableScreen(false);
    Q_EMIT( Answered(result));
}

QString apex::ApexScreen::GetResultXML( ) const
{
    return m_lastResult.toXML();
}
