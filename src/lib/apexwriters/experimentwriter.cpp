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

#include "apexdata/connection/connectiondata.h"

#include "apexdata/experimentdata.h"

#include "apexdata/filter/filtersdata.h"

#include "apexdata/procedure/proceduredata.h"

#include "apextools/exceptions.h"
#include "apextools/version.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"

#include "calibrationwriter.h"
#include "connectionswriter.h"
#include "datablockswriter.h"
#include "deviceswriter.h"
#include "experimentwriter.h"
#include "filterswriter.h"
#include "generalparameterswriter.h"
#include "procedureswriter.h"
#include "resultparameterswriter.h"
#include "screenswriter.h"
#include "stimuliwriter.h"

#include <QDebug>
#include <QDir>

using namespace XERCES_CPP_NAMESPACE;
using namespace apex::writer;
using namespace apex::ApexXMLTools;
using apex::data::ExperimentData;


void ExperimentWriter::write(const ExperimentData& data, const QString& file,
                             const QStringList& screens)
{
    // initialize XML engine
    DOMImplementation* impl =
        DOMImplementationRegistry::getDOMImplementation(X("Core"));

    // create new DOM tree
    DOMDocument* doc = impl->createDocument(X(EXPERIMENT_NAMESPACE), X("apex:apex"), 0);

    //set the attributes of root apex element
    DOMElement* apex = doc->getDocumentElement();
    apex->setAttribute(X("xmlns:xsi"),
                       X("http://www.w3.org/2001/XMLSchema-instance"));
    apex->setAttribute(X("xsi:schemaLocation"), X(EXPERIMENT_NAMESPACE
            " https://gilbert.med.kuleuven.be/apex/schemas/" SCHEMA_VERSION "/experiment.xsd"));
//     apex->setAttribute(X("xmlns:apex"), X(EXPERIMENT_NAMESPACE));
//     apex->setPrefix(X("apex"));

    //start writing the data
    //create a DOMElement to test each write
    DOMElement* e;

    //description
    QString description = data.experimentDescription();
    if (!description.isEmpty())
    {
        doc->getDocumentElement()->appendChild(XMLutils::CreateTextElement(
                                                   doc, "description", description));
    }

    //ProcedureConfig
    e = ProceduresWriter::addElement(doc, *data.procedureData());
    throwIfNull(e, "procedures");

    //ScreensData
    e = ScreensWriter::addElement(doc, *data.screensData(), screens);
    throwIfNull(e, "screens");

    //DatablocksData
    e = DatablocksWriter::addElement(doc, *data.datablocksData());
    throwIfNull(e, "datablocks");

    //DevicesData
    e = DevicesWriter::addElement(doc, *data.devicesData());
    throwIfNull(e, "devices");

    //FiltersData
    const data::FiltersData& filters = *data.filtersData();
    if (filters.size() > 0)
    {
        e = FiltersWriter::addElement(doc, filters);
        throwIfNull(e, "filters");
    }

    //StimuliData
    e = StimuliWriter::addElement(doc, *data.stimuliData());
    throwIfNull(e, "stimuli");

    //ConnectionsData
    const data::ConnectionsData* connections = data.connectionsData();
    if (connections->size() > 0)
    {
        e = ConnectionsWriter::addElement(doc, *connections);
        throwIfNull(e, "connections");
    }

    //CalibrationData
    if (data.calibrationData() != 0)
    {
        e = CalibrationWriter::addElement(doc, *data.calibrationData());
        throwIfNull(e, "calibration");
    }

    //ResultParameters
    e = ResultParametersWriter::addElement(doc, *data.resultParameters());
    throwIfNull(e, "resultparameters");

    //GeneralParameters
    e = GeneralParametersWriter::addElement(doc, *data.generalParameters());
    throwIfNull(e, "generalparameters");

    qCDebug(APEX_RS) << "curr path: " << QDir::currentPath();

    if (!XMLutils::WriteElement(doc->getDocumentElement(), file))
    {
        throw ApexStringException(QString("ExperimentWriter: error writing "
                                          "to file %1").arg(file));
    }

    qCDebug(APEX_RS) << "done writing experiment";
}

void ExperimentWriter::throwIfNull(DOMElement* e, const QString& failingWrite)
{
    if (e == 0)
    {
        QString message("ExperimentWriter: failed to write %1data");
        throw ApexStringException(message.arg(failingWrite));
    }

    e = 0;
}






























