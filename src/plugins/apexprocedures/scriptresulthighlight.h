#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTRESULTHIGHLIGHT_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTRESULTHIGHLIGHT_H_

#include "apexdata/procedure/procedureinterface.h"

namespace apex
{
namespace data
{

class ScriptResultHighlight : public QObject, public ResultHighlight
{
    Q_OBJECT
    Q_PROPERTY(bool correct READ getCorrect WRITE setCorrect)
    Q_PROPERTY(QString highlightElement READ getHighlightElement WRITE
                   setHighlightElement)
    Q_PROPERTY(bool overrideCorrectFalse READ getOverrideCorrectFalse WRITE
                   setOverrideCorrectFalse)
    Q_PROPERTY(bool showCorrectFalse READ getShowCorrectFalse WRITE
                   setShowCorrectFalse)

public:
    ScriptResultHighlight(QObject *parent = 0) : QObject(parent)
    {
    }

    bool getCorrect() const
    {
        return correct;
    }
    void setCorrect(bool p)
    {
        correct = p;
    }

    QString getHighlightElement() const
    {
        return highlightElement;
    }
    void setHighlightElement(const QString &p)
    {
        highlightElement = p;
    }

    bool getOverrideCorrectFalse()
    {
        return overrideCorrectFalse;
    }
    void setOverrideCorrectFalse(bool p)
    {
        overrideCorrectFalse = p;
    }

    bool getShowCorrectFalse()
    {
        return showCorrectFalse;
    }
    void setShowCorrectFalse(bool p)
    {
        showCorrectFalse = p;
    }
};
}
}

#endif // _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTRESULTHIGHLIGHT_H_
