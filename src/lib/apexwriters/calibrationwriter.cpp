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

#include "apexdata/calibration/calibrationdata.h"
#include "apexdata/calibration/soundlevelmeterdata.h"

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "calibrationwriter.h"

#include <QStringList>

using namespace apex;

using apex::writer::CalibrationWriter;
using apex::data::CalibrationData;
using apex::data::SoundLevelMeterData;
using apex::data::CalibrationParameterData;

QDomElement CalibrationWriter::addElement(QDomDocument *doc,
                                          const CalibrationData &data)
{
    QDomElement rootElement = doc->documentElement();
    QDomElement calibration = doc->createElement(QSL("calibration"));
    rootElement.appendChild(calibration);

    // profile attribute
    QString profile = data.calibrationProfile();
    if (!profile.isEmpty())
        calibration.setAttribute(QSL("profile"), profile);

    if (data.soundLevelMeterData() != 0) {
        calibration.appendChild(
            addSoundLevelMeter(doc, *data.soundLevelMeterData()));
    }

    calibration.appendChild(addStimuli(doc, data.availableStimuli()));
    calibration.appendChild(addParameters(doc, data.parameters()));

    return calibration;
}

QDomElement
CalibrationWriter::addSoundLevelMeter(QDomDocument *doc,
                                      const SoundLevelMeterData &data)
{
    Q_ASSERT(data.containsSupportedData());

    QDomElement slm = doc->createElement(QSL("soundlevelmeter"));

    // plugin
    Q_ASSERT(data.hasParameter("plugin"));
    slm.appendChild(XmlUtils::createTextElement(
        doc, "plugin", data.valueByType("plugin").toString()));

    // transducer
    appendParameterTo(doc, &slm, "transducer", data);

    // frequence_weighting
    QString fw = data.frequencyWeightingType();
    Q_ASSERT(!fw.isEmpty());
    slm.appendChild(
        XmlUtils::createTextElement(doc, "frequency_weighting", fw));

    // time_weighting
    QString tw = data.timeWeightingType();
    Q_ASSERT(!tw.isEmpty());
    slm.appendChild(XmlUtils::createTextElement(doc, "time_weighting", tw));

    // type
    QString mt = data.measurementType();
    Q_ASSERT(!mt.isEmpty());
    slm.appendChild(XmlUtils::createTextElement(doc, "type", mt));

    // percentile
    appendParameterTo(doc, &slm, "percentile", data);
    // time
    appendParameterTo(doc, &slm, "time", data);
    // accuracy
    appendParameterTo(doc, &slm, "accuracy", data);
    // maxiterations
    appendParameterTo(doc, &slm, "maxiterations", data);

    return slm;
}

void CalibrationWriter::appendParameterTo(QDomDocument *doc, QDomElement *slm,
                                          const QString &param,
                                          const SoundLevelMeterData &data)
{
    if (data.hasParameter(param)) {
        slm->appendChild(XmlUtils::createTextElement(
            doc, param, data.valueByType(param).toString()));
    }
}

QDomElement CalibrationWriter::addParameters(
    QDomDocument *doc, const QMap<QString, CalibrationParameterData> &data)
{
    Q_ASSERT(data.size() >= 1);

    QDomElement parameters = doc->createElement(QSL("parameters"));

    QMap<QString, CalibrationParameterData>::const_iterator it;
    for (it = data.begin(); it != data.end(); it++) {
        CalibrationParameterData param = it.value();
        QDomElement parameter = doc->createElement(QSL("parameter"));
        parameter.setAttribute(QSL("id"), it.key());

        parameter.appendChild(XmlUtils::createTextElement(
            doc, "targetamplitude", param.finalTargetAmplitude()));
        parameter.appendChild(XmlUtils::createTextElement(
            doc, "calibrationamplitude", param.defaultTargetAmplitude()));
        parameter.appendChild(
            XmlUtils::createTextElement(doc, "mute", param.muteParameter()));
        parameter.appendChild(
            XmlUtils::createTextElement(doc, "min", param.minimumParameter()));
        parameter.appendChild(
            XmlUtils::createTextElement(doc, "max", param.maximumParameter()));

        parameters.appendChild(parameter);
    }

    return parameters;
}

QDomElement CalibrationWriter::addStimuli(QDomDocument *doc,
                                          const QStringList &data)
{
    Q_ASSERT(data.count() >= 1);

    QDomElement stimuli = doc->createElement(QSL("stimuli"));

    Q_FOREACH (QString stimulusId, data) {
        QDomElement stimulus = doc->createElement(QSL("stimulus"));
        stimulus.setAttribute(QSL("id"), stimulusId);
        stimuli.appendChild(stimulus);
    }

    return stimuli;
}
