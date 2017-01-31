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

#include "apexdata/screen/spinboxelement.h"

#include "apextools/apextools.h"

#include "gui/guidefines.h"

#include "screen/screenrundelegate.h"
#include "screen/spinboxrundelegate.h"

#include <QShortcut>

namespace apex
{
namespace rundelegates
{

const ScreenElement* SpinBoxRunDelegate::getScreenElement() const
{
    return element;
}

QWidget* SpinBoxRunDelegate::getWidget()
{
    return this;
}

bool SpinBoxRunDelegate::hasText() const
{
    return true;
}

bool SpinBoxRunDelegate::hasInterestingText() const
{
    return true;
}

const QString SpinBoxRunDelegate::getText() const
{
    return text();
}

void SpinBoxRunDelegate::resizeEvent( QResizeEvent* e )
{
    QSpinBox::resizeEvent (e);
    setFont (initialFont);
    ApexTools::shrinkTillItFits (this, text(), QSize (2, 2));

  if (element->GetReset()) {
    if (element->HasDefault())
        QSpinBox::setValue((int)element->GetValue());
    else
        QSpinBox::clear();
  }
}

void SpinBoxRunDelegate::connectSlots( gui::ScreenRunDelegate* d )
{
    connect( this, SIGNAL( answered( ScreenElementRunDelegate* ) ),
             d, SIGNAL( answered( ScreenElementRunDelegate* ) ) );
}

}
}

void apex::rundelegates::SpinBoxRunDelegate::sendAnsweredSignal()
{
    emit answered( this );
}

apex::rundelegates::SpinBoxRunDelegate::SpinBoxRunDelegate(
        ExperimentRunDelegate* p_exprd,
    QWidget* parent, const SpinBoxElement* e, const QFont& defaultFont ) :
      QSpinBox( parent ),
      ScreenElementRunDelegate(p_exprd, e),
      element(  e )
{
    setObjectName(element->getID());
    setStyleSheet(element->getStyle());

    if (! e->getShortCut("up").isEmpty()) {
        QShortcut* sc = new QShortcut(  e->getShortCut("up"), this );
        connect(sc, SIGNAL(activated()), this, SLOT(stepUp()));
    }
    if (! e->getShortCut("down").isEmpty()) {
        QShortcut* sc = new QShortcut(  e->getShortCut("down"), this );
        connect(sc, SIGNAL(activated()), this, SLOT(stepDown()));
    }


    QFont font = defaultFont;
    if ( element->getFontSize() != -1 )
        font.setPointSize( element->getFontSize() );
    QSpinBox::setFont( font );
    initialFont = font;

    // QSpinBox only implements int, not float, this is restricted by the apex schema
    QSpinBox::setMinimum((int)element->GetMin());
    QSpinBox::setMaximum((int)element->GetMax());
    if (element->HasDefault())
        QSpinBox::setValue((int)element->GetValue());
    QSpinBox::setSingleStep((int)element->GetStep());
    if (! element->GetParameter().isEmpty()) {
        QSpinBox::setToolTip( element->GetParameter() );
    }
}

