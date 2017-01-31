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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_LABELRUNDELEGATEBASE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_LABELRUNDELEGATEBASE_H_

#include "screenelementrundelegate.h"

#include <QLabel>

class QFont;

namespace apex
{
    class ExperimentRunDelegate;
namespace rundelegates
{
    using data::ScreenElement;

    /**
     * The LabelRunDelegateBase class is a common base class for
     * ScreenElementRunDelegate classes representing their
     * ScreenElement using a QLabel. It contains functions that are
     * common to such subclasses.
     */
    class LabelRunDelegateBase
        : public QLabel, public ScreenElementRunDelegate
    {
        Q_OBJECT
    public:
        LabelRunDelegateBase( ExperimentRunDelegate* p_exprd,
                              QWidget* parent, const ScreenElement* se,
                              const QFont& defaultFont );

        QLabel* getWidget();

        bool hasText() const;
        bool hasInterestingText() const;
        const QString getText() const;
        void setText( const QString& text );

        void resizeEvent( QResizeEvent* e );
        virtual void setEnabled( const bool );
    private:
        QString text;
        QFont initialFont;
    };
}
}
#endif
