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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_PANELELEMENTS_H_
#define _EXPORL_SRC_LIB_APEXMAIN_GUI_PANELELEMENTS_H_

#include "screen/screenelementrundelegate.h"

#include "guidefines.h"

#include <QLabel>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>

namespace apex
{
namespace gui
{
using rundelegates::ScreenElementRunDelegate;
/**
  * ApexProgressBar.
  * a usable QProgressBar implementation
  ************************************** */
class ApexProgressBar : public QProgressBar
{
public:
    /**
      * Constructor.
      * @param a_pParent the parent widget
      */
    ApexProgressBar(QWidget *a_pParent) : QProgressBar(a_pParent)
    {
        QProgressBar::reset();
    }

    /**
      * Destructor.
      */
    ~ApexProgressBar()
    {
    }

    /**
      * Reset the progress.
      */
    void mp_Reset()
    {
        QProgressBar::reset();
    }

    /**
      * Set frameless.
      */
    void mp_SetNoFrame()
    {
        QProgressBar::setWindowFlags(Qt::FramelessWindowHint);
    }

protected:
    /**
      * Override the indicator painting.
      */
    virtual bool setIndicator(QString &indicator, int progress, int totalSteps)
    {
        if (progress == 0 && totalSteps == 0)
            indicator = "";
        else
            indicator =
                QString::number(progress) + " / " + QString::number(totalSteps);
        return true; // repaint immedeately
    }
};

/**
  * ApexStatusWindow
  */
class ApexStatusWindow : public QListWidget
{
public:
    /**
      * Constructor.
      * @param a_pParent the parent widget
      */
    ApexStatusWindow(QWidget *a_pParent) : QListWidget(a_pParent)
    {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        setSelectionMode(QAbstractItemView::NoSelection);

        QFont f(QListWidget::font());
        f.setFamily(sc_sDefaultFont);
        f.setBold(false);
        f.setPointSize(8);
        setFont(f);
        QFontMetrics fm(f);
        setMinimumHeight(fm.lineSpacing() * 3);
        setMaximumHeight(fm.lineSpacing() * 3);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    /**
      * Destructor.
      */
    ~ApexStatusWindow()
    {
    }

    /**
      * Add a message at the top of the list.
      * @param ac_sMessage text string
      */
    void mp_AddMessage(const QString &ac_sMessage)
    {
        addItem(ac_sMessage);
        setCurrentRow(count() - 1);
        //        ensureCurrentVisible();
    }
};

/**
  * ApexFeedbackPicture
  */
class ApexFeedbackPicture : public QLabel
{
    QPixmap *okPixmap;
    QPixmap *nokPixmap;

public:
    /**
      * Constructor.
      * @param a_pParent the parent widget
     * @param okFilename filename of the file to be shown for positive
     * feedback. If empty the default will be used.
     * @param nOkFilename idem for negative feedbadck
      */
    ApexFeedbackPicture(QWidget *a_pParent, QString okFilename,
                        QString nOkFilename);

    /**
      * Destructor.
      */
    ~ApexFeedbackPicture();

    void feedBack(const ScreenElementRunDelegate::FeedbackMode mode);
};

/**
 * StatusPicture
 */
class StatusPicture : public QLabel
{
public:
    enum status {
        STATUS_ANSWERING,
        STATUS_LISTENING,
        STATUS_WAITING_FOR_START
    };

private:
    QPixmap *answeringPixmap;
    QPixmap *listeningPixmap;
    QPixmap *waitingForStartPixmap;

public:
    StatusPicture(QWidget *a_pParent, QString answeringFilename,
                  QString listeningFilename, QString waitingForStartFilename);

    /**
         * Destructor.
     */
    ~StatusPicture();

    void setStatus(const status mode);
};
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_PANELELEMENTS_H_
