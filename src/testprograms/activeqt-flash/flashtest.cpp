#include "testobject.h"
#include "flashwidget.h"

#include "windows.h"		// sleep


#include <QDebug>
#include <QApplication>

#include <QAxWidget>


int main(int argc, char ** argv) {
    QApplication app(argc, argv);
    qDebug() << "bla";


    // direct QAxWidget use
    /*QAxWidget* mFlashWidget = new QAxWidget ("ShockwaveFlash.ShockwaveFlash");
    mFlashWidget->dynamicCall ("LoadMovie(int,const QString&)",0, "C:\\apex\\examples\\movies\\car\\introcar.swf");
    mFlashWidget->show();*/

    // direct flashwidget use
    /*FlashWidget fw;
    fw.loadMovie( "C:\\apex\\examples\\movies\\car\\goodcar.swf");
    fw.show();
    fw.play();*/


    // from test object (signal/slot)
    Test t;
    QMetaObject::invokeMethod(&t, "test",
                              Qt::QueuedConnection);


    /*ShockwaveFlashObjects::ShockwaveFlash swf;
    swf.LoadMovie(0,"C:\\apex\\examples\\movies\\car\\introcar.swf");
    swf.Play();
    swf.show();

    if (swf.IsPlaying())
            qDebug() << "playing";
    else
            qDebug() << "not playing";*/


    app.exec();
}

