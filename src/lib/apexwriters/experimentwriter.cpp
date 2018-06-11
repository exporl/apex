/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "apexdata/connection/connectiondata.h"

#include "apexdata/experimentdata.h"

#include "apexdata/filter/filtersdata.h"

#include "apexdata/procedure/proceduredata.h"

#include "apextools/exceptions.h"
#include "apextools/version.h"

#include "apextools/xml/xmltools.h"

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

using namespace apex;
using namespace apex::writer;
using apex::data::ExperimentData;

void ExperimentWriter::write(const ExperimentData &data, const QString &file,
                             const QStringList &screens)
{
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction(
        "xml", "version=\"1.0\" encoding=\"UTF-8\""));

    QDomElement apex =
        doc.createElementNS(QL1S(APEX_NAMESPACE), QSL("apex:apex"));
    apex.setAttribute(QSL("xmlns:xsi"),
                      QSL("http://www.w3.org/2001/XMLSchema-instance"));
    apex.setAttribute(QSL("xsi:schemaLocation"),
                      QL1S(APEX_NAMESPACE " " EXPERIMENT_SCHEMA_URL));
    doc.appendChild(apex);

    QDomElement e;

    // description
    QString description = data.experimentDescription();
    if (!description.isEmpty())
        apex.appendChild(
            XmlUtils::createTextElement(&doc, "description", description));

    // ProcedureConfig
    e = ProceduresWriter::addElement(&doc, *data.procedureData());
    throwIfNull(e, "procedures");

    // ScreensData
    e = ScreensWriter::addElement(&doc, *data.screensData(), screens);
    throwIfNull(e, "screens");

    // DatablocksData
    e = DatablocksWriter::addElement(&doc, *data.datablocksData());
    throwIfNull(e, "datablocks");

    // DevicesData
    e = DevicesWriter::addElement(&doc, *data.devicesData());
    throwIfNull(e, "devices");

    // FiltersData
    const data::FiltersData &filters = *data.filtersData();
    if (filters.size() > 0) {
        e = FiltersWriter::addElement(&doc, filters);
        throwIfNull(e, "filters");
    }

    // StimuliData
    e = StimuliWriter::addElement(&doc, *data.stimuliData());
    throwIfNull(e, "stimuli");

    // ConnectionsData
    const data::ConnectionsData *connections = data.connectionsData();
    if (connections->size() > 0) {
        e = ConnectionsWriter::addElement(&doc, *connections);
        throwIfNull(e, "connections");
    }

    // CalibrationData
    if (data.calibrationData()) {
        e = CalibrationWriter::addElement(&doc, *data.calibrationData());
        throwIfNull(e, "calibration");
    }

    // ResultParameters
    e = ResultParametersWriter::addElement(&doc, *data.resultParameters());
    throwIfNull(e, "resultparameters");

    // GeneralParameters
    e = GeneralParametersWriter::addElement(&doc, *data.generalParameters());
    throwIfNull(e, "generalparameters");

    qCDebug(APEX_RS) << "curr path: " << QDir::currentPath();

    if (!XmlUtils::writeDocument(doc, file)) {
        throw ApexStringException(QString("ExperimentWriter: error writing "
                                          "to file %1")
                                      .arg(file));
    }

    qCDebug(APEX_RS) << "done writing experiment";
}

void ExperimentWriter::throwIfNull(const QDomElement &e, const QString &where)
{
    if (e.isNull())
        throw ApexStringException(
            tr("ExperimentWriter: failed to write %1data").arg(where));
}
