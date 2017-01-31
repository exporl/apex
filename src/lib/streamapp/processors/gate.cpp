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

#include "gate.h"
#include "utils/checks.h"

#include <QDebug>

using namespace streamapp;

Gate::Gate( const unsigned ac_nChan, const unsigned ac_nSize, mt_FadeFunc a_pfnGainFunc ):
    IGainProcessor( ac_nChan, ac_nSize ),
  mv_dGateOn( 0.0 ),
  mv_dGateOff( 0.0 ),
  mv_dGateLength( 0.0 ),
  m_pfnGate( utils::PtrCheck( a_pfnGainFunc ) )
{
}

Gate::~Gate()
{
}

void Gate::setMutedAfterwards(bool value)
{
    mutedAfterwards = value;
}

bool Gate::isMutedAfterwards() const
{
    return mutedAfterwards;
}

double Gate::mf_dGetCurGain()
{
  if( IGainProcessor::mv_dSamples >= mv_dGateOn &&
      IGainProcessor::mv_dSamples <  mv_dGateOff )
  {
    const double dPosInGate = IGainProcessor::mv_dSamples - mv_dGateOn;
    return m_pfnGate( dPosInGate, mv_dGateLength );
  }
  if (IGainProcessor::mv_dSamples >= mv_dGateOff && mutedAfterwards)
      return 0.0;
  return 1.0;
}

void Gate::mp_SetGateOn( const double ac_dSample )
{
    qDebug("Gate: starting gate on at %g samples", ac_dSample);
    mv_dGateOn = ac_dSample;
    mv_dGateOff = ac_dSample + mv_dGateLength;
}

void Gate::mp_SetGateOff( const double ac_dSample )
{
    qDebug("Gate: starting gate off at %g samples", ac_dSample);
    mv_dGateOff = ac_dSample;
    mv_dGateLength = ac_dSample - mv_dGateOn;
}

void Gate::mp_SetGateLength( const double ac_dSamples )
{
  mv_dGateLength = ac_dSamples;
  mv_dGateOff = mv_dGateOn + ac_dSamples;
}
