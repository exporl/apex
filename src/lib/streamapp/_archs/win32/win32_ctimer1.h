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

#ifndef SOMETIMERRR_H
#define SOMETIMERRR_H

#include "win32_headers.h"

  //some timer ripped from the net, seems ok
class CTIMER
{
public:
  CTIMER();
  ~CTIMER();

  void InitTimers();
  void WaitTillMS(__int64);
  bool HasPassedMS(__int64);
  bool HasPassedSinceStartMS(__int64);
  DWORD GetPassedMS();
  DWORD GetPassed(int);

  DWORD sinceprev();

protected:
  __int64 LASTCALL;
  __int64 prev;
  static __int64 TicksPerMS;
};

#endif //#ifndef SOMETIMERRR_H
