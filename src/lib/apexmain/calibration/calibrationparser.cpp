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

#include "apexdata/calibration/calibrationdata.h"
#include "apexdata/calibration/soundlevelmeterdata.h"

#include "apextools/exceptions.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "parser/simpleparametersparser.h"

#include "calibrationparser.h"

namespace apex
{

namespace parser
{

data::CalibrationData *CalibrationParser::Parse(const QDomElement &a_pCalib)
{
    data::CalibrationData *calibdata = new data::CalibrationData;

    calibdata->setCalibrationProfile(a_pCalib.attribute(QSL("profile")));

    for (QDomElement currentNode = a_pCalib.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString stimtag(currentNode.tagName());

        if (stimtag == "soundlevelmeter") {
            SimpleParametersParser parser;
            data::SoundLevelMeterData *d = new data::SoundLevelMeterData();
            parser.Parse(currentNode, d);
            calibdata->setSoundLevelMeterData(d);
        } else if (stimtag == "stimuli") {
            // parse stimuli
            for (QDomElement currentStim = currentNode.firstChildElement();
                 !currentStim.isNull();
                 currentStim = currentStim.nextSiblingElement())
                calibdata->addAvailableStimulus(
                    currentStim.attribute(QSL("id")));
        } else if (stimtag == "parameters") {
            // parse parameters
            for (QDomElement currentStim = currentNode.firstChildElement();
                 !currentStim.isNull();
                 currentStim = currentStim.nextSiblingElement()) {
                const QString tag(currentStim.tagName());
                if (tag == "parameter") {
                    double defaultTargetAmplitude = 0;
                    bool hasDefaultTargetAmplitude = false;
                    double finalTargetAmplitude = 0;
                    double minimumParameter = -50;
                    double maximumParameter = 10;
                    double defaultParameter = 0;
                    double muteParameter = -150;

                    const QString id(currentStim.attribute(QSL("id")));
                    for (QDomElement currentPar =
                             currentStim.firstChildElement();
                         !currentPar.isNull();
                         currentPar = currentPar.nextSiblingElement()) {
                        const QString tagp(currentPar.tagName());
                        if (tagp == "calibrationamplitude") {
                            defaultTargetAmplitude =
                                currentPar.text().toDouble();
                            hasDefaultTargetAmplitude = true;
                        } else if (tagp == "targetamplitude") {
                            finalTargetAmplitude = currentPar.text().toDouble();
                        } else if (tagp == "mute") {
                            muteParameter = currentPar.text().toDouble();
                        } else if (tagp == "min") {
                            minimumParameter = currentPar.text().toDouble();
                        } else if (tagp == "max") {
                            maximumParameter = currentPar.text().toDouble();
                        } else if (tagp == "default") {
                            defaultParameter = currentPar.text().toDouble();
                        } else {
                            throw ApexStringException(
                                "Calibration: unknown tag: " + tagp);
                        }
                    }
                    calibdata->addParameter(
                        id,
                        data::CalibrationParameterData(
                            minimumParameter, maximumParameter,
                            defaultParameter, muteParameter,
                            hasDefaultTargetAmplitude ? defaultTargetAmplitude
                                                      : finalTargetAmplitude,
                            finalTargetAmplitude));
                } else {
                    throw ApexStringException("Calibration: unknown tag: " +
                                              tag);
                }
            }
        } else {
            throw ApexStringException("Calibration: unknown tag: " + stimtag);
        }
    }

    return calibdata;
}

} // namespace apex

} // namespace parser
