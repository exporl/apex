#ifndef _EXPORL_SRC_PROGRAMS_EXPERIMENTUPGRADER_UPGRADERCMD_H_
#define _EXPORL_SRC_PROGRAMS_EXPERIMENTUPGRADER_UPGRADERCMD_H_

#include "upgradercore.h"

#include <QObject>
#include <QApplication>

/* UpgraderCMD.
 * This class gets called when the experimentUpgrader is invoked with command line parameters,
 * and without the -g or --gui parameter.
 * It performes the upgrade of the experiment file without a graphical interface. All communication
 * with the user is through the console.
 *
 * Commandline options:
 * -f or --file [SOURCE] [DESTINATION] specifies which file [SOURCE] has to be converted
 * and in which name [DESTINATION] it has to be saved.
 * -g or --gui invokes the gui, then this class is not used.
 */

class upgraderCMD : public QObject
{
    Q_OBJECT
public:
    /* Constructor expects:
     * QString source - Source file name
     * QString destination - destination file name
     * QApplication parent
     */
    explicit upgraderCMD(const QString, const QString, QApplication *parent = 0);
    ~upgraderCMD();

signals:
    void finished();

public slots:
    void newMessage(const QString&);
    void getConsent(QString);

private:
    upgraderCore *core;
};

#endif // _EXPORL_SRC_PROGRAMS_EXPERIMENTUPGRADER_UPGRADERCMD_H_
