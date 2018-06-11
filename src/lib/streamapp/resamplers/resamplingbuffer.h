/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#ifndef __RESAMPLINGBUF_H_
#define __RESAMPLINGBUF_H_

#include "containers/matrix.h"
#include "processors/buffer.h"

namespace streamapp
{

class IResampler;

/**
  * Class to resample and buffer in one go
  * in order to reduce mem usage
  * Callback will most likely be the EpochProcessor
  * ps don't forget to set BasicBuffer's buffer to fill!
  ****************************************************** */
class ResamplingBuffer : public streamapp::BasicBuffer
{
public:
    ResamplingBuffer(IResampler &a_Resampler, const unsigned ac_nOutputSize);
    ~ResamplingBuffer();

    virtual const Stream &mf_DoProcessing(const Stream &ac_StrToProc);

    // getters

private:
    IResampler &m_Resampler;
};
}

#endif //__RESAMPLINGBUF_H_
