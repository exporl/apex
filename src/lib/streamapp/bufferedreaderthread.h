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

#ifndef __STR_BUFFEREDREADERTHREAD_H__
#define __STR_BUFFEREDREADERTHREAD_H__

#include "containers/dynarray.h"
#include "appcore/threads/locks.h"
#include "appcore/threads/thread.h"
#include "appcore/managedsingleton.h"

namespace streamapp
{

  class BufferedReader;


    /**
      * BufferedReaderThread
      *   a thread for filling BufferReader's buffers.
      ************************************************ */
  class BufferedReaderThread : public appcore::IThread
  {
  public:
      /**
        * Constructor.
        * Starts thread with low priority. FIXME?
        */
    BufferedReaderThread();

      /**
        * Destructor.
        */
    ~BufferedReaderThread();

      /**
        * Add a reader to fill the buffer for.
        * @param a_pReader a pointer, won't be touched
        */
    void mp_AddReader( BufferedReader* a_pReader );

      /**
        * Remove a reader.
        * @param a_pReader a pointer
        */
    void mp_RemoveReader( BufferedReader* a_pReader );

      /**
        * Set the idle time.
        * @param ac_nTimeout timeout in milliseconds
        */
    void mp_SetTimeout( const int ac_nTimeout = 15 );

  private:
    typedef DynamicArray< BufferedReader* > mt_Readers;

      /**
        * Implementation of the IThread method.
        */
    void mp_Run();

    int         mv_nInterval;
    mt_Readers  m_Readers;
    const appcore::CriticalSection mc_Lock;
  };


    /**
      * theBufferedReaderThread
      *   singleton used when auto-registering readers.
      */
  s_make_safe_singleton( BufferedReaderThread, theBufferedReaderThread )

}

#endif //#ifndef __STR_BUFFEREDREADERTHREAD_H__
