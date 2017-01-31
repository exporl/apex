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

#include "apexmodule.h"
#include "apexcontrol.h"

using namespace apex;

/**
 * Returns the result of the module in XML format
 * @return
 */
QString ApexModule::GetResultXML( ) const
{
        return tr("<undef/>");
}

/**
 * Last words to the XML file after experiment is finished
 * @return
 */
QString apex::ApexModule::GetEndXML( ) const
{
        return QString();
}


apex::ApexModule::ApexModule( ExperimentRunDelegate & p_rd ):
        m_rd(p_rd)
{

}
