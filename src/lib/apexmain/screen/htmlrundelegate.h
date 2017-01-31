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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLRUNDELEGATE_H_

#include "screenelementrundelegate.h"
#include "htmlapi.h"

#include <QWebView>
#include <QScopedPointer>


// TODO: move code from rtresultsink into apexwebpage and reuse here

namespace apex
{
    class ExperimentRunDelegate;
  namespace data
  {
    class ScreenElement;
    class HtmlElement;
  }

  namespace rundelegates
  {
    using data::HtmlElement;
    using data::ScreenElement;

    /**
     * The HtmlRunDelegate class is an implementation of
     * ScreenElementRunDelegate representing a HtmlElement.
     */
    class HtmlRunDelegate
      : public QWebView, public ScreenElementRunDelegate
    {
      Q_OBJECT

      QFont initialFont;
      const HtmlElement* element;
      QScopedPointer<HtmlAPI> api;
      bool hasEnabledWaiting;
      bool hasFinishedLoading;

      void enable();

    public:
        HtmlRunDelegate( ExperimentRunDelegate* p_rd,
                         QWidget* parent, const HtmlElement* e);

      const ScreenElement* getScreenElement() const;

      QWidget* getWidget();
      bool hasText() const;
      bool hasInterestingText() const;
      const QString getText() const;
      void connectSlots( gui::ScreenRunDelegate* d );
      void feedBack(const FeedbackMode& mode);
    signals:
      void answered( ScreenElementRunDelegate* );
    protected:
      void resizeEvent( QResizeEvent* e );
      void changeEvent ( QEvent * event );
    public slots:
      void sendAnsweredSignal();
    private slots:
      void loadFinished(bool ok);
    };
  }
}

#endif
