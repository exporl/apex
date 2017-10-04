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

#include "apexdata/datablock/datablockdata.h"

#include "apexdata/device/wavdevicedata.h"

#include "apexdata/experimentdata.h"

#include "runner/experimentrundelegate.h"

#include "stimulus/datablock.h"

#include "streamapp/file/wavefile.h"

#include "streamapp/silentreader.h"

#include "streamapp/utils/stringexception.h"

#include "wavstimulus/streamgenerator.h"

#include "wavdatablock.h"

#include <QFile>
#include <QString>

#include <QtGlobal>

using namespace apex;
using namespace stimulus;
using namespace streamapp;

WavDataBlock::WavDataBlock(const data::DatablockData &data,
                           const QString &filename,
                           const ExperimentRunDelegate *experiment)
    : WavCompatibleDataBlock(data, filename, experiment), m_dSilenceLength(0.0)
{
    if (filename.startsWith(QSL("silence:"))) {
        bool ok;
        m_dSilenceLength =
            filename.mid(8).toDouble(&ok) * (double)data.nbLoops();
        if (!ok || m_dSilenceLength == 0.0) // FIXME [job refactory]
            throw ApexStringException(
                tr("Invalid silence length: a silence is defined by "
                   "\'silence:\' followed by a"
                   " number that represents the length of the silence."));

        qCDebug(APEX_RS, "Silence datablock with length=%f ms",
                m_dSilenceLength);

        if (data.nbChannels() == 0)
            this->data.setNbChannels(1);
        // throw(ApexStringException("Number of channels for silence must be
        // set"));
        qCDebug(APEX_RS, "Warning using default number of channels (1) for "
                         "silence datablock");
    } else {
        if (!filename.endsWith(QSL(".wav")))
            throw(ApexStringException(
                QString(tr("Invalid wav filename (%1): should end in .wav"))
                    .arg(filename)));

        // check wavefile validity
        InputWaveFile w;
        bool bOpen = false;
        try {
            bOpen = w.mp_bOpen(filename);
        } catch (utils::StringException &e) {
            throw(ApexStringException("bad wavfile " + filename + "; reason: " +
                                      QString(e.mc_sError.data())));
        }
        if (!bOpen) {
            throw(ApexStringException("wavfile not found " + filename));
        } else {
            const unsigned nChan = w.mf_nChannels();
            if (data.nbChannels() != 0) {
                if (data.nbChannels() != nChan)
                    throw(ApexStringException("wavfile " + filename +
                                              " wrong number of channels"));
            } else {
                this->data.setNbChannels(nChan);
            }

            // check sample rate
            const data::WavDeviceData *wdd =
                dynamic_cast<const data::WavDeviceData *>(
                    experiment->GetData().deviceById(data.device()));
            Q_ASSERT(wdd || "Couldn't get device");
            if (w.mf_lSampleRate() != wdd->sampleRate()) {
                throw ApexStringException(
                    "Error: sample rate of datablock " + data.id() +
                    " does not correspond to the sample rate of " +
                    GetDevice());
            }
        }
        w.mp_bClose();
    }
}

WavDataBlock::~WavDataBlock()
{
}

PositionableAudioFormatReaderStream *
WavDataBlock::GetWavStream(const unsigned ac_nBufferSize,
                           const unsigned long ac_nFs) const
{
    PositionableAudioFormatReader *p = 0;
    if (m_dSilenceLength != 0.0) {
        p = new SilentReader(data.nbChannels(), ac_nFs, m_dSilenceLength);
    } else {
        InputWaveFile *pI = new InputWaveFile();
        if (!pI->mp_bOpen(filename)) {
            delete pI;
            // can never be reached if ctor check is ok
            Q_ASSERT(0 && "cannot open wavefile");
            return 0;
        }
        p = pI;
    }
    PosAudioFormatStream *pRet =
        new PosAudioFormatStream(p, ac_nBufferSize, true);
    pRet->mp_SetNumLoops(data.nbLoops());
    return pRet;
}

DataBlock *WavDataBlock::GetCopy(QString id)
{
    qCDebug(APEX_RS, "Copying WAV datablock");
    WavDataBlock *ret = new WavDataBlock(GetParameters(), filename, experiment);
    ret->data.setId(id);
    return ret;
}
