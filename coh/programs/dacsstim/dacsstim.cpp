/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
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

// stimulates a DACS in high-rate 5cpc, file format is binary n*4 bytes:
//   int16 (LSB/MSB): amplitude, bool (16bit): trigger
// preparing the stimulus takes about 25% as long as streaming itself (NIC 2.1)

#include "coh/coh.h"

#include "common/exception.h"
#include "common/utils.h"

#include <nic/c_interface/CBiphasicStimulus.h>
#include <nic/c_interface/CSequence.h>
#include <nic/c_interface/CSequenceCommand.h>
#include <nic/c_interface/CStimulusCommand.h>
#include <nic/c_interface/CTriggerCommand.h>

#include <services/c_interface/CNICStreamClient.h>

#include <QDateTime>
#include <QFile>

#include <QtEndian>

using namespace cmn;

void functionErrorHandler(int result, const char *message)
{
    if (result == Call_Failed)
        throw Exception(
            QString::fromLatin1("NIC function failed: %1").arg(QL1S(message)));
}

template <typename T>
T newErrorHandler(T ptr, const char *message)
{
    if (!ptr)
        throw Exception(
            QString::fromLatin1("NIC new failed: %1").arg(QL1S(message)));
    return ptr;
}

#define NIC_FUNC_EH(func) functionErrorHandler(func, #func)
#define NIC_NEW_EH(ptr) newErrorHandler(ptr, #ptr)

int main(int argc, char *argv[])
{
    if (argc < 5) {
        qCDebug(EXPORL_COH, "Usage: %s <device> <count> <period> <stimuli>...",
                argv[0]);
        return 1;
    }

    for (unsigned j = 4; j < unsigned(argc); ++j) {
        qCDebug(EXPORL_COH, "%s", argv[j]);
        QFile file(QFile::decodeName(argv[j]));
        if (!file.open(QIODevice::ReadOnly)) {
            qFatal("Unable to open file: %s", argv[j]);
            return 2;
        }
        QByteArray contents = file.readAll();

        try {
            NICStreamClientHandle clientHandle =
                NIC_NEW_EH(NICStreamClientNew(argv[1]));
            SequenceCommandHandle sequenceHandle =
                NIC_NEW_EH(SequenceNew(QByteArray(argv[2]).toUInt()));
            double period = QByteArray(argv[3]).toDouble();

            qCDebug(EXPORL_COH, "Frames: %d", contents.length() / 4);
            for (unsigned i = 0; i < unsigned(contents.length()); i += 4) {
                qint16 amplitude =
                    qFromLittleEndian<qint16>((uchar *)contents.data() + i);
                bool trigger =
                    qFromLittleEndian<qint16>((uchar *)contents.data() + i + 2);
                unsigned cielectrode = (amplitude & 0xf80) >> 7;
                unsigned ciamplitude = (amplitude & 0x07f) << 1;
                // 256/5MHz
                StimulusHandle stimulusHandle = NIC_NEW_EH(BiphasicStimulusNew(
                    cielectrode, -3, ciamplitude, 12, 6, period));
                SequenceCommandHandle commandHandle =
                    trigger ? NIC_NEW_EH(TriggerCommandNew(stimulusHandle))
                            : NIC_NEW_EH(StimulusCommandNew(stimulusHandle));
                NIC_FUNC_EH(stimulusDelete(stimulusHandle));
                NIC_FUNC_EH(sequenceAppend(sequenceHandle, commandHandle));
                NIC_FUNC_EH(sequenceCommandDelete(commandHandle));
            }

            QDateTime now = QDateTime::currentDateTime();
            NIC_FUNC_EH(nscSendData(clientHandle, sequenceHandle));
            NIC_FUNC_EH(sequenceCommandDelete(sequenceHandle));
            qCDebug(EXPORL_COH, "Time for sending data: %d ms",
                    now.msecsTo(QDateTime::currentDateTime()));

            StreamStatus status;
            NIC_FUNC_EH(nscGetStreamStatus(clientHandle, &status));
            qCDebug(EXPORL_COH, "Initial Status: %d", status);
            NIC_FUNC_EH(nscStartStream(clientHandle));

            do {
                milliSleep(1000);
                NIC_FUNC_EH(nscGetStreamStatus(clientHandle, &status));
                qCDebug(EXPORL_COH, "Status: %d", status);
            } while (status != Idle);

            NIC_FUNC_EH(nscStopStream(clientHandle));
            NIC_FUNC_EH(nscDelete(clientHandle));
        } catch (const std::exception &e) {
            qFatal("%s", e.what());
            return 2;
        }
    }

    return 0;
}
