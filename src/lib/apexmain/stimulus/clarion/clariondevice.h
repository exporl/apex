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

#ifndef __CLARIONDEVICE_H__
#define __CLARIONDEVICE_H__

#include "xml/xmlkeys.h"
#include "stimulus/filter.h"
#include "stimulus/outputdevice.h"

namespace clarion
{
  class IClarionWrapper;
}

namespace apex
{
  namespace data {
    class ClarionDeviceData;
  }

  namespace stimulus
  {
    using clarion::IClarionWrapper;

      /**
        * Clarion
        *   apex Clarion Device implementation.
        *************************************** */
    class ClarionDevice : public OutputDevice
    {
    public:
      ClarionDevice( data::ClarionDeviceData*   a_Parameters );
      ~ClarionDevice();

      void AddFilter    ( const Filter&     ac_Filter     );
      void AddInput     ( const DataBlock&  ac_DataBlock  );
      void RemoveAll    (                                 );

      void PlayAll      ();
      void StopAll      ();
      bool AllDone      ();

      //bool HasParameter( const QString& ac_ParamID );

      bool CanConnect() const
      { return false; }

        /**
          * @note Clarion can't do sequence, but we return true anyway because
          * mode 'normal' isn't implemented in StimulusOutput ;-
          * FIXME
          */
      bool CanSequence() const
      { return true; }

      void SetSequence( const DataBlockMatrix* ac_pSequence );

      const QString& GetModule() const
      { return XMLKeys::sc_sClarionDevice; }

       virtual bool SetParameter ( const QString& type, const int channel, const QVariant& value );

        /**
          * Reset the filter to its initial state and set all internal parameters to built in
          * default values
          * throw exception if problem
          */
      virtual void Reset();

        /**
          * Prepare filter for processing
          * throw exception if problem
          */
      virtual void Prepare();

      virtual void SetSilenceBefore (double time)
      {
          Q_UNUSED (time);
          qFatal("Not implemented");
      };

      virtual void AddFilter (Filter& a_Filter)
      {
          Q_UNUSED (a_Filter);
          qFatal("Not implemented");
      };

    private:
      void LoadPlugin();
      void mp_CheckParameters();
      //void mp_SetAllParameters();

      bool                            mv_bErrorOccured;
      QString                         mv_sCurrentFile;
      data::ClarionDeviceData*        m_Params;
      std::auto_ptr<IClarionWrapper>  m_pClarion;
    };

  }
}

#endif //#ifndef __CLARIONDEVICE_H__
