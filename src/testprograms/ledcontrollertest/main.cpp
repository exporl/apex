#include "ledcontrollertest.h"

#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc,argv);
    LedControllerTest test;
    test.exec();
}
