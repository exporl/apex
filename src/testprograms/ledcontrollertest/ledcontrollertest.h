#include "ui_ledcontrollertest.h"
#include "ledcontroller.h"

#include <QObject>

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

