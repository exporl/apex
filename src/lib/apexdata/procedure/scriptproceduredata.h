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

#ifndef _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_SCRIPTPROCEDUREDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_SCRIPTPROCEDUREDATA_H_

#include "proceduredata.h"

#include <QList>

namespace apex
{

namespace parser
{
class ScriptProcedureDataParser;
}
namespace data
{
struct tScriptProcedureParameter {
    QString name;
    QString value;
    tScriptProcedureParameter(const QString &p_name, const QString &p_value);

    bool operator==(const tScriptProcedureParameter &other) const;
};

typedef QList<tScriptProcedureParameter> tScriptProcedureParameterList;

/**
@author Tom Francart,,,
*/
class APEXDATA_EXPORT ScriptProcedureData : public ProcedureData
{
public:
    ScriptProcedureData();

    ~ScriptProcedureData();

    const QString script() const;
    void setScript(const QString &p);
    const QString adjustParameter() const;
    void setAdjustParameter(const QString &p);
    bool debugger() const;
    void setDebugger(bool p);

    virtual Type type() const;
    virtual QString name() const;

    //! Return all parameters defined via <parameter
    //! name="bla">value</parameter> in the apex:/procedure/parameters section
    const tScriptProcedureParameterList &customParameters() const;
    void appendParameter(const QString &name, const QString &value);

    bool operator==(const ScriptProcedureData &other) const;

private:
    QString m_script;
    QString m_adjust_parameter;
    bool m_debugger;

    tScriptProcedureParameterList m_paramList;
};
}
}

#endif
