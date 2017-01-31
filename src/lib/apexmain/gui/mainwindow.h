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

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include "gui/apexmainwndbase.h"
//#include "gui/mainwindowconfig.h"

#include "screen/screenrundelegate.h"
#include "screen/screenelementrundelegate.h"

#include <random.h>

#include <QScopedPointer>

class QWidget;
class QString;

namespace apex
{
class ScreenStatusReporter;
class ConsoleStatusReporter;
    namespace rundelegates
    {
        class ScreenElementRunDelegate;
    }
    namespace data {
        class ScreensData;
    }

  class MRU;
  class ApexControl;
  class ScreenResult;

  namespace gui
  {

    class IPanel;
    class ApexCentralWidget;
    class ScreenRunDelegate;
    using rundelegates::ScreenElementRunDelegate;

      /**
        * ApexMainWindow
        *   the main application window.
        ******************************** */
        class ApexMainWindow : public ApexMainWndBase
    {
      Q_OBJECT

      //friend class apex::ApexControl;   //!allow all access
    public:
      /**
       * Constructor.
       * @param a_pParent the parent widget
       */
        ApexMainWindow( QWidget* a_pParent=0);

        void setExperimentRunDelegate(ExperimentRunDelegate* p_rd);

      /**
       * Destructor.
       */
      ~ApexMainWindow();

    public:
      /**
       * Setting the state.
       */
      void Paused();
      void Initted();
      void Started();
      void Stopped();
      void Finished();
      void ExperimentLoaded();

      /**
       * Clear the screen.
       */
      void ClearScreen();

      /**
       * Clear the screen and the panel.
       */
      void Clear();

      /**
       * Mru.
       */
      MRU* GetMru();
      const QString& GetOpenDir() const;
      void MruLoad( const QString& ac_sIniFile );
      void MruSave( const QString& ac_sIniFile );
      void MruFile( const QString& ac_sFile );
      void SetOpenDir( const QString& ac_sDir );
      void MruClear();

      /**
       * Set the icon.
       * @param ac_sIcon the absolute icon path
       */
      void SetIcon( const QString& ac_sIcon );

      /**
       * Applies the configuration for the currently loaded experiment.
       * This sets the color and the panel.
       */
      void ApplyConfig(const data::ScreensData* const c);

        /**
          * this is quite a hack: we suppose child mode always uses flash movies;
          * once a movie is clicked, the keyboard input events disappear.. they are not received by qt,
          * nor by FlashPlayerWidget::winEvent, nor by FlashPlayerWidget::m_pMovie::OnKeyDown, nor by
          * FlashPlayerWidget::m_pMovie::m_MovieOnKeyDown... They're just gone. Here we give all focus
          *  back to qt, so both the menu shortcuts and movie shortcuts can be used again.
          */
      void ReclaimFocus();

      QString fetchVersion() const;
      QString fetchDiffstat() const;

    public slots:
      /**
       * Set a new screen.
       * Sets the new content and shows it.
       * Does nothing if ac_pScreen is the same as the current screen.
       * @param ac_pScreen the screen
       */
      void SetScreen( gui::ScreenRunDelegate* ac_Screen );

      /**
       * Enable/Disable all elements in screen
       * @param ac_bEnable false for disable
       */
      void EnableScreen( const bool ac_bEnable = true );

      /**
       * Show feedback on the element with ID ac_sID,
       * and in the panel if there is one (panel only does positive/negative)
       * @param ac_eMode the kind of feedback
       * @param ac_sID the element to put feedback on
       */
      void FeedBack( const ScreenElementRunDelegate::FeedbackMode& ac_eMode, const QString& ac_sID );

      /**
       * Same as FeedBack( ScreenElement::HighlightFeedback, ac_sID ).
       * @param ac_sID the element to highlight
       */
      void HighLight( const QString& ac_sID );

      /**
       * Stop highlighting the element.
       */
      void EndHighLight();

      /**
       * Stop showing feedback.
       * Also clears textcontent from elements.
       */
      void EndFeedBack();

      /**
       * Display a message in the panel.
       * @param ac_sMessage the text string
       */
      void AddStatusMessage( const QString& ac_sMessage );

      /**
       * Set the max number of steps for the progressbar, if any.
       * @param ac_nTrials the number of trials
       */
      void SetNumTrials( const unsigned ac_nTrials );

      /**
       * Set the current step the progressbar, if any.
       * @param ac_nTrial the current trial
       */
      void SetProgress( const unsigned ac_nTrial );

      /**
       * Enable the stop button in the panel
       */
      void EnableStop       ( const bool ac_bEnable = true );

      /**
       * Enable the repeat button in the panel
       */
      void EnableRepeat       ( const bool ac_bEnable = true );

        /**
          * Enable Skip ( F7 )
          */
      void EnableSkip( const bool ac_bEnable = true );

        /**
          * Enable AutoAnswer
          */
      void EnableAutoAnswer( const bool ac_bEnable = true );

      QWidget* centralWidget();

      void updateStatusReporting();

    signals:
     void Answered( const ScreenResult* a_Answer );
       //!NOTE see ApexMainWndBase for all other signals
     void StopOutput();
     void RepeatOutput();


        public slots:
        //!help
      void helpContents();
      void helpDeletePluginCache();
      void helpShowPluginDialog();
      void helpEditApexconfig();
      void helpAbout();

        //!menu/panel enablers
      void EnableStart      ( const bool ac_bEnable = true );
      void EnablePause      ( const bool ac_bEnable = true );
      void EnableSave       ( const bool ac_bEnable = true );
      void EnableOpen       ( const bool ac_bEnable = true );
      void EnableExperiment ( const bool ac_bEnable = true );
      void EnableCalibration( const bool ac_bEnable = true );

        /**
          * Response on an element
         * Create a ScreenResult object and fill it with data from the
         * current screen
          * @param ac_Element the element.
          */
      void AnswerFromElement( ScreenElementRunDelegate* );

      void doDeterministicAutoAnswer();

    protected:
        /**
          * Override this to do appropriate actions
          */
      void closeEvent( QCloseEvent* event );

    private:
        ExperimentRunDelegate* m_rd;
        const data::ScreensData*        m_config;

      QScopedPointer<MRU>                    m_pMRU;
      QScopedPointer<IPanel>                 m_pPanel;
      QScopedPointer<ApexCentralWidget>      m_pCentral;

      QString m_sSavefileName;
      ScreenRunDelegate* runningScreen;
      ScreenElementRunDelegate* m_pCurFeedback;

      QScopedPointer<ConsoleStatusReporter> consoleStatus;
      QScopedPointer<ScreenStatusReporter> screenStatus;

      Random randomGenerator;
    };

  }
}

#endif //#ifndef __MAINWINDOW_H__
