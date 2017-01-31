#ifndef _EXPORL_SRC_PROGRAMS_EXPERIMENTUPGRADER_UPGRADERCORE_H_
#define _EXPORL_SRC_PROGRAMS_EXPERIMENTUPGRADER_UPGRADERCORE_H_

#include <QObject>
#include <QDomDocument>
#include <QScopedPointer>

#include "apexmain/experimentparser.h"

/* Class upgraderCore
 * This class gets its input from the upgraderGui or the upgraderCmd class and does the actual upgrader.
 * It calls the experimentparser, and saves the result to the specified file.
 */

class upgraderCore : public QObject
{
    Q_OBJECT
public:
    explicit upgraderCore(QObject *parent = 0);
    void setSourceFile(const QString&);
    void setDestinationFile(const QString&, bool);
    void setConsent(bool);
    void saveOutputFile();
    QVector<int> getVersion();
    ~upgraderCore();

signals:
    void displayMessage(QString message);
    void askConsent(QString fileName);

public slots:
    void determineVersion();
    void startUpgrade();

private:
    //File names and status flags:
    QString sourceFile;
    QString destinationFile;
    bool conversionReady;
    QDomDocument doc;
    bool writeConsent;
    QString convertedText;

    //VersionNumbers
    static int majorVersion; //should this be a vector?
    static int minorVersion;
    static int subVersion;
    QVector<int> oldVersion; //Version of the input file
};

#endif // _EXPORL_SRC_PROGRAMS_EXPERIMENTUPGRADER_UPGRADERCORE_H_
