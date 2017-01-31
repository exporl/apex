#ifndef _EXPORL_SRC_PROGRAMS_STATEMACHINEVISUALIZER_STATEMACHINEVISUALIZER_H_
#define _EXPORL_SRC_PROGRAMS_STATEMACHINEVISUALIZER_STATEMACHINEVISUALIZER_H_

class QAbstractState;
class QAbstractTransition;
class QStateMachine;
class QString;
class QStringList;

class StateMachineVisualizer
{
    public:

        void draw(const QStateMachine* machine, const QString& fileName);

    private:

        void drawChildStates(const QAbstractState* abstractParent,
                             const QString& fileName);

        QString label(const QAbstractState* state);
        QString id(const QAbstractState* state, const QStringList& existingIds);
        QString nodeAttributes(const QAbstractState* state);
        QString edgeAttributes(const QAbstractTransition* transition);
};

#endif
