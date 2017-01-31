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

//#include "apexmultiprocedure.h"
#include "apexmultiprocedureconfig.h"
//#include "appcore/deleter.h"
#include <algorithm>

#include "apextools.h"

#include "xml/xercesinclude.h"
using namespace xercesc;
using namespace apex::data;

ApexMultiProcedureConfig::ApexMultiProcedureConfig(ApexProcedureParameters* p_parameters, DOMElement*)
                : ApexProcedureConfig(p_parameters)                    // we don't want trials to be parsed
{
    // FIXME: remove
}

ApexMultiProcedureConfig::ApexMultiProcedureConfig():
        ApexProcedureConfig()
{
}


ApexMultiProcedureConfig::~ApexMultiProcedureConfig()
{
        /*for ( tProcConfigList::const_iterator it= m_procedures.begin(); it!= m_procedures.end(); ++it) {

                ZeroizingDeleter() ( *it );

        }*/

    //[ stijn ] write short-hand
//   for_each( m_procedures.begin(), m_procedures.end(), appcore::ZeroizingDeleter() );
  qDeleteAll(m_procedures);
}


/**
 * Get any trial from all procedures
 * @param p_name
 * @return
 */
ApexTrial * ApexMultiProcedureConfig::GetTrial( const QString & p_name ) const
{
        // for each procedure
        for ( tProcConfigList::const_iterator it= m_procedures.begin(); it!= m_procedures.end(); ++it) {
                try {
                        return (*it)->GetTrial(p_name);
                } catch (ApexStringException) {

                }
        }

        throw ApexStringException("MultiProcedureConfig: trial not found");
}

const tProcConfigList& ApexMultiProcedureConfig::GetProcedures() const
{
	return m_procedures;
}



void ApexMultiProcedureConfig::ShowChildren()
{
    qDebug("ApexMultiProcedureConfig::ShowChildren");
    for ( tProcConfigList::const_iterator it= m_procedures.begin(); it!= m_procedures.end(); ++it) {

            qDebug("Child: %s",
                   qPrintable( (*it)->GetID()));

    }
    qDebug("------");
}

bool ApexMultiProcedureConfig::operator==(const ApexMultiProcedureConfig& other)
{
    return  ApexProcedureConfig::operator==(other) &&
            ApexTools::haveSameContents(m_procedures, other.m_procedures);
}

