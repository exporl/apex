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

#include "tracer.h"
using namespace utils;

namespace
{
Tracer *pCurTracer = 0;
}

Tracer::Tracer()
{
}

Tracer::~Tracer()
{
}

void Tracer::sf_SetCurrentTracer(Tracer *const ac_pTracer,
                                 const bool ac_bDeletePrevious /*= false*/)
{
    Tracer *const pOld = pCurTracer;
    pCurTracer = ac_pTracer;
    if (ac_bDeletePrevious && pOld != 0)
        delete pOld;
}

void Tracer::sf_Trace(const String &ac_sMessage)
{
    if (pCurTracer)
        pCurTracer->mp_TraceOne(ac_sMessage);
}

bool Tracer::sv_bTraceAll = false;
