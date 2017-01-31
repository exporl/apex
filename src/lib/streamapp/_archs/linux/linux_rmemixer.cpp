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

#include "../../mixers/rmemixer.h"

using namespace streamapp;
using namespace streamapp::rme;


bool f_bRmeIsAvailable()
{
  return false;
}

void f_RmeSetGain( const double /*ac_dGainIndB*/, const unsigned /*ac_nInputChannel*/, const unsigned /*ac_nOutputChannel*/ )
{
  return;
}

double f_dRmeGetGain( const unsigned /*ac_nInputChannel*/, const unsigned /*ac_nOutputChannel*/ )
{
  return 0.0;
}


void f_RmeMute( const unsigned /*ac_nInputChannel*/, const unsigned /*ac_nOutputChannel*/ )
{
}

void f_RmeUnMute( const double /*ac_dGain*/, const unsigned /*ac_nInputChannel*/, const unsigned /*ac_nOutputChannel*/ )
{
}

const IRmeMixerInfo* f_RmeCreate()
{
    return 0;
}
