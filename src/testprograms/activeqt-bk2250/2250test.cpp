#include <QDebug>
#include <QApplication>

#include <QAxWidget>
#include <QAxObject>
#include <QUuid>

#include <qt_windows.h>
#include "basicenvremoteapi.h"      // generated using qt dumpcpp

int main(int argc, char ** argv) {
    QApplication app(argc, argv);
    qDebug() << "bla";


    // poging 1: Michael

    /*BasicEnvRemoteAPI::I2250Connect * connectInterface = 0;
    BasicEnvRemoteAPI::RemoteAPI ra(connectInterface);
    qDebug("ra constructed");*/

    /*const GUID __declspec(selectany) CLSID_RemoteAPI =
        {0x5bc25821,0x4936,0x4e9e,{0x9a,0xcf,0x12,0xe1,0x2a,0xdd,0xa9,0xcc}};*/

    // gebeurt in constructor:
    //ra.setControl(QUuid("5bc25821-4936-4e9e-9acf12e12adda9c"));
    //qDebug() << connectInterface->GetInstrumentNames();

    /*
    void * connectInterfaceV;
    BasicEnvRemoteAPI::I2250Connect * connectInterface = new BasicEnvRemoteAPI::I2250Connect();
    qDebug("connectinterface created");
    connectInterface->queryInterface(QUuid("d0ad98e5-8c38-43b1-a37a-e4ab4d1bfed4"), (void**) &connectInterfaceV);
    qDebug("connectinterface queried");
    connectInterface = new BasicEnvRemoteAPI::I2250Connect((IDispatch*)connectInterfaceV);
    BasicEnvRemoteAPI::RemoteAPI connectInterfaceR(connectInterface);
    */

    //qDebug() << connectInterfaceR.GetInstrumentNames();


    /*BasicEnvRemoteAPI::I2250Connect * connectInterface; // = new BasicEnvRemoteAPI::I2250Connect();
    qDebug() << "Connectinterface created";
    BasicEnvRemoteAPI::RemoteAPI ra;
    qDebug() << "Remoteapi created";
    if (! ra.queryInterface(
            QUuid("{d0ad98e5-8c38-43b1-a37a-e4ab4d1bfed4}"),
            (void**) &connectInterface) ) {
        qDebug("Queryinterface failed");
        return 1;
    }*/
    // VC header connectinterface: {0xd0ad98e5,0x8c38,0x43b1,{0xa3,0x7a,0xe4,0xab,0x4d,0x1b,0xfe,0xd4}};


    const GUID CLSID_RemoteAPI =
        {0x5bc25821,0x4936,0x4e9e,{0x9a,0xcf,0x12,0xe1,0x2a,0xdd,0xa9,0xcc}};
    const GUID IID_I2250Connect =
        {0xd0ad98e5,0x8c38,0x43b1,{0xa3,0x7a,0xe4,0xab,0x4d,0x1b,0xfe,0xd4}};
    const GUID IID_I2250Control =
        {0xa994720d,0xd71b,0x405e,{0x8e,0x90,0x2e,0xc8,0xcf,0xbb,0xf6,0x03}};
    const GUID IID_I2250Instrument =
        {0x288be79a,0x6c9e,0x44a7,{0x91,0x20,0xef,0xd8,0x57,0x9d,0xdf,0xca}};

    IUnknown* connectInterfaceP = 0;
    IUnknown* controlInterfaceP = 0;
    IUnknown* instrumentInterfaceP = 0;

    CoCreateInstance(CLSID_RemoteAPI, NULL,
        CLSCTX_INPROC_SERVER,IID_I2250Connect, (void**) &connectInterfaceP);
    if (!connectInterfaceP)
        qFatal("connectInterfaceP==0");

    connectInterfaceP->QueryInterface(IID_I2250Control,(void**)&controlInterfaceP);
    if (!controlInterfaceP)
        qFatal("controlInterfaceP==0");

    connectInterfaceP->QueryInterface(IID_I2250Instrument,(void**)&instrumentInterfaceP);
    if (!instrumentInterfaceP)
        qFatal("instrumentInterfaceP==0");


    QString unicodeName = QLatin1String("GetInstrumentNames");
    OLECHAR *names = (wchar_t*)unicodeName.utf16();
    DISPID dispid;
    HRESULT q =
    ((IDispatch*)connectInterfaceP)->GetIDsOfNames(IID_NULL, &names, 1, LOCALE_USER_DEFAULT, &dispid);
    if (q!=S_OK)
        qDebug("GetIDsOfNames returned error");

    if (dispid == DISPID_UNKNOWN)
        qDebug("dispid == DISPID_UNKNOWN");

    // Start Qt stuff
    BasicEnvRemoteAPI::I2250Connect connectInterface((IDispatch*)connectInterfaceP);

    qDebug("Calling GetInstrumentNames");
    qDebug() << connectInterface.GetInstrumentNames();

    // poging 3: qaxobject
    /*QAxObject ao;
    ao.setControl("d0ad98e5-8c38-43b1-a37a-e4ab4d1bfed4");
    ao.dynamicCall("GetInstrumentNames()");*/

}

