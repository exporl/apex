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

#include "calibrationwriter.h"
#include "xml/apexxmltools.h"
#include "calibration/calibrationdata.h"
#include "calibration/soundlevelmeterdata.h"

#include "xml/xercesinclude.h"

#include <QStringList>

using namespace XERCES_CPP_NAMESPACE;
using namespace apex::ApexXMLTools;

using apex::writer::CalibrationWriter;
using apex::data::CalibrationData;
using apex::data::SoundLevelMeterData;
using apex::data::CalibrationParameterData;


DOMElement* CalibrationWriter::addElement(DOMDocument* doc,
                                          const CalibrationData& data)
{
    DOMElement* rootElement = doc->getDocumentElement();
    DOMElement* calibration = doc->createElement(X("calibration"));
    rootElement->appendChild(calibration);

    //profile attribute
    QString profile = data.calibrationProfile();
    if (!profile.isEmpty())
        calibration->setAttribute(X("profile"), S2X(profile));

    if (data.soundLevelMeterData() != 0)
    {
        calibration->appendChild(addSoundLevelMeter(doc,
                                 *data.soundLevelMeterData()));
    }

    calibration->appendChild(addStimuli(doc, data.availableStimuli()));
    calibration->appendChild(addParameters(doc, data.parameters()));

    return calibration;
}

DOMElement* CalibrationWriter::addSoundLevelMeter(DOMDocument* doc,
        const SoundLevelMeterData& data)
{
    Q_ASSERT(data.containsSupportedData());

    DOMElement* slm = doc->createElement(X("soundlevelmeter"));

    //plugin
    Q_ASSERT(data.hasParameter("plugin"));
    slm->appendChild(XMLutils::CreateTextElement(doc, "plugin",
                                        data.valueByType("plugin").toString()));

    //transducer
    appendParameterTo(slm, doc, "transducer", data);

    //frequence_weighting
    QString fw = data.frequencyWeightingType();
    Q_ASSERT(!fw.isEmpty());
    slm->appendChild(XMLutils::CreateTextElement(doc, "frequency_weighting", fw));

    //time_weighting
    QString tw = data.timeWeightingType();
    Q_ASSERT(!tw.isEmpty());
    slm->appendChild(XMLutils::CreateTextElement(doc, "time_weighting", tw));

    //type
    QString mt = data.measurementType();
    Q_ASSERT(!mt.isEmpty());
    slm->appendChild(XMLutils::CreateTextElement(doc, "type", mt));

    //plugin
    Q_ASSERT(data.hasParameter("plugin"));
    slm->appendChild(XMLutils::CreateTextElement(doc, "plugin",
                     data.valueByType("plugin").toString()));

    //transducer
    appendParameterTo(slm, doc, "transducer", data);
    //percentile
    appendParameterTo(slm, doc, "percentile", data);
    //time
    appendParameterTo(slm, doc, "time", data);
    //accuracy
    appendParameterTo(slm, doc, "accuracy", data);
    //maxiterations
    appendParameterTo(slm, doc, "maxiterations", data);

    return slm;
}

void CalibrationWriter::appendParameterTo(DOMElement* slm, DOMDocument* doc,
                                 QString param, const SoundLevelMeterData& data)
{
    if (data.hasParameter(param))
    {
        slm->appendChild(XMLutils::CreateTextElement(doc, param,
                         data.valueByType(param).toString()));
    }
}

DOMElement* CalibrationWriter::addParameters(DOMDocument* doc,
                           const QMap<QString, CalibrationParameterData>& data)
{
    Q_ASSERT(data.size() >= 1);

    DOMElement* parameters = doc->createElement(X("parameters"));

    QMap<QString, CalibrationParameterData>::const_iterator it;
    for (it = data.begin(); it != data.end(); it++)
    {
        CalibrationParameterData param = it.value();
        DOMElement* parameter = doc->createElement(X("parameter"));
        parameter->setAttribute(X("id"), S2X(it.key()));

        parameter->appendChild(XMLutils::CreateTextElement(doc,
                              "targetamplitude", param.finalTargetAmplitude()));
        parameter->appendChild(XMLutils::CreateTextElement(doc,
                       "calibrationamplitude", param.defaultTargetAmplitude()));
        parameter->appendChild(XMLutils::CreateTextElement(doc,
                               "mute", param.muteParameter()));
        parameter->appendChild(XMLutils::CreateTextElement(doc,
                               "min", param.minimumParameter()));
        parameter->appendChild(XMLutils::CreateTextElement(doc,
                               "max", param.maximumParameter()));

        parameters->appendChild(parameter);
    }

    return parameters;
}

DOMElement* CalibrationWriter::addStimuli(DOMDocument* doc,
                                          const QStringList& data)
{
    Q_ASSERT(data.count() >= 1);

    DOMElement* stimuli = doc->createElement(X("stimuli"));

    Q_FOREACH(QString stimulusId, data)
    {
        DOMElement* stimulus = doc->createElement(X("stimulus"));
        stimulus->setAttribute(X("id"), S2X(stimulusId));
        stimuli->appendChild(stimulus);
    }

    return stimuli;
}























