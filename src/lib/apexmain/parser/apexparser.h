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
 
#ifndef APEX_PARSERPARSER_H
#define APEX_PARSERPARSER_H

#include <QObject>

//from libtools
#include "xml/apexxmltools.h"
#include "status/errorlogger.h"

namespace apex {

namespace parser {

/**
All parsers should derive from this base

  @author Tom Francart,,, <tom.francart@med.kuleuven.be>
*/
    class APEX_EXPORT Parser : public ErrorLogger
{
public:
    Parser();

    virtual ~Parser();

    void AddStatusMessage( QString p_message );

};

}
}

#endif
