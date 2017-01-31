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
 
#ifndef SCREENRUNDELEGATE_H
#define SCREENRUNDELEGATE_H

#include "apextypedefs.h"
#include "screen/screenelement.h"
#include "screen/rundelegatedefines.h"
#include "screen/screenelementrundelegate.h"
//#include "elementfactory.h"

#include <QObject>
#include <QLayout>

#include <memory>

namespace apex
{

class ApexScreenResult;
class ExperimentRunDelegate;

namespace gui
{

using data::Screen;
using data::ScreenElement;
using rundelegates::ScreenElementRunDelegate;
using rundelegates::elementToRunningMapT;
using rundelegates::spinBoxListT;
using rundelegates::SpinBoxRunDelegate;

/**
 * The ScreenRunDelegate class represents a \ref Screen towards
 * apex.  It contains a ScreenElementRunDelegate for every
 * ScreenElement in its screen, and keeps a "elementToRunning" map
 * from every ScreenElement to its ScreenElementRunDelegate, and
 * it knows about default fonts and such...
 *
 * The elementToRunning map is not strictly necessary.  If this
 * class would just keep the ScreenElementRunDelegates in a
 * vector, then it could just iterate over them and look for the
 * one corresponding to a certain ScreenElement.  In that sense,
 * the map is meant for optimisation.
 *
 * In order to retrieve all ScreenElementRunDelegates of this
 * ScreenRunDelegate, you should iterate over the values of the
 * elementToRunningMap.  They are not kept here in any other way.
 */
class ScreenRunDelegate
            : public QObject
{
        Q_OBJECT

    public:
        ScreenRunDelegate( ExperimentRunDelegate* p_exprd,
                           const data::Screen* screen, QWidget* parent,
                           const QString& defaultFont, int defaultFontSize );
        ~ScreenRunDelegate();

        QLayout* getLayout();
        /**
         * Return a widget containing the layout
         */
        QWidget* getWidget();


        void feedBack( const ScreenElementRunDelegate::mt_eFeedBackMode& mode,
                       const QString feedbackElementID );
        void feedBack( const ScreenElementRunDelegate::mt_eFeedBackMode& mode,
                       ScreenElementRunDelegate* feedBackElem );

        void enableWidgets( bool enable );
        void showWidgets();
        void hideWidgets();
        // clear user input if any
        void clearText();
        // add interesting texts from elements in the screen to the
        // result
        void addInterestingTexts( ApexScreenResult& result );
        void addScreenParameters( ApexScreenResult& result );

        const Screen* getScreen() const;

        ScreenElementRunDelegate* getFeedBackElement();
        QString getDefaultAnswerElement() const;

    signals:
        void answered( ScreenElementRunDelegate* );
        void newStimulus( stimulus::Stimulus* );
        void newAnswer( const QString& );
    private:
        QLayout* layout;
        std::auto_ptr<QWidget> widget;

        const data::Screen* screen;
        ScreenElementRunDelegate* feedBackElement;
        ScreenElementRunDelegate* rootElement;
        elementToRunningMapT elementToRunningMap;
        spinBoxListT spinBoxList;     //! list of all spin boxes, to be used for quickly having them set parameters, only necessary for performance
        QString defaultFont;
        int defaultFontSize;
};

}

}

#endif
