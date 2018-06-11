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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_PANEL_H_
#define _EXPORL_SRC_LIB_APEXMAIN_GUI_PANEL_H_

#include "screen/screenelementrundelegate.h"

#include <QWidget>

#include <memory>

class QObject;
class QString;
class QPushButton;
class FlashWidget;
class QVBoxLayout;

namespace apex
{
namespace data
{
class ScreensData;
}

namespace rundelegates
{
class ScreenElementRunDelegate;
}

struct tPanelConfig;

namespace gui
{

class ApexProgressBar;
class ApexStatusWindow;
class ApexFeedbackPicture;
class StatusPicture;
class PanelQProxy;

using rundelegates::ScreenElementRunDelegate;

/**
  * PanelQProxy
  *   the QObjectProxy for the Panel. Emits signals from the buttons.
  */
class PanelQProxy : public QObject
{
    Q_OBJECT

signals:
    void ms_Start();
    void ms_Stop();
    void ms_Pause();
    void ms_Panic();
    void ms_Repeat();

public:
    ~PanelQProxy()
    {
    }

    friend class IPanel; //! only allow creation through Panel

private:
    PanelQProxy()
    {
    }
};

/**
  * IPanel
  *   panel interface
  ******************* */
class IPanel
{
    const QScopedPointer<PanelQProxy> objectProxy;

protected:
    /**
      * Constructor.
      */
    IPanel() : objectProxy(new PanelQProxy())
    {
    }

public:
    /**
      * Destructor.
      */
    virtual ~IPanel()
    {
    }

    // get access to the signal slot proxy
    PanelQProxy *getSignalSlotProxy();

    /**
      * Show/Hide the entire panel.
      * @param ac_bVal true for show
      */
    virtual void mp_Show(const bool ac_bVal) = 0;

    /**
     * Show feedback to the user, corresponding to the mode
     * specified by mode.
     */
    virtual void
    feedBack(const ScreenElementRunDelegate::FeedbackMode mode) = 0;

    /**
      * Set the config.
      * @param ac_Config the configuration
      * @return true when succeeded
      */
    virtual bool mp_bSetConfig(const data::ScreensData &ac_Config) = 0;

    /**
      * Set the progressbar position.
      * @param ac_nProgress the progress
      */
    virtual void mp_SetProgress(const unsigned ac_nProgress) = 0;

    /**
      * Set the progressbar total.
      * @param ac_nProgress the number of steps
      */
    virtual void mp_SetProgressSteps(const unsigned ac_nProgress) = 0;

    /**
      * Set the text.
      * @param ac_sText text string
      */
    virtual void mp_SetText(const QString &ac_sText) = 0;

    /**
      * Enable/Disable the buttons, if any
      * @param ac_bVal false for disable
      */
    virtual void mp_EnableStart(const bool)
    {
    }
    virtual void mp_EnableStop(const bool)
    {
    }
    virtual void mp_EnablePause(const bool)
    {
    }
    virtual void mp_EnableEmergency(const bool)
    {
    }
    virtual void mp_EnableRepeat(const bool)
    {
    }
};

/**
  * Panel
  *   the default panel on the right hand side of the screen.
  *   Has start/stop/pause/status/feedback/emergency/progress.
  *   Derives form ScreenElement, but with constraints: the signal ms_Answered
  *is not
  *   used and neither is the FeedBackMode mc_eHighLight.
  ************************************************************************************
  */
class Panel : public QWidget, public IPanel
{
public:
    /**
      * Constructor.
      * @param a_pParent the parent widget
      */
    Panel(QWidget *a_pParent, const data::ScreensData *c);

    /**
      * Destructor.
      */
    ~Panel();

    /**
      * Creates the content.
      * @param ac_Config the configuration
      * @return true on success
      */
    bool mp_bSetConfig(const data::ScreensData &ac_Config);

    /**
      * Enable/Disable the buttons.
      * @param ac_bVal false for disable
      */
    void mp_EnableStart(const bool ac_bVal);
    void mp_EnableStop(const bool ac_bVal);
    void mp_EnablePause(const bool ac_bVal);
    void mp_EnableEmergency(const bool ac_bVal);
    void mp_EnableRepeat(const bool ac_bVal);
    void mp_EnableStatusPicture(const bool ac_bVal);

    /**
      * Toggle the pause button's text between Pause/Continue
      */
    void mp_Paused();

    void setPaused(bool paused);

    /**
      * Show/Hide the entire panel.
      * @param ac_bVal true for show
      */
    void mp_Show(const bool ac_bVal);

    /**
      * Acquire the Panel's widget.
      * @return the QWidget
      */
    QWidget *getWidget()
    {
        return this;
    }

    /**
      * FeedBack.
      * For normal mode: a picture.
      * For child mode: one frame forward or backward in the movie.
      * @param ac_eMode only positive/negative and off are supported
      */
    void feedBack(const ScreenElementRunDelegate::FeedbackMode ac_eMode);

    /**
      * Set the progressbar position.
      * @param ac_nProgress the progress
      */
    void mp_SetProgress(const unsigned ac_nProgress);

    /**
      * Set the progressbar total.
      * @param ac_nProgress the number of steps
      */
    void mp_SetProgressSteps(const unsigned ac_nProgress);

    /**
      * Has text in status window.
      * @return true since we have it
      */
    virtual bool mf_bHasText() const
    {
        return true;
    }

    /**
      * Get the text.
      * @return a reference to the text string
      */
    QString mf_sGetText() const;

    /**
      * Set the text.
      * @param ac_sText text string
      */
    void mp_SetText(const QString &ac_sText);

private:
    void mp_Layout();
    const data::ScreensData *m_pMainWindowConfig;

    QPushButton *m_pStart;
    QPushButton *m_pStop;
    QPushButton *m_pPause;
    QPushButton *m_pEmergency;
    QPushButton *m_pRepeat; //! repeat stimulus button
    ApexProgressBar *m_pProgressBar;
    ApexStatusWindow *m_pStatusWindow;
    ApexFeedbackPicture *m_pFeedbackPicture;
    StatusPicture *m_pStatusPicture;
    bool showEmergency;
    bool showProgress;
    bool showRepeat;

    QGridLayout *m_pLayout;
};

#ifdef FLASH
/**
  * ChildModePanel
  *   the childmode panel.
  *   Has a flash movie
  *   Derives form ScreenElement, but with constraints: the signal ms_Answered
  *is not
  *   used and neither is the FeedBackMode HighlightFeedback.
  ************************************************************************************
  */
class ChildModePanel : public QWidget, public IPanel
{
public:
    /**
      * Constructor.
      * @param a_pParent the parent widget
      */
    ChildModePanel(QWidget *a_pParent);

    /**
      * Destructor.
      */
    ~ChildModePanel();

    /**
      * Creates the content.
      * @param ac_Config the configuration
      * @return true on success
      */
    bool mp_bSetConfig(const data::ScreensData &ac_Config);

    /**
     * Show/Hide the entire panel.
     * @param ac_bVal true for show
     */
    void mp_Show(const bool ac_bVal);

    /**
      * Acquire the Panel's widget.
      * @return the QWidget
      */
    QWidget *mf_pGetElement() const
    {
        return (QWidget *)this;
    }

    /**
      * FeedBack.
      * For normal mode: a picture.
      * For child mode: one frame forward or backward in the movie.
      * @param ac_eMode only positive/negative and off are supported
      */
    void feedBack(const ScreenElementRunDelegate::FeedbackMode ac_eMode);

    /**
      * Set the progressbar position.
      * @param ac_nProgress the progress
      */
    void mp_SetProgress(const unsigned ac_nProgress);

    /**
      * Set the progressbar total.
      * @param ac_nProgress the number of steps
      */
    void mp_SetProgressSteps(const unsigned ac_nProgress);

    /**
      * Has text in status window.
      * @return false since we don't have it
      */
    virtual bool mf_bHasText() const
    {
        return false;
    }

    void mp_SetText(const QString & /*ac_sText*/)
    {
    }

private:
    FlashWidget *m_pMovie;
    ApexProgressBar *m_pProgressBar;
    bool m_bShowProgressBar;
    unsigned long m_nCurrentProgressFrame;
    QVBoxLayout *m_pLayout;
};
#endif //#ifdef FLASH
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_PANEL_H_
