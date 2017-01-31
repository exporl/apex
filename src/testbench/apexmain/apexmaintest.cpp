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

#include "apexmaintest.h"

//libapex includes
#include "stimulus/l34/qicparser.h"
#include "stimulus/l34/aseqparser.h"
#include "stimulus/l34/l34datablock.h"
#include "datablock/datablockdata.h"
#include "map/apexmap.h"

#include "services/paths.h"

#include "apextools.h"

using namespace apex;
using namespace apex::stimulus;
using namespace apex::data;

QString ApexMainTest::name() const
{
    return "libapex";
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
            qDebug() << e.what();
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
                qDebug() << e.what();
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
        QString fromfile(t.readAll());
        file.close();

        // Compare reference data
        QCOMPARE(fromfile, xmldata.join("\n"));
        
    } catch (ApexStringException &e) {
        qDebug() << e.what();
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
        QFile wfile(filename+"-out.xml");
        wfile.open(QIODevice::WriteOnly);
        QTextStream out(&wfile);
        out << xmldata.join("\n");
        wfile.close();

        QFile file(testDir.path() + "/" + filename + ".xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        QTextStream t(&file);
        QString fromfile(t.readAll());
        file.close();
        QCOMPARE(fromfile, xmldata.join("\n"));
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
        QFile file(testDir.path() + "/" +filename+".xml");
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        out << xmldata.join("\n");
        file.close();

        //qDebug(qPrintable(filename+".xml" + " to be read by checkmapping2.m"));

        /*QFile file(testDir.path() + "/" + filename + ".xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        QTextStream t(&file);
        QString fromfile(t.readAll());
        file.close();
        QCOMPARE(fromfile, xmldata.join("\n"));*/
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
    
    // parse aseq files and write XML results
        QStringList files;
        files << "invalid1.aseq";

        apex::Paths paths;
        QDir testDir(paths.GetBasePath());
        testDir.cd("data/tests/libapex/");
        
        for (int i=0; i<files.size(); ++i) {
            qDebug() << "Testing file " << files[i];

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
    basemap.setMode(ChannelMap::modeToStimulationModeType("MP1+2"));
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


Q_EXPORT_PLUGIN2(test_libapex, ApexMainTest);














