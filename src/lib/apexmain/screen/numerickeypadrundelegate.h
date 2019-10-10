#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_NUMERICKEYPADRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_NUMERICKEYPADRUNDELEGATE_H_

#include "numerickeypadwidget.h"
#include "screenelementrundelegate.h"

#include "apexdata/screen/numerickeypadelement.h"

namespace apex
{
class ExperimentRunDelegate;

namespace data
{
class NumericKeypadElement;
}

namespace rundelegates
{
using namespace data;
using namespace gui;

class NumericKeypadRunDelegate : public QObject, public ScreenElementRunDelegate
{
    Q_OBJECT

    const NumericKeypadElement *element;

public:
    NumericKeypadRunDelegate(ExperimentRunDelegate *p_rd, QWidget *parent,
                             const NumericKeypadElement *e,
                             const QFont &defaultFont);
    const ScreenElement *getScreenElement() const;
    QWidget *getWidget();
    bool hasText() const;
    bool hasInterestingText() const;
    const QString getText() const;
    void connectSlots(ScreenRunDelegate *d);
    void clearText();
signals:
    void answered(ScreenElementRunDelegate *);

private:
    NumericKeypadWidget numericKeypadWidget;
};
}
}

#endif
