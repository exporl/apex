#include "apexmain/gui/flashwidget.h"

#include "apextools/global.h"

#include "testobject.h"

#include <windows.h>

#include <Qt>

void Test::test()
{
    fw = new FlashWidget;

    // set background color
    QPalette np = fw->palette();
    np.setColor(QPalette::Inactive, QPalette::Window, Qt::green);
    np.setColor(QPalette::Active, QPalette::Window, Qt::green);
    np.setColor(QPalette::Active, QPalette::Button, Qt::green);
    fw->setPalette(np);

    // QObject::connect(fw, SIGNAL(OnReadyStateChange(int)), this,
    // SLOT(stateChange(int)));
    // connect(&fw, SIGNAL(invalid(int)), &t, SLOT(stateChange(int));

    bool result;

    connect(fw, SIGNAL(finished()), this, SLOT(movieFinished()));
    connect(fw, SIGNAL(mouseReleased(Qt::MouseButton)), this,
            SLOT(movieClicked()));

    fw->show();

    //  result = fw->loadMovie( "C:\\apex\\invalid");
    result = fw->loadMovie("C:\\apex\\examples\\movies\\car\\goodcar.swf");
    if (!result)
        qCDebug(APEX_RS) << "Error loading";
    // fw.dynamicCall ("LoadMovie(int,const QString&)",0,
    // "C:\\apex\\examples\\movies\\car\\introcar.swf");

    // fw->setAutoFillBackground(true);

    /*QWidget* test = new QWidget;
    test->setPalette(np);
    test->show();*/

    result = fw->play();
    if (!result)
        qCDebug(APEX_RS) << "Error playing";
}

void Test::movieFinished()
{
    qCDebug(APEX_RS, "Test::movieFinished()");
}

void Test::movieClicked()
{
    qCDebug(APEX_RS, "Test::movieClicked()");
}

void Test::stateChange(int state)
{
    qCDebug(APEX_RS) << "ReadyState: " << state;
}
