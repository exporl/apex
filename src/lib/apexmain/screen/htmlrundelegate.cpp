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

#include "apexdata/screen/htmlelement.h"

#include "apextools/apextools.h"

#include "gui/guidefines.h"

#include "screen/screenrundelegate.h"

#include "accessmanager.h"

#include "htmlapi.h"
#include "htmlrundelegate.h"

#include <QVariant>
#include <QWebFrame>

// TODO: error handling (functions missing in javascript etc.)
namespace apex
{
namespace rundelegates
{

const ScreenElement* HtmlRunDelegate::getScreenElement() const
{
    return element;
}

void HtmlRunDelegate::loadFinished(bool ok) {
    hasFinishedLoading = ok;
    if(hasEnabledWaiting && hasFinishedLoading) {
        enable();
        hasEnabledWaiting = false;
    }
}

void HtmlRunDelegate::changeEvent ( QEvent * event )
{
    if (event->type() == QEvent::EnabledChange) {
        //qCDebug(APEX_RS, " HtmlRunDelegate::changeEvent: QEvent::EnabledChange, isenabled()=%d", isEnabled());
        if (isEnabled()) {
            enable();
        }
    }
}

void HtmlRunDelegate::enable() {
    if(!hasFinishedLoading) {
        hasEnabledWaiting = true;
        return;
    }

    QString code( "reset(); enabled();");
    QVariant jr = this->page()->mainFrame()->evaluateJavaScript(code);

    qCDebug(APEX_RS) << code;
    qCDebug(APEX_RS) << "Javascript result: " << jr;
}

QWidget* HtmlRunDelegate::getWidget()
{
    return this;
}

bool HtmlRunDelegate::hasText() const
{
    return true;
}

bool HtmlRunDelegate::hasInterestingText() const
{
    return true;
}

const QString HtmlRunDelegate::getText() const
{
    QVariant jr = this->page()->mainFrame()->evaluateJavaScript(
                "getResult();" );
    return jr.toString();
}

void HtmlRunDelegate::resizeEvent( QResizeEvent* e )
{
    QWebView::resizeEvent (e);
    //setFont (initialFont);

}

void HtmlRunDelegate::connectSlots( gui::ScreenRunDelegate* d )
{
    connect( this, SIGNAL( answered( ScreenElementRunDelegate* ) ),
             d, SIGNAL( answered( ScreenElementRunDelegate* ) ) );
}

}
}

void apex::rundelegates::HtmlRunDelegate::sendAnsweredSignal()
{
//    qCDebug(APEX_RS, "HtmlRunDelegate::sendAnsweredSignal()");
    Q_EMIT answered( this );
}

apex::rundelegates::HtmlRunDelegate::HtmlRunDelegate(
        ExperimentRunDelegate* p_rd,
    QWidget* parent, const HtmlElement* e ) :
      QWebView( parent ),
      ScreenElementRunDelegate(p_rd,e),
      element( e ),
      hasEnabledWaiting(false),
      hasFinishedLoading(false)
{
    setObjectName(element->getID());
    setCommonProperties(this);
    //    qCDebug(APEX_RS, "Creating Html with parent %p", parent);

    // Create API and expose
    api.reset( new HtmlAPI() );
    connect(api.data(), SIGNAL(answered()), this, SLOT(sendAnsweredSignal()));
    this->page()->mainFrame()->addToJavaScriptWindowObject( "api", api.data() );

    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    QUrl p_page = e->page();
    AccessManager* am = new AccessManager(this);
    page()->setNetworkAccessManager(am);
    qCDebug(APEX_RS) << "loading page " << p_page;
    connect(page(), SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));

    // The QUrl for the QWebFrame expects the correct number of slashes after the scheme. p_page doesn't have this but QUrl::fromUserInput can add them (important for Windows).
    page()->mainFrame()->load( am->prepare(p_page) );

    /*if ( !element->getShortCut().isEmpty() ) {
        QPushHtml::setShortcut( e->getShortCut() );
    }*/

    //connect( this, SIGNAL( clicked() ), this, SLOT( sendAnsweredSignal() ) );

    /*if (! e->getFGColor().isEmpty()) {
        QPalette p=palette();
        p.setColor(QPalette::Active, QPalette::HtmlText, QColor(e->getFGColor()));
        setPalette(p);
    }
    if (! e->getBGColor().isEmpty()) {
        QPalette p=palette();
        p.setColor(QPalette::Active, QPalette::Html, QColor(e->getBGColor()));
        setPalette(p);
    }

    QFont font = defaultFont;
    if ( element->getFontSize() != -1 )
        font.setPointSize( element->getFontSize() );
    QPushHtml::setFont( font );
    initialFont = font;

    // set maximum height to N*font height
    QFontMetrics fm( font );
    setMaximumHeight( fm.height()*5 );
    setMinimumHeight( fm.height());

    feedBack(ScreenElementRunDelegate::NoFeedback );*/
}


void apex::rundelegates::HtmlRunDelegate::feedBack
        (const FeedbackMode& )
{
    /*QColor newFGColor_enabled, newFGColor_disabled;
    QColor newBGColor_enabled, newBGColor_disabled;
    QPalette dp;        // default palette


    if ( mode == ScreenElementRunDelegate::NoFeedback )    {
        //setDown(false);
        setProperty("role", "none");
        setStyleSheet(styleSheet());
        if (! element->getFGColor().isEmpty()) {
            newFGColor_disabled=element->getFGColor();
            newFGColor_enabled=newFGColor_disabled;
        } else {
            newFGColor_disabled = dp.color(QPalette::Disabled,
                QPalette::HtmlText);
            newFGColor_enabled = dp.color(QPalette::Active,
                QPalette::HtmlText);
        }
        if (! element->getBGColor().isEmpty()) {
            newBGColor_disabled=element->getBGColor();
            newBGColor_enabled=newBGColor_disabled;
        } else {
            newBGColor_disabled = dp.color(QPalette::Disabled,
                                 QPalette::Html);
            newBGColor_enabled = dp.color(QPalette::Active,
                                           QPalette::Html);
        }

    } else if ( mode == ScreenElementRunDelegate::HighlightFeedback ) {
        setProperty("role", "highlight");
        setStyleSheet(styleSheet());
        newBGColor_disabled=QColor(0xff, 0xff, 0x90);      // light yellow
    } else if ( mode == ScreenElementRunDelegate::NegativeFeedback ) {
        setProperty("role", "negative");
        setStyleSheet(styleSheet());
        newBGColor_disabled=Qt::red;
    } else if ( mode == ScreenElementRunDelegate::PositiveFeedback ) {
        setProperty("role", "positive");
        setStyleSheet(styleSheet());
        newBGColor_disabled=Qt::darkGreen;
    }

    QPalette palette(this->palette());
    if (newBGColor_disabled.isValid())
        palette.setColor(QPalette::Disabled, QPalette::Html,
                         newBGColor_disabled);
    if (newBGColor_enabled.isValid())
        palette.setColor(QPalette::Active, QPalette::Html,
                         newBGColor_enabled);

    if (newFGColor_disabled.isValid())
        palette.setColor(QPalette::Disabled, QPalette::HtmlText,
                         newFGColor_disabled);
    if (newFGColor_enabled.isValid())
        palette.setColor(QPalette::Active, QPalette::HtmlText,
                         newFGColor_enabled);
    setPalette(palette);*/

}

