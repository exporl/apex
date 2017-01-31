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

#ifndef APEXPluginProcedurePARAMETERS_H
#define APEXPluginProcedurePARAMETERS_H

#include "apexprocedureparameters.h"

#include <map>
#include <QList>

namespace apex
{

namespace parser
{
class ApexPluginProcedureParametersParser;
}
namespace data
{
struct tPluginProcedureParameter
{
    QString name;
    QString value;
    tPluginProcedureParameter(const QString& p_name, const QString& p_value):
            name(p_name),
            value(p_value) { }

    bool operator==(const tPluginProcedureParameter& other) const;
};

typedef QList<tPluginProcedureParameter> tPluginProcedureParameterList;

/**
@author Tom Francart,,,
*/
class APEXDATA_EXPORT PluginProcedureParameters : public ApexProcedureParameters
{
    public:
        PluginProcedureParameters(XERCES_CPP_NAMESPACE::DOMElement* p_paramElement);
        PluginProcedureParameters();

        ~PluginProcedureParameters();

        friend class PluginProcedure;
        friend class PluginProcedureParametersParser;

        const QString GetScript() const { return m_script; };
        const QString GetAdjustParameter() const { return m_adjust_parameter;};
        bool  GetDebugger() const { return m_debugger; };

        virtual t_type GetType() const;


        //! Return all parameters defined via <parameter name="bla">value</parameter> in the apex:/procedure/parameters section
        const tPluginProcedureParameterList& GetCustomParameters() const { return m_paramList;};

        bool operator==(const PluginProcedureParameters& other) const;

    private:
        virtual bool SetParameter(const QString& p_name, const QString& p_id, const QString& p_value, XERCES_CPP_NAMESPACE::DOMElement*);


        friend class parser::ApexPluginProcedureParametersParser;

    private:
        QString m_script;
        QString m_adjust_parameter;
        bool m_debugger;

        tPluginProcedureParameterList m_paramList;

};

}
}

#endif
