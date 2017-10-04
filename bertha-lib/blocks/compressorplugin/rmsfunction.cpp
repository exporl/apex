/******************************************************************************
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Originally written by David Vazquez Couto.                                 *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "rmsfunction.h"

#include <math.h>
#include <stdio.h>

RmsFunction::RmsFunction() : buffer(1), bufferIndex(0), acc(0)
{
}

RmsFunction::~RmsFunction()
{
}

void RmsFunction::setRmsBufferSize(unsigned bufferSize)
{
    Q_ASSERT(bufferSize > 0);

    buffer = QVector<double>(bufferSize);
    bufferIndex = 0;
    acc = 0;
}

double RmsFunction::process(double input)
{
    acc -= buffer[bufferIndex];
    buffer[bufferIndex] = pow(input, 2);
    acc += buffer[bufferIndex];
    bufferIndex = (bufferIndex + 1) % buffer.size();
    return sqrt(acc / buffer.size());
}

void RmsFunction::printBuffer() const
{
    Q_FOREACH (double value, buffer)
        printf("%.15f ", value);
    printf("\n");
}
