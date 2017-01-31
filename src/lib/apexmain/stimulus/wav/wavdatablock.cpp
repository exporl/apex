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

#include "wavdatablock.h"
#include "stimulus/wav/streamgenerator.h"
#include "stimulus/datablock.h"
#include "file/wavefile.h"
#include "silentreader.h"
#include "datablock/datablockdata.h"
#include <utils/stringexception.h>

#include "runner/experimentrundelegate.h" // get the device for checking sample rate

//from libdata
#include "experimentdata.h"
#include "device/wavdevicedata.h"

#include <qstring.h>

using namespace apex;
using namespace stimulus;
using namespace streamapp;

WavDataBlock::WavDataBlock(const data::DatablockData& data, const QUrl& filename,
                           const ExperimentRunDelegate* experiment) :
    WavCompatibleDataBlock(data, filename, experiment),
    m_dSilenceLength( 0.0 )
{
    if( filename.scheme() == "silence" ) {
        bool ok;
        const QString temp(filename.toString().remove(0,8).remove('/') );
        m_dSilenceLength = temp.toDouble(&ok) * (double) data.nbLoops();
        if( !ok || m_dSilenceLength == 0.0 ) //FIXME [job refactory]
            throw( ApexStringException("Invalid silence length ") );

        qDebug("Silence datablock with length=%f ms", m_dSilenceLength);

        if( data.nbChannels() == 0 )
            this->data.setNbChannels(1);
        //throw( ApexStringException( "Number of channels for silence must be set" ) );
        qDebug("Warning using default number of channels (1) for silence datablock");
    } else {
        if( !filename.path().endsWith( ".wav" ) )
            throw(ApexStringException("Invalid wav filename: should end in .wav"));

        //check wavefile validity
        InputWaveFile w;
        bool bOpen = false;
        try {
            bOpen = w.mp_bOpen( filename.path().toAscii().data() );
        } catch( utils::StringException& e ) {
            throw( ApexStringException( "bad wavfile " + filename.path() +
                        "; reason: " + QString( e.mc_sError.data() ) ) );
        }
        if( !bOpen ) {
            throw( ApexStringException( "wavfile not found " + filename.path() ) );
        } else {
            const unsigned nChan = w.mf_nChannels();
            if( data.nbChannels() != 0 ) {
                if( data.nbChannels() != nChan )
                    throw( ApexStringException( "wavfile " + filename.path() +
                                " wrong number of channels" ) );
            } else {
                this->data.setNbChannels(nChan);
            }

            // check sample rate
            const data::WavDeviceData* wdd = dynamic_cast<const data::WavDeviceData*>
                    (experiment->GetData().deviceById(data.device()));
            Q_ASSERT(wdd || "Couldn't get device");
            if (w.mf_lSampleRate() !=  wdd->sampleRate() ) {
                throw ApexStringException("Error: sample rate of datablock " +
                        data.id() +
                        " does not correspond to the sample rate of " +
                        GetDevice());
            }
        }
        w.mp_bClose();
    }
}

WavDataBlock::~WavDataBlock() {}

PositionableAudioFormatReaderStream* WavDataBlock::GetWavStream
        (const unsigned ac_nBufferSize, const unsigned long ac_nFs ) const
{
    PositionableAudioFormatReader* p = 0;
    if( m_dSilenceLength != 0.0 ) {
        p = new SilentReader( data.nbChannels(), ac_nFs, m_dSilenceLength );
    } else {
        InputWaveFile* pI = new InputWaveFile();
        if( !pI->mp_bOpen(filename.path().toAscii().data())) {
            delete pI;
            //can never be reached if ctor check is ok
            assert( 0 && "cannot open wavefile" );
            return 0;
        }
        p = pI;
    }
    PosAudioFormatStream* pRet = new PosAudioFormatStream( p, ac_nBufferSize,
            true, true );
    pRet->mp_SetNumLoops(data.nbLoops());
    return pRet;
}


DataBlock* WavDataBlock::GetCopy(QString id)
{
    qDebug("Copying WAV datablock");
    WavDataBlock* ret = new WavDataBlock(GetParameters(), filename, experiment);
    ret->data.setId(id);
    return ret;
}

