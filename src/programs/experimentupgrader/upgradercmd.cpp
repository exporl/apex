#include "upgradercmd.h"
#include <iostream>
#include <QTextStream>

upgraderCMD::upgraderCMD(const QString in, const QString out, QApplication *parent) : QObject(parent)
{
    //start the core:
    core = new upgraderCore(); //necessary to pass parent?

    //connect the signals:
    connect(core, SIGNAL(displayMessage(QString)), this, SLOT(newMessage(QString)));
    connect(core, SIGNAL(askConsent(QString)), this, SLOT(getConsent(QString)));
    connect(this, SIGNAL(finished()), parent,SLOT(quit()));
    //the other slots don't need to be connected.

    core->setSourceFile(in);
    core->setDestinationFile(out, false);
    core->determineVersion();
    core->startUpgrade(); //Will automatically quit if not needed
    emit finished();
    exit(0); //quit() is called from the wrong thread?
}

void upgraderCMD::newMessage(const QString &message)
{
    std::cout<<message.toStdString()<<std::endl;
}

void upgraderCMD::getConsent(QString name)
{
    QTextStream qIn(stdin);
    std::cout<<"File "<<name.toStdString()<<" allready exists, overwrite? (Y/N)"<<std::endl;
    QString answer;
    qIn >> answer;
    if(answer.toUpper() == QString("Y")) {
        //there is consent, so write the output file again:
        core->setConsent(true);
        core->saveOutputFile();
    }
    else {
        std::cout<<"Output file could not be written, aborting"<<std::endl;
        this->~upgraderCMD();
    }
}

upgraderCMD::~upgraderCMD()
{
    core->~upgraderCore(); //stop the core
}

