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

#ifndef __WAVDEVICE_H__
#define __WAVDEVICE_H__

#include "stimulus/outputdevice.h"
#include "wavdeviceio.h"
#include "apextypedefs.h"
#include "xml/xmlkeys.h"
#include "connection/connection.h"

namespace apex {

namespace data {

class WavDeviceData;
}

namespace stimulus {

class WavFilter;

class ApexSeqStream;

typedef std::map<std::string, ApexSeqStream*> tSeqMap;
typedef std::map<std::string, StreamGenerator*> tGenMap;
typedef std::vector< WavFilter* > tWavFilters;


/**
  * WavDevice
  *   Device implementation for playing wavefiles,
  *   or generated stimuli.
  ************************************************ */

class WavDevice : public OutputDevice {
    Q_DECLARE_TR_FUNCTIONS(WavDevice);
    public:
        /**
          * Constructor.
          */
        WavDevice( data::WavDeviceData* p_data );       // FIXME make const

        /**
          * Destructor.
          */
        virtual ~WavDevice();

        /**
          * Implementation of the Device method.
          */
        void AddFilter( Filter& a_Filter );

        /**
          * Implementation of the Device method.
          */
        void AddInput( const DataBlock&  ac_DataBlock );

        /**
          * Implementation of the Device method.
          */
        void RemoveAll();

        /**
          * Implementation of the Device method.
          */
        bool AddConnection( const tConnection& ac_pConnection );

        /**
          * Implementation of the Device method.
          */
        void SetSequence  ( const DataBlockMatrix* ac_pSequence );

        /**
          * Implementation of the Device method.
          */
//         DataBlock* CreateOffLine();

        /**
          * Implementation of the Device method.
          */
        //OutputDevice* CreateOffLineCopy();

        /**
          * Implementation of the Device method.
          */
        void PlayAll      ();

        /**
          * Implementation of the Device method.
          */
        void StopAll      ();

        void Finish();

        /**
          * Implementation of the Device method.
          */
        bool AllDone();

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

        //FIXME: [job refactory] move implementations to cpp


        /**
          * Implementation of the Device method.
          */
        bool HasParameter( const QString& ac_ParamID );

        /**
          * Implementation of the Device method.
          */
        void RestoreParametersImpl();

        /**
          * Implementation of the Device method.
          */
        bool CanConnect() const
        { return true; }

        /**
          * Implementation of the Device method.
          */
        bool CanSequence() const
        { return true; }

        /**
          * Implementation of the Device method.
          */
        bool CanOffLine() const
        { return true; }

        /**
          * Implementation of the Device method.
          */
        bool IsOffLine() const
        { return mv_bOffLine; }

        /**
          * Implementation of the Device method.
          */
        const QString& GetModule() const
        { return XMLKeys::sc_sWavDevice; }

        /**
          * Implementation of the Device method.
          */
        QString GetResultXML() const;

        /**
          * Implementation of the Device method.
          * Gets clipping info.
          */
        bool GetInfo( const unsigned ac_nType, void* a_pInfo ) const;


        virtual void SetSilenceBefore( double time ) { m_SilenceBefore=time;};

    private:
        void SetConnectionParams();
        void CheckClipping();

    private:
        bool mf_bIsNamedConnection( const QString& ac_sID ) const;
        void mp_RemoveNamedConnection( const QString& ac_sFromID );
        void mp_AddSeqStream( ApexSeqStream* pStream, const QString& ac_sID );

        data::WavDeviceData*    data;       // FIXME make const
        bool                mv_bOffLine;
        std::string         mv_sOffLine;//temp
        unsigned            mv_nOffLine;//
        double              m_SilenceBefore;
        tSeqMap             m_InputStreams;
        tGenMap             m_Generators;
        tWavFilters         m_InformFilters;
        tConnections        m_NamedConnections;
        /**
         * Cache for parameters that refer to connections
         * They have to be cached until the Prepare call because
         * connections cannot be set before they are made (when
         * a stimulus is loaded
         */
        QMap<QString,QVariant> m_connection_param_cache;

        WavDeviceIO         m_IO;


        QString m_driver;       // real driver and card name, as looked
        QString m_card;         // up in mainconfig

        mutable QStringList xmlresults;     // cache for GetResultXML()


};

}
}

#endif //#ifndef __WAVDEVICE_H__
