/******************************************************************************
 * Copyright (C) 2017  Sanne Raymaekers <sanne.raymaekers@gmail.com>          *
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

#include "commontest.h"

#include "common/global.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"
#include "common/testutils.h"

#include <QTemporaryFile>
#include <QTest>

using namespace cmn;

void CommonTest::sndfilewrappertest()
{
    TEST_EXCEPTIONS_TRY

    const QString inputPath = Paths::searchFile(QL1S("testdata/sin1000Hz.wav"),
                                                Paths::dataDirectories());
    QTemporaryFile outputFile(QL1S("common-sndwrappertest-XXXXXX.wav"));
    outputFile.open();
    outputFile.close();

    SF_INFO inputSfInfo;
    SndFile input(inputPath, SFM_READ, &inputSfInfo);

    SF_INFO outputSfInfo;
    outputSfInfo.samplerate = inputSfInfo.samplerate;
    outputSfInfo.channels = inputSfInfo.channels;
    outputSfInfo.format = inputSfInfo.format;
    SndFile output(outputFile.fileName(), SFM_RDWR, &outputSfInfo);

    QVector<float> buffer(4096, 0);
    while (unsigned readSize = sf_readf_float(input.get(), buffer.data(), 4096))
        sf_writef_float(output.get(), buffer.data(), readSize);

    sf_seek(input.get(), 0, SEEK_SET);
    output.reset(outputFile.fileName(), SFM_READ, &outputSfInfo);
    QVector<float> compareBuffer(4096, 0);
    while (unsigned readSize =
               sf_readf_float(input.get(), buffer.data(), 4096)) {
        sf_readf_float(output.get(), compareBuffer.data(), readSize);
        ARRAYCOMP(buffer, compareBuffer, readSize);
    }

    SF_INFO sfInfo;
    QVERIFY_EXCEPTION_THROWN(SndFile(QSL("non-existant"), SFM_READ, &sfInfo),
                             std::exception);

    TEST_EXCEPTIONS_CATCH
}
