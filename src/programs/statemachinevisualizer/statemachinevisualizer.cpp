#include "apextools/global.h"

#include "statemachinevisualizer.h"

#include <QDebug>
#include <QEventTransition>
#include <QFile>
#include <QLoggingCategory>
#include <QSignalTransition>
#include <QStateMachine>
#include <QStringList>
#include <QTextStream>

#define REPLACE_PATTERN "${state}"

Q_DECLARE_LOGGING_CATEGORY(APEX_STATEMACHINEVISUALIZER)
Q_LOGGING_CATEGORY(APEX_STATEMACHINEVISUALIZER, "apex.statemachinevisualizer")

void StateMachineVisualizer::draw(const QStateMachine *machine,
                                  const QString &filePattern)
{
    if (!filePattern.contains(REPLACE_PATTERN)) {
        qCDebug(APEX_STATEMACHINEVISUALIZER) << "Filename should contain"
                                             << REPLACE_PATTERN;
        return;
    }

    drawChildStates(machine, filePattern);
}

void StateMachineVisualizer::drawChildStates(
    const QAbstractState *abstractParent, const QString &filePattern)
{
    const QState *parent = qobject_cast<const QState *>(abstractParent);
    if (parent == 0)
        return;

    QList<const QAbstractState *> states =
        parent->findChildren<const QAbstractState *>();

    Q_FOREACH (const QAbstractState *state, states) {
        if (state->parentState() != parent)
            states.removeAll(state);
    }

    if (states.isEmpty())
        return;

    QString parentLabel = label(parent);
    QString fileName =
        QString(filePattern).replace(REPLACE_PATTERN, parentLabel);
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
        qCDebug(APEX_STATEMACHINEVISUALIZER) << "Failed to open file"
                                             << fileName << "for writing";
        return;
    }

    QTextStream stream(&file);
    stream << "digraph " << parentLabel << " {\n";

    QMap<const QAbstractState *, QString> idMap;

    // draw entry point
    const QAbstractState *initialState = parent->initialState();
    if (initialState != 0) {
        idMap[0] = "__init__";
        stream << idMap[0] << " [shape=point];\n";
    }

    // draw nodes
    Q_FOREACH (const QAbstractState *state, states) {
        QString stateLabel = label(state);
        QString stateId = id(state, idMap.values());
        idMap[state] = stateId;
        stream << stateId << " [" << nodeAttributes(state) << "];\n";
    }

    stream << "\n";

    // draw transition to initial state
    if (initialState != 0)
        stream << idMap[0] << " -> " << idMap[initialState] << ";\n";

    // draw transitions
    Q_FOREACH (const QAbstractState *abstractState, states) {
        const QState *state = qobject_cast<const QState *>(abstractState);
        if (state == 0)
            continue;

        Q_ASSERT(idMap.contains(state));
        QString stateId = idMap[state];

        Q_FOREACH (const QAbstractTransition *transition,
                   state->transitions()) {
            Q_FOREACH (const QAbstractState *target,
                       transition->targetStates()) {
                if (!idMap.contains(target)) {
                    qCDebug(APEX_STATEMACHINEVISUALIZER)
                        << "While drawing state" << parentLabel
                        << "transition from state" << label(state) << "to state"
                        << label(target) << "which does not exist";

                    continue;
                }

                stream << stateId << " -> " << idMap[target] << " ["
                       << edgeAttributes(transition) << "];\n";
            }
        }
    }

    stream << "}\n";
    file.close();

    Q_FOREACH (const QAbstractState *state, states)
        drawChildStates(state, filePattern);
}

QString StateMachineVisualizer::label(const QAbstractState *state)
{
    QString ret = state->objectName();

    if (ret.isEmpty())
        ret = state->metaObject()->className();

    return ret;
}

QString StateMachineVisualizer::id(const QAbstractState *state,
                                   const QStringList &existingIds)
{
    QString idTemplate = label(state).replace(' ', '_') + "%0";
    QString ret = idTemplate.arg("");
    int i = 1;

    while (existingIds.contains(ret))
        ret = idTemplate.arg(i++);

    return ret;
}

QString StateMachineVisualizer::nodeAttributes(const QAbstractState *state)
{
    QString ret = QString("label=\"%0\", shape=%1").arg(label(state));
    QString shape;

    if (state->inherits("QFinalState"))
        shape = "doublecircle";
    else
        shape = "circle";

    return ret.arg(shape);
}

QString
StateMachineVisualizer::edgeAttributes(const QAbstractTransition *transition)
{
    QString ret = "label=\"%0\"";
    QString label;

    if (transition->inherits("QSignalTransition")) {
        label = static_cast<const QSignalTransition *>(transition)->signal();
        // somehow, the first char of the signal is always '2'...
        label.remove(0, 1);
    } else if (transition->inherits("QEventTransition")) {
        QEvent::Type type =
            static_cast<const QEventTransition *>(transition)->eventType();
        label = QString("QEvent(type=%0)").arg(type);
    } else
        label = "Unknown";

    return ret.arg(label);
}
