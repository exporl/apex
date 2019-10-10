#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_NUMERICKEYPADELEMENT_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_NUMERICKEYPADELEMENT_H_

#include "screenelement.h"

namespace apex
{
namespace data
{
class APEXDATA_EXPORT NumericKeypadElement : public ScreenElement
{

public:
    NumericKeypadElement(const QString &id, ScreenElement *parent);
    ~NumericKeypadElement();

    ElementTypeT elementType() const;
    void visit(ScreenElementVisitor *v);
    void visit(ScreenElementVisitor *v) const;
};
}
}
#endif
