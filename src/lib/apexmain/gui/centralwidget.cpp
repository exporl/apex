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
 
#include "screen/screenelement.h"
#include "screen/screenelementrundelegate.h"

#include "centralwidget.h"
#include "gui/guidefines.h"

#include <utils/dataconversion.h>

#include <assert.h>

#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QPainter>

using namespace apex;
using namespace apex::gui;
using namespace dataconversion;

ApexCentralWidget::ApexCentralWidget( QMainWindow* a_pParent ) :
        QWidget( a_pParent ),
        m_pPanel( 0 ),
        m_pScreen( 0 ),
        m_pMainLayout(0),
        m_pMainWindow(a_pParent),
        m_ScreenBG( sc_DefaultBGColor ),
        m_Good( Qt::green ),
        m_Bad( Qt::red ),
        m_High( Qt::white ),
        m_pFeedBack( 0 ),
        m_bFeedBackDirty( false )
{
    setObjectName("background");

    m_pMainLayout = new QGridLayout(this);
    //a_pParent->setLayout(m_pMainLayout);
    // set size to maximum size
    m_pMainLayout->setRowStretch(0,1);
    m_pMainLayout->setMargin(0);

    QPalette pal = palette();
    pal.setColor (backgroundRole(), sc_DefaultBGColor);
    setPalette (pal);

    setAutoFillBackground( true );

}

ApexCentralWidget::~ApexCentralWidget()
{
}

void ApexCentralWidget::mp_SetPanel( QWidget* a_pPanel )
{

    if ( m_pPanel )
        m_pMainLayout->removeWidget( m_pPanel );
    m_pPanel = a_pPanel;
    if ( m_pPanel )
    {
//   m_pPanel->setParent( m_pMainLayout);
        m_pMainLayout->addWidget( m_pPanel, 0,1);
        m_pMainLayout->setColumnStretch(1, 2);
        m_pMainLayout->setColumnStretch(0, 8);

        //m_pPanel->setMinimumHeight( this->height());
    }
    if ( m_pScreen )
    {

        /* tItemLayout* p = m_pLayout->mf_pGetLayoutFor( m_pScreen );
        if( m_pPanel )
        p->m_dXsiz = 0.8;
        else
        p->m_dXsiz = 1.0;*/
    }
}

void ApexCentralWidget::mp_SetScreen( QLayout* a_pScreen )
{
    if ( m_pScreen )
        m_pMainLayout->removeItem( m_pScreen );
    m_pScreen = a_pScreen;
    if ( m_pScreen )
    {
        if ( m_pPanel )
        {
            m_pMainLayout->addLayout( m_pScreen, 0, 0);
        }
        else
            m_pMainLayout->addLayout( m_pScreen, 0, 0);

        /* int tc=m_pMainLayout->columnCount();
        int tr=m_pMainLayout->rowCount();*/
    }
}

void ApexCentralWidget::setScreenWidget(QWidget* w)
{
//    qDebug("ApexCentralWidget::setScreenWidget(%p)", w);
    w->setParent(this);
    m_pMainLayout->addWidget(w,0,0);
}

void ApexCentralWidget::mp_ClearContent()
{
    m_pPanel = 0;
    m_pScreen = 0;
    m_pFeedBack = 0;
}

void ApexCentralWidget::mp_SetFeedBackElement( ScreenElementRunDelegate* a_pElement )
{
    m_pFeedBack = a_pElement;
    m_bFeedBackDirty = true;
}

void ApexCentralWidget::mp_LayoutNow()
{
}

void ApexCentralWidget::paintEvent( QPaintEvent* /*e*/ )
{
    //draw feedback if needed
    if ( m_bFeedBackDirty && m_pFeedBack )
    {
        QPainter painting( this );
        QPen pen;
        assert( m_pFeedBack->getWidget() );
        const QRect& rex( m_pFeedBack->getWidget()->geometry() );
        const ScreenElementRunDelegate::mt_eFeedBackMode& eMode =
            m_pFeedBack->getFeedBackMode();

        if ( eMode == ScreenElementRunDelegate::mc_eOff )
        {
            pen.setColor( m_ScreenBG );
            m_bFeedBackDirty = false;
        }
        else if ( eMode == ScreenElementRunDelegate::mc_eHighLight )
        {
            pen.setColor( m_High );
            m_bFeedBackDirty = true;
        }
        else if ( eMode == ScreenElementRunDelegate::mc_eNegative )
        {
            pen.setColor( m_Bad );
            m_bFeedBackDirty = true;
        }
        else if ( eMode == ScreenElementRunDelegate::mc_ePositive )
        {
            pen.setColor( m_Good );
            m_bFeedBackDirty = true;
        }

        pen.setWidth( 2 );
        painting.setPen( pen );
        painting.drawRect( rex.x() - 5, rex.y() - 5, rex.width() + 10, rex.height() + 10 );
    }
}

void ApexCentralWidget::setBackgroundColor( const QColor& ac_Color )
{
    QPalette pal = palette();
    pal.setColor (backgroundRole(), ac_Color);
    setPalette (pal);

    m_ScreenBG = ac_Color;
}
