/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_PROCEDUREPARSERSPARENT_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_PROCEDUREPARSERSPARENT_H_

#include "apexdata/procedure/procedureinterface.h"

namespace apex
{
namespace data
{
class ProcedureData;
}

namespace parser
{

class ProcedureParsersParent : public ProcedureParserInterface
{
    Q_DECLARE_TR_FUNCTIONS(ProcedureParsersParent)
public:
    ProcedureParsersParent();

    void Parse(const QDomElement &p_base, data::ProcedureData *c);

protected:
    /*!
     * If you want to implement extra checks regarding the trials, for
     * example you want to check if the list of trials is not empty, then
     * you should do this in this function.
     * \return true if the trials are good; false otherwise
     */
    virtual bool trialsValid() = 0;
    virtual void ParseTrials(const QDomElement &p_parent);

    data::ProcedureData *currentConfig;

    virtual void SetProcedureParameters(const QDomElement &p_base) = 0;
};
}
}

#endif
