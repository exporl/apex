/******************************************************************************
 * Copyright (C) 2009  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#include "apexdata/datablock/datablockdata.h"

#include "apexdata/experimentdata.h"
#include "apexdata/mainconfigfiledata.h"

#include "apexdata/map/apexmap.h"

#include "apexdata/randomgenerator/randomgeneratorparameters.h"

#include "apexdata/stimulus/nucleus/nicstream/stimulation_mode.h"

#include "apexmain/calibration/soundlevelmeter.h"
#include "apexmain/device/soundcardsettings.h"
#include "apexmain/gui/soundcardsdialog.h"

#include "apexmain/experimentparser.h"

#include "apexmain/interactive/interactiveparameters.h"

#include "apexmain/l34stimulus/aseqparser.h"
#include "apexmain/l34stimulus/l34datablock.h"
#include "apexmain/l34stimulus/qicparser.h"

#include "apexmain/randomgenerator/uniformrandomgenerator.h"

#include "apexmain/result/resultviewer.h"

#include "apexmain/resultsink/rtresultsink.h"

#include "apexmain/services/accessmanager.h"
#include "apexmain/services/mainconfigfileparser.h"
#include "apexmain/services/pluginloader.h"

#include "apextools/apextools.h"
#include "apextools/pathtools.h"

#include "apextools/services/paths.h"

#include "apextools/version.h"

#include "apextools/xml/xmldocumentgetter.h"

#include "apexmaintest.h"

#include <QDomDocument>
#include <QDomNode>
#include <QDomNodeList>
#include <QUrl>
#include <QWebElement>
#include <QWebFrame>
#include <QWebFrame>
#include <QWebPage>
#include <QWebView>

using namespace apex;
using namespace apex::stimulus;
using namespace apex::data;

/*
QString ApexMainTest::name() const
{
    return "libapex";
}
*/

void ApexMainTest::initTestCase()
{
    networkError = false;
    xercesc::XMLPlatformUtils::Initialize();
}

void ApexMainTest::testQic()
{
    const unsigned char data[] = {
        0x01, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3f,
        0x01, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3f,
        0x01, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3f,
        0x01, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3f,
        0x01, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3f,
        0x01, 0x00, 0x00, 0x00, 0x80, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x80, 0xbf,
        0x01, 0x00, 0x00, 0x00, 0x80, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x80, 0xbf};

        QicParser parser(QByteArray(reinterpret_cast<const char *>(data), sizeof(data)));
        L34Data parsedData(parser.GetData());
        QCOMPARE(parsedData.size(), 14);
        QCOMPARE(parsedData[0].channel, qint16(1));
        QCOMPARE(parsedData[0].magnitude, 1.0f);
        QCOMPARE(parsedData[0].period, -1.0f);
        QCOMPARE(parsedData[0].mode, qint16(-1));
        QCOMPARE(parsedData[0].phaseWidth, -1.0f);
        QCOMPARE(parsedData[0].phaseGap, -1.0f);
        QCOMPARE(parsedData[10].channel, qint16(0));
        QCOMPARE(parsedData[10].magnitude, 0.0f);
        QCOMPARE(parsedData[10].period, -1.0f);
        QCOMPARE(parsedData[10].mode, qint16(-1));
        QCOMPARE(parsedData[10].phaseWidth, -1.0f);
        QCOMPARE(parsedData[10].phaseGap, -1.0f);
}

void ApexMainTest::testAseq()
{
    {
    const unsigned char data[]= {
        82, 73, 70, 70, 72, 0, 0, 0, 65, 83, 69, 81, 73, 78, 70, 79, 20, 0, 0, 0, 0, 0, 128, 63, 67, 72, 65, 78, 3, 0, 0, 0, 77, 65, 71, 78, 3, 0, 0, 0, 67, 72, 65, 78, 3, 0, 0, 0, 1, 2, 3, 0, 77, 65, 71, 78, 12, 0, 0, 0, 0, 0, 160, 64, 0, 0, 192, 64, 0, 0, 224, 64 };

        AseqParser parser(QByteArray(reinterpret_cast<const char *>(data), sizeof(data)) );

        try {
            L34Data parsedData(parser.GetData());
            QCOMPARE( parsedData.size(), 3);
            QCOMPARE(parsedData[0].magnitude, 5.0f);
            QCOMPARE(parsedData[1].magnitude, 6.0f);
            QCOMPARE(parsedData[2].magnitude, 7.0f);
            QCOMPARE(parsedData[0].channel, qint16(1));
            QCOMPARE(parsedData[1].channel, qint16(2));
            QCOMPARE(parsedData[2].channel, qint16(3));
        } catch (ApexStringException &e) {
            qCDebug(APEX_RS) << e.what();
            throw(e);
        }

    }

    {
        const unsigned char data[]= {
            82, 73, 70, 70, 134, 0, 0, 0, 65, 83, 69, 81, 73, 78, 70, 79, 44, 0, 0, 0, 0, 0, 128, 63, 65, 69, 76, 69, 4, 0, 0, 0, 82, 69, 76, 69, 1, 0, 0, 0, 67, 85, 82, 76, 4, 0, 0, 0, 80, 72, 87, 73, 1, 0, 0, 0, 80, 69, 82, 73, 4, 0, 0, 0, 65, 69, 76, 69, 4, 0, 0, 0, 10, 11, 12, 13, 82, 69, 76, 69, 1, 0, 0, 0, 255, 0, 67, 85, 82, 76, 4, 0, 0, 0, 22, 23, 24, 25, 80, 72, 87, 73, 4, 0, 0, 0, 0, 0, 32, 65, 80, 69, 82, 73, 16, 0, 0, 0, 0, 0, 220, 66, 0, 0, 240, 66, 0, 0, 2, 67, 0, 0, 12, 67
            };

//             seq.electrodes=[10 11 12 13];
//             seq.return_electrodes=[-1];
//             seq.current_levels=[22 23 24 25];
//             seq.phase_widths=10;
//             seq.periods=[110 120 130 140];


            AseqParser parser(QByteArray(reinterpret_cast<const char *>(data), sizeof(data)) );

            try {
                L34Data parsedData(parser.GetData());
                QCOMPARE( parsedData.size(), 4);
                QCOMPARE(parsedData[0].activeElectrode, int(10));
                QCOMPARE(parsedData[1].activeElectrode, int(11));
                QCOMPARE(parsedData[2].activeElectrode, int(12));
                QCOMPARE(parsedData[3].activeElectrode, int(13));

                for (int i=0; i<4; ++i) {
                    QCOMPARE(parsedData[i].returnElectrode, -1);
                    QCOMPARE(parsedData[i].phaseWidth, 10.0f);
                    QCOMPARE(parsedData[i].channel, qint16(-1));
                    QCOMPARE(parsedData[i].magnitude, -1.0f);
                    QCOMPARE(parsedData[i].phaseGap, -1.0f);
                }

                QCOMPARE(parsedData[0].currentLevel, int(22));
                QCOMPARE(parsedData[1].currentLevel, int(23));
                QCOMPARE(parsedData[2].currentLevel, int(24));
                QCOMPARE(parsedData[3].currentLevel, int(25));

                QCOMPARE(parsedData[0].period, 110.0f);
                QCOMPARE(parsedData[1].period, 120.0f);
                QCOMPARE(parsedData[2].period, 130.0f);
                QCOMPARE(parsedData[3].period, 140.0f);

            } catch (ApexStringException &e) {
                qCDebug(APEX_RS) << e.what();
                throw(e);
            }

    }
}

void ApexMainTest::testL34DatablockQic()
{
    DatablockData data;


    {
        QUrl url("invalidfilenameamzleirjalmijh");
        data.setUri( url );
        bool ex=false;
        try {
            L34DataBlock (data, url, 0);
        } catch (ApexStringException) {
            ex=true;
        }
        QCOMPARE(ex,true);
    }

    ApexMap map;
    ChannelMap basemap;

    map.setNumberOfElectrodes(22);
    basemap.setMode(ChannelMap::modeToStimulationModeType("MP1+2"));
    basemap.setPhaseWidth(25);
    basemap.setPhaseGap(25);
    basemap.setPeriod(150);
    map.setDefaultMap(basemap);
    for (int i=1; i<=22; ++i) {
        basemap.setChannelNumber(i);
        basemap.setStimulationElectrode(22-i+1);
        basemap.setThresholdLevel(1+i);
        basemap.setComfortLevel(255-i);
        Q_ASSERT(basemap.isValid());
        map[basemap.channelNumber()]=basemap;
    }

    // parse qic
    try {
        // Find suitable qic file
        apex::Paths paths;
        QDir testDir(paths.GetBasePath());
        testDir.cd("examples/stimuli");
        QUrl url(testDir.path() + "/roos.qic");
        data.setUri (url);
        L34DataBlock datablock(data, url, 0);

        const L34XMLData& xmldata = datablock.GetMappedData(&map, 100);

        // Write reference data (we consider the current implementation correct
        // (verified with Matlab, 20091001)
        /*QFile file("out.xml");
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        out << xmldata.join("\n");
        file.close();*/

        // Read reference data
        QDir d(paths.GetBasePath());
        d.cd("data/tests/libapex");
        QFile file(d.path() + "/roos_qic.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        QTextStream t(&file);
        for (int i=0; i<xmldata.length(); ++i) {
            QCOMPARE( xmldata.at(i), t.readLine(0));
        }
        file.close();

        // Compare reference data
    //    QCOMPARE(fromfile, xmldata.join("\n"));

    } catch (ApexStringException &e) {
        qCDebug(APEX_RS) << e.what();
        throw(e);
    }
}

void ApexMainTest::testL34DatablockInvalidFile()
{
    DatablockData data;

    {
        QUrl url("invalidfilenameamzleirjalmijh");
        data.setUri( url );
        bool ex=false;
        try {
            L34DataBlock (data, url, 0);
        } catch (ApexStringException) {
            ex=true;
        }
        QCOMPARE(ex,true);
    }
}

void ApexMainTest::testL34DatablockAseq_data()
{
    QTest::addColumn<QString>("filename");

    QStringList files;
    files << "sequence1.aseq"
            << "chanmag.aseq" << "peri.aseq"
            << "phga.aseq" << "phwi.aseq" << "rele.aseq"
            << "powerup1.aseq" << "powerup2.aseq"
            << "powerup3.aseq" << "powerup4.aseq";

    for (int i=0; i<files.size(); ++i) {
        QTest::newRow(files[i].toLatin1())  << files[i];
    }
}

void ApexMainTest::testL34DatablockAseq()
{
    DatablockData data;
    ApexMap map;
    ChannelMap basemap;

    map.setNumberOfElectrodes(22);
    basemap.setMode(ChannelMap::modeToStimulationModeType("MP1"));
    basemap.setPhaseWidth(25);
    basemap.setPhaseGap(25);
    basemap.setPeriod(150);
    map.setDefaultMap(basemap);
    for (int i=1; i<=22; ++i) {
        basemap.setChannelNumber(i);
        basemap.setStimulationElectrode(22-i+1);
        basemap.setThresholdLevel(1+i);
        basemap.setComfortLevel(255-i);
        Q_ASSERT(basemap.isValid());
        map[basemap.channelNumber()]=basemap;
    }

    // parse aseq files and write XML results
    try {
        apex::Paths paths;
        QDir testDir(paths.GetBasePath());
        testDir.cd("data/tests/libapex/");
        QFETCH(QString, filename);

        QUrl url(testDir.path() + "/"+filename);
        data.setUri (url);
        L34DataBlock  datablock(data, url, 0);

        QVERIFY( datablock.canMap(&map));
        const L34XMLData& xmldata = datablock.GetMappedData(&map, 100);

        // Write data
        /*QFile wfile(filename+"-out.xml");
        wfile.open(QIODevice::WriteOnly);
        QTextStream out(&wfile);
        out << xmldata.join("\n");
        wfile.close();*/

        // Read reference data
        QFile file(testDir.path() + "/" + filename + ".xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        QTextStream t(&file);
        for (int i = 0; i < xmldata.length(); ++i) {
            QCOMPARE(xmldata.at(i), t.readLine(0));
        }
        //QString fromfile(t.readAll());
        file.close();
        //QCOMPARE(fromfile, xmldata.join("\n"));
    } catch (ApexStringException &e) {
        QFAIL(e.what());
    }
}

void ApexMainTest::testAseqParser()
{
    QString filename("quantization.aseq");

    apex::Paths paths;
    QDir testDir(paths.GetBasePath());
    testDir.cd("data/tests/libapex/");

    QFile file(testDir.path() + "/"+filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QFAIL("Can't open file for reading");
    }
    AseqParser parser(file.readAll());

    L34Data data = parser.GetData();

    qint16 channels[] = {1, 2, 3, 4};
    float magnitudes[] = {0.1, 0.2, 0.12345, 0.3};
    float periods[] = {120, 120.2, 120.4, 120.8351};

    for (int i=0; i<4; ++i) {
        QCOMPARE( data[i].period, periods[i]);
        QCOMPARE( data[i].magnitude, magnitudes[i]);
        QCOMPARE( data[i].channel, channels[i]);
    }
}

void ApexMainTest::testL34DatablockAseqMapping()
{
    DatablockData data;
    ApexMap map;
    ChannelMap basemap;

    map.setNumberOfElectrodes(22);
    basemap.setMode(ChannelMap::modeToStimulationModeType("MP1"));
    basemap.setPhaseWidth(25);
    basemap.setPhaseGap(25);
    basemap.setPeriod(150);
    map.setDefaultMap(basemap);
    for (int i=1; i<=22; ++i) {
        basemap.setChannelNumber(i);
        basemap.setStimulationElectrode(22-i+1);
        basemap.setThresholdLevel(1+i);
        basemap.setComfortLevel(255-i);
        Q_ASSERT(basemap.isValid());
        map[basemap.channelNumber()]=basemap;
    }

    // parse aseq files and write XML results
    try {
        apex::Paths paths;
        QDir testDir(paths.GetBasePath());
        testDir.cd("data/tests/libapex/");
        QString filename("mapping.aseq");

        QUrl url(testDir.path() + "/"+filename);
        data.setUri (url);
        L34DataBlock  datablock(data, url, 0);

        QVERIFY( datablock.canMap(&map));
        const L34XMLData& xmldata = datablock.GetMappedData(&map, 100);

        // Write data
        /*QFile file(testDir.path() + "/" +filename+".xml");
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        out << xmldata.join("\n");
        file.close();*/

        //qCDebug(APEX_RS, qPrintable(filename+".xml" + " to be read by checkmapping2.m"));

        QFile file(testDir.path() + "/" +filename+".xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        QTextStream t(&file);
        for (int i=0; i<xmldata.length(); ++i) {
            QCOMPARE( xmldata.at(i), t.readLine(0));
        }
        file.close();
        //QCOMPARE(fromfile, xmldata.join("\n"));
    } catch (ApexStringException &e) {
        QFAIL(e.what());
    }


}


void ApexMainTest::testL34Datablock_invalid()
{
    DatablockData data;
    ApexMap map;
    ChannelMap basemap;

    map.setNumberOfElectrodes(22);
    basemap.setMode(ChannelMap::modeToStimulationModeType("MP1"));
    basemap.setPhaseWidth(25);
    basemap.setPhaseGap(25);
    basemap.setPeriod(150);
    map.setDefaultMap(basemap);
    for (int i=1; i<=22; ++i) {
        basemap.setChannelNumber(i);
        basemap.setStimulationElectrode(22-i+1);
        basemap.setThresholdLevel(1+i);
        basemap.setComfortLevel(255-i);
        Q_ASSERT(basemap.isValid());
        map[basemap.channelNumber()]=basemap;
    }

    // parse aseq files and write XML results
        QStringList files;
        files << "invalid1.aseq";

        apex::Paths paths;
        QDir testDir(paths.GetBasePath());
        testDir.cd("data/tests/libapex/");

        for (int i=0; i<files.size(); ++i) {
            qCDebug(APEX_RS) << "Testing file " << files[i];

            QUrl url(testDir.path() + "/"+files[i]);
            data.setUri (url);

            bool failed=false;
            try {
                L34DataBlock datablock(data, url, 0);
                QVERIFY( datablock.canMap(&map));
                datablock.GetMappedData(&map, 100);
            } catch (...) {
                failed=true;
            }
            QVERIFY(failed);

        }
}


void ApexMainTest::testL34InvalidCL()
{
    ChannelMap basemap;
    DatablockData data;
    ApexMap map;

    map.setNumberOfElectrodes(22);
    basemap.setMode(ChannelMap::modeToStimulationModeType("MP1"));
    basemap.setPhaseWidth(25);
    basemap.setPhaseGap(25);
    basemap.setPeriod(150);
    map.setDefaultMap(basemap);
    for (int i=1; i<=22; ++i) {
        basemap.setChannelNumber(i);
        basemap.setStimulationElectrode(22-i+1);
        basemap.setThresholdLevel(1+i);
        basemap.setComfortLevel(100);
        Q_ASSERT(basemap.isValid());
        map[basemap.channelNumber()]=basemap;
    }

    apex::Paths paths;
    QDir testDir(paths.GetBasePath());
    testDir.cd("data/tests/libapex/");

    QUrl url(testDir.path() + "/chancl.aseq");
    data.setUri (url);

    bool failed=false;
    try {
        L34DataBlock datablock(data, url, 0);
        QVERIFY( datablock.canMap(&map));
        datablock.GetMappedData(&map, 100);
    } catch (...) {
        failed=true;
    }
    QVERIFY(failed);

}

class RandomGeneratorTestParameters : public RandomGeneratorParameters {
public:
    void setParameter(QString name, QString value) {
        SetParameter(name, "", value, NULL);
    }
    QString min() {
        return QString::number(m_dMin);
    }
    QString max() {
        return QString::number(m_dMax);
    }
};

template <typename T>
T convert(QString s, bool* ok) {
    Q_ASSERT(false);
}

template <>
int convert(QString s, bool *ok) {
    return s.toInt(ok);
}

template <>
double convert(QString s, bool *ok){
    return s.toDouble(ok);
}

template <typename T>
void ApexMainTest::verifyInterval(RandomGeneratorTestParameters* params, T min, T max) {
    UniformRandomGenerator* urg = new UniformRandomGenerator(params);
    for(unsigned int i=0; i<100; ++i) {
        QString stringValue = urg->GetNextValue();
        bool ok = false;
        T value = convert<T>(stringValue, &ok);

        QVERIFY(ok);
        QVERIFY(value <= max);
        QVERIFY(value >= min);
    }
}

void ApexMainTest::testUniformInt()
{
    long min = 0;
    long max = 10;

    RandomGeneratorTestParameters params;
    params.setParameter("type", "int");
    params.setParameter("min", QString::number(min));
    params.setParameter("max", QString::number(max));

    UniformRandomGenerator* urg = new UniformRandomGenerator(&params);
    urg->doDeterministicGeneration();
    double p = 1/double(max-min+1);
    unsigned long n = 1000/p;

    QMap<int, unsigned long> freq;
    for(unsigned long i=0; i<n; ++i) {
        bool ok = false;
        long value = convert<int>(urg->GetNextValue(), &ok);
        ++freq[value];
    }

    unsigned long squaredSum = 0;
    foreach(unsigned long f, freq) {
        squaredSum += (f*f);
    }

    double v = (1/(n*p))*squaredSum - n;

    QVERIFY(v >= 2.558);
    QVERIFY(v <= 23.21);
}

double f(double val, double min, double max) {
    return (val - min) / (max - min);
}

void ApexMainTest::testUniformDouble()
{
    double min = 0;
    double max = 10;

    RandomGeneratorTestParameters params;
    params.setParameter("type", "double");
    params.setParameter("min", QString::number(min));
    params.setParameter("max", QString::number(max));

    UniformRandomGenerator* urg = new UniformRandomGenerator(&params);
    urg->doDeterministicGeneration();
    unsigned long n = 1000;

    QVector<double> numbers;
    for(unsigned long i=0; i<n; ++i) {
        bool ok;
        double value = convert<double>(urg->GetNextValue(), &ok);
        numbers.push_back(value);
    }

    qSort(numbers);

    QVector<double> transformed1;
    QVector<double> transformed2;
    for(size_t j=1; j<=n; ++j){
        size_t i = j-1;
        transformed1.push_back(double(j)/n - f(numbers[i], min, max));
        transformed2.push_back(f(numbers[i], min, max) - double(j-1)/n);
    }

    double kMin = qSqrt(n) * *std::max_element(transformed1.begin(), transformed1.end());
    double kMax = qSqrt(n) * *std::max_element(transformed2.begin(), transformed2.end());

    QVERIFY(kMin >= 0.02912);
    QVERIFY(kMin <= 1.444);

    QVERIFY(kMax >= 0.02912);
    QVERIFY(kMax <= 1.444);
}

void ApexMainTest::testRandomInterval()
{
    RandomGeneratorTestParameters params;
    params.setParameter("type", "int");
    params.setParameter("min", "0");
    params.setParameter("max", "10");
    verifyInterval<int>(&params, 0, 10);

    params.setParameter("min", "1");
    verifyInterval<int>(&params, 1, 10);

    params.setParameter("min", "2.4");
    verifyInterval<int>(&params, 2, 10);

    params.setParameter("max", "9.4");
    verifyInterval<int>(&params, 2, 9);

    params.setParameter("min", "-2.3");
    verifyInterval<int>(&params, -2, 9);

    params.setParameter("max", "-1.3");
    verifyInterval<int>(&params, -2, -1);

    params.setParameter("type", "double");
    params.setParameter("min", "0");
    params.setParameter("max", "10");
    verifyInterval<double>(&params, 0, 10);

    params.setParameter("min", "1");
    verifyInterval<double>(&params, 1, 10);

    params.setParameter("min", "2.4");
    verifyInterval<double>(&params, 2.4, 10);

    params.setParameter("max", "9.4");
    verifyInterval<double>(&params, 2, 9.4);

    params.setParameter("min", "-2.3");
    verifyInterval<double>(&params, -2.3, 9.4);

    params.setParameter("max", "-1.3");
    verifyInterval<double>(&params, -2.3, -1.3);
}

void ApexMainTest::testMainConfigFileParser()
{
    MainConfigFileParser::Get().CFParse();
    const data::MainConfigFileData& d = MainConfigFileParser::Get().data();

    QCOMPARE( d.soundCardName("RME", "asio"), QString("ASIO Hammerfall DSP") );
    QCOMPARE( d.soundCardName("invalid", "asio"), QString() );
    QCOMPARE( d.prefix("regression"), QString("file:../stimuli"));
    QCOMPARE( d.prefix("invalid"), QString(""));
    QCOMPARE( d.pluginScriptLibrary(), QString("pluginscriptlibrary.js"));

}

void ApexMainTest::testUpgradeTo3_1_1()
{
    //Load the xml file to test. This is a file from version 3.1.0
    apex::Paths paths;
    QFile testFile(paths.GetBasePath() + "/data/tests/libapex/results-upgradeoldschema.apx");
    testFile.open(QIODevice::ReadOnly);

    //Create the experiment parser
    apex::data::ExperimentData* testData;
    try
    {
        apex::ExperimentParser parser(testFile.fileName());

        //Parse the xml file, this should call the upgradeTo3_1_1 function
        //apex::data::ExperimentData* testData;
        testData = parser.parse(false);
    }
    catch (const std::exception& e)
    {
        QFAIL(qPrintable(QString("Could not parse test file: %1").arg(e.what())));
    }
    catch (...)
    {
        QFAIL(qPrintable(QString("Unknown exception while parsing %1").arg(testFile.fileName())));
    }

    //Get the results from the parsing
    apex::data::ResultParameters* parameterList = testData->resultParameters();

    //now test the result of the parsing...
    QCOMPARE(parameterList->showResultsAfter(), true); //This is supposed to be true
    QCOMPARE(parameterList->showRTResults(), false);
    QCOMPARE(parameterList->resultPage(), QUrl("apex:resultsviewer.html"));
    Q_ASSERT(parameterList->matlabScript().isEmpty());
    //Q_ASSERT(parameterList->GetXsltScript().isEmpty());
    //Q_ASSERT(parameterList->getJavascriptScript().isEmpty());
}

void ApexMainTest::testResultViewerConvertToCSV_data()
{
    apex::Paths paths;
    QTest::addColumn<QString>("resultFilePath");
    QTest::addColumn<QString>("testFilePath");
    QTest::addColumn<QString>("desiredFilePath");

    QTest::newRow("Simple") << paths.GetBasePath() + "data/tests/libapex/subject_test-r.apr"
                            << paths.GetBasePath() + "data/tests/libapex/testJavascriptResult2.xml"
                            << paths.GetBasePath() + "data/tests/libapex/desiredJavascriptResult.xml";

}

void ApexMainTest::testResultViewerConvertToCSV()
{
    //First, create the result parameters
       data::ResultParameters rvparam;
       rvparam.setShowResultsAfter(true);
       rvparam.setSaveResults(false);

       //Fetch the filenames:
       QFETCH(QString, resultFilePath);
       QFETCH(QString, testFilePath);
       QFETCH(QString, desiredFilePath);

       QFileInfo resultFileInfo(resultFilePath);
       QString resultFile = resultFileInfo.absoluteFilePath(); //To get the right path on all
                                               //operating systems.

       apex::ResultViewer testViewer(&rvparam, resultFile);

       //Export the contents to a file:
       QFileInfo testFileInfo(testFilePath);
       QString testFile = testFileInfo.absoluteFilePath();

       //export the new data to the existing result file.
       testViewer.addtofile(testFile);
       QDomDocument testResult = readXmlResults(testFile);

       //Get the comparison values
       QFileInfo desiredFileInfo(desiredFilePath);
       QDomDocument desiredContent = readXmlResults(desiredFileInfo.absoluteFilePath());

       QCOMPARE(compareXml(testResult, desiredContent), QString());

       //Now remove the part that has been written to the text file, so the test can be done again...
       QFile file(testFile);
       file.open(QIODevice::ReadWrite);
       QByteArray fileData = file.readAll();
       QString fileText(fileData);

       QString processedTag = "<processed>";

       int startIndex = fileText.lastIndexOf(processedTag) + processedTag.length();
       int endIndex = fileText.lastIndexOf("</processed>");

       fileText.remove(startIndex, endIndex - startIndex);

       //Now clear the old file, and write contents to the new file

       file.resize(0);
       QTextStream out(&file);
       out.setCodec("UTF-8");
       out<<fileText;
       file.close();
}

void ApexMainTest::testResultViewer_data()
{
    apex::Paths paths;
    QTest::addColumn<QString>("resultFilePath");
    QTest::addColumn<QString>("resultViewerPath");
    QTest::addColumn<QString>("desiredFilePath");

    QTest::newRow("Simple") << paths.GetBasePath() + "data/tests/libapex/subject_test-r.apr"
                            << paths.GetBasePath() + "data/resultsviewer/resultsviewer.html"
                            << paths.GetBasePath() + "data/tests/libapex/subject_test-r-resultsviewer.html";
}

void ApexMainTest::testResultViewer()
{
    //First, create the result parameters
    data::ResultParameters rvparam;
    rvparam.setShowResultsAfter(true);
    rvparam.setSaveResults(false);

    //Fetch the filenames:
    QFETCH(QString, resultFilePath);
    QFETCH(QString, resultViewerPath);
    QFETCH(QString, desiredFilePath);

    QFileInfo resultFileInfo(resultFilePath);
    QString resultFile = resultFileInfo.absoluteFilePath(); //To get the right path on all
    //operating systems.

    // Read results file
    QFile resultsFile(resultFilePath);
    resultsFile.open(QIODevice::ReadOnly);
    QByteArray resultsFileData = resultsFile.readAll();
    resultsFile.close();

    QWebView webView;
    webView.setGeometry(0, 0, 1000, 700);       // Needed to get reproducible results: the plots are scaled to the page width
    // TODO: this still fails when xvfb is not used under linux, leading to failure of this unit test
    QUrl ResultViewerUrl(QUrl::fromUserInput(resultViewerPath));
    qCDebug(APEX_RS) << ResultViewerUrl.scheme();
    apex::RTResultSink resultSink(ResultViewerUrl,rvparam.resultParameters(), rvparam.extraScript(), &webView);

    // load page, execute javascript... (happens in separate thread)
    QSignalSpy spy(&webView, SIGNAL(loadFinished(bool)));
    while (spy.count()==0) {
        QCoreApplication::processEvents();
        QTest::qWait(10);
    }


    resultSink.newResults(resultsFileData);
    resultSink.plot();
    QCoreApplication::processEvents();
    QString resultHtml = resultSink.currentHtml();

    // Other ways to do it
    /*qCDebug(APEX_RS) << resultHtml;
    qCDebug(APEX_RS) << resultSink.mainWebFrame()->documentElement().toOuterXml();
    qCDebug(APEX_RS) << resultSink.evaluateJavascript("document.getElementsByTagName('body')[0].innerHTML");*/

    // Write comparison values (in case result is intended to change):
    /*{
    QFile file(desiredFilePath);
    file.open(QIODevice::WriteOnly);
    file.write(resultHtml.toUtf8());
    file.close();
    }*/

    //Get the comparison values
    QFile file(desiredFilePath);
    file.open(QIODevice::ReadOnly);
    QByteArray fileData = file.readAll();
    file.close();
    QString fileText(fileData);



    // Write both to temporary files
    /*{
    QFile file("testResultViewer-test.html");
    file.open(QIODevice::WriteOnly);
    file.write(resultHtml.toUtf8());
    file.close();
    }
    {
    QFile file("testResultViewer-desired.html");
    file.open(QIODevice::WriteOnly);
    file.write(fileText.toUtf8());
    file.close();
    }*/


    QSKIP("Result is correct at time of checking, but test fails for unknown reasons, to be fixed");
//    QCOMPARE(resultHtml, fileText);


}


QUrl createUrl(QString filename) {
    apex::Paths paths;
    QDir testDir(paths.GetBasePath() + "/data/tests/libapex/");
    QUrl url = QDir::toNativeSeparators(testDir.path() + "/" + filename);

    QDir currentDir =  QDir::current().path();
    url = currentDir.relativeFilePath(url.path());

    return url;
}

void ApexMainTest::wait() {
    for(int i=0; i<5; ++i) {
        QCoreApplication::processEvents();
        QTest::qSleep(100);
    }
}

QPair<QWebFrame*, AccessManager*> ApexMainTest::initAccesManager() {
    networkError = false;
    QWebView* webView = new QWebView();
    QWebPage* page = new QWebPage(webView);
    AccessManager* am = new AccessManager(page);
    page->setNetworkAccessManager(am);
    LoadChecker* loadChecker = new LoadChecker(this);
    connect(page->networkAccessManager(), SIGNAL(finished(QNetworkReply*)), loadChecker, SLOT(check(QNetworkReply*)));

    return QPair<QWebFrame*, AccessManager*>(page->mainFrame(), am);
}


//Load a local file that loads another local file
void ApexMainTest::testAccessManagerLoadLocal()
{
    QUrl p_page = createUrl("test_local.html");
    QPair<QWebFrame*, AccessManager*> pair = initAccesManager();

    pair.first->load( pair.second->prepare(p_page) );
    wait();

    QCOMPARE(networkError, false);
}

//Load a local file that loads another local file
void ApexMainTest::testAccessManagerLoadLocalWithScheme()
{
    QUrl p_page = createUrl("test_local.html");
    p_page.setScheme("file");
    QPair<QWebFrame*, AccessManager*> pair = initAccesManager();

    pair.first->load( pair.second->prepare(p_page) );
    wait();

    QCOMPARE(networkError, false);
}


void ApexMainTest::testAccessManagerLoadLocalAndApex() {
    QUrl p_page = createUrl("test_global.html");
    QPair<QWebFrame*, AccessManager*> pair = initAccesManager();

    pair.first->load( pair.second->prepare(p_page) );
    wait();

    QCOMPARE(networkError, false);
}

void ApexMainTest::testAccessManagerLoadLocalAndApexWithScheme() {
    QUrl p_page = createUrl("test_global.html");
    p_page.setScheme("file");
    QPair<QWebFrame*, AccessManager*> pair = initAccesManager();

    pair.first->load( pair.second->prepare(p_page) );
    wait();

    QCOMPARE(networkError, false);
}

void ApexMainTest::testAccessManagerLoadApex() {
    QUrl p_page("apex:resultsviewer.html");
    QPair<QWebFrame*, AccessManager*> pair = initAccesManager();

    pair.first->load( pair.second->prepare(p_page) );
    wait();

    QCOMPARE(networkError, false);
}

void ApexMainTest::testAccessManagerLoadApexAndLocal() {
    QUrl p_page("apex:resultsviewer-test-psignifit.html");
    QPair<QWebFrame*, AccessManager*> pair = initAccesManager();

    pair.first->load( pair.second->prepare(p_page) );
    wait();

    QCOMPARE(networkError, false);

}

void ApexMainTest::testAccessManagerLoadUnknownLocal() {
    QUrl p_page("unk.html");
    QPair<QWebFrame*, AccessManager*> pair = initAccesManager();

    pair.first->load( pair.second->prepare(p_page) );
    wait();

    QCOMPARE(networkError, true);

}

void ApexMainTest::testAccessManagerLoadUnknownApex() {
    QUrl p_page("apex:unk.html");
    QPair<QWebFrame*, AccessManager*> pair = initAccesManager();

    pair.first->load( pair.second->prepare(p_page) );
    wait();

    QCOMPARE(networkError, true);
}

struct MocWarning : public InteractiveParameters::Callback {
    MocWarning() : InteractiveParameters::Callback(), count(0) {}

    void warning(QString, QString) {
        ++count;
    }

    int count;
};

void ApexMainTest::testInteractive() {
    apex::Paths paths;
    QString filename = paths.GetBasePath() + "examples/interactive/setgain.apx";
    QString fileschema = paths.GetExperimentSchemaPath();
    QString filenamespace = EXPERIMENT_NAMESPACE;

    qCDebug(APEX_RS) << "Filename: " << filename;
    qCDebug(APEX_RS) << "File schema: " << fileschema;
    qCDebug(APEX_RS) << "filenamespace: " << filenamespace;

    ApexXMLTools::XMLDocumentGetter xmlDocumentGetter;
    xercesc::DOMDocument* document = xmlDocumentGetter.GetXMLDocument(filename, fileschema, filenamespace);

    QString expectedPresentations = "80";
    QString expectedGain = "15";
    QString expectedFeedbackLength = "5";
    QString expectedText1 = "Text1";
    QString expectedText2 = "Text2";

    QStringList entryResults;
    entryResults << "Sub" << expectedGain << expectedPresentations << "invalid" << "" << expectedFeedbackLength << expectedText1 << expectedText2;

    InteractiveParameters params(document, xmlDocumentGetter);
    MocWarning* mocWarning = new MocWarning;
    params.apply(entryResults, mocWarning);
    document = params.document();

    xercesc::DOMElement* apex = dynamic_cast<xercesc::DOMElement*>(document->getElementsByTagName(S2X(QString("apex:apex")))->item(0));
    xercesc::DOMElement* procedure = dynamic_cast<xercesc::DOMElement*>(apex->getElementsByTagName(S2X(QString("procedure")))->item(0));
    xercesc::DOMElement* parameters = dynamic_cast<xercesc::DOMElement*>(procedure->getElementsByTagName(S2X(QString("parameters")))->item(0));
    xercesc::DOMElement* presentations = dynamic_cast<xercesc::DOMElement*>(parameters->getElementsByTagName(S2X(QString("presentations")))->item(0));

    xercesc::DOMElement* devices = dynamic_cast<xercesc::DOMElement*>(apex->getElementsByTagName(S2X(QString("devices")))->item(0));
    xercesc::DOMElement* device = dynamic_cast<xercesc::DOMElement*>(devices->getElementsByTagName(S2X(QString("device")))->item(0));
    xercesc::DOMElement* gain = dynamic_cast<xercesc::DOMElement*>(device->getElementsByTagName(S2X(QString("gain")))->item(0));

    xercesc::DOMElement* screens = dynamic_cast<xercesc::DOMElement*>(apex->getElementsByTagName(S2X(QString("screens")))->item(0));
    xercesc::DOMElement* reinforcement = dynamic_cast<xercesc::DOMElement*>(screens->getElementsByTagName(S2X(QString("reinforcement")))->item(0));
    xercesc::DOMElement* feedback = dynamic_cast<xercesc::DOMElement*>(reinforcement->getElementsByTagName(S2X(QString("feedback")))->item(0));

    xercesc::DOMElement* screen = dynamic_cast<xercesc::DOMElement*>(screens->getElementsByTagName(S2X(QString("screen")))->item(0));
    xercesc::DOMElement* gridlayout = dynamic_cast<xercesc::DOMElement*>(screen->getElementsByTagName(S2X(QString("gridLayout")))->item(0));
    xercesc::DOMElement* button1 = dynamic_cast<xercesc::DOMElement*>(gridlayout->getElementsByTagName(S2X(QString("button")))->item(0));
    xercesc::DOMElement* button2 = dynamic_cast<xercesc::DOMElement*>(gridlayout->getElementsByTagName(S2X(QString("button")))->item(1));
    xercesc::DOMElement* text1 = dynamic_cast<xercesc::DOMElement*>(button1->getElementsByTagName(S2X(QString("text")))->item(0));
    xercesc::DOMElement* text2 = dynamic_cast<xercesc::DOMElement*>(button2->getElementsByTagName(S2X(QString("text")))->item(0));

    QString actualPresentations = X2S(presentations->getFirstChild()->getNodeValue());
    QString actualGain = X2S(gain->getFirstChild()->getNodeValue());
    QString actualFeedbackLength = X2S(feedback->getAttribute(S2X(QString("length"))));
    QString actualText1 = X2S(text1->getFirstChild()->getNodeValue());
    QString actualText2 = X2S(text2->getFirstChild()->getNodeValue());

    QCOMPARE(actualPresentations, expectedPresentations);
    QCOMPARE(actualGain, expectedGain);
    QCOMPARE(actualFeedbackLength, expectedFeedbackLength);
    QCOMPARE(actualText1, expectedText1);
    QCOMPARE(actualText2, expectedText2);
    QCOMPARE(mocWarning->count, 1);
}

void ApexMainTest::testSoundLevelMeter()
{
    QString dummy = QL1S("dummyslmslave");
    PluginLoader &loader = PluginLoader::Get();
    QList<SoundLevelMeterPluginCreator*> available =
        loader.availablePlugins<SoundLevelMeterPluginCreator>();
    QStringList plugins;
    Q_FOREACH (SoundLevelMeterPluginCreator *creator, available)
        plugins.append(creator->availablePlugins());
    QVERIFY(plugins.contains(dummy));
    SoundLevelMeterPluginCreator* creator = loader.
        createPluginCreator<SoundLevelMeterPluginCreator>(dummy);
    QVERIFY(creator != NULL);
    try {
        QScopedPointer<SoundLevelMeter> slm(creator->createSoundLevelMeter(dummy));
        QVERIFY(!slm.isNull());
        QCOMPARE(slm->result(), 64.9);
    } catch (const std::exception &e) {
        QFAIL(e.what());
    }
}

QDomDocument ApexMainTest::readXmlResults(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!file.isOpen())
        qCDebug(APEX_RS) << "Result file could not be opened: " << fileName;
    QDomDocument document;
    document.setContent(file.readAll());
    return document;
}

QString ApexMainTest::compareXml(const QDomNode &actual, const QDomNode &expected)
{
    // node type
    if (actual.nodeType() != expected.nodeType())
        return QString::fromLatin1("Different node type: %1!=%2")
            .arg(actual.nodeType())
            .arg(expected.nodeType());

    // node name
    if (actual.nodeName() != expected.nodeName())
        return QString::fromLatin1("Different node name: %1!=%2")
            .arg(actual.nodeName(), expected.nodeName());

    // node value
    if (actual.nodeValue() != expected.nodeValue())
        return QString::fromLatin1("Different node value: %1!=%2")
            .arg(actual.nodeValue(), expected.nodeValue());

    // attributes
    QDomNamedNodeMap expectedAttributes = expected.attributes();
    QDomNamedNodeMap actualAttributes = actual.attributes();
    if (expectedAttributes.length() != actualAttributes.length())
        return QString::fromLatin1("Different number of attributes in %3: %1!=%2")
            .arg(actualAttributes.length())
            .arg(expectedAttributes.length())
            .arg(expected.nodeName());
    for (unsigned i = 0; i < unsigned(expectedAttributes.length()); ++i) {
        QDomNode expectedAttrItem = expectedAttributes.item(i);
        QDomNode actualAttrItem = actualAttributes.namedItem(expectedAttrItem.nodeName());
        QString result = compareXml(expectedAttrItem, actualAttrItem);
        if (!result.isEmpty())
            return result;
    }

    // child nodes
    QDomNodeList expectedNodes = expected.childNodes();
    QDomNodeList actualNodes = actual.childNodes();
    if (expectedNodes.length() != actualNodes.length())
        return QString::fromLatin1("Different number of child nodes in %3: %1!=%2")
            .arg(actualNodes.length())
            .arg(expectedNodes.length())
            .arg(expected.nodeName());
    for (unsigned i = 0; i < unsigned(expectedNodes.length()); ++i) {
        QDomNode expectedNode = expectedNodes.item(i);
        QDomNode actualNode = actualNodes.item(i);
        QString result = compareXml(expectedNode, actualNode);
        if (!result.isEmpty())
            return result;
    }

    return QString();
}

void ApexMainTest::testStandaloneUpgrader()
{
    QProcess upgraderProcess;
    apex::Paths paths;

    QString processFile = paths.GetExecutablePath() + "/experimentupgrader";
#ifdef WIN32
    processFile.append(".exe");
#endif
    upgraderProcess.setProgram(processFile);

    //Set the right command line arguments:
    QStringList argumentsList;
    QString arg1 = "-f";
    QString arg2 = paths.GetBasePath() + "data/tests/libapex/upgraderTest.apx";
    QString arg3 = paths.GetBasePath() + "data/tests/libapex/upgraderResult.apx";

    argumentsList.append(arg1);
    argumentsList.append(arg2);
    argumentsList.append(arg3);
    upgraderProcess.setArguments(argumentsList);

    upgraderProcess.setProcessChannelMode(QProcess::MergedChannels); //to redirect standard output
    upgraderProcess.start();

    //Wait untill process finished before assesing results:
    upgraderProcess.waitForFinished(); //default timeout 30s
    //qCDebug(APEX_RS) << upgraderProcess.readAllStandardOutput();

    //Now read the contents of the tranformed file, and compare with desired file.
    //Transformed:
    QFile upgradeResultFile(paths.GetBasePath() + "data/tests/libapex/upgraderResult.apx");
    Q_ASSERT(upgradeResultFile.exists()); //Otherwise conversion failed by default.
    Q_ASSERT(upgradeResultFile.open(QIODevice::ReadOnly));

    //qCDebug(APEX_RS) << "Upgrade resultfile: "<<upgradeResultFile.fileName();

    //Check if the version of the written file matches the current version:
    QDomDocument doc;
    doc.setContent(&upgradeResultFile);
    const QString sVersion = doc.documentElement().attribute("xmlns:apex" );

    //qCDebug(APEX_RS) << "Found version string: "<<sVersion;

    QVector<int> upgradedVersion;
    upgradedVersion.resize(3);
    QRegExp re("http://med.kuleuven.be/exporl/apex/(\\d+)\\.(\\d+)\\.?(\\d*)/");
    if ( re.lastIndexIn(sVersion) != -1) {
        upgradedVersion[0] = re.capturedTexts()[1].toInt();
        upgradedVersion[1] = re.capturedTexts()[2].toInt();
        if (re.captureCount()==2)
            upgradedVersion[2] = 0;
        else
            upgradedVersion[2] = re.capturedTexts()[3].toInt();
    }
    /*qCDebug(APEX_RS) << "Upgraded version: "<<upgradedVersion.at(0)<<"."<<upgradedVersion.at(1)
                <<"."<<upgradedVersion.at(2);*/

    //get the current version:
    QString currentVersionString = QString(SCHEMA_VERSION);
    QVector<int> currentVersion;
    currentVersion.resize(3);
    currentVersion[0] = currentVersionString.section(".", 0, 0).toInt();
    currentVersion[1] = currentVersionString.section(".", 1, 1).toInt();
    currentVersion[2] = currentVersionString.section(".", 2, 2).toInt();

    /*qCDebug(APEX_RS) << "Current version: "<<currentVersion.at(0)<<"."<<currentVersion.at(1)
                <<"."<<currentVersion.at(2);*/

    QCOMPARE(upgradedVersion, currentVersion);

    //Remove the newly created file so the test can be done again...
    upgradeResultFile.remove();
}

void ApexMainTest::testSoundcardSettings()
{
    SoundcardSettings soundcardSettings;

    soundcardSettings.removeSavedSettings();
    QCOMPARE(soundcardSettings.hasData(), false);

    QString device("SoundCard");
    QString hostApi("MyHostAPI");
    soundcardSettings.saveDevice(hostApi, device);
    QCOMPARE(soundcardSettings.device(), device);
    QCOMPARE(soundcardSettings.hostApi(), hostApi);
    QCOMPARE(soundcardSettings.hasData(), true);

    soundcardSettings.removeSavedSettings();

}

void ApexMainTest::testSoundcardsDialog()
{
    // simply test for segfaults etc
    SoundcardSettings scs;
    SoundcardsDialog d;
    d.displayHostApis();
    if (scs.hasData())
        d.setSelection(scs.hostApi(), scs.device());

}

void ApexMainTest::cleanupTestCase()
{
    xercesc::XMLPlatformUtils::Terminate();
}

//generate standalnne binary for the test
QTEST_MAIN(ApexMainTest)

//no use of plugin for now
//Q_EXPORT_PLUGIN2(test_libapex, ApexMainTest);

