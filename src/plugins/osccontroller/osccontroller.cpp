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
#include <QTimer>

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

class OscMessage : public QObject
{
    Q_OBJECT
public:
    OscMessage(const lo_address &oscAddress, const QString &prefix,
               const QString &address, const QString &message)
        : oscAddress(oscAddress),
          prefix(prefix),
          address(address),
          message(message)
    {
    }

public Q_SLOTS:
    bool send()
    {
        qCDebug(
            APEX_OSC, "Calling OSC SEND, %s, at time: %s", qPrintable(message),
            qPrintable(QString::number(QDateTime::currentMSecsSinceEpoch())));
        QString modifiedAddress = address;
        modifiedAddress.replace(QL1S("-"), QL1S("/"));
        if (!lo_send(oscAddress, QString::fromLatin1("/%1/%2")
                                     .arg(prefix, modifiedAddress)
                                     .toUtf8()
                                     .data(),
                     "s", message.toUtf8().data())) {
            qCWarning(APEX_OSC, "OSC error %d: %s",
                      lo_address_errno(oscAddress),
                      lo_address_errstr(oscAddress));
            Q_EMIT sent();
            return false;
        }
        Q_EMIT sent();
        return true;
    }

Q_SIGNALS:
    void sent();

private:
    const lo_address &oscAddress;
    QString prefix;
    QString address;
    QString message;
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

private:
    lo_address oscAddress;
    QString prefix;
    QMap<QString, QString> parameters;

    /* Value of the variableParameter in Apex
     * which triggers playStimulus() output
     * e.g.: "SPIN"
     */
    QString outputTrigger;
    unsigned sendDelayMS;
};

// OscController ===============================================================

OscController::OscController()
{
    qCDebug(APEX_OSC, "Initializing osccontroller");
    oscAddress = lo_address_new(NULL, "9002");
    prefix = QL1S("apex");
    sendDelayMS = 0;
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
    sendDelayMS = 0;
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
    if (type == QL1S("senddelayms")) {
        bool ok;
        sendDelayMS = value.toUInt(&ok);
        if (!ok)
            sendDelayMS = 0;
        return ok;
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
            OscMessage message(oscAddress, prefix, i.key(), i.value());
            if (!message.send())
                return false;
        }
    }

    OscMessage message(oscAddress, prefix, QL1S("prepareFinished"),
                       QString::number(QDateTime::currentMSecsSinceEpoch()));
    return message.send();
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
            OscMessage *message =
                new OscMessage(oscAddress, prefix, i.key(), i.value());
            qCDebug(APEX_OSC, "Osc playStimulus message with delay %d.",
                    sendDelayMS);
            connect(message, SIGNAL(sent()), message, SLOT(deleteLater()));
            QTimer::singleShot(sendDelayMS, Qt::PreciseTimer, message,
                               SLOT(send()));
        }
    }

    OscMessage message(oscAddress, prefix, QL1S("playStimulusFinished"),
                       QString::number(QDateTime::currentMSecsSinceEpoch()));
    message.send();
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
