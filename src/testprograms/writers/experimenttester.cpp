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

#include "experimenttester.h"
#include "tools.h"

#include "xml/apexxmltools.h"
#include "xml/xercesinclude.h"

#include "experimentparser.h"

#include "experimentwriter.h"

#include <QDir>

using namespace apex::ApexXMLTools;

bool ExperimentTester::Test(QStringList dirsToCheck)
{
    bool result = true;

    for (int i = 0; i < dirsToCheck.size(); i++)
    {
        QString dirToCheck = RootDir + dirsToCheck.at(i);

        QDir dir(dirToCheck);
        QStringList filters;
        filters << "*.xml";
        dir.setNameFilters(filters);
        QStringList files = dir.entryList();

        // for each of the files to be tested
        for (int i = 0; i < files.size(); ++i)
        {
            // add prefix to filename
            QString testfile(dirToCheck + "/" + files.at(i));

            if (!QFile::exists(testfile))
                qFatal("Fatal: File %s does not exist.", qPrintable(testfile));

            apex::ExperimentParser parser(testfile);
            apex::data::ExperimentData* data = parser.parse();
            qDebug("experiment parsed");
            QString tempfile = QString("__out__%1").arg(files.at(i));
            qDebug("outfile: %s", qPrintable(tempfile));

            try
            {
                apex::writer::ExperimentWriter::write(*data, "../../outfiles/" + tempfile);
            }
            catch ( xercesc_2_7::IOException e)
            {
                qDebug(qPrintable(QString::fromUtf16(e.getMessage())));
                throw e;
            }

            // check whether the original document en the generated document match
            QString compfile("outfiles/" + tempfile);
            qDebug("compfile: %s", qPrintable(compfile));
            result = CompareDocuments(testfile, compfile) && result;
        }
    }

    return result;
}

// void ExperimentTester::ParseData(DOMElement* root)
// {
//     Q_ASSERT(XMLutils::GetTagName(root) == "apex:apex");
//
//     apex::ExperimentParser parser;
//
//     for (DOMNode* currentNode = root->getFirstChild(); currentNode != NULL;
//             currentNode = currentNode->getNextSibling())
//     {
//         Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);
//         Q_ASSERT(XMLutils::GetTagName(currentNode) == "filter");
//
//         QString id = XMLutils::GetAttribute(currentNode, "id");
//         filtersdata[id] = parser.ParseFilter(dynamic_cast<DOMElement*>(currentNode), 0);
//     }
// }
//
// DOMElement* ExperimentTester::WriteData(DOMDocument* doc)
// {
//     return apex::writer::ExperimentWriter::addElement(doc, data);
// }
