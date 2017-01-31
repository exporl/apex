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

#ifndef SLIDERELEMENT_H
#define SLIDERELEMENT_H

#include "screenelement.h"

#include <QString>
//#include <QtGui>

#include "global.h"

namespace apex
{
namespace data
{
/**
 * The SliderElement is an implementation of a \ref
 * ScreenElement representing a "slider".
 */
class APEXDATA_EXPORT SliderElement
            : public ScreenElement
{
        QString text;
    public:
        SliderElement(const QString& id, ScreenElement* parent);
        ~SliderElement();

        ElementTypeT elementType() const;
        void visit(ScreenElementVisitor* v);
        void visit(ScreenElementVisitor* v) const;

        bool HasDefault() const { return hasDefault; };

        // Setter
        void SetOrient(const Qt::Orientation h_or_v) { orient = h_or_v; };
        Qt::Orientation GetOrient() const { return orient; };
        void SetMin(const float p) { minimum = p; };
        float GetMin() const { return minimum; };
        void SetMax(const float p) { maximum = p; };
        float GetMax() const { return maximum; };
        void SetValue(const float val) { value = val; hasDefault = true; };
        float GetValue() const { return value; };
        void SetTickPos(const /*QSlider::TickPosition*/int tpos) { tickPos = tpos; };
        /*QSlider::TickPosition*/int GetTickPos() const { return tickPos; };
        void SetTickInt(const int tint) { tickInt = tint; };
        void SetReset(const bool res) { reset=res; };
        void SetParameter(const QString par) { parameter = par; };
        void SetStepSize(const int sSize) {stepSize = sSize;};
        void SetPageSize(const int pSize) {pageSize = pSize;};

        // Getter
        int GetTickInt() const { return tickInt; };
        bool GetReset() const { return reset; };
        const QString& GetParameter() const { return parameter; };
        int GetStepSize() const {return stepSize;};
        int GetPageSize() const {return pageSize;};

        bool operator==(const SliderElement& other) const;

    private:
        bool hasDefault;
        Qt::Orientation orient;
        float value;
        float minimum;
        float maximum;
        /*QSlider::TickPosition*/int tickPos;
        int stepSize;
        int pageSize;
        int tickInt;
        bool reset;
        QString parameter;
};
}
}
#endif
