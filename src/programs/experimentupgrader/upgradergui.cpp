#include "upgradergui.h"
#include "ui_upgradergui.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>

upgraderGui::upgraderGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::upgraderGui)
{
    ui->setupUi(this);
    this->show();

    //Start the core
    core = new upgraderCore(this);
    //Connect the signals:
    connect(core, SIGNAL(displayMessage(QString)), this, SLOT(showMessage(QString)));
}

upgraderGui::~upgraderGui()
{
    delete ui;
    delete core;
}

void upgraderGui::on_actionOpen_File_triggered()
{
    //select the input file
    getInputFileName();

    if(inputName.isEmpty()) {
        //No inputname has been set, so don't ask for an output name...
        return;
    }

    //Select the output file
    getOutputFileName();

    //check if the files have been properly selected:
    if(outputName.lastIndexOf(".") < 1) //because the suffix will be there
    {
        //No output file has been selected, so there is no conversion to be done
        inputName.clear();
        outputName.clear();

        //The user can now select new files...
        return;
    }

    //Files have been selected, now enable the right menu options.
    ui->actionOpen_File->setEnabled(false);
    ui->actionClose_file->setEnabled(true);
    ui->actionStart_conversion->setEnabled(true);

    core->setSourceFile(inputName);
    core->setDestinationFile(outputName, true);
    core->determineVersion();
}

void upgraderGui::on_actionStart_conversion_triggered()
{
    core->startUpgrade();
}

void upgraderGui::getInputFileName()
{
    bool setInput = true;
    while(setInput && inputName.isEmpty())
    {
        inputName = QFileDialog::getOpenFileName(parentWidget(), tr("Open experiment file"),
                   QDir::current().absolutePath(), tr("Apex Experiment Files (*.apx)"));
        if(inputName.isEmpty())
        {
            //No input filename has been selected, ask if user wants to set a new one.
            setInput = askNewNameDialog("No file name has been selected",
                                        "Do you want to select a new one?",
                                        QMessageBox::Question);
        }

    }
}

void upgraderGui::getOutputFileName()
{
    //If no output name has been set yet, ask for it.
    bool setOutput = true;
    while(setOutput && outputName.isEmpty()) {
        outputName = QFileDialog::getSaveFileName(parentWidget(), tr("Save converted file as"),
                                                  QDir::current().absolutePath(), tr("Apex Experiment File (*.apx)"));
        if(outputName.isEmpty())
        {
            //no output file name has been set, ask if user wants to set a new one.
            setOutput = askNewNameDialog("No file name has been selected",
                                         "Do you want to select a new one?"
                                         , QMessageBox::Question);
        }
    //If the file exits, consent for overwrite is automaticly asked for.

    }

    //to set the right file extension:
    QFileInfo outputInfo(outputName);
    if(outputInfo.completeSuffix() != "apx") { //could this enable double file extensions?
        outputName.append(".apx");
    }
}

bool upgraderGui::checkFileWriteConsent()
{
    //Check if the file allready exists, and if it can be overwritten
    return !QFile(outputName).exists() ||
        askNewNameDialog("The specified output file allready exists",
                "Overwrite?", QMessageBox::Question);
}

bool upgraderGui::askNewNameDialog(QString message, QString info, QMessageBox::Icon icon)
{
    QMessageBox askBox;
    askBox.setText(message);
    askBox.setInformativeText(info);
    askBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    askBox.setDefaultButton(QMessageBox::Yes);
    askBox.setIcon(icon);
    int answer = askBox.exec();

    switch (answer) {
    case QMessageBox::Yes:
        return true;
    case QMessageBox::No:
        return false;
    default: //Should not be reached
        Q_ASSERT(false);
        // silence the compiler
        return false;
    }
}

void upgraderGui::showMessage(QString message)
{
    ui->plainTextEdit->appendPlainText(message);
}

void upgraderGui::on_actionClose_file_triggered()
{
    //File is closed, so clear both input and output files.
    inputName.clear();
    outputName.clear();

    //Update the buttons
    ui->actionOpen_File->setEnabled(true);
    ui->actionClose_file->setEnabled(false);
    ui->actionStart_conversion->setEnabled(false);

    //Clear the message window
    ui->plainTextEdit->clear();
}

void upgraderGui::on_actionVersion_triggered()
{
    //Extract the version numbers from core:
    QVector<int> versionVector = core->getVersion();
    //Show the dialog that has to display the version box.
    QMessageBox versionBox;
    versionBox.setText("Apex experiment conversion tool.");
    QString versionString = tr("Apex schema version: %1.%2.%3").arg(versionVector.at(0))
            .arg(versionVector.at(1)).arg(versionVector.at(2));
    versionBox.setInformativeText(versionString);
    versionBox.addButton(QMessageBox::Ok);
    versionBox.exec();
}
