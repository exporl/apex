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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_PICTURELABELRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_PICTURELABELRUNDELEGATE_H_

#include "screenelementrundelegate.h"

#include <QLabel>

namespace apex
{
namespace data
{
class ScreenElement;
class PictureLabelElement;
}

namespace rundelegates
{
using data::PictureLabelElement;
using data::ScreenElement;

/**
 * The PictureLabelRunDelegate class is an implementation of
 * ScreenElementRunDelegate representing a PictureLabelElement.
 */
class PictureLabelRunDelegate : public QLabel, public ScreenElementRunDelegate
{
    Q_OBJECT

    const PictureLabelElement *element;
    const QPixmap *pixmap, *pixmapDisabled;

public:
    PictureLabelRunDelegate(ExperimentRunDelegate *p_exprd, QWidget *parent,
                            const PictureLabelElement *e);
    ~PictureLabelRunDelegate();

    const ScreenElement *getScreenElement() const;

    QWidget *getWidget();

    void setEnabled(const bool e);
};
}
}

#endif
