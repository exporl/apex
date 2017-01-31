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
#include "screen/apexscreenresult.h"

#include "services/errorhandler.h"

#include "screenrundelegate.h"
#include "apexcontrol.h"

namespace apex
{
namespace gui
{

using data::ScreenElement;

void ScreenRunDelegate::feedBack(
    const ScreenElementRunDelegate::mt_eFeedBackMode& mode,
    const QString feedbackElementID )
{
    const tScreenElementMap& elems( screen->getIDToElementMap() );
    tScreenElementMap::const_iterator i = elems.find( feedbackElementID );
    if( i != elems.end() )
    {
        ScreenElement* fbelem = i.value();
        elementToRunningMapT::iterator i = elementToRunningMap.find( fbelem );
        if ( i == elementToRunningMap.end() )
            qDebug( "ScreenRunDelegate::feedBack (rundelegate for %s): not found", qPrintable (feedbackElementID));
        feedBack( mode, i->second );
    }
    else {
     // qDebug ("ScreenRunDelegate::FeedBack (%s): not found", qPrintable (feedbackElementID));
        ErrorHandler::Get().addItem(StatusItem(StatusItem::WARNING, tr("Feedback"),
            QString(tr("Cannot show feedback, %1 not found")).arg(
                    feedbackElementID)));
    }
}

void ScreenRunDelegate::feedBack(
    const ScreenElementRunDelegate::mt_eFeedBackMode& mode,
    ScreenElementRunDelegate* feedBackElem )
{
  if ( feedBackElem )
  {
    feedBackElem->feedBack( mode );
    feedBackElement = feedBackElem;
  }
}

ScreenElementRunDelegate* ScreenRunDelegate::getFeedBackElement()
{
    return feedBackElement;
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
    : layout( 0 ), widget(new QWidget(0)), screen( s ), feedBackElement( 0 ), rootElement( 0 ),
      defaultFont( df ), defaultFontSize( dfs )
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
  for( elementToRunningMapT::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* p = it->second;
    p->connectSlots( this );

    //qDebug("elementToRunningMap: %p -> %p", it->first, it->second);
  }

  // create spinboxlist:
  for ( elementToRunningMapT::const_iterator it=elementToRunningMap.begin(); it!=elementToRunningMap.end(); ++it) {
    if ( (*it).first->elementType()==ScreenElement::SpinBox) {
        spinBoxList.push_back(dynamic_cast<SpinBoxRunDelegate*>(  it->second));
        qDebug("Adding %s to spinBoxList", qPrintable ((*it).first->getID()));
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
  for( elementToRunningMapT::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* p = it->second;
    p->setEnabled( enable );
    if( enable )
    {
        if(!screen->getDefaultAnswerElement().isEmpty() &&
            (p->getID() == screen->getDefaultAnswerElement() ))
        p->getWidget()->setFocus();
    }
  }
}

void ScreenRunDelegate::showWidgets()
{
  for( elementToRunningMapT::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* p = it->second;
    if ( p->getWidget() )
      p->getWidget()->show();
  }
}

void ScreenRunDelegate::hideWidgets()
{
  for( elementToRunningMapT::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* p = it->second;
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
  for( elementToRunningMapT::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* p = it->second;
    if( p->hasText() )
      p->clearText();
  }
}

void ScreenRunDelegate::addInterestingTexts( ApexScreenResult& result )
{
  //add everything else that doesn't belong to buttongroup
  for( elementToRunningMapT::const_iterator it = elementToRunningMap.begin();
       it != elementToRunningMap.end(); ++it )
  {
    ScreenElementRunDelegate* el = it->second;
//      qDebug(el->getID() + " hastext: " + QString::number( el->hasText()) + " hasinterestingtext " + QString::number( el->hasInterestingText()));
    if( el->hasText() && el->hasInterestingText() ) {
      result[ el->getID() ] = el->getText();
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
void ScreenRunDelegate::addScreenParameters( ApexScreenResult& result )
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

}

}
