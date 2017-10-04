/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_SPINBOXELEMENT_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_SPINBOXELEMENT_H_

#include "screenelement.h"

#include <QString>

#include "apextools/global.h"

namespace apex
{
namespace data
{
/**
 * The SpinBoxElement is an implementation of a \ref
 * ScreenElement representing a "spinbox".
 */
class APEXDATA_EXPORT SpinBoxElement : public ScreenElement
{
public:
    SpinBoxElement(const QString &id, ScreenElement *parent);
    ~SpinBoxElement();

    ElementTypeT elementType() const;
    void visit(ScreenElementVisitor *v);
    void visit(ScreenElementVisitor *v) const;

    void SetMin(const float p)
    {
        minimum = p;
    };
    float GetMin() const
    {
        return minimum;
    };
    void SetMax(const float p)
    {
        maximum = p;
    };
    float GetMax() const
    {
        return maximum;
    };
    void SetStep(const float p)
    {
        step = p;
    };
    float GetStep() const
    {
        return step;
    };
    void SetValue(const float p)
    {
        value = p;
        hasDefault = true;
    };
    float GetValue() const
    {
        return value;
    };
    bool HasDefault() const
    {
        return hasDefault;
    };
    bool GetReset() const
    {
        return reset;
    };
    void SetReset(const bool p)
    {
        reset = p;
    };

    void SetParameter(const QString p)
    {
        parameter = p;
    };
    const QString &GetParameter() const
    {
        return parameter;
    };

    bool operator==(const SpinBoxElement &other) const;

private:
    bool hasDefault;
    float minimum;
    float maximum;
    float step;
    float value;
    bool reset;
    QString parameter;
};
}
}
#endif
