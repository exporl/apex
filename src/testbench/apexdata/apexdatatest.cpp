/******************************************************************************
 * Copyright (C) 2009  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "apexdatatest.h"

//libdata includes
#include "datablock/datablockdata.h"
#include "calibration/calibrationdata.h"
#include "calibration/soundlevelmeterdata.h"
#include "connection/connectiondata.h"
#include "procedure/apexanswer.h"
#include "corrector/correctordata.h"
#include "map/apexmap.h"

#include "apextools.h"

using namespace apex;
using namespace apex::data;

QString ApexDataTest::name() const
{
    return "libdata";
}

void ApexDataTest::testSimpleParameters()
{
    TEST_COPY(SimpleParameters);

    {
    SimpleParameters params;
    Parameter param0("", "foo", 0, 0, false);
    params.addParameter(param0);
    Parameter param1("", "foo", 0, 1, false);
    params.addParameter(param1);
    EXPECT_EXCEPTION(params.valueByType("foo"));

    params.setId("job");
    Q_FOREACH (Parameter param, params.parameters())
    {
        QVERIFY2(param.owner() == "job", "SimpleParameters::setId did not set "
                                         "the owner of all parameters");
    }
    }

    {
    SimpleParameters params;
    Parameter param0("", "foo", 0, 0, false);
    params.addParameter(param0);
    QCOMPARE(params.valueByType("foo").toInt(), 0);
    Parameter param1("", "foo", 1, NO_CHANNEL, false);
    params.addParameter(param1); //this should replace param0
    QCOMPARE(params.valueByType("foo").toInt(), 1);
    Parameter param2("", "foo", 2, 2, false);
    params.addParameter(param2); //and this should replace param1
    QCOMPARE(params.valueByType("foo").toInt(), 2);
    }
}

void ApexDataTest::testDatablockData()
{
    TEST_COPY_INIT(DatablockData, initDatablockData);
}

void ApexDataTest::initDatablockData(DatablockData* data)
{
    data->setDescription("dummy description");
    data->setDirectData("Some Direct DATA");
    data->setId("this IS nO real data");
}

void ApexDataTest::testCalibrationData()
{
}

void ApexDataTest::testCalibrationParameterData()
{
    TEST_COPY(CalibrationParameterData);

    //test the non-standard ctor
    double minimumParameter = ApexTools::Randn();
    double maximumParameter = ApexTools::Randn();
    double defaultParameter = ApexTools::Randn();
    double muteParameter = ApexTools::Randn();
    double defaultTargetAmplitude = ApexTools::Randn();
    double finalTargetAmplitude = ApexTools::Randn();

    CalibrationParameterData data(minimumParameter,
                                  maximumParameter,
                                  defaultParameter,
                                  muteParameter,
                                  defaultTargetAmplitude,
                                  finalTargetAmplitude);

    QCOMPARE(data.minimumParameter(), minimumParameter);
    QCOMPARE(data.maximumParameter(), maximumParameter);
    QCOMPARE(data.defaultParameter(), defaultParameter);
    QCOMPARE(data.muteParameter(), muteParameter);
    QCOMPARE(data.defaultTargetAmplitude(), defaultTargetAmplitude);
    QCOMPARE(data.finalTargetAmplitude(), finalTargetAmplitude);
}

void ApexDataTest::testConnectionData()
{
    TEST_COPY_INIT(ConnectionData, initConnectionData);
}

void ApexDataTest::initConnectionData(ConnectionData* data)
{
    data->setMatchType(MATCH_REGEXP);
    data->setDevice("dummy");
    data->setFromChannel(0, "dummy from");
    data->setToChannel(1, "dummy to");
}

void ApexDataTest::testCorrectorData()
{
    TEST_COPY_INIT(CorrectorData, initCorrectorData);
}

void ApexDataTest::initCorrectorData(apex::data::CorrectorData* data)
{
    data->setLanguage(CorrectorData::ENGLISH);
    data->addAnswer(1, "foo");
    data->addAnswer(2, "bar");
}

void ApexDataTest::testApexMap()
{
    TEST_COPY(ApexMap);

    //test for isComplete. should return true if electrodes 1..22 are mapped
    //and throw an exception otherwise
    //insert electrodes 1..21
    ApexMap map;
    for (int i = 1; i <= 21; i++)
        map.insert(i, ChannelMap());

    EXPECT_EXCEPTION(map.isComplete());

    //insert the last electrode to make the map complete
    map.insert(22, ChannelMap());
    QVERIFY(map.isComplete());
}

void ApexDataTest::testChannelMap()
{
    TEST_COPY(ChannelMap);
}

void ApexDataTest::testApexAnswer()
{
    TEST_COPY(ApexAnswer);

    //test conversion operator
    ApexAnswer answer;
    answer.setString("foo");
    QVERIFY((QString)answer == "foo");
}

void ApexDataTest::testSoundLevelMeterDataCopy()
{
    TEST_COPY(SoundLevelMeterData);
}

void ApexDataTest::testSoundLevelMeterDataSupported()
{
    QFETCH(QString, frequency_weighting);
    QFETCH(QString, time_weighting);
    QFETCH(QString, type);
    QFETCH(double, percentile);
    QFETCH(int, time);

    SoundLevelMeterData data;
    data.setValueByType("frequency_weighting", frequency_weighting);
    data.setValueByType("time_weighting", time_weighting);
    data.setValueByType("type", type);
    data.setValueByType("percentile", percentile);
    data.setValueByType("time", time);

    QVERIFY2(data.containsSupportedData(), data.errorString().toLatin1());
}

void ApexDataTest::testSoundLevelMeterDataSupported_data()
{
    QTest::addColumn<QString>("frequency_weighting");
    QTest::addColumn<QString>("time_weighting");
    QTest::addColumn<QString>("type");
    QTest::addColumn<double>("percentile");
    QTest::addColumn<int>("time");

    QTest::newRow("good1") << "z" << "s" << "rms"  << 0.0 << 1;
    QTest::newRow("good2") << "a" << "f" << "rms"  << 1.0 << 10;
    QTest::newRow("good3") << "c" << "i" << "peak" << 0.5 << 9999;
}


void ApexDataTest::testSoundLevelMeterDataUnsupported()
{
    QFETCH(QString, frequency_weighting);
    QFETCH(QString, time_weighting);
    QFETCH(QString, type);
    QFETCH(double, percentile);
    QFETCH(int, time);

    SoundLevelMeterData data;
    data.setValueByType("frequency_weighting", frequency_weighting);
    data.setValueByType("time_weighting", time_weighting);
    data.setValueByType("type", type);
    data.setValueByType("percentile", percentile);
    data.setValueByType("time", time);

    QVERIFY2(!data.containsSupportedData(), data.errorString().toLatin1());
}


void ApexDataTest::testSoundLevelMeterDataUnsupported_data()
{
    QTest::addColumn<QString>("frequency_weighting");
    QTest::addColumn<QString>("time_weighting");
    QTest::addColumn<QString>("type");
    QTest::addColumn<double>("percentile");
    QTest::addColumn<int>("time");

    QTest::newRow("bad fw")    << "f" << "s" << "rms"  << 0.0  << 1;
    QTest::newRow("bad tw")    << "a" << "t" << "rms"  << 1.0  << 10;
    QTest::newRow("bad type")  << "c" << "i" << "type" << 0.5  << 9999;
    QTest::newRow("bad perc")  << "z" << "s" << "rms"  << 1.1  << 1;
    QTest::newRow("bad perc2") << "z" << "s" << "rms"  << -0.9 << 1;
    QTest::newRow("bad time")  << "z" << "s" << "rms"  << 0.0  << -1;
    QTest::newRow("bad time2") << "z" << "s" << "rms"  << 0.0  << 0;
}

Q_EXPORT_PLUGIN2(test_libdata, ApexDataTest);














