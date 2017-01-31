/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "xml/apexxmltools.h"
#include "xml/xmlkeys.h"
#include "calibrationparser.h"
#include "exceptions.h"
#include "parser/simpleparametersparser.h"

#include "xml/xercesinclude.h"

//from libdata
#include "calibration/calibrationdata.h"
#include "calibration/soundlevelmeterdata.h"

using namespace apex::ApexXMLTools;
using namespace XERCES_CPP_NAMESPACE;

namespace apex
{

namespace parser
{

data::CalibrationData *CalibrationParser::Parse
    (XERCES_CPP_NAMESPACE::DOMElement *a_pCalib)
{
    data::CalibrationData* calibdata = new data::CalibrationData;

    calibdata->setCalibrationProfile (XMLutils::GetAttribute (a_pCalib,
                "profile"));

    for (DOMNode *currentNode = a_pCalib->getFirstChild();
            currentNode != NULL;
            currentNode = currentNode->getNextSibling()) {
        const QString stimtag (XMLutils::GetTagName (currentNode));

        if (stimtag == "soundlevelmeter") {
            SimpleParametersParser parser;
            data::SoundLevelMeterData* d = new data::SoundLevelMeterData();
            parser.Parse((DOMElement*)currentNode, d);
            calibdata->setSoundLevelMeterData(d);
        } else if (stimtag == "stimuli") {
            //parse stimuli
            for (DOMNode *currentStim = currentNode->getFirstChild();
                    currentStim != NULL;
                    currentStim = currentStim->getNextSibling())
                calibdata->addAvailableStimulus (XMLutils::GetAttribute
                        (currentStim, "id"));
        } else if (stimtag == "parameters") {
            //parse parameters
            for (DOMNode *currentStim = currentNode->getFirstChild();
                    currentStim != NULL;
                    currentStim = currentStim->getNextSibling()) {
                const QString tag (XMLutils::GetTagName (currentStim));
                if (tag == "parameter") {
                    double defaultTargetAmplitude = 0;
                    bool hasDefaultTargetAmplitude = false;
                    double finalTargetAmplitude = 0;
                    double minimumParameter = -50;
                    double maximumParameter = 10;
                    double defaultParameter = 0;
                    double muteParameter = -150;

                    const QString id (XMLutils::GetAttribute (currentStim, "id"));
                    for (DOMNode *currentPar = currentStim->getFirstChild();
                            currentPar != NULL;
                            currentPar = currentPar->getNextSibling()) {
                        const QString tagp (XMLutils::GetTagName (currentPar));
                        if (tagp == "calibrationamplitude") {
                            defaultTargetAmplitude = XMLutils::GetFirstChildText
                                (currentPar).toDouble();
                            hasDefaultTargetAmplitude = true;
                        } else if (tagp == "targetamplitude") {
                            finalTargetAmplitude = XMLutils::GetFirstChildText
                                (currentPar).toDouble();
                        } else if (tagp == "mute") {
                            muteParameter = XMLutils::GetFirstChildText
                                (currentPar).toDouble();
                        } else if (tagp == "min") {
                            minimumParameter = XMLutils::GetFirstChildText
                                (currentPar).toDouble();
                        } else if (tagp == "max") {
                            maximumParameter = XMLutils::GetFirstChildText
                                (currentPar).toDouble();
                        } else if (tagp == "default") {
                            defaultParameter = XMLutils::GetFirstChildText
                                (currentPar).toDouble();
                        } else {
                            throw ApexStringException
                                ("Calibration: unknown tag: " + tagp);
                        }
                    }
                    calibdata->addParameter (id, data::CalibrationParameterData
                            (minimumParameter, maximumParameter,
                             defaultParameter, muteParameter,
                             hasDefaultTargetAmplitude ? defaultTargetAmplitude
                             : finalTargetAmplitude, finalTargetAmplitude));
                } else {
                    throw ApexStringException
                        ("Calibration: unknown tag: " + tag);
                }
            }
        } else {
            throw ApexStringException ("Calibration: unknown tag: " + stimtag);
        }
    }

    return calibdata;
}

} // namespace apex

} // namespace parser
