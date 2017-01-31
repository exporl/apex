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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_SLIDERRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_SLIDERRUNDELEGATE_H_

#include "screenelementrundelegate.h"

#include <QSlider>

namespace apex
{
  namespace data
  {
    class ScreenElement;
    class SliderElement;
  }
namespace rundelegates
  {
    using data::SliderElement;
    using data::ScreenElement;

    /**
     * The SliderRunDelegate class is an implementation of
     * ScreenElementRunDelegate representing a SliderElement.
     */
    class SliderRunDelegate
      : public QSlider, public ScreenElementRunDelegate
    {
      Q_OBJECT

          QFont initialFont;
      const SliderElement* element;
    public:
        SliderRunDelegate( ExperimentRunDelegate* p_exprd,
                           QWidget* parent, const SliderElement* e,
                         const QFont& defaultFont );

      const ScreenElement* getScreenElement() const;

      QWidget* getWidget();
      bool hasText() const;
      bool hasInterestingText() const;
      const QString getText() const;
      void connectSlots( gui::ScreenRunDelegate* d );
    signals:
      void answered( ScreenElementRunDelegate* );
    protected:
      void resizeEvent( QResizeEvent* e );
    public slots:
      void sendAnsweredSignal();
    };
  }
}


#endif // {:-o)= Milo was here














