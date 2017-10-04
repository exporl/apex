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

#include "matrixmixer.h"
#include "utils/stringutils.h"
using namespace streamapp;

tStringVector MatrixMixer::mf_PrintMatrix() const
{
    const unsigned nIChan = mf_nGetNumInputChannels();
    const unsigned nOChan = mf_nGetNumOutputChannels();

    tStringVector ret;
    std::string tempLine("__| ");
    for (unsigned i = 0; i < nOChan; ++i)
        tempLine.append(toString(i) + " ");
    ret.push_back(tempLine + "\n");

    for (unsigned i = 0; i < nIChan; ++i) {
        tempLine.clear();
        tempLine.append(toString(i) + " | ");
        for (unsigned j = 0; j < nOChan; ++j) {
            const double d = mf_dGetMatrixNode(i, j);
            if (d == sc_dMixerMin)
                tempLine.append("x ");
            else
                tempLine.append(toString(mf_dGetMatrixNode(i, j)) + " ");
        }
        ret.push_back(tempLine + "\n");
    }
    return ret;
}
