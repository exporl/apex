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

#ifndef FLASHPLAYERWIDGET_H
#define FLASHPLAYERWIDGET_H

#ifdef WIN32

#include <QPushButton>

class CFlashWnd;

namespace ShockwaveFlashObjects
{
  struct IShockwaveFlash;
}

  /**
    * FlashPlayer Widget
    *   embeds Shockwave Flash into a QPushButton.
    *   Reason for choosing QPushButton instead of QWidget
    *   is to receive keyboard shortcus.
    ****************************************************** */
class FlashPlayerWidget : public QPushButton
{
  Q_OBJECT
public:
    /**
      * Constructor.
      * @param a_pParent the parent widget
      */
  FlashPlayerWidget( QWidget* a_pParent );

    /**
      * Destructor.
      */
  virtual ~FlashPlayerWidget();

    /**
      * Try to create the flash container.
      * @return false if flash is not available
      */
  bool mf_bCreateFlashObject();

    /**
      * Set a new movie.
      * @param ac_sPath fully qualified path
      */
  bool mp_bSetMovie( const QString& ac_sPath );

    /**
      * Start the movie and run to it's end.
      */
  void mp_Start();

    /**
      * Stop the movie.
      */
  void mp_Stop();

    /**
      * Jump to a specific frame.
      * @param the frame
      */
  void mp_GoToFrame( const long ac_nFrameNum );

    /**
      * Check if the movie finished playing.
      * Note that this also returns true if it hasn't
      * started yet!
      * @return true if finished
      */
  bool mf_bFinished() const;

    /**
      * Used to poll the widget.
      * @see mf_bFinished
      * @param a_pWidget the widget to check
      */
  static bool smf_bFinished( FlashPlayerWidget* a_pWidget )
  {
    return a_pWidget->mf_bFinished();
  }

    /**
      * Check if the flash element is properly initialized.
      * @return false if no flash avaliable
      */
  bool mf_bCreated() const
  {
    return mv_bCreated;
  }

    /**
      * Allow the ms_FlashClicked signal.
      * @param ac_bAllow true to allow emitting clicks
      */
  void mp_AllowEmitClicks( const bool ac_bAllow )
  {
    mv_bEmit = ac_bAllow;
  }

    /**
      * Get the CWnd container.
      * @return a pointer
      
  CMovieWnd* mp_GetCWnd() const
  {
    return m_pMovie;
  }*/

    /**
      * Override for proper resizing.
      */
  void resizeEvent( QResizeEvent* );

    /**
      * Override to catch mouse clicks
      * on the flash element.
      */
  bool winEvent( MSG*, long* );

signals:
    /**
      * Emitted upon left mouse click,
      * unless mp_AllowEmitClicks was set false.
      */
  void ms_FlashClicked();

private:
  double GetAspectRatio();

    /**
      * Get the ShockWave Flash container.
      * @return a pointer
      */
  ShockwaveFlashObjects::IShockwaveFlash* mp_GetFlash() const;

  void mp_AdjustFlashColour();

  CFlashWnd* m_pMovie;
  bool       mv_bEmit;
  bool       mv_bCreated;
};

#endif
#endif
