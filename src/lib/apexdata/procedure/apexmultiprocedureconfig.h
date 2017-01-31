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

#ifndef APEXAPEXMULTIPROCEDURECONFIG_H
#define APEXAPEXMULTIPROCEDURECONFIG_H

#include <vector>

//from libtools
#include "global.h"

//from libdata
#include "procedure/apexprocedureconfig.h"

namespace apex
{
namespace data
{
typedef std::vector<ApexProcedureConfig*> tProcConfigList;

/**
@author Tom Francart,,,
*/
class APEXDATA_EXPORT ApexMultiProcedureConfig : public ApexProcedureConfig
{
        friend class ApexMultiProcedure;
        friend class ApexMultiProcedureFactory;

    public:
        ApexMultiProcedureConfig(ApexProcedureParameters* p_parameters, XERCES_CPP_NAMESPACE::DOMElement* p_base);
        ApexMultiProcedureConfig();

        void ShowChildren();

        ~ApexMultiProcedureConfig();

        void AddProcedureConfig(ApexProcedureConfig* proc)
        {
            Q_CHECK_PTR(proc);
            qDebug("Adding child to multiprocedureconfig");
            m_procedures.push_back(proc);
        };

        ApexTrial* GetTrial(const QString& p_name) const;
        const tProcConfigList& GetProcedures() const;
        
        bool operator==(const ApexMultiProcedureConfig& other);

    private:
        tProcConfigList m_procedures;
        virtual std::vector<ApexTrial*>& GetTrials() {return m_trials;};

};

}
}
#endif
