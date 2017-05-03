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


#include "apexmain/calibration/soundlevelmeter.h"

#include "apextools/exceptions.h"

#include "common/slaveserver.h"

#include <QObject>

using namespace cmn;

class ProtoSlmCreator :
    public QObject,
    public SoundLevelMeterPluginCreator
{
    Q_OBJECT
    Q_INTERFACES (SoundLevelMeterPluginCreator)
    Q_PLUGIN_METADATA(IID "apex.protoslm")
public:
    virtual QStringList availablePlugins() const;

    virtual SoundLevelMeter *createSoundLevelMeter(const QString &name) const;
};

class ProtoSlm :
    public QObject,
    public SoundLevelMeter
{
    Q_OBJECT
public:
    ProtoSlm(const QString &name);
    virtual ~ProtoSlm();

    // TODO these should be thrown
    virtual QString errorString() const;

    virtual bool initialize();
    virtual bool connect();
    virtual bool disconnect();

    virtual QStringList transducers() const;
    virtual bool setTransducer(const QString &name);

    virtual QStringList instruments() const;
    virtual bool setInstrument(const QString &instrument);

    // TODO these should be QEnums
    virtual bool startMeasurement(int type_measurement, int frequency_weighting,
            int time_weighting, double percentile);
    virtual bool stopMeasurement();

    // TODO these should be QEnums
    virtual bool measure(int type_measurement, int frequency_weighting, int time_weighting);
    virtual bool measure(const QString &what);
    virtual bool measure();

    virtual double result() const;

private:
    // TODO JSON?
    static QStringList toStringList(const QByteArray &result);
    bool catchError(const QByteArray &function, QByteArray *output,
            const QList<QByteArray> &parameters) const;

    mutable QScopedPointer<SlaveServer> server;
    mutable QString error;
};

// ProtoSlm ====================================================================

ProtoSlm::ProtoSlm(const QString &name)
{
    server.reset(new SlaveServer(name, QString(), QString(), false));
    if (!initialize()) {
        throw ApexStringException(errorString());
    }
}


ProtoSlm::~ProtoSlm()
{
}

QStringList ProtoSlm::toStringList(const QByteArray &result)
{
    return QString::fromUtf8(result).split(QL1S(","));
}

bool ProtoSlm::catchError(const QByteArray &function, QByteArray *output,
        const QList<QByteArray> &parameters) const
{
    try {
        server->call(function, output, parameters);
        return true;
    } catch (const std::exception &e) {
        error = QString::fromLocal8Bit(e.what());
        return false;
    }
}

QString ProtoSlm::errorString() const
{
    return error;
}

bool ProtoSlm::initialize() {
    return catchError("initialize", NULL, QList<QByteArray>());
}

bool ProtoSlm::connect()
{
    return catchError("connect", NULL, QList<QByteArray>());
}

bool ProtoSlm::disconnect()
{
    return catchError("disconnect", NULL, QList<QByteArray>());
}

QStringList ProtoSlm::transducers() const
{
    QByteArray result;
    catchError("transducers", &result, QList<QByteArray>());
    return toStringList(result);
}

bool ProtoSlm::setTransducer(const QString &name)
{
    return catchError("setTransducer", NULL, QList<QByteArray>() << name.toUtf8());
}

QStringList ProtoSlm::instruments() const
{
    QByteArray result;
    catchError("instruments", &result, QList<QByteArray>());
    return toStringList(result);
}

bool ProtoSlm::setInstrument(const QString &name)
{
    return catchError("setInstrument", NULL, QList<QByteArray>() << name.toUtf8());
}

bool ProtoSlm::startMeasurement(int type_measurement, int frequency_weighting,
        int time_weighting, double percentile)
{
    return catchError("startMeasurement", NULL, QList<QByteArray>()
            << QByteArray::number(type_measurement)
            << QByteArray::number(frequency_weighting)
            << QByteArray::number(time_weighting)
            << QByteArray::number(percentile));
}


bool ProtoSlm::stopMeasurement()
{
    return catchError("stopMeasurement", NULL, QList<QByteArray>());
}

bool ProtoSlm::measure()
{
    return catchError("measure0", NULL, QList<QByteArray>());
}

bool ProtoSlm::measure(int type_measurement, int frequency_weighting, int time_weighting)
{
    return catchError("measure3", NULL, QList<QByteArray>()
            << QByteArray::number(type_measurement)
            << QByteArray::number(frequency_weighting)
            << QByteArray::number(time_weighting));
}

bool ProtoSlm::measure(const QString &measurement)
{
    return catchError("measure1", NULL, QList<QByteArray>()
            << measurement.toUtf8());
}


double ProtoSlm::result() const
{
    QByteArray result;
    catchError("result", &result, QList<QByteArray>());
    return result.toDouble();
}

// ProtoSlmCreator =============================================================

QStringList ProtoSlmCreator::availablePlugins() const
{
    return QStringList() << QL1S("dummyslmslave") << QL1S("bkslmslave");
}

SoundLevelMeter * ProtoSlmCreator::createSoundLevelMeter(const QString &name) const
{
    return new ProtoSlm(name);
}

#include "protoslmplugin.moc"
