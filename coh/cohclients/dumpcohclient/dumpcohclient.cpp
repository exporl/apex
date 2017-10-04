/******************************************************************************
 * Copyright (C) 2013  Michael Hofmann <mh21@mh21.de>                         *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
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

#include "coh/cohclient.h"
#include "coh/cohexpander.h"
#include "coh/cohtextdumper.h"

#include "common/exception.h"

#include <QDateTime>
#include <QFile>
#include <QStringList>

#include <QtPlugin>

using namespace cmn;
using namespace coh;

class DumpCohClientCreator : public QObject, public CohClientCreator
{
    Q_OBJECT
    Q_INTERFACES(coh::CohClientCreator)
    Q_PLUGIN_METADATA(IID "coh.dumpcohclient")
public:
    /** Creates a new dummy CI driver instance for a given device. The caller is
     * responsible of freeing the returned pointer.
     *
     * @param device file name of the dump file which will be written on send()
     * @return device driver instance
     *
     * @throws std::exception on errors
     */
    virtual CohClient *createCohClient(const QString &device);

    virtual QStringList cohDevices() const;

    virtual QString cohDriverName() const;
};

class DumpCohClient : public QObject, public CohClient
{
    Q_OBJECT
public:
    DumpCohClient(const QString &device);
    ~DumpCohClient();

    // CohClient implementation
    virtual void send(CohSequence *sequence);
    virtual void start(Coh::TriggerMode trigger);
    virtual void stop();
    virtual Coh::Status status() const;
    virtual bool needsReloadForStop() const;

private:
    const QString device;
    QDateTime started;
};

// DumpCohClient ===============================================================

DumpCohClient::DumpCohClient(const QString &device) : device(device)
{
}

DumpCohClient::~DumpCohClient()
{
}

void DumpCohClient::send(CohSequence *sequence)
{
    QFile file(device);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Append))
        throw Exception(tr("Unable to open file for writing: %1").arg(device));
    QScopedPointer<CohSequence> expandedSequence(expandCohSequence(sequence));
    file.write(dumpCohSequenceText(expandedSequence.data()).toUtf8());
}

void DumpCohClient::start(Coh::TriggerMode trigger)
{
    Q_UNUSED(trigger);
    started = QDateTime::currentDateTime();
}

void DumpCohClient::stop()
{
    started = QDateTime();
}

Coh::Status DumpCohClient::status() const
{
    const QDateTime now(QDateTime::currentDateTime());
    return started.isValid() && started.msecsTo(now) < 1000 ? Coh::Streaming
                                                            : Coh::Idle;
}

bool DumpCohClient::needsReloadForStop() const
{
    return false;
}

// DumpCohClientCreator ========================================================

CohClient *DumpCohClientCreator::createCohClient(const QString &device)
{
    return new DumpCohClient(device);
}

QStringList DumpCohClientCreator::cohDevices() const
{
    return QStringList() << QL1S("dump.txt");
}

QString DumpCohClientCreator::cohDriverName() const
{
    return QL1S("dump");
}

#include "dumpcohclient.moc"
