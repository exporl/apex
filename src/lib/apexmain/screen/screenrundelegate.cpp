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
 
#include "gui/guidefines.h"
//#include "gui/mainwindowconfig.h"

//from libdata
#include "experimentdata.h"

#include "screen/rundelegatecreatorvisitor.h"
#include "screen/screen.h"
#include "screen/screenelement.h"
#include "screen/spinboxelement.h"
#include "screen/spinboxrundelegate.h"
#include "screen/screenresult.h"

#include "services/errorhandler.h"

#include "screenrundelegate.h"
#include "apexcontrol.h"

namespace apex
{
namespace gui
{

using data::ScreenElement;

void ScreenRunDelegate::feedback(ScreenElementRunDelegate::FeedbackMode mode,
                                 const QString& feedbackElementId)
{
    QString warning;

    if (screen->idToElementMap().contains(feedbackElementId))
    {
        ScreenElement* element = screen->idToElementMap()[feedbackElementId];

        if (elementToRunningMap.contains(element))
            feedback(mode, elementToRunningMap[element]);
        else
            warning = tr("No rundelegate found for %1").arg(feedbackElementId);
    }
    else
        warning = tr("No screen found for %1").arg(feedbackElementId);

    if (!warning.isEmpty())
    {
        QString message = tr("Cannot show feedback because: %1").arg(warning);
        ErrorHandler::Get().addWarning(tr("Feedback"), message);
    }
}

void ScreenRunDelegate::feedback(ScreenElementRunDelegate::FeedbackMode mode,
                                 ScreenElementRunDelegate* feedbackElement)
{
    if (feedbackElement != 0)
    {
        feedbackElement->feedBack(mode);
        this->feedbackElement = feedbackElement;
    }
}

ScreenElementRunDelegate* ScreenRunDelegate::getFeedBackElement()
{
    return feedbackElement;
}

ScreenRunDelegate::~ScreenRunDelegate()
{
    // QLayouts delete their children...
}

QString ScreenRunDelegate::getDefaultAnswerElement() const
{
    return screen->getDefaultAnswerElement();
}

ScreenRunDelegate::ScreenRunDelegate( ExperimentRunDelegate* p_exprd,
                                      const Screen* s, QWidget* parent,
                                      const QString& df, int dfs )
    : layout( 0 ), widget(new QWidget(0)), screen( s ), feedbackElement( 0 ), rootElement( 0 ),
      defaultFont( df ), defaultFontSize( dfs ), enabled(false)
{
    Q_UNUSED (parent);

//    qDebug("Screen %s: widget=%p", qPrintable(s->getID()), widget.get());

  if ( defaultFont.isEmpty() )
    defaultFont = sc_sDefaultFont;
  if ( defaultFontSize == -1 )
    defaultFontSize = sc_nDefaultFontSize;
  QFont font( defaultFont );
  font.setPointSize( defaultFontSize );

  rundelegates::RunDelegateCreatorVisitor delcreator( p_exprd,
          widget.get(), elementToRunningMap, font );

  rootElement = delcreator.createRunDelegate( screen->getRootElement() );
  for( ElementToRunningMap::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* p = it.value();
    p->connectSlots( this );

    //qDebug("elementToRunningMap: %p -> %p", it->first, it->second);
  }

  // create spinboxlist:
  for ( ElementToRunningMap::const_iterator it=elementToRunningMap.begin(); it!=elementToRunningMap.end(); ++it) {
    if ( it.key()->elementType()==ScreenElement::SpinBox) {
        spinBoxList.push_back(dynamic_cast<SpinBoxRunDelegate*>(  it.value()));
        qDebug("Adding %s to spinBoxList", qPrintable (it.key()->getID()));
    }
  }


  layout = rootElement->getLayout();

  if (layout)
      widget->setLayout(layout);
  else
      qFatal("error");
}

void ScreenRunDelegate::enableWidgets( bool enable )
{
  enabled = enable;

  for( ElementToRunningMap::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* p = it.value();
    p->setEnabled( enable );
    if( enable )
    {
        if(!screen->getDefaultAnswerElement().isEmpty() &&
            (p->getID() == screen->getDefaultAnswerElement() ))
        p->getWidget()->setFocus();
    }
  }
}

bool ScreenRunDelegate::widgetsEnabled() const
{
    return enabled;
}

void ScreenRunDelegate::showWidgets()
{
  for( ElementToRunningMap::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* p = it.value();
    if ( p->getWidget() )
      p->getWidget()->show();
  }
}

void ScreenRunDelegate::hideWidgets()
{
  for( ElementToRunningMap::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* p = it.value();
    if ( p->getWidget() )
        p->getWidget()->hide();
  }
}

QLayout* ScreenRunDelegate::getLayout()
{
    return layout;
}

void ScreenRunDelegate::clearText()
{
  //clear user input if any
  for( ElementToRunningMap::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* p = it.value();
    if( p->hasText() )
      p->clearText();
  }
}

void ScreenRunDelegate::addInterestingTexts( ScreenResult& result )
{
  //add everything else that doesn't belong to buttongroup
  for( ElementToRunningMap::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* el = it.value();
//      qDebug(el->getID() + " hastext: " + QString::number( el->hasText()) + " hasinterestingtext " + QString::number( el->hasInterestingText()));
    if( el->hasText() && el->hasInterestingText() ) {
      result.map()[ el->getID() ] = el->getText();
     }
     /* else {
                qDebug("not interesting");
     }*/
  }
}

/**
 * Add values of parameters that are directly set by screenelements (eg SpinBox)
 * @param result
 */
void ScreenRunDelegate::addScreenParameters( ScreenResult& result )
{
    for ( spinBoxListT::const_iterator it = spinBoxList.begin(); it!=spinBoxList.end(); ++it) {
        if ( ! (*it)->GetElement()->GetParameter().isEmpty() && ! (*it)->text().isEmpty())
            result.SetStimulusParameter((*it)->GetElement()->GetParameter(), QString::number((*it)->value()) );
    }

}

const Screen* ScreenRunDelegate::getScreen() const
{
    return screen;
}

QWidget* ScreenRunDelegate::getWidget()
{
    return widget.get();
}

void ScreenRunDelegate::setAnswer(const QString& answer)
{
    emit newAnswer(answer);
}

}

}
