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

#include "stimulitester.h"
#include "tools.h"
#include "exceptions.h"

#include "stimulus/stimuliparser.h"
#include "stimulus/stimulidata.h"
//#include "gui/mainwindowconfig.h"
//#include "datablock/datablockswriter.h"
#include "stimuliwriter.h"

#include "xml/xercesinclude.h"

using namespace xercesc;

using namespace apex;
using namespace apex::data;

#include <QStringList>

StimuliTester::StimuliTester()
{
    stimulidata = new StimuliData();
}

StimuliTester::~StimuliTester()
{
    delete stimulidata;
}

void StimuliTester::ParseData(DOMElement* root)
{

    parser::StimuliParser parser;
    // use the parser to parse data into the datastructure
    try
    {
        parser.Parse(root, stimulidata, QString());
    }
    catch (ApexStringException& e)
    {
        qCDebug(APEX_RS, "Exception: %s",  e.what());
    }
}

DOMElement* StimuliTester::WriteData(DOMDocument* doc)
{
    // use the apex writer to convert the data structure into a DOM tree
    writer::StimuliWriter writer;
    return writer.addElement(doc, *stimulidata);
}


