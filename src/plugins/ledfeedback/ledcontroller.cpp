#include "ledcontroller.h"

#include <QDebug>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif
#include <iowkit.h>
#include <exception>




const unsigned char byteA = 2;          // byte that controls leds 1-8
const unsigned char byteB = 1;          // byte that controls leds 9-16

class LedControllerPrivate
{
public:
    IOWKIT_HANDLE devHandle;
    QString eString;
};


LedController::LedController():
    d(new LedControllerPrivate)
{
    d->eString="";

    d->devHandle = IowKitOpenDevice();
    if (d->devHandle == NULL)
    {
        d->eString = "Could not open IOWarrior";
        return;
    }
    // Get number of IOWs in system
    int numIows = IowKitGetNumDevs();
    if (numIows!=1)
        qDebug() << "Warning: multiple IOWarriors found, using the first one";

    IowKitSetWriteTimeout(d->devHandle, 100);          // 100ms

 

    /*for (int i=0; i<32; ++i)
    {
        unsigned long value = ~(1<<i);
        write(value);
        qDebug("%d, %x", i, value);
        usleep (500000);
    }*/

    qDebug("Sending test pattern to leds");
    for (int i=0; i<13; ++i)
    {
        setLed(i+1);
        #ifdef Q_OS_WIN32
        Sleep(20);
        #endif
        #ifdef Q_WS_X11
        usleep (20000);
        #endif
    }
    clear();
}


LedController::~LedController()
{
    clear();
    IowKitCloseDevice(d->devHandle);
    delete d;
}

QString LedController::errorMessage()
{
    return d->eString;
}

void LedController::setLed(int number)
{
    unsigned long bits;
    if (number<=8)
        bits= 1<< (number-1 + byteA*8);
    else if (number>8 && number <=16)
        bits= 1<< (number-9 + byteB*8);
    else {
        d->eString = "LedController: error: invalid led number";
        return;
    }
    //qDebug("** writing %x", bits);
    write(~bits);
}

void LedController::clear()
{
    write(0xFFFFFFFF);
}

// from iowkittest.c, included with IOWarrior SDK
bool LedController::write(unsigned long value)
{
    IOWKIT_HANDLE devHandle = d->devHandle;
    IOWKIT_REPORT rep;
    IOWKIT56_IO_REPORT rep56;
    
    // Init report
    switch (IowKitGetProductId(devHandle))
    {
        // Write simple value to IOW40
        case IOWKIT_PRODUCT_ID_IOW40:
            memset(&rep, 0xff, sizeof(rep));
            rep.ReportID = 0;
            rep.Value = value;
            return IowKitWrite(devHandle, IOW_PIPE_IO_PINS,
                               (PCHAR) &rep, IOWKIT40_IO_REPORT_SIZE) == IOWKIT40_IO_REPORT_SIZE;
                               // Write simple value to IOW24
        case IOWKIT_PRODUCT_ID_IOW24:
            memset(&rep, 0xff, sizeof(rep));
            rep.ReportID = 0;
            rep.Bytes[0] = (UCHAR) value;
            return IowKitWrite(devHandle, IOW_PIPE_IO_PINS,
                               (PCHAR) &rep, IOWKIT24_IO_REPORT_SIZE) == IOWKIT24_IO_REPORT_SIZE;
                               // Write simple value to IOW56
        case IOWKIT_PRODUCT_ID_IOW56:
            memset(&rep56, 0xff, sizeof(rep56));
            rep56.ReportID = 0;
            rep56.Bytes[6] = (UCHAR) value;
            return IowKitWrite(devHandle, IOW_PIPE_IO_PINS,
                               (PCHAR) &rep56, IOWKIT56_IO_REPORT_SIZE) == IOWKIT56_IO_REPORT_SIZE;
        default:
            return FALSE;
    }
}
