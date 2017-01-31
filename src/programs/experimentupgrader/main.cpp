#include "upgradercmd.h"
#include "upgradergui.h"

#include <QApplication>
#include <QString>
#include <QMap>
#include <QString>
#include <iostream>

using namespace std;

//Variables to pass to constructor:
bool fileSet = false;
QString fileIn = "";
QString fileOut = "";
bool nogui = false;


void showCommands(char* name)
{
    cout<<"Usage: " << name <<" Option(s)" <<endl
       <<"Options:"<<endl
      <<"\t-f, --file [SOURCE] [DESTINATION]\t Specify source"
        " and destination file."<<endl
     <<"\t-h, --help\tDisplay this help"<<endl
    <<"\t-g, --gui\tDisplay the interface"<<endl;
        exit(1); //Only exit if user tried to give arguments...
}

void setFiles(QString in, QString out)
{
    fileIn = in;
    fileOut = out;
    fileSet = true;
}

int main(int argc, char *argv[])
{
    //convert the command line arguments to QString:
    QStringList args;

    //QString* args;
    //args = new QString[argc];
    for(int i=0; i<argc; i++)
        args.append(QString(argv[i]));

    //Parsing the command line options:
    //Only parse if there are command line options, otherwise just show the interface.
    if(argc > 1)
    {
        nogui = true; //There are command line options, so no gui.

        //Map the char* to an integer for the switch statement:
        QMap<QString, int> optionMap;
        optionMap[QString("-f")] = 1;
        optionMap[QString("--file")] = 1;
        optionMap[QString("-g")] = 2;
        optionMap[QString("--gui")] = 2;

        for(int i=1; i<argc; i++) { //Loop over the options, skip the first one.
            switch (optionMap.value(QString(argv[i]), -1)) {
            case 1:
                if(argc >= i+3) {
                    //There are files specified:
                    setFiles(args[i+1], args[i+2]);
                    i+=2;
                }
                else {
                    //The file option is there, but there are no files specified:
                    showCommands(argv[0]);
                }
                break;
            case 2:
                nogui = false;
                break;
            default:
                showCommands(argv[0]); //There were wrong commands.
            }
        }
    }

    //If no gui, use the cmd version:
    if(nogui){
        QApplication a(argc, argv); //TODO: Should be QCoreApplication
        //Not possible yet because of a QWidget further downstream.
        upgraderCMD *upgrader = new upgraderCMD(fileIn, fileOut, &a);
        QObject::connect(upgrader, SIGNAL(finished()), &a, SLOT(quit()));
        return a.exec();
    }
    else {
        //Start the upgrader with the gui
        QApplication a(argc, argv);
        //upgraderGui *upgrader = new upgraderGui(&a);
        upgraderGui *upgrader = new upgraderGui();
        Q_UNUSED(upgrader);
        //Should there be some connections for the quit slot?
        return a.exec();
    }
}
