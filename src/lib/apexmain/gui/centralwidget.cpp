/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexdata/screen/screenelement.h"
#include "apextools/apextools.h"

#include "gui/guidefines.h"

#include "screen/screenelementrundelegate.h"

#include "streamapp/utils/dataconversion.h"

#include "centralwidget.h"

#include <QGuiApplication>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QPainter>
#include <QtWidgets>

using namespace apex;
using namespace apex::gui;
using namespace dataconversion;

ApexCentralWidget::ApexCentralWidget(QMainWindow *a_pParent)
    : QWidget(a_pParent),
      m_pPanel(0),
      m_pScreen(0),
      m_pMainLayout(0),
      m_ScreenBG(sc_DefaultBGColor),
      m_Good(Qt::green),
      m_Bad(Qt::red),
      m_High(Qt::white),
      m_pFeedBack(0),
      m_bFeedBackDirty(false)
{
    setObjectName("background");

    m_pMainLayout = new QHBoxLayout;
    setLayout(m_pMainLayout);
    m_pMainLayout->setMargin(0);

#ifdef Q_OS_ANDROID
    connect(QGuiApplication::primaryScreen(),
            SIGNAL(availableGeometryChanged(QRect)), this,
            SLOT(onAvailableGeometryChanged(QRect)));
#endif

    QPalette pal = palette();
    pal.setColor(backgroundRole(), sc_DefaultBGColor);
    setPalette(pal);

    setAutoFillBackground(true);
}

ApexCentralWidget::~ApexCentralWidget()
{
}

void ApexCentralWidget::mp_SetPanel(QWidget *a_pPanel)
{
    if (m_pPanel)
        m_pMainLayout->removeWidget(m_pPanel);
    m_pPanel = a_pPanel;
    if (m_pPanel)
        m_pMainLayout->addWidget(m_pPanel, 2);
}

void ApexCentralWidget::mp_SetScreen(QLayout *a_pScreen)
{
    if (m_pScreen)
        m_pMainLayout->removeItem(m_pScreen);
    m_pScreen = a_pScreen;
    if (m_pScreen)
        m_pMainLayout->addLayout(m_pScreen, 8);
}

void ApexCentralWidget::setScreenWidget(QWidget *w)
{
    w->setParent(this);
    m_pMainLayout->insertWidget(0, w, 8);

#if defined(Q_OS_ANDROID)
    w->showMaximized();
    QSize maximumSize = w->size();
    maximumSize.setHeight(w->maximumSize().height());
    w->setMaximumSize(maximumSize);
#endif
}

void ApexCentralWidget::mp_ClearContent()
{
    m_pPanel = 0;
    m_pScreen = 0;
    m_pFeedBack = 0;
}

void ApexCentralWidget::mp_SetFeedBackElement(
    ScreenElementRunDelegate *a_pElement)
{
    m_pFeedBack = a_pElement;
    m_bFeedBackDirty = true;
}

void ApexCentralWidget::mp_LayoutNow()
{
}

void ApexCentralWidget::paintEvent(QPaintEvent * /*e*/)
{
}

void ApexCentralWidget::setBackgroundColor(const QColor &ac_Color)
{
    QPalette pal = palette();
    pal.setColor(backgroundRole(), ac_Color);
    setPalette(pal);

    m_ScreenBG = ac_Color;
}

void ApexCentralWidget::onAvailableGeometryChanged(const QRect &geometry)
{
    /* Force horizontal 4/1 propertion on screen orientation change */
    if (m_pMainLayout->count() > 0) {
        QWidget *widget = m_pMainLayout->itemAt(0)->widget();
        if (widget) {
            QSize maximumSize = widget->maximumSize();
            if (m_pPanel) {
                maximumSize.setWidth(geometry.width() * 4 / 5);
                QSize panelSize = m_pPanel->maximumSize();
                panelSize.setWidth(geometry.width() * 1 / 5);
                m_pPanel->setMaximumSize(panelSize);
            } else {
                maximumSize.setWidth(geometry.width());
            }
            widget->setMaximumSize(maximumSize);
        }
    }
}
