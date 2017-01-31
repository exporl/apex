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
 
#include "mixer.h"
#include "utils/stringutils.h"
#include <iostream>

using namespace streamapp;

void IMatrixMixer::sf_PrintMixerInfo( const IMatrixMixer& ac_Mixer )
{
  std::cout << "*** IMixer Info ****" << std::endl;
  std::cout << "mf_nGetNumMasterInputs: " << ac_Mixer.mf_nGetNumMasterInputs() << std::endl;
  std::cout << "mf_nGetNumMasterOutputs: " << ac_Mixer.mf_nGetNumMasterOutputs() << std::endl;
  if( ac_Mixer.mf_nGetNumMasterInputs() )
    std::cout << "mf_sGetMasterInputName: " << ac_Mixer.mf_sGetMasterInputName( 0 ) << std::endl;
  if( ac_Mixer.mf_nGetNumMasterOutputs() )
    std::cout << "mf_sGetMasterOutputName: " << ac_Mixer.mf_sGetMasterOutputName( 0 ) << std::endl;
  std::cout << "mf_nGetNumHardwareInputs: " << ac_Mixer.mf_nGetNumHardwareInputs() << std::endl;
  std::cout << "mf_nGetNumSoftwareInputs: " << ac_Mixer.mf_nGetNumSoftwareInputs() << std::endl;
  std::cout << "mf_nGetNumHardwareOutputs: " << ac_Mixer.mf_nGetNumHardwareOutputs() << std::endl;
  const unsigned nTotalIns = ac_Mixer.mf_nGetNumSoftwareInputs() + ac_Mixer.mf_nGetNumHardwareInputs();
  for( unsigned i = 0 ; i < nTotalIns ; ++i )
    std::cout << "InputChannel " << i << " = " << ac_Mixer.mf_sGetInputName( i ).c_str() <<std::endl;
  const unsigned nTotalOuts = ac_Mixer.mf_nGetNumHardwareOutputs();
  for( unsigned i = 0 ; i < nTotalOuts ; ++i )
    std::cout << "OutputChannel " << i << " = " << ac_Mixer.mf_sGetOutputName( i ).c_str() <<std::endl;
}

