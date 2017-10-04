/******************************************************************************
 * Copyright (C) 2016  Michael Hofmann <mh21@mh21.de>                         *
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

#include "apexmain/device/plugincontrollerinterface.h"

#include "apextools/global.h"

#include "common/global.h"

#include <lo/lo.h>

#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>
#include <QObject>
#include <QStringList>

Q_DECLARE_LOGGING_CATEGORY(APEX_OSC)
Q_LOGGING_CATEGORY(APEX_OSC, "apex.osc")

class OscControllerCreator : public QObject, public PluginControllerCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginControllerCreator)

    Q_PLUGIN_METADATA(IID "apex.osccontroller")

public:
    virtual QStringList availablePlugins() const;

    virtual PluginControllerInterface *
    createController(const QString &name) const;
};

class OscController : public QObject, public PluginControllerInterface
{
    Q_OBJECT
public:
    OscController();
    ~OscController();

    virtual void resetParameters();

    virtual bool isValidParameter(const QString &type, int channel) const;
    virtual bool setParameter(const QString &type, int channel,
                              const QString &value);

    virtual bool prepare();
    virtual void playStimulus();

    bool send(const QString &address, const QString &message);

private:
    lo_address oscAddress;
    QString prefix;
    QMap<QString, QString> parameters;

    /* Value of the variableParameter in Apex
     * which triggers playStimulus() output
     * e.g.: "SPIN"
     */
    QString outputTrigger;
};

// OscController ===============================================================

OscController::OscController()
{
    qCDebug(APEX_OSC, "Initializing osccontroller");
    oscAddress = lo_address_new(NULL, "9002");
    prefix = QL1S("apex");
}

OscController::~OscController()
{
    qCDebug(APEX_OSC, "Deleting osccontroller");
    lo_address_free(oscAddress);
}

void OscController::resetParameters()
{
    qCDebug(APEX_OSC, "Resetting parameters");
    parameters.clear();
}

bool OscController::isValidParameter(const QString &type, int channel) const
{
    Q_UNUSED(type);
    Q_UNUSED(channel);

    return true;
}

bool OscController::setParameter(const QString &type, int channel,
                                 const QString &value)
{
    if (channel != -1) {
        qCDebug(APEX_OSC, "osccontroller wrong channel");
        return false;
    }
    if (type == QL1S("prefix")) {
        prefix = value;
        return true;
    }
    if (type == QL1S("oscoutputtrigger")) {
        outputTrigger = value;
        return true;
    }
    // no idea why this one is set
    if (type == QL1S("plugin")) {
        return false;
    }
    qCDebug(APEX_OSC, "Saving OSC parameter %s: %s", qPrintable(type),
            qPrintable(value));
    parameters[type] = value;
    return true;
}

bool OscController::prepare()
{
    QMapIterator<QString, QString> i(parameters);
    while (i.hasNext()) {
        i.next();
        if (i.key().contains(QL1S("prepare")) &&
            i.key().contains(parameters[QL1S("stimulustype")])) {
            if (!send(i.key(), i.value()))
                return false;
        }
    }

    return send(QL1S("prepareFinished"),
                QString::number(QDateTime::currentMSecsSinceEpoch()));
}

void OscController::playStimulus()
{
    //  Mechanism for inhibiting osc stimulus output for certain stimuli
    if (parameters[QL1S("stimulustype")] != outputTrigger)
        return;

    //  Sending all message that do not contain the keyword 'prepare' in the OSC
    //  address
    QMapIterator<QString, QString> i(parameters);
    while (i.hasNext()) {
        i.next();
        if (!i.key().contains(QL1S("prepare"))) {
            if (!send(i.key(), i.value()))
                qCDebug(APEX_OSC, "OSC message send failed");
        }
    }

    send(QL1S("playStimulusFinished"),
         QString::number(QDateTime::currentMSecsSinceEpoch()));
}

bool OscController::send(const QString &address, const QString &message)
{
    qCDebug(APEX_OSC, "Calling OSC SEND at time: %s",
            qPrintable(QString::number(QDateTime::currentMSecsSinceEpoch())));
    QString modifiedAddress = address;
    modifiedAddress.replace(QL1S("-"), QL1S("/"));
    if (!lo_send(oscAddress, QString::fromLatin1("/%1/%2")
                                 .arg(prefix, modifiedAddress)
                                 .toUtf8()
                                 .data(),
                 "s", message.toUtf8().data())) {
        qCWarning(APEX_OSC, "OSC error %d: %s", lo_address_errno(oscAddress),
                  lo_address_errstr(oscAddress));
        return false;
    }
    return true;
}

// OscControllerCreator ========================================================

QStringList OscControllerCreator::availablePlugins() const
{
    return QStringList() << QL1S("osccontroller");
}

PluginControllerInterface *
OscControllerCreator::createController(const QString &name) const
{
    try {
        if (name == QL1S("osccontroller"))
            return new OscController();
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "osccontroller.moc"
