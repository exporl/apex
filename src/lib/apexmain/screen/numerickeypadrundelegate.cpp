#include "numerickeypadrundelegate.h"

#include "screen/screenrundelegate.h"

namespace apex
{
namespace rundelegates
{

const ScreenElement *NumericKeypadRunDelegate::getScreenElement() const
{
    return element;
}

QWidget *NumericKeypadRunDelegate::getWidget()
{
    return &numericKeypadWidget;
}

bool NumericKeypadRunDelegate::hasText() const
{
    return true;
}

bool NumericKeypadRunDelegate::hasInterestingText() const
{
    return true;
}

const QString NumericKeypadRunDelegate::getText() const
{
    return numericKeypadWidget.getTriplet();
}

void NumericKeypadRunDelegate::connectSlots(ScreenRunDelegate *d)
{
    connect(this, &NumericKeypadRunDelegate::answered, d,
            &ScreenRunDelegate::answered);
}

NumericKeypadRunDelegate::NumericKeypadRunDelegate(
    ExperimentRunDelegate *p_rd, QWidget *parent, const NumericKeypadElement *e,
    const QFont &defaultFont)
    : QObject(parent),
      ScreenElementRunDelegate(p_rd, e),
      element(e),
      numericKeypadWidget(parent)
{
    numericKeypadWidget.setObjectName(element->getID());
    setCommonProperties(&numericKeypadWidget);

    QFont font = defaultFont;
    if (element->getFontSize() != -1)
        font.setPointSize(element->getFontSize());
    numericKeypadWidget.setFont(font);

    connect(&numericKeypadWidget, &NumericKeypadWidget::tripletSubmitted,
            [&]() { Q_EMIT answered(this); });
}

void NumericKeypadRunDelegate::clearText()
{
    numericKeypadWidget.clear();
}
}
}
