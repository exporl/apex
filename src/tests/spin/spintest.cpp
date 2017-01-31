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

#include "apexdata/calibration/calibrationdata.h"

#include "apexdata/device/wavdevicedata.h"

#include "apexdata/experimentdata.h"

#include "apexdata/filter/filterdata.h"
#include "apexdata/filter/filtersdata.h"

#include "apexdata/procedure/adaptiveproceduredata.h"
#include "apexdata/procedure/proceduredata.h"

#include "apexmain/calibration/calibrationdatabase.h"

#include "apexmain/experimentparser.h"

#include "apexspin/spincalibrator.h"
#include "apexspin/spinconfigfileparser.h"
#include "apexspin/spinexperimentconstants.h"
#include "apexspin/spinexperimentcreator.h"
#include "apexspin/spinusersettings.h"

#include "apextools/pathtools.h"
#include "apextools/random.h"

#include "spintest.h"

#include <QDebug>

using namespace spin;
using namespace spin::data;
using namespace spin::constants;
using namespace apex;
using namespace apex::data;

QString createTempDirectory (const QString prefix=QString())
{
#ifdef Q_OS_UNIX
    QByteArray name = QFile::encodeName
        (QDir::temp().filePath (QLatin1String ("spintest-") +
                                (!prefix.isEmpty() ? prefix + QLatin1Char ('-') :
                                 QString()) + QLatin1String ("XXXXXX")));
    // FIXME: error message not translated
    if (!mkdtemp (name.data()))
        qFatal ("Unable to create temporary directory");
    return QFile::decodeName (name);
#else
    Random random;
    for (unsigned i = 0; i < 65536; ++i) {
        QString name = QDir::temp().filePath (QLatin1String ("spintest-") +
                (!prefix.isEmpty() ? prefix + QLatin1Char ('-') :
                 QString()) + QString::number (random.nextUInt(), 36));
        if (QDir (name).mkdir (name))
            return name;
    }
    // FIXME: error message not translated
    qFatal ("Unable to create temporary directory");
#endif
}

void SpinTest::initTestCase()
{
    xercesc::XMLPlatformUtils::Initialize();         // setup XML environment
}

void SpinTest::cleanupTestCase()
{
    xercesc::XMLPlatformUtils::Terminate();
}

void SpinTest::testSnrDefined()
{
    {       // headphones
    SpinUserSettings s;
    s.setSpeakerType(HEADPHONE);
    SpeakerLevels left;
    left.speech=50;
    left.hasSpeech=true;
    left.hasNoise=false;
    s.addLevels(left, Headphone::LEFT);
    QCOMPARE(s.snrDefined(), false);

    SpeakerLevels right;
    right.speech=50;
    right.hasSpeech=true;
    right.noise=50;
    right.hasNoise=true;
    s.addLevels(right, Headphone::RIGHT);
    QCOMPARE(s.snrDefined(), true);
    QCOMPARE(s.snr(), 0.0);

    }

    {       // free field
    SpinUserSettings s;
    s.setSpeakerType(FREE_FIELD);

    SpeakerLevels l;
    l.hasSpeech=true;
    l.speech=50;
    l.hasNoise=false;
    s.addLevels(l,-90);
    s.addLevels(l,0);

    l.hasNoise=true;
    l.noise=60;
    s.addLevels(l,90);
    QCOMPARE(s.snrDefined(), true);
    QCOMPARE(s.snr(), -5.2287874528);

    l.noise=50;
    s.addLevels(l,270);
    QCOMPARE(s.snrDefined(), false);

    }

}

double totalNoiseGain(ExperimentData& d)
{
    const FiltersData* filters = d.filtersData();
    double result=0;

    for (FiltersData::const_iterator it=filters->begin();
         it!=filters->end(); ++it)
    {
        QString id(it.key());
        if (id.startsWith(FILTER_TYPE_NOISE)) {
            qCDebug(APEX_RS) << "noise filter id" << id;
            qCDebug(APEX_RS) << "    gain:    " << it.value()->valueByType("gain").toDouble();
            qCDebug(APEX_RS) << "    basegain:" << it.value()->valueByType("basegain").toDouble();
            result+=it.value()->valueByType("gain").toDouble();
            result+=it.value()->valueByType("basegain").toDouble();
        } //else  qCDebug(APEX_RS, "Not a noise filter: %s", qPrintable(id));
    }
    return result;
}


 double totalSpeechGain(ExperimentData& d)
 {
     const FiltersData* filters = d.filtersData();
     double result = 0;

     for (FiltersData::const_iterator it=filters->begin(); it!=filters->end();
             ++it) {
         QString id(it.key());
         if (id.startsWith(FILTER_TYPE_SPEECH) /*&& id != FILTER_TYPE_SPEECH*/) {
             qCDebug(APEX_RS) << "speech filter id" << id;
             qCDebug(APEX_RS) << "    gain:    " << it.value()->valueByType("gain").toDouble();
             qCDebug(APEX_RS) << "    basegain:" << it.value()->valueByType("basegain").toDouble();
             result += it.value()->valueByType("gain").toDouble();
             result += it.value()->valueByType("basegain").toDouble();
         }
     }
     return result;
 }


 void SpinTest::testTotalGain()
 {
     QDir d( qApp->applicationDirPath() );
     d.cdUp();
     d.cdUp();
     QString basepath(d.path());
     QString configFile( apex::PathTools::GetConfigFilePath(basepath, "spin.xml"));
     QString schemaFile(basepath + "/data/schemas/spin.xsd");

     SpinUserSettings s;
     SpinConfig configuration =
         spin::parser::SpinConfigFileParser::parse(configFile, schemaFile);


     // print available speech materials:
     qCDebug(APEX_RS) << "Available speech materials: "
         <<configuration.speechmaterials().keys();
     QString targetSpeechmaterial( "LIST" );
     qCDebug(APEX_RS) << "Using speech material " << targetSpeechmaterial;
     QString targetCategory("vrouw");
//      if ( configuration.speechmaterials()[targetSpeechmaterial].hasCategories()) {
//          qCDebug(APEX_RS) << "Available Categories: "
//              <<configuration.speechmaterials()[targetSpeechmaterial].categories();
//          targetCategory=
//              configuration.speechmaterials()[targetSpeechmaterial].
//              categories()[0];
//      }

     /*qCDebug(APEX_RS) << "Available lists: ";
     Q_FOREACH(List list,
             configuration.speechmaterials()[targetSpeechmaterial]
             .lists(targetCategory))
         qCDebug(APEX_RS) << list.id;*/
     QString targetList(configuration.speechmaterials()[targetSpeechmaterial]
             .lists(targetCategory)[0].id);


     SpeakerLevels levels;
     levels.hasNoise=true;
     levels.hasSpeech=true;

     qCDebug(APEX_RS) << "Available noise materials:" << configuration.noises().keys();

     s.setSubjectName("subjectname");
     s.setSpeechmaterial(targetSpeechmaterial);
     s.setSpeechcategory(targetCategory);
     s.setNoisematerial("LISTvrouw_ltass");
     s.setList(targetList);
     s.setSpeakerType(HEADPHONE);
     levels.noise=50;
     levels.speech=60;
     s.addLevels( levels,Headphone::LEFT);
     s.setProcedureType(CONSTANT);
     // The following are only relevant for adaptive procedures
     //s.setAdaptingMaterial(SPEECH);
     //s.addStepsize(2);
     //s.setRepeatFirst(true);
     s.setNoiseStopsBetweenTrials(false);
     s.setPersonBeforeScreen(EXPERIMENTER);
     s.setTimeBeforeFirstStimulus(0);
     // ...

     QCOMPARE(configuration.subjectScreen().isEmpty(), false);

     // get temporary filename
     QString dir( createTempDirectory());
     QString expfile( dir + "/totalgain.apx");

     SpinExperimentCreator creator(configuration, s);
     creator.createExperimentFile(expfile);

     // parse experiment file
     apex::ExperimentParser expParser(expfile);
     QScopedPointer<apex::data::ExperimentData> experiment(
             expParser.parse(false) );

     // Calculate total gain of speech path
     // = amplifier(gain+basegain) +
     double speechgain=totalSpeechGain(*experiment);

     // Calculate total gain of noise path
     // = dataloop(gain+basegain)
     double noisegain=totalNoiseGain(*experiment);

     qCDebug(APEX_RS, "speechgain=%f, noisegain=%f", speechgain, noisegain);
     qCDebug(APEX_RS, "levels.speech=%f, levels.noise=%f", levels.speech, levels.noise);

     QCOMPARE(speechgain-noisegain, levels.speech-levels.noise);

     const CalibrationData* calibrationData( experiment->calibrationData() );
     const CalibrationParameterData paramdata =
         calibrationData->parameters()[creator.deviceChannelParameterName(0)];

     // calibration is done at the level of the noise
     QCOMPARE(paramdata.finalTargetAmplitude()-noisegain+speechgain, levels.speech);
     QCOMPARE(paramdata.finalTargetAmplitude(), levels.noise);


     // remove temporary file
     QDir theDir(dir);
     theDir.remove(expfile);
     QDir::root().rmdir(dir);

 }

void SpinTest::testCalibration()
{
#if QT_VERSION < 0x050000
    QSKIP("Skipping calibration test - calibration not implemented", SkipAll);
#else
    QSKIP("Skipping calibration test - calibration not implemented");
#endif

    try
    {
    apex::CalibrationDatabase db;

    //remove everything from the calibration database
    Q_FOREACH (QString setup, db.hardwareSetups())
    {
        Q_FOREACH (QString profile, db.calibrationProfiles(setup))
        {
            Q_FOREACH (QString param, db.parameterNames(setup, profile))
                db.remove(setup, profile, param);
        }
    }

    QString hwSetup1 = "default";
    QString hwSetup2 = "notthedefault";
    QString spinProfile1 = "spin_test1";
    QString spinProfile2 = "spin_test2";
    QString profile1 = "non-spin_test1";

    double target1 = 70.0;
    double target2 = 50.0;
    double output1 = 75.0;
    double output2 = 45.0;

    //add some spin calibrations to the database
    db.calibrate(hwSetup1, spinProfile1, "gain0", target1, output1);
    db.calibrate(hwSetup1, spinProfile1, "gain1", target2, output2);
    db.calibrate(hwSetup2, spinProfile2, "gain0", target2, output2);

    //and a non-spin calibration
    db.calibrate(hwSetup1, profile1, "gain0", target1, output1);

    //get the xml and schema files for spin
    QDir d(qApp->applicationDirPath());
    d.cdUp();
    d.cdUp();
    QString basepath(d.path());
    QString configFile(apex::PathTools::GetConfigFilePath(basepath, "spin.xml"));
    QString schemaFile(basepath + "/data/schemas/spin.xsd");

    SpinConfig config = spin::parser::SpinConfigFileParser::parse(configFile,
                                                                  schemaFile);
    SpinUserSettings settings;
    settings.setSpeechmaterial("FrMatrix");
    settings.setNoisematerial("lint_ltass");
    settings.setList("1");
    settings.setSpeakerType(HEADPHONE);
    settings.setProcedureType(ADAPTIVE);
    SpeakerLevels levels;
    levels.hasNoise = true;
    levels.hasSpeech = true;
    settings.addLevels(levels, Headphone::LEFT); //left is gain0

    //let spin calibrate gain0
    //QWidget parentWidget;
    SpinCalibrator calibrator(config, settings, 0);

    bool changesMade;
    calibrator.calibrate(&changesMade);
    }
    catch (/*ApexException*/std::exception& e)
    {
        qFatal("std::exception: %s", e.what());
    }
    catch (int e)
    {
        qFatal("Stop throwing ints: %i", e);
    }
    catch (const char* e)
    {
        qFatal("Please, no chars either: %s", e);
    }
    catch (...)
    {
        qFatal("something else thrown???");
    }
}

void SpinTest::testAdaptiveWithoutNoise()
{
    QDir d(qApp->applicationDirPath());
    d.cdUp(); d.cdUp();
    QString basepath(d.path());
    QString configFile(apex::PathTools::GetConfigFilePath(basepath, "spin.xml"));
    QString schemaFile(basepath + "/data/schemas/spin.xsd");

    SpinUserSettings s;
    SpinConfig configuration =
        spin::parser::SpinConfigFileParser::parse(configFile, schemaFile);
    QString targetSpeechmaterial("LIST");
    QString targetCategory("vrouw");
    QString targetList(configuration.speechmaterials()[targetSpeechmaterial]
            .lists(targetCategory)[0].id);

    SpeakerLevels levels;
    levels.hasNoise = false;
    levels.hasSpeech = true;

    s.setSubjectName("subjectname");
    s.setSpeechmaterial(targetSpeechmaterial);
    s.setSpeechcategory(targetCategory);
    s.setNoisematerial("LISTvrouw_ltass");
    s.setList(targetList);
    s.setSpeakerType(HEADPHONE);
    levels.speech = 60;
    s.addLevels(levels, Headphone::LEFT);
    s.setProcedureType(ADAPTIVE);
    s.setAdaptingMaterial(SPEECH);
    s.addStepsize(2);
    s.setRepeatFirst(true);
    s.setNoiseStopsBetweenTrials(false);
    s.setPersonBeforeScreen(EXPERIMENTER);
    s.setTimeBeforeFirstStimulus(0);
    // ...

    QCOMPARE(configuration.subjectScreen().isEmpty(), false);
    QCOMPARE(s.speechLevel(), 60.0);

    QString dir(createTempDirectory());
    QString expfile(dir + "/totalgain.apx");

    SpinExperimentCreator creator(configuration, s);
    creator.createExperimentFile(expfile);

    apex::ExperimentParser expParser(expfile);
    QScopedPointer<apex::data::ExperimentData> experiment(expParser.parse(false));

    QCOMPARE(experiment->filtersData()->value("amplifier_channel0")->valueByType("gain").toDouble(), 0.0 );
    QCOMPARE(experiment->filtersData()->value("amplifier_channel0")->valueByType("basegain").toDouble(), 0.0 );
    QCOMPARE(experiment->filtersData()->value("amplifier")->valueByType("basegain").toDouble(), -5.0 );

    // Speech is attenuated by 5 dB per default (for LIST by woman)
    QCOMPARE(totalSpeechGain(*experiment), -5.0);
    QCOMPARE(static_cast<const AdaptiveProcedureData*>
            (experiment->procedureData())->startValue(),
            60.0); // level
    QCOMPARE(static_cast<const AdaptiveProcedureData*>
            (experiment->procedureData())->largerIsEasier(),
            true);

    // remove temporary file
    QDir(dir).remove(expfile);
    QDir::root().rmdir(dir);
}

void SpinTest::testAdaptiveWithNoise()
{
    QDir d(qApp->applicationDirPath());
    d.cdUp(); d.cdUp();
    QString basepath(d.path());
    QString configFile(apex::PathTools::GetConfigFilePath(basepath, "spin.xml"));
    QString schemaFile(basepath + "/data/schemas/spin.xsd");

    SpinUserSettings s;
    SpinConfig configuration =
        spin::parser::SpinConfigFileParser::parse(configFile, schemaFile);
    QString targetSpeechmaterial("LIST");
    QString targetCategory("vrouw");
    QString targetList(configuration.speechmaterials()[targetSpeechmaterial]
            .lists(targetCategory)[0].id);

    SpeakerLevels levels;
    levels.hasNoise = true;
    levels.hasSpeech = true;

    s.setSubjectName("subjectname");
    s.setSpeechmaterial(targetSpeechmaterial);
    s.setSpeechcategory(targetCategory);
    s.setNoisematerial("LISTvrouw_ltass");
    s.setList(targetList);
    s.setSpeakerType(HEADPHONE);
    levels.noise = 50;
    levels.speech = 60;
    s.addLevels(levels, Headphone::LEFT);
    s.setProcedureType(ADAPTIVE);
    s.setAdaptingMaterial(SPEECH);
    s.addStepsize(2);
    s.setRepeatFirst(true);
    s.setNoiseStopsBetweenTrials(false);
    s.setPersonBeforeScreen(EXPERIMENTER);
    s.setTimeBeforeFirstStimulus(0);
    // ...

    QCOMPARE(configuration.subjectScreen().isEmpty(), false);
    QCOMPARE(s.speechLevel(), 60.0);

    QString dir(createTempDirectory());
    QString expfile(dir + "/totalgain.apx");

    SpinExperimentCreator creator(configuration, s);
    creator.createExperimentFile(expfile);

    apex::ExperimentParser expParser(expfile);
    QScopedPointer<apex::data::ExperimentData> experiment(expParser.parse(false));

    QCOMPARE(experiment->filtersData()->value("amplifier_channel0")->valueByType("gain").toDouble(), 0.0 );
    QCOMPARE(experiment->filtersData()->value("amplifier_channel0")->valueByType("basegain").toDouble(), 0.0 );
    QCOMPARE(experiment->filtersData()->value("amplifier")->valueByType("basegain").toDouble(), -5.0 );

    // Speech is attenuated by 5 dB per default
    QCOMPARE(totalSpeechGain(*experiment), -5.0);
    QCOMPARE(static_cast<const AdaptiveProcedureData*>
            (experiment->procedureData())->startValue(),
            10.0); // snr
    QCOMPARE(static_cast<const AdaptiveProcedureData*>
            (experiment->procedureData())->largerIsEasier(),
            true);

    // remove temporary file
    QDir(dir).remove(expfile);
    QDir::root().rmdir(dir);
}

void SpinTest::testAdaptiveWithNoiseAdaption()
{
    QDir d(qApp->applicationDirPath());
    d.cdUp(); d.cdUp();
    QString basepath(d.path());
    QString configFile(apex::PathTools::GetConfigFilePath(basepath, "spin.xml"));
    QString schemaFile(basepath + "/data/schemas/spin.xsd");

    SpinUserSettings s;
    SpinConfig configuration =
        spin::parser::SpinConfigFileParser::parse(configFile, schemaFile);
    QString targetSpeechmaterial("LIST");
    QString targetCategory("vrouw");
    QString targetList(configuration.speechmaterials()[targetSpeechmaterial]
            .lists(targetCategory)[0].id);

    SpeakerLevels levels;
    levels.hasNoise = true;
    levels.hasSpeech = true;

    s.setSubjectName("subjectname");
    s.setSpeechmaterial(targetSpeechmaterial);
    s.setSpeechcategory(targetCategory);
    s.setNoisematerial("LISTvrouw_ltass");
    s.setList(targetList);
    s.setSpeakerType(HEADPHONE);
    levels.noise = 60;
    levels.speech = 50;
    s.addLevels(levels, Headphone::LEFT);
    s.setProcedureType(ADAPTIVE);
    s.setAdaptingMaterial(NOISE);
    s.addStepsize(2);
    s.setRepeatFirst(true);
    s.setNoiseStopsBetweenTrials(false);
    s.setPersonBeforeScreen(EXPERIMENTER);
    s.setTimeBeforeFirstStimulus(0);
    // ...

    QCOMPARE(configuration.subjectScreen().isEmpty(), false);

    QString dir(createTempDirectory());
    QString expfile(dir + "/totalgain.apx");

    SpinExperimentCreator creator(configuration, s);
    creator.createExperimentFile(expfile);

    apex::ExperimentParser expParser(expfile);
    QScopedPointer<apex::data::ExperimentData> experiment(expParser.parse(false));

    // Speech and noise are attenuated by 5 dB per default, snr start value will
    // be set by procedure
    QCOMPARE(totalSpeechGain(*experiment), -5.0);
    QCOMPARE(totalNoiseGain(*experiment), -5.0);
    QCOMPARE(static_cast<const AdaptiveProcedureData*>
            (experiment->procedureData())->startValue(),
            -10.0);
    QCOMPARE(static_cast<const AdaptiveProcedureData*>
            (experiment->procedureData())->largerIsEasier(),
            true);

    // remove temporary file
    QDir(dir).remove(expfile);
    QDir::root().rmdir(dir);
}

void SpinTest::testAudioDriver_data()
{
    QTest::addColumn<unsigned>("setting");
    QTest::addColumn<QString>("driver");
    QTest::addColumn<unsigned>("padzero");

    QTest::newRow("multiface") << unsigned(RmeMultiface)
        << "asio" << 0u;
    QTest::newRow("fireface") << unsigned(RmeFirefaceUc)
        << "asio" << 2u;
    QTest::newRow("lynxone") << unsigned(LynxOne)
        << "portaudio" << 1u;
#ifdef Q_OS_WIN32
    QTest::newRow("defaultsoundcard") << unsigned(DefaultSoundcard)
        << "asio" << 1u;
#else
    QTest::newRow("defaultsoundcard") << unsigned(DefaultSoundcard)
        << "portaudio" << 1u;
#endif
}

void SpinTest::testAudioDriver()
{
    QFETCH(unsigned, setting);
    QFETCH(QString, driver);
    QFETCH(unsigned, padzero);

    QDir d(qApp->applicationDirPath());
    d.cdUp(); d.cdUp();
    QString basepath(d.path());
    QString configFile(apex::PathTools::GetConfigFilePath(basepath, "spin.xml"));
    QString schemaFile(basepath + "/data/schemas/spin.xsd");

    SpinUserSettings s;
    SpinConfig configuration =
        spin::parser::SpinConfigFileParser::parse(configFile, schemaFile);
    QString targetSpeechmaterial("LIST");
    QString targetCategory("vrouw");
    QString targetList(configuration.speechmaterials()[targetSpeechmaterial]
            .lists(targetCategory)[0].id);

    SpeakerLevels levels;
    levels.hasNoise = true;
    levels.hasSpeech = true;

    s.setSubjectName("subjectname");
    s.setSpeechmaterial(targetSpeechmaterial);
    s.setSpeechcategory(targetCategory);
    s.setNoisematerial("LISTvrouw_ltass");
    s.setList(targetList);
    s.setSpeakerType(FREE_FIELD);
    levels.noise = 60;
    levels.speech = 50;
    s.addLevels(levels, 0);
    levels.hasNoise = false;
    s.addLevels(levels, 90);
    s.addLevels(levels, 180);
    s.addLevels(levels, 270);
    s.setProcedureType(ADAPTIVE);
    s.setAdaptingMaterial(NOISE);
    s.addStepsize(2);
    s.setRepeatFirst(true);
    s.setNoiseStopsBetweenTrials(false);
    s.setPersonBeforeScreen(EXPERIMENTER);
    s.setTimeBeforeFirstStimulus(0);
    s.setSoundCard(SoundCard(setting));
    // ...

    QString dir(createTempDirectory());
    QString expfile(dir + "/audiodriver.apx");

    SpinExperimentCreator creator(configuration, s);
    creator.createExperimentFile(expfile);

    apex::ExperimentParser expParser(expfile);
    QScopedPointer<apex::data::ExperimentData> experiment(expParser.parse(false));

    const QString masterDevice = experiment->devicesData()->masterDevice();
    const WavDeviceData * deviceData =
        dynamic_cast<WavDeviceData*>(experiment->devicesData()->deviceData(masterDevice));

    QCOMPARE(deviceData->driverString(), driver);
    QCOMPARE(deviceData->valueByType("padzero").toUInt(), padzero);

    // remove temporary file
    QDir(dir).remove(expfile);
    QDir::root().rmdir(dir);
}

//generate the stand alone test binary
QTEST_MAIN(SpinTest)
