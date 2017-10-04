/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Maarten Lambert.                               *
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

#include "bertha/blockdata.h"
#include "bertha/blockinterface.h"
#include "bertha/blockprocessor.h"

#include "common/testutils.h"

#include "tests.h"

using namespace bertha;

void BerthaTest::testPlugin()
{
    TEST_EXCEPTIONS_TRY

    BlockData data(QLatin1String("id"), QLatin1String("Gain"));
    data.setParameter(QLatin1String("gain"), 20);
    BlockProcessor blockProcessor(data, 4096, 44100);
    QCOMPARE(blockProcessor.parameterValue(0).toDouble(), 20.0);
    blockProcessor.setParameterValue(
        0, blockProcessor.prepareParameterValue(0, 30));
    QCOMPARE(blockProcessor.parameterValue(0).toDouble(), 30.0);

    TEST_EXCEPTIONS_CATCH
}
