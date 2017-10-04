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

#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_CONSTANTPROCEDUREDATAPARSER_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_CONSTANTPROCEDUREDATAPARSER_H_

#include "proceduredataparser.h"

namespace apex
{
namespace data
{
class ConstantProcedureData;
}

namespace parser
{

class ConstantProcedureDataParser : public ProcedureDataParser
{
public:
    ConstantProcedureDataParser();

    virtual ~ConstantProcedureDataParser(){};

    /**
     * Parse the given xml data structure into the given parameters structure
     * does not take ownership of p_parameters
     */
    virtual void Parse(const QDomElement &p_base,
                       data::ConstantProcedureData *p_data);

protected:
    virtual bool SetParameter(const QString &tag, const QString &id,
                              const QString &value, const QDomElement &node,
                              data::ProcedureData *data) Q_DECL_OVERRIDE;
};
}
}

#endif
