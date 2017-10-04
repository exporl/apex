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

#include "apexdata/filter/filterdata.h"
#include "apexdata/filter/wavparameters.h"

#include "apextools/apextools.h"
#include "apextools/exceptions.h"

#include "stimulus/filtertypes.h"

#include "streamapp/audiosamplebuffer.h"

#include "streamapp/utils/stringutils.h"

#include "wavstimulus/wavdeviceio.h"

#include "streamgenerator.h"
#include "wavdatablock.h"

#include <iostream>

using namespace apex;
using namespace stimulus;
using namespace streamapp;

DataLoopGenerator::DataLoopGenerator(PositionableInputStream *const ac_pInput,
                                     const int nChannels,
                                     const unsigned bufferSize,
                                     const unsigned long ac_nFs,
                                     const bool ac_bRandom, double jump)
    : StreamGenerator(nChannels, bufferSize, ac_nFs),
      m_pData(ac_pInput),
      m_bRandom(ac_bRandom),
      m_jump(jump)
{

    if (ac_bRandom && m_jump)
        throw ApexStringException(
            QString(tr("DataLoopGenerator: cannot random jump and jump to a "
                       "fixed location")));

    if (m_pData)
        SetInputStream(m_pData);
}

void DataLoopGenerator::SetInputStream(PositionableInputStream *const ac_pInput)
{
    Q_CHECK_PTR(ac_pInput);

    m_pData = ac_pInput;

    if (m_bRandom) {
        const unsigned long nSamples =
            m_pData->mf_lTotalSamples() / m_pData->mf_nGetNumLoops();
        // const unsigned long lPos = (unsigned long) ApexTools::RandomRange(
        // 0.0, (double) nSamples );
        const unsigned long lPos = m_Random.nextUInt(nSamples);
        m_pData->mp_SeekPosition(lPos);
        std::cout << "Random started at " + toString(lPos) + " out of " +
                         toString(nSamples) + " samples."
                  << std::endl;
    } else if (m_jump) {
        double total_length =
            double(m_pData->mf_lTotalSamples()) / mf_lSampleRate();
        if (m_jump > total_length)
            throw ApexStringException(
                tr("DataLoopGenerator: cannot jump to a location that is "
                   "outside the wave file"));
        m_pData->mp_SeekPosition(unsigned(m_jump * mf_lSampleRate()));
        qCDebug(APEX_RS, "Jumping to position %u",
                unsigned(m_jump * mf_lSampleRate()));
    }

    m_pData->mp_SetNumLoops(0xffff);
}

const Stream &DataLoopGenerator::Read()
{
    const Stream &r = m_pData->Read();
    AudioSampleBuffer b(r);
    b.mp_ApplyGain(mv_dSignalAmp);
    return r;
}

DataLoopGenerator::~DataLoopGenerator()
{
    delete m_pData;
}

StreamGenerator *StreamGeneratorFactory::CreateGenerator(
    const QString &ac_sType, data::FilterData *pParams, unsigned long lFs,
    unsigned nBuffSize, bool deterministic)
{
    data::WavFilterParameters *params = (data::WavFilterParameters *)pParams;
    Q_CHECK_PTR(params);
    const unsigned nChannels = params->numberOfChannels();
    // const unsigned nBuffSize = params->GetBlockSize();
    // const unsigned long lFs  = params->GetSampleRate();

    Q_ASSERT(nChannels != 0);

    StreamGenerator *pRet = 0;

    if (ac_sType == sc_sFilterNoiseGeneratorType) {
        pRet = new NoiseGenerator(nChannels, nBuffSize, lFs, deterministic);
    } else if (ac_sType == sc_sFilterSinglePulseGenType) {
        SinglePulseGenerator *p = new SinglePulseGenerator(
            params->numberOfChannels(), nBuffSize, lFs);
        data::SinglePulseGeneratorParameters *pa =
            (data::SinglePulseGeneratorParameters *)params;
        p->mp_SetPolarity(pa->polarity());
        p->mp_SetNumToGenerate(pa->pulseWidth());
        pRet = p;
    } else if (ac_sType == sc_sFilterDataLoopType) {
        data::DataLoopGeneratorParameters *pa =
            (data::DataLoopGeneratorParameters *)params;

        pRet = new DataLoopGenerator(0, nChannels, nBuffSize, lFs,
                                     pa->startsWithRandomJump(), pa->jump());

    } else if (ac_sType == sc_sFilterSineGenType) {
        SineGenerator *p = new SineGenerator(nChannels, nBuffSize, lFs);
        data::SineGeneratorParameters *pa =
            (data::SineGeneratorParameters *)params;
        p->m_Gen.mp_SetPhrequency(pa->frequency());
        p->m_Gen.mp_SetPhase(pa->phase());
        pRet = p;
    } else
        pRet = 0;

    Q_CHECK_PTR(pRet);

    //  qCDebug(APEX_RS, "gain of " + params->GetID() + "=" +
    //  QString::number(params->mf_dGain()));
    pRet->mp_SetSignalAmp(params->baseGain() + (params->invertGain()
                                                    ? -params->gain()
                                                    : +params->gain()));
    if (((data::WavGeneratorParameters *)params)->isContinuous())
        pRet->mp_SetContinuous();

    return pRet;
}
