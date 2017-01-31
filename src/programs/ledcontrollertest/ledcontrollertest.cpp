#include "apextools/global.h"

#include "ledcontrollertest.h"
#include "ui_ledcontrollertest.h"

#include <QMessageBox>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

LedControllerTest::LedControllerTest():
    myDialog(new Ui::Dialog())
{
    if (!lc.errorMessage().isEmpty())
        QMessageBox::critical(&dialog, "Error", lc.errorMessage());

    myDialog->setupUi(&dialog);

    QButtonGroup* bg=new QButtonGroup(&dialog);
    for (int i=1; i<=16; ++i) {
        QPushButton* b = new QPushButton(QString::number(i), &dialog);
        myDialog->buttonLayout->addWidget(b,(i-1)/8,(i-1)%8);
        bg->addButton(b,i);
    }
    connect(bg, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));

    connect(myDialog->button_nightrider, SIGNAL(clicked(bool)),
            this, SLOT(nightRider()));

    connect(myDialog->buttonQuit, SIGNAL(clicked(bool)),
            this, SLOT(quit()));

    connect(myDialog->buttonClear, SIGNAL(clicked(bool)),
            this, SLOT(clear()));

    connect(myDialog->buttonRandom, SIGNAL(clicked(bool)),
            this, SLOT(random()));
}

LedControllerTest::~LedControllerTest()
{
    delete myDialog;
}


void LedControllerTest::exec()
{
    dialog.exec();
}


void LedControllerTest::buttonClicked(int i)
{
    lc.setLed(i);
    qCDebug(APEX_LEDFEEDBACKPLUGIN, "Setting led %d", i);
}

void LedControllerTest::nightRider()
{
    for (int i=0; i<13; ++i)
    {
        lc.setLed(i+1);
        #ifdef Q_OS_WIN32
        Sleep(200);
        #endif
        #ifdef Q_WS_X11
        usleep (200000);
        #endif
    }
}

void LedControllerTest::quit()
{
    dialog.done(0);
}

void LedControllerTest::clear()
{
    lc.clear();
}


void LedControllerTest::random()
{
    int delay=20;
    for (int i=0; i<5000/delay; ++i)  // 5 seconds
    {
        unsigned long r=qrand() + (qrand()<<16);
        lc.write(r);
        #ifdef Q_OS_WIN32
        Sleep(delay);
        #endif
        #ifdef Q_WS_X11
        usleep (delay*1000);
        #endif
    }
}
