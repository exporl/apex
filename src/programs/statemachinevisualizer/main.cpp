#include "statemachinevisualizer.h"

#include "apexmain/experimentcontrol.h"

#include <QDir>

int main(int argc, char *argv[])
{
    QDir dir;

    if (argc == 2)
        dir = argv[1];

    apex::ExperimentControl ec;
    StateMachineVisualizer v;
    v.draw(ec.machine(), dir.absoluteFilePath("fsm_${state}.dot"));
}
