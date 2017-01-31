#ifndef _EXPORL_SRC_PROGRAMS_EXPERIMENTUPGRADER_UPGRADERGUI_H_
#define _EXPORL_SRC_PROGRAMS_EXPERIMENTUPGRADER_UPGRADERGUI_H_

#include <QMainWindow>
#include <QMessageBox>

#include "upgradercore.h"

namespace Ui {
class upgraderGui;
}

class upgraderGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit upgraderGui(QWidget *parent = 0);
    ~upgraderGui();

public slots:
    /*void openFile();
    void closeFile();
    void quit();
    void startConversion();
    void displayVersion();*/

private slots:
    void on_actionOpen_File_triggered();

    void on_actionStart_conversion_triggered();
    void showMessage(QString);

    void on_actionClose_file_triggered();

    void on_actionVersion_triggered();

private:
    Ui::upgraderGui *ui;

    //File to be converted:
    QString inputName;
    QString outputName;

    upgraderCore *core;

    //To select the filenames:
    void getInputFileName();
    void getOutputFileName();
    bool checkFileWriteConsent();
    bool askNewNameDialog(QString, QString, QMessageBox::Icon); //Shows a file error with the defined message
                                        //Returns true if the user wants to select a new file
};

#endif // _EXPORL_SRC_PROGRAMS_EXPERIMENTUPGRADER_UPGRADERGUI_H_
