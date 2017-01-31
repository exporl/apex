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

#ifndef APEX_PARSERDATABLOCKSPARSER_H
#define APEX_PARSERDATABLOCKSPARSER_H

#include "parser/apexparser.h"
#include "fileprefix.h"


#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
    class DOMElement;
    class DOMDocument;
};

class QWidget;

namespace apex {

    namespace data {
        class DatablocksData;
        class DatablockData;
    }

namespace parser {

/**
Parses the <datablocks> element

    @author Tom Francart,,, <tom.francart@med.kuleuven.be>
*/
class APEX_EXPORT DatablocksParser : public Parser
{
public:
    /**
     * parent: parent widget for showing dialogs, if 0 no dialogs will be shown
     */
    DatablocksParser(QWidget* parent=0);

    ~DatablocksParser();

    data::DatablocksData Parse(XERCES_CPP_NAMESPACE::DOMElement* dom,
                               const QString& scriptLibraryFile, const QVariantMap& scriptParameters );

    data::DatablockData* ParseDatablock(
                                        XERCES_CPP_NAMESPACE::DOMElement* p_datablock,
                                         const data::FilePrefix p_prefix );

    private:
        QWidget* m_parent;

};

}

}

#endif
