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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_CENTRALWIDGET_H_
#define _EXPORL_SRC_LIB_APEXMAIN_GUI_CENTRALWIDGET_H_

#include <QColor>
#include <QHBoxLayout>
#include <QRect>
#include <QWidget>

class QLayout;
class QMainWindow;

namespace apex
{
namespace data
{
class ScreenElement;
}
namespace rundelegates
{
class ScreenElementRunDelegate;
}

namespace gui
{
using rundelegates::ScreenElementRunDelegate;

/**
 * ApexCentralWidget
 *   the central widget of the main window
 *   takes care of painting the main window's content
 *   and it's layout
 **************************************************** */
class ApexCentralWidget : public QWidget
{
    Q_OBJECT
public:
    /**
      * Constructor.
      * @param a_pParent the parent
      */
    ApexCentralWidget(QMainWindow *a_pParent);

    /**
      * Destructor.
      */
    ~ApexCentralWidget();

    /**
      * Set the panel.
      * Panel gets 20% of the size, at the right side.
      * @param a_pPanel the panel widget
      */
    void mp_SetPanel(QWidget *a_pPanel);

    /**
      * Set the screen.
      * Screen gets 80% of the size, at the left side,
      * or 100% if no panel was set.
      * @param a_pScreen the layout containing created widgets or 0
      */
    void mp_SetScreen(QLayout *a_pScreen);
    void setScreenWidget(QWidget *w);

    /**
      * Set both panel and screen to 0.
      */
    void mp_ClearContent();

    /**
      * Apply layout.
      * Use this after changing elements if the layout needs to be redrawn
     * immediately.
      */
    void mp_LayoutNow();

    /**
      * Override settig background color to know what to draw for feedback ==
     * mc_eOff
      * @param ac_Color the color
      */
    void setBackgroundColor(const QColor &ac_Color);

    /**
      * Set the positive feedback color.
      * @param ac_Color the color
      */
    void mp_SetGoodColor(const QColor &ac_Color)
    {
        m_Good = ac_Color;
    }

    /**
      * Set the negative feedback color.
      * @param ac_Color the color
      */
    void mp_SetBadColor(const QColor &ac_Color)
    {
        m_Bad = ac_Color;
    }

    /**
      * Set the highlight feedback.
      * @param ac_Color the color
      */
    void mp_SetHighColor(const QColor &ac_Color)
    {
        m_High = ac_Color;
    }

    /**
     * Set the current feedback element
     * @param el the element
     */
    void mp_SetFeedBackElement(ScreenElementRunDelegate *el);

protected:
    /**
      * Override the painting to allow drawing feedback.
      * @param e the paint event
      */
    void paintEvent(QPaintEvent *e);

public Q_SLOTS:
    void onAvailableGeometryChanged(const QRect &geometry);

private:
    QWidget *m_pPanel;
    QLayout *m_pScreen;
    QHBoxLayout *m_pMainLayout;
    QColor m_ScreenBG;
    QColor m_Good;
    QColor m_Bad;
    QColor m_High;
    ScreenElementRunDelegate *m_pFeedBack;
    bool m_bFeedBackDirty;
};
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_CENTRALWIDGET_H_
