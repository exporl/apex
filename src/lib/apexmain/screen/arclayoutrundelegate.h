/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_ARCLAYOUTRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_ARCLAYOUTRUNDELEGATE_H_

#include "apexdata/screen/screenelement.h"

#include "apextools/gui/arclayout.h"

#include "rundelegatedefines.h"
#include "screenelementrundelegate.h"

#include <QLayout>

class QFont;

namespace apex
{

class ExperimentRunDelegate;

namespace data
{
class ArcLayoutElement;
class ScreenElement;
}
namespace rundelegates
{
using data::ArcLayoutElement;
using data::ScreenElement;

/**
 * The ArcLayoutRunDelegate class is an implementation of
 * ScreenElementRunDelegate representing a ArcLayoutElement.
 */
class ArcLayoutRunDelegate : public ScreenElementRunDelegate, public ArcLayout
{
    const ArcLayoutElement *element;

public:
    ArcLayoutRunDelegate(ExperimentRunDelegate *p_parent,
                         const ArcLayoutElement *e, QWidget *parent,
                         ElementToRunningMap &elementToRunningMap,
                         const QFont &font);

    const ScreenElement *getScreenElement() const;
    QWidget *getWidget();

    virtual QLayout *getLayout();
};
}
}

#endif
