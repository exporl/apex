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

#ifndef _BERTHA_SRC_BLOCKS_COMPRESSORPLUGIN_RMSFUNCTION_H_
#define _BERTHA_SRC_BLOCKS_COMPRESSORPLUGIN_RMSFUNCTION_H_

#include <QVector>

class RmsFunction
{
public:
    RmsFunction();
    virtual ~RmsFunction();
    void setRmsBufferSize(unsigned bufferSize);
    double process(double value);

    void printBuffer() const;

private:
    QVector<double> buffer;
    unsigned bufferIndex;
    double acc;
};

#endif
