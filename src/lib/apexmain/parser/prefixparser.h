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

#ifndef APEX_PARSERPREFIXPARSER_H
#define APEX_PARSERPREFIXPARSER_H

#include "parser/apexparser.h"
#include "fileprefix.h"

#include <QCoreApplication> //tr

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
    class DOMElement;
};

namespace apex {

    namespace data {
        class FilePrefix;
    }

namespace parser {

/**
	@author Tom Francart,,, <tom.francart@med.kuleuven.be>
*/
class PrefixParser : public Parser
{
    Q_DECLARE_TR_FUNCTIONS(PrefixParser)
public:
    PrefixParser();

    ~PrefixParser();

//    static const QString Parse(XERCES_CPP_NAMESPACE::DOMElement* currentNode);

    static data::FilePrefix Parse(XERCES_CPP_NAMESPACE::DOMElement* currentNode);

};

}

}

#endif
