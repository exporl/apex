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

#ifndef APEXAPEXPROCEDURECONFIG_H
#define APEXAPEXPROCEDURECONFIG_H

#include <vector>
#include <memory>

//from libdata
#include "apexprocedureparameters.h"
#include "apextrial.h"

//from libtools
#include "global.h"

namespace apex
{

namespace data
{
/**
Contains all configuration information for a procedure

@author Tom Francart,,,
*/
class APEXDATA_EXPORT ApexProcedureConfig
{

    public:
        ApexProcedureConfig(ApexProcedureParameters* p_parameters/*, XERCES_CPP_NAMESPACE::DOMElement* p_base, ApexProcedureFactory* p_fact=0*/);
        ApexProcedureConfig();
        virtual ~ApexProcedureConfig();

        virtual bool AddTrial(ApexTrial* p_trial);
        virtual ApexTrial* GetTrial(const QString& p_name) const;
        virtual ApexProcedureParameters* GetParameters();
        virtual const ApexProcedureParameters& GetParameters() const;

        virtual const std::vector<ApexTrial*>& GetTrials() const {return m_trials;};


#ifdef PRINTPROCEDURECONFIG
        void SetID(const QString& p_id) { qDebug("ProcedureConfig: Set id: "+p_id); m_id=p_id;};
        const QString& GetID() const { qDebug("ProcedureConfig: Get id: "+m_id); return m_id; };
#else
        void SetID(const QString& p_id) { m_id=p_id;};
        const QString& GetID() const { return m_id; };
#endif

        /**
         * We take ownership of the given pointer
         */
        void SetParameters(ApexProcedureParameters* p);

        virtual bool operator==(const ApexProcedureConfig& other) const;

    protected:
        tTrialList m_trials;           // TODO Maybe change to QObjectList for QtScript
        std::auto_ptr<ApexProcedureParameters> m_parameters;
        QString m_id;

};

}
}
#endif
