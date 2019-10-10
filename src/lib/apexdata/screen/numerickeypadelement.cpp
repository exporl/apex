#include "numerickeypadelement.h"
#include "screenelementvisitor.h"

namespace apex
{
namespace data
{

NumericKeypadElement::NumericKeypadElement(const QString &id,
                                           ScreenElement *parent)
    : ScreenElement(id, parent)
{
}

NumericKeypadElement::~NumericKeypadElement()
{
}

void NumericKeypadElement::visit(ScreenElementVisitor *v)
{
    v->visitNumericKeypad(this);
}

void NumericKeypadElement::visit(ScreenElementVisitor *v) const
{
    v->visitNumericKeypad(this);
}

ScreenElement::ElementTypeT NumericKeypadElement::elementType() const
{
    return NumericKeypad;
}
}
}
