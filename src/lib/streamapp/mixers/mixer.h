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
 
#ifndef __ISTRMIXER_H__
#define __ISTRMIXER_H__

#include "streamappdefines.h"
#include <string>
#include <assert.h>

namespace streamapp
{

    /**
      * IMixer
      *   interface for devices that can attenuate and/or amplify,
      *   and eventually mix (in the sense of route).
      *   A gain of -150.0dB or less must equal 'mute' in the implementation.
      *   IMixer expects implementations to have a number of channels of which
      *   the gain be set individually.
      ************************************************************************* */
  class IMixer
  {
  protected:
      /**
        * Protected Constructor.
        */
    IMixer()
    {}
  public:
      /**
        * Destructor.
        */
    virtual ~IMixer()
    {}

      /**
        * mt_Range
        *   struct specifying the minimum and maximum range for the gain
        **************************************************************** */
    struct mt_Range
    {
      GainType m_dMinimum;
      GainType m_dMaximum;
    };

      /**
        * Get the range for the gain.
        * @return an mt_Range struct
        */
    virtual mt_Range mf_tGetRange() const = 0;

      /**
        * Initialize all output levels to 0dB,
        * and all input levels to -150dB.
        * Must also make a sensible default routing if it's a matrix.
        * Always check the implementation's documentation for what it actually does!
        */
    virtual void mp_Initialize() = 0;

      /**
        * Setters for Master Input and Output gains.

        * @param ac_dGainIndB the gain in dB which is coerced to nearest if out of range
        * @param ac_nChannel  the channel to apply gain to
        */
      /**
        * Set the master input gain.
        * @param ac_dGainIndB the gain
        */
    virtual void mp_SetMasterInputGain( const GainType ac_dGainIndB ) = 0;

      /**
        * Set the master input gain for one channel.
        * @param ac_dGainIndB the gain
        * @param ac_nChannel the channel
        */
    virtual void mp_SetMasterInputGain( const GainType ac_dGainIndB, const unsigned ac_nChannel ) = 0;

      /**
        * Set the master output gain.
        * @param ac_dGainIndB the gain
        */
    virtual void mp_SetMasterOutputGain( const GainType ac_dGainIndB ) = 0;

      /**
        * Set the master output gain for one channel.
        * @param ac_dGainIndB the gain
        * @param ac_nChannel the channel
        */
    virtual void mp_SetMasterOutputGain( const GainType ac_dGainIndB, const unsigned ac_nChannel ) = 0;

      /**
        * Get the master input gain.
        * @param ac_nChannel the channel number, must be in range
        * @return gain in dB
        */
    virtual GainType mf_dGetMasterInputGain( const unsigned ac_nChannel ) const = 0;

      /**
        * Get the master input gain.
        * @param ac_nChannel
        * @return
        */
    virtual GainType mf_dGetMasterOutputGain( const unsigned ac_nChannel ) const = 0;

      /**
        * Get the number of master inputs.
        * @return the number
        */
    virtual unsigned mf_nGetNumMasterInputs() const = 0;

      /**
        * Get the number of master outputs.
        * @return
        */
    virtual unsigned mf_nGetNumMasterOutputs() const = 0;

      /**
        * Get the names of master input channel.
        * @param ac_nChannel the channel number, must be in range
        * @return name if supported, else null string
        */
    virtual const std::string& mf_sGetMasterInputName( const unsigned ac_nChannel ) const = 0;

      /**
        * Get the names of master output channel.
        * @param ac_nChannel the channel number, must be in range
        * @return name if supported, else null string
        */
    virtual const std::string& mf_sGetMasterOutputName( const unsigned ac_nChannel ) const = 0;
  };

    /**
      * IMatrixMixer
      *   interface for IMixers that can also route channels.
      *   While an IMixer just attenuates some channels (hence it's not a 'real' mixer)
      *   without changing the signal path, the IMatrixMixer can also mix one channel to another,
      *   eg mixing input channel 0 to both outputchannel 0 and 1.
      *   If the mixer has no master, the implementation must set the gain
      *   on all related channels (eg all inputchannels for mp_SetMasterInputGain()
      *   Implementations must document the exact behaviour carefully.
      ******************************************************************************************* */
  class IMatrixMixer : public IMixer
  {
  protected:
      /**
        * Protected Constructor.
        */
    IMatrixMixer()
    {
    }

  public:
      /**
        * Destructor.
        */
    ~IMatrixMixer()
    {
    }

      /**
        * Number of Hardware Input channels.
        * Hardware inputs always come first in the matrix
        * @return number
        */
    virtual unsigned mf_nGetNumHardwareInputs() const = 0;

      /**
        * Number of Software Input channels.
        * Software inputs always come after the hardware ones
        * @return number
        */
    virtual unsigned mf_nGetNumSoftwareInputs() const = 0;

      /**
        * Number of Hardware Output channels (may not include master).
        * Software inputs always come after the hardware ones.
        * @return number
        */
    virtual unsigned mf_nGetNumHardwareOutputs() const = 0;

      /**
        * Get the name of an input channel.
        * @param ac_nChannel the channel number, must be in range
        * @return name if supported, else null string
        */
    virtual const std::string& mf_sGetInputName( const unsigned ac_nChannel ) const = 0;

      /**
        * Get the name of an input channel.
        * @param ac_nChannel the channel number, must be in range
        * @return name if supported, else null string
        */
    virtual const std::string& mf_sGetOutputName( const unsigned ac_nChannel ) const = 0;

      /**
        * Set gain on a specified node.
        * @param ac_dGainIndB the gain in dB
        * @param ac_nInputChannel the input channel of the node, range 0 -> mf_nGetNumHardwareInputs + mf_nGetNumSoftwareInputs
        * @param ac_nOutputChannel the output channel of the node, range 0-> mf_nGetNumHardwareOutputs
        */
    virtual void mp_SetGain( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel ) = 0;

      /**
        * Get gain on a specified node.
        * @param ac_nInputChannel the input channel of the node
        * @param ac_nOutputChannel the output channel of the node
        * @return the gain in dB
        */
    virtual GainType mf_dGetGain( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel ) const = 0;

      /**
        * Mute a specified node.
        * On some mixers this is not the same as setting the gain to -150.0 or smaller.
        * Eg if a hardware input on the Rme is not muted, the signal is routed to
        * the software output, independent of the node's gain.
        * @param ac_nInputChannel the input channel of the node
        * @param ac_nOutputChannel the output channel of the node
        */
    virtual void mp_Mute( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel ) = 0;

      /**
        * UnMute a specified node.
        * @see mp_Mute
        * @param ac_dGainIndB the new gain of the node
        * @param ac_nInputChannel the input channel of the node
        * @param ac_nOutputChannel the output channel of the node
        */
    virtual void mp_UnMute( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel ) = 0;

      /**
        * Utility to print IMatrixMixer's specs
        * @param ac_Mixer the mixer
        */
    static void sf_PrintMixerInfo( const IMatrixMixer& ac_Mixer );
  };

}

#endif //#ifndef __ISTRMIXER_H__
