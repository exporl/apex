#include "upgradercore.h"
#include "apexmain/experimentparser.h"

#include <QFile>
#include <QTextStream>

//Set the version numbers:
QString currentVersionString = QString(SCHEMA_VERSION);
int upgraderCore::majorVersion = currentVersionString.section(".", 0, 0).toInt();
int upgraderCore::minorVersion = currentVersionString.section(".", 1, 1).toInt();
int upgraderCore::subVersion = currentVersionString.section(".", 2, 2).toInt();

upgraderCore::upgraderCore(QObject *parent) : QObject(parent)
{
    //set the status flags.
    conversionReady = false;
}

void upgraderCore::setSourceFile(const QString &source)
{
    sourceFile = source; //is this even needed?
    //Try to open and read the contents of the inputfile:
    QFile file(sourceFile);
    bool fileOpen = true;

    if (!file.open(QIODevice::ReadOnly))
        fileOpen = false;
    if (!doc.setContent(&file)) {
        fileOpen = false;}

    file.close();
    if(!fileOpen) {
        //Something went wrong while opening the file, aborting
        emit displayMessage("Could not open input file, aborting");
    }
    else
        emit displayMessage("Original file has been read.");
}

void upgraderCore::setDestinationFile(const QString &dest, bool consent)
{
    destinationFile = dest;
    writeConsent = consent;
}

void upgraderCore::setConsent(bool newConsent)
{
    writeConsent = newConsent;
}

void upgraderCore::determineVersion()
{
    //Check if input file has been set:
    if(doc.isNull()){
        emit displayMessage("No source file has been set");
        return;
    }

    const QString sVersion = doc.documentElement().attribute("xmlns:apex" );

    QRegExp re("http://med.kuleuven.be/exporl/apex/(\\d+)\\.(\\d+)\\.?(\\d*)/");
    if ( re.lastIndexIn(sVersion) != -1) {
        oldVersion.resize(3);
        oldVersion[0] = re.capturedTexts()[1].toInt();
        oldVersion[1] = re.capturedTexts()[2].toInt();
        if (re.captureCount()==2)
            oldVersion[2] = 0;
        else
            oldVersion[2] = re.capturedTexts()[3].toInt();
    }
    else {
        emit displayMessage("version number could not be read, aborting");
        return;
    }

    QString versionMessage1 = tr("Original version: %1.%2.%3").arg(oldVersion[0]).arg(oldVersion[1]).arg(oldVersion[2]);
    QString versionMessage2 = tr("Current version: %1.%2.%3").arg(majorVersion).arg(minorVersion).arg(subVersion);

    emit displayMessage(versionMessage1);
    emit displayMessage(versionMessage2);

    //Check if conversion needs to be done:
    if(majorVersion > oldVersion[0])
        conversionReady = true; //can this conversion be done?
    else if(majorVersion == oldVersion[0])
    {
        if(minorVersion > oldVersion[1])
            conversionReady = true;
        else if(minorVersion == oldVersion[1])
        {
            if(subVersion > oldVersion[2])
                conversionReady = true;
        }
    }

    emit displayMessage("File ready to be converted.");
}

void upgraderCore::startUpgrade()
{
    if(!conversionReady)
        return;
    if(destinationFile.isEmpty()) {
        emit displayMessage("No output file has been set, aborting");
        return;
    }
    QScopedPointer<apex::ExperimentParser> parser (new apex::ExperimentParser(sourceFile));
    try
    {
        parser->parse(false);
    }
    catch (const std::exception& e) {
        QString message = QString("Could not convert experiment file: %1").arg(e.what());
        emit displayMessage(message);
    }
    catch (...) {
        QString message = QString("Unknown exception while converting %1").arg(sourceFile);
        emit displayMessage(message);
    }

    //conversion complete
    emit displayMessage("Conversion complete.");
    convertedText = parser->getUpgradedConfigFile();
    saveOutputFile();
}

void upgraderCore::saveOutputFile()
{
    QFile outputFile(destinationFile);
    //Check if the file allready exists and can be overwritten:
    if(outputFile.exists() && !writeConsent) {
        emit askConsent(destinationFile);
        return; //When consent is there, this function can be called again.
    }

    if(!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        emit displayMessage("Could not write to output file, aborting");
        return;
    }

    //Now get content from xml document to file:
    //QString contentString = parser->getUpgradedConfigFile();
    //if(contentString.isEmpty()) {
    if(convertedText.isEmpty()) {
        emit displayMessage("Unknown error during conversion, no output written");
        return;
    }
    QTextStream contentStream(&outputFile);
    //contentStream << contentString;
    contentStream << convertedText;
    outputFile.close();

    emit displayMessage("New experiment file saved");

    //do something here to exit the program?
}

QVector<int> upgraderCore::getVersion()
{
    QVector<int> vec(3);
    vec[0] = majorVersion;
    vec[1] = minorVersion;
    vec[2] = subVersion;
    return vec;
}

upgraderCore::~upgraderCore()
{

}

