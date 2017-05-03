#ifndef _APEX_SRC_PROGRAMS_LEDCONTROLLERTEST_LEDCONTROLLERTEST_H_
#define _APEX_SRC_PROGRAMS_LEDCONTROLLERTEST_LEDCONTROLLERTEST_H_


#include "ledcontroller.h"

#include <QObject>
#include <QDialog>

namespace Ui {
    class Dialog;
};

class LedControllerTest:
    public QObject
{
    Q_OBJECT

    public:
        LedControllerTest();
        ~LedControllerTest();

        void exec();

    public slots:
        void buttonClicked(int);
        void nightRider();
        void quit();
        void clear();
        void random();

    private:
        QDialog dialog;
        Ui::Dialog* myDialog;

        LedController lc;
};

#endif
