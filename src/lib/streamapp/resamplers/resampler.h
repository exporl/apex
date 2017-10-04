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

#ifndef RESAMPLER_H
#define RESAMPLER_H

#include "streamappdefines.h"

#include <QtGlobal>

namespace streamapp
{
/**
  * Resampler interface
  *   a resampler changes the samplerate of the input stream.
  *   This is done by using an anti-alias filter and adding or
  *   removing samples from the input, so the output stream will
  *   have more or less samples, depending on resp. up or down sampling.
  *
  *   TODO - split up and down since they require different sizes, or
  *         change the call to eg
  *         mf_UpSample( const Stream& ac_InputStr, Stream& ac_OutputStr )
  *        - allow setting filter
  *
  ************************************************************************ */
class IResampler
{
protected:
    /**
      * Protected Constructor.
      * @param ac_nChan the number of channels
      * @param ac_nBufferSize the size of the input
      * @param ac_nDownFactor the resample factor
      * @param ac_nFilterTaps the number of taps to use
      */
    IResampler(const unsigned ac_nChan, const unsigned ac_nBufferSize,
               const unsigned ac_nDownFactor, const unsigned ac_nFilterTaps)
        : mc_nChan(ac_nChan),
          mc_nSize(ac_nBufferSize),
          mc_nOutSize(ac_nBufferSize / ac_nDownFactor),
          mc_nDownFactor(ac_nDownFactor),
          mc_nFilterTaps(ac_nFilterTaps)
    {
        Q_ASSERT((ac_nBufferSize % ac_nDownFactor) == 0);
    }

public:
    /**
      * Destructor.
      */
    virtual ~IResampler()
    {
    }

    /**
      * Sample up.
      * @param ac_InputStr the stream to upsample
      * @return the upsampled stream
      */
    virtual const Stream &mf_UpSample(const Stream &ac_InputStr) = 0;

    /**
      * Sample down.
      * @param ac_InputStr the stream to downsample
      * @return the downsampled stream
      */
    virtual const Stream &mf_DownSample(const Stream &ac_InputStr) = 0;

    /**
      * Get the number of channels.
      * @param the number
      */
    virtual const unsigned &mf_nGetNumChannels() const
    {
        return mc_nChan;
    }

    /**
      * Get the buffersize.
      * @param the size in samples
      */
    virtual const unsigned &mf_nGetInputBufferSize() const
    {
        return mc_nSize;
    }

    /**
      * Get the output buffersize.
      * FIIIIXMEEEEE
      * @param the size in samples
      */
    virtual const unsigned &mf_nGetOutputBufferSize()
    {
        return mc_nOutSize;
    }

    /**
      * Get the resample factor.
      * @param the factor
      */
    virtual const unsigned &mf_nGetDownFactor() const
    {
        return mc_nDownFactor;
    }

    /**
      * Get the number of filter taps.
      * @return the number
      */
    virtual const unsigned &mf_nGetNumFiterTaps() const
    {
        return mc_nFilterTaps;
    }

protected:
    const unsigned mc_nChan;
    const unsigned mc_nSize;
    const unsigned mc_nOutSize;
    const unsigned mc_nDownFactor;
    const unsigned mc_nFilterTaps;

    IResampler(const IResampler &);
    IResampler &operator=(const IResampler &);
};
}

#endif // RESAMPLER_H
