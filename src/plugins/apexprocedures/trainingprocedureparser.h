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

#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_TRAININGPROCEDUREPARSER_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_TRAININGPROCEDUREPARSER_H_

#include "procedureparsersparent.h"

namespace apex
{

class ProcedureData;

namespace parser
{

class TrainingProcedureParser : public ProcedureParsersParent
{
public:
    TrainingProcedureParser();

    data::ProcedureData *parse(const QDomElement &base);

protected:
    virtual bool trialsValid();
    virtual void SetProcedureParameters(const QDomElement &p_base);
};
}
}

#endif
