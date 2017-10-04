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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_GRIDLAYOUTRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_GRIDLAYOUTRUNDELEGATE_H_

#include "apexdata/screen/screenelement.h"

#include "rundelegatedefines.h"
#include "screenelementrundelegate.h"

#include <QGridLayout>

class QFont;

namespace apex
{

class ExperimentRunDelegate;

namespace data
{
class GridLayoutElement;
class ScreenElement;
}
namespace rundelegates
{
using data::GridLayoutElement;
using data::ScreenElement;

/**
 * The GridLayoutRunDelegate class is an implementation of
 * ScreenElementRunDelegate representing a GridLayoutElement.
 */
class GridLayoutRunDelegate : public QGridLayout,
                              public ScreenElementRunDelegate
{
    const GridLayoutElement *element;

public:
    GridLayoutRunDelegate(ExperimentRunDelegate *p_exprd,
                          const GridLayoutElement *e, QWidget *parent,
                          ElementToRunningMap &elementToRunningMap,
                          const QFont &font);

    /*~GridLayoutRunDelegate() {
        qCDebug(APEX_RS, "nop");
    }*/

    const ScreenElement *getScreenElement() const;
    QWidget *getWidget();
    QLayout *getLayout();
};
}
}

#endif
