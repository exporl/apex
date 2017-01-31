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
 
#ifndef __BUFFERDROPCHECK_H__
#define __BUFFERDROPCHECK_H__

#include "utils/timer.h"
#include "callback/callback.h"
#include "appcore/threads/criticalsection.h"
using namespace appcore;

namespace streamapp
{

  /**

      *   CHECKME something weird: should be done using a CriticalSection:
      *   if it cannot be entered when mf_Callback() is called, it means
      *   mf_Callback() is still in use, which clearly indicates a buffer underrun.
      *   Doesn't work with portaudio however, it seems there is a double lock somewhere..

  **/

    /**
      * BufferDropCheck
      *   callback for checking bufferdrops.
      *   Uses a very simple technique: if the time needed
      *   for the callback is larger then the period set,
      *   a bufferdrop happens.
      ***************************************************** */
  class BufferDropCheck : public Callback
  {
  public:
      /**
        * Constructor.
        * @param ac_bDeleteSlave if true, deletes the callback(s) set by mp_InstallCallback
        */
    BufferDropCheck( const bool ac_bDeleteSlave );

      /**
        * Destructor.
        */
    ~BufferDropCheck();

      /**
        * The callback.
        */
    void mf_Callback();

      /**
        * Set the maximum period the main callback can process.
        * For soundcards this is buffersize divided by samplerate.
        * @param ac_dTimeInSec the time in seconds
        */
    void mp_SetMaxCallbackTime( const double ac_dTimeInSec );

      /**
        * Set the callback to be called in mf_Callback()
        * @param a_pCallback the callback or 0 for none
        */
    void mp_InstallMainCallback( Callback* a_pCallback );

      /**
        * Get the current slave callback.
        * @return the callback
        */
    Callback* mf_pGetMainCallback();

      /**
        * Set the callback to be called in mf_Callback()
        * when a bufferdrop occurs
        * @param a_pCallback the callback or 0 for none
        */
    void mp_InstallNoLockCallback( Callback* a_pCallback );

      /**
        * Get the current no lock callback.
        * @return the callback
        */
    Callback* mf_pGetNoLockCallback();

      /**
        * Print drops to std::out.
        * @param ac_bSet true for print
        */
    void mp_SetPrintToStdOut( const bool ac_bSet = true );

      /**
        * Get printing status.
        * @return true if currently printing to std::out
        */
    const bool& mf_bGetPrintToStdOut() const;

  private:
    Callback*             mv_pDlock;
    Callback*             mv_pNlock;
    bool                  mv_bPrint;
    double                mv_dTime;
    ITimer* const         mc_pTimer;
    const bool            mc_bDelete;
    const CriticalSection mc_Lock;
  };

}

#endif //#ifndef __BUFFERDROPCHECK_H__
