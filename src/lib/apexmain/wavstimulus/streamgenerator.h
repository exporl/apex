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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_STREAMGENERATOR_H_
#define _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_STREAMGENERATOR_H_

#include "apextools/apextools.h"
#include "apextools/random.h"

#include "streamapp/audioformatstream.h"
#include "streamapp/defines.h"

#include "streamapp/generators/sinegenerator.h"

#include "streamapp/utils/dataconversion.h"
#include "streamapp/utils/math.h"

#include <QCoreApplication>

#include <QtGlobal>

using namespace streamapp;

namespace apex
{
namespace data
{
class FilterData;
}
namespace stimulus
{
class WavDataBlock;
// class ApexFilterParameters;

/**
  * StreamGenerator
  *   base class for generators in Apex.
  *   Looping is not implemented since this
  *   doesn't make much sense for a generator.
  ******************************************** */
class StreamGenerator : public PositionableInputStream
{
protected:
    /**
      * Constructor.
      * @param ac_nChan the number of channels
      * @param ac_nBufferSize the buffersize
      * @param ac_lFs the samplerate
      */
    StreamGenerator(const unsigned ac_nChan, const unsigned ac_nBufferSize,
                    const unsigned long ac_lFs = 0L)
        : mc_nChannels(ac_nChan),
          mc_nBufferSize(ac_nBufferSize),
          mc_lSignalFs(ac_lFs),
          mv_lPosition(0),
          mv_dSignalAmp(1.0),
          mv_bContinuous(false)
    {
    }

public:
    /**
      * Destrcutor.
      */
    virtual ~StreamGenerator()
    {
    }

    /**
      * Called by WavGenerator to reset the generator.
      * Implementation is normally empty,
      * except for SinglePulseGenerator.
      */
    virtual void Reset()
    {
    }

    virtual void Prepare()
    {
    }

    /**
      * Set the amplitude.
      * @param ac_dGain amplitude, always in dB
      */
    INLINE virtual void mp_SetSignalAmp(const double ac_dGain)
    {
        mv_dSignalAmp = dataconversion::gf_dBtoLinear(ac_dGain);
    }

    /**
      * Set frequency.
      * FIXME this has no place here
      */
    INLINE virtual void mp_SetFrequency(const double)
    {
    }

    /**
      * Set continuous.
      * If this is set, WavDevice will continue calling the generator
      * between trials, so there's always audible output.
      */
    INLINE void mp_SetContinuous()
    {
        mv_bContinuous = true;
    }

    /**
      * Check if it's a continuous generator.
      * @return true if so
      */
    INLINE bool mf_bContinuous() const
    {
        return mv_bContinuous;
    }

    /**
      * Implementation of the InputStream method.
      */
    virtual const Stream &Read() = 0;

    /**
      * Implementation of the PositionableInputStream method.
      */
    INLINE virtual unsigned mf_nGetNumChannels() const
    {
        return mc_nChannels;
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    INLINE virtual unsigned mf_nGetBufferSize() const
    {
        return mc_nBufferSize;
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    INLINE virtual unsigned long mf_lSampleRate() const
    {
        return mc_lSignalFs;
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    INLINE virtual unsigned long mf_lSamplesLeft() const
    {
        return sc_lInfinite;
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    INLINE virtual unsigned long mf_lTotalSamples() const
    {
        return sc_lInfinite;
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    INLINE virtual unsigned long mf_lCurrentPosition() const
    {
        return mv_lPosition;
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    INLINE virtual void mp_SeekPosition(const unsigned long ac_lPosition)
    {
        mv_lPosition = ac_lPosition;
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    INLINE bool mf_bCanLoop() const
    {
        return false;
    }

protected:
    const unsigned mc_nChannels;
    const unsigned mc_nBufferSize;
    const unsigned long mc_lSignalFs; // FIXME isn't really needed ??
    unsigned long mv_lPosition;

    double mv_dSignalAmp;
    bool mv_bContinuous;

    Random m_Random;
};

/**
  * SineGenerator
  *   generates a simple sine.
  **************************** */
class SineGenerator : public StreamGenerator
{
public:
    /**
      * Constructor.
      */
    SineGenerator(const unsigned ac_nChan, const unsigned ac_nBufferSize,
                  const unsigned long ac_lFs)
        : StreamGenerator(ac_nChan, ac_nBufferSize, ac_lFs),
          m_Gen(ac_nChan, ac_nBufferSize, ac_lFs)
    {
    }

    /**
      * Destructor.
      */
    ~SineGenerator()
    {
    }

    /**
      * Implementation of the InputStream method.
      */
    INLINE const Stream &Read()
    {
        return m_Gen.Read();
    }

    /**
      * Set the frequency.
      * @param ac_dGain frequency in Hz
      */
    INLINE virtual void mp_SetFrequency(const double ac_dFreq)
    {
        m_Gen.mp_SetPhrequency(ac_dFreq);
    }

    /**
      * Set the peak amplitude.
      * @param ac_dGain amplitude, always in dB
      */
    INLINE void mp_SetSignalAmp(const double ac_dGain)
    {
        m_Gen.mp_SetAmplitude(dataconversion::gf_dBtoLinear(ac_dGain));
    }

    streamapp::SineGenerator m_Gen;
};

/**
  * DataLoopGenerator
  *   loops any PositionableInputStream.
  ************************************** */
class DataLoopGenerator : public StreamGenerator
{
    Q_DECLARE_TR_FUNCTIONS(DataLoopGenerator);

public:
    /**
      * Constructor.
      * @param ac_pInput the stream to loop, can be 0 on construction and set
     * later using setinputstream
      * @param ac_lFs the samplerate
      * @param ac_bRandom if true, starts at a random position ( 0 <
     * ac_pInput.mf_lTotalSamples )
      */
    DataLoopGenerator(PositionableInputStream *const ac_pInput,
                      const int nChannels, const unsigned bufferSize,
                      const unsigned long ac_lFs, const bool ac_bRandom,
                      double jump);

    void SetInputStream(PositionableInputStream *const ac_pInput);

    /**
      * Destructor.
      */
    ~DataLoopGenerator();

    /**
      * Implementation of the InputStream method.
      */
    const Stream &Read();

private:
    PositionableInputStream *m_pData;
    bool m_bRandom;
    double m_jump;
};

/**
  * NoiseGenerator
  *   generates gaussian noise.
  ***************************** */
class NoiseGenerator : public StreamGenerator
{
public:
    /**
      * Constructor.
      */
    NoiseGenerator(const unsigned ac_nChan, const unsigned ac_nBufferSize,
                   const unsigned long ac_nFs, bool deterministic = false)
        : StreamGenerator(ac_nChan, ac_nBufferSize, ac_nFs),
          m_Buf(ac_nChan, ac_nBufferSize, true)
    {
        if (deterministic)
            randomGenerator.setSeed(0);
    }

    /**
      * Destructor.
      */
    ~NoiseGenerator()
    {
    }

    /**
      * Implementation of the InputStream method.
      */
    const Stream &Read()
    {
        for (unsigned i = 0; i < mc_nChannels; ++i) {
            for (unsigned j = 0; j < mc_nBufferSize; ++j) {
                // [Tom] maak normaal verdeeld: (cfr Numerical Recipes in C,
                // Press)
                const double myRand = randomGenerator.nextGaussian();
                m_Buf.mp_Set(i, j, myRand * mv_dSignalAmp);
            }
        }
        return m_Buf;
    }

private:
    StreamBuf m_Buf;
    Random randomGenerator;
};

/**
  * SinglePulseGenerator
  *   generates a single pulse with settable width.
  ************************************************* */
class SinglePulseGenerator : public StreamGenerator
{
public:
    /**
      * Constructor.
      */
    SinglePulseGenerator(const unsigned ac_nChan, const unsigned ac_nBufferSize,
                         const unsigned long ac_nFs)
        : StreamGenerator(ac_nChan, ac_nBufferSize, ac_nFs),
          m_Buf(ac_nChan, ac_nBufferSize, true),
          mv_dPolarity(1.0),
          mv_nGenerations(0),
          mv_nToGenerate(1)
    {
    }

    /**
      * Destructor.
      */
    ~SinglePulseGenerator()
    {
    }

    /**
      * Prepare.
      * This will result in the next Read() call generating the pulse.
      */
    INLINE void Prepare()
    {
        mv_nGenerations = 0;
    }

    /**
     * Reset.
     * This will result in the next Read() call generating the pulse.
     */
    INLINE void Reset()
    {
        mv_nGenerations = 0;
    }

    /**
      * Set the width of the pulse.
      * @param ac_nToGenerate width in number of samples
      */
    INLINE void mp_SetNumToGenerate(const unsigned ac_nToGenerate)
    {
        Q_ASSERT(ac_nToGenerate < mc_nBufferSize); // this should be enough,
                                                   // it's called 'single pulse'
                                                   // after all
        mv_nToGenerate = ac_nToGenerate;
    }

    /**
      * Set the polarity.
      * @param true for positive, false for negative.
      */
    void mp_SetPolarity(const bool ac_bPolarity)
    {
        if (ac_bPolarity)
            mv_dPolarity = 1.0;
        else
            mv_dPolarity = -1.0;
    }

    /**
      * Implementation of the InputStream method.
      */
    const Stream &Read()
    {
        if (mv_nGenerations == 0) {
            const double dSampleVal = mv_dPolarity * mv_dSignalAmp;
            for (unsigned j = 0; j < mv_nToGenerate; ++j)
                for (unsigned i = 0; i < mc_nChannels; ++i)
                    m_Buf.mp_Set(i, j, dSampleVal);
            ++mv_nGenerations;
        } else if (mv_nGenerations == 1) {
            m_Buf.mp_Clear();
            mv_nGenerations++;
        }
        return m_Buf;
    }

private:
    StreamBuf m_Buf;
    double mv_dPolarity;
    unsigned mv_nGenerations;
    unsigned mv_nToGenerate;
};

/**
  * StreamGeneratorFactory
  *   class for getting generators.
  ********************************* */
class StreamGeneratorFactory
{
private:
    /**
      * Private Constructor.
      */
    StreamGeneratorFactory()
    {
    }

    /**
      * Private Destructor.
      */
    ~StreamGeneratorFactory()
    {
    }

public:
    /**
      * Create a generator.
      * @param ac_sType the type, @see filtertypes.h
      * @param pParams the parameters
      * @return a StreamGenerator (must be deleted by callee) or 0 if type isn't
     * found.
      */
    static StreamGenerator *CreateGenerator(const QString &ac_sType,
                                            data::FilterData *pParams,
                                            unsigned long sr, unsigned bs,
                                            bool deterministic = false);
};
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_STREAMGENERATOR_H_
