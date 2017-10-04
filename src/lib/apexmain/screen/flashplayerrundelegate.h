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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_FLASHPLAYERRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_FLASHPLAYERRUNDELEGATE_H_

#include "screenelementrundelegate.h"

class FlashWidget;

namespace apex
{
class ExperimentRunDelegate;
namespace data
{
class ScreenElement;
class FlashPlayerElement;
}

namespace rundelegates
{

using data::ScreenElement;
using data::FlashPlayerElement;

/**
* The FlashPlayerRunDelegate class is an implementation of
* ScreenElementRunDelegate representing a FlashPlayerElement.
*/
class FlashPlayerRunDelegate : public QObject, public ScreenElementRunDelegate
{
    Q_OBJECT
public:
    /**
  * Constructor.
  */
    FlashPlayerRunDelegate(ExperimentRunDelegate *p_rd, QWidget *parent,
                           const FlashPlayerElement *e);

    /**
  * Destructor.
  */
    ~FlashPlayerRunDelegate();

    /**
* Implementation of the ScreenElementRunDelegate method.
*/
    const ScreenElement *getScreenElement() const;

    /**
  * Implementation of the ScreenElementRunDelegate method.
  */
    QWidget *getWidget();

    /**
  * Implementation of the ScreenElementRunDelegate method.
  */
    void connectSlots(ScreenRunDelegate *d);

    /**
  * Implementation of the ScreenElementRunDelegate method.
  */
    void feedBack(const FeedbackMode &mode);

    /**
  * Implementation of the ScreenElementRunDelegate method.
  */
    virtual void setEnabled(const bool e = true);

public slots:
    /**
  * Cllaed for clicks/keyboard shortcuts
  */
    void sendAnsweredSignal();

    /**
  * Override to start/stop movie.
  */
    void setVisible(bool);

signals:
    void answered(ScreenElementRunDelegate *e);

private:
    void setMovie(const QString &path, const bool startNow = true);
    void mp_StartWait();

    const FlashPlayerElement *element;
    FlashWidget *fw;

private slots:
    void movieFinished();
};
}
}

#endif
