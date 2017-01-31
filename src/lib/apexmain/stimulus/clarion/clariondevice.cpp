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

#include "parameters/parameter.h"

#include "services/mainconfigfileparser.h"

#include "clariondatablock.h"
#include "clariondevice.h"
#include "clarionwrapper.h"
#include "exceptions.h"

#include <containers/array.h>

//from libdata
#include "device/clariondevicedata.h"

#include <QMap>
#include <QPluginLoader>
#include <QString>
#include <QMessageBox>

using namespace apex;
using namespace clarion;
using namespace apex::stimulus;

namespace clarion
{
  void sf_ShowError( const QString& ac_sReason )
  {
      QMessageBox::critical (NULL, "ClarionDevice", ac_sReason);
  }
}

ClarionDevice::ClarionDevice( data::ClarionDeviceData* a_pParameters ) :
    OutputDevice( a_pParameters ),
  mv_bErrorOccured( false ),
  mv_sCurrentFile(),
  m_Params( a_pParameters)
{
    LoadPlugin();
    if( !m_pClarion.get() )
    {
        mv_bErrorOccured = true;
        throw( ApexStringException( "ClarionDevice: couldn't find an implementation" ) );
    }
    QMessageBox::information (NULL, "ClarionDevice", "Now connecting to Clarion.");
    if( !m_pClarion->mp_bConnect() )
    {
        mv_bErrorOccured = true;
        throw( ApexStringException( "ClarionDevice: " + QString( m_pClarion->mf_cGetLastError() ) ) );
    }
    QMessageBox::information (NULL, "ClarionDevice", "Connected!");
}

void ClarionDevice::LoadPlugin() {
    QString filename(Paths::Get().GetBinaryPluginPath() +
            "/clarionplugin.dll" );
    qDebug("Looking for %s", qPrintable (filename));

                // load plugin into pluginfilterinterface
    QPluginLoader loader (filename);
    loader.load();
    if (!loader.isLoaded())
        //throw PluginNotFoundException ( loader.errorString());
        throw ApexStringException ( loader.errorString());

    std::auto_ptr<ClarionWrapperCreatorInterface> creator(
            qobject_cast<ClarionWrapperCreatorInterface*>  (loader.instance()));
    if (!creator.get())
//        throw PluginNotFoundException ("Unknown error");
        throw ApexStringException ("Unknown error");

    m_pClarion.reset( creator->create() );
    if (!m_pClarion.get())
//        throw PluginNotFoundException ("Unknown error");
        throw ApexStringException ("Unknown error");

}

ClarionDevice::~ClarionDevice()
{
}

void ClarionDevice::AddFilter( const Filter& /*ac_Filter*/ )
{
    qFatal("Not implemented");
}

void ClarionDevice::SetSequence( const DataBlockMatrix* ac_pSequence )
{
    assert( ac_pSequence->mf_nGetChannelCount() == 1 && ac_pSequence->mf_nGetBufferSize() == 1 );

    ClarionDataBlock* pData = (ClarionDataBlock*)( ac_pSequence->operator()( 0, 0 ) );
    AddInput( *pData );
}

void ClarionDevice::AddInput( const DataBlock& ac_DataBlock )
{
    const ClarionDataBlock& data = dynamic_cast<const ClarionDataBlock&>( ac_DataBlock );

    //only load and check parameters when it's a new datablock
    if( data.GetID() != mv_sCurrentFile )
    {
        mv_sCurrentFile = data.GetID();
        if( !m_pClarion->mp_bLoadFile( data.GetUrl().path() ) )
        {
            mv_bErrorOccured = true;
            sf_ShowError( QString( m_pClarion->mf_cGetLastError() ) );
        }

        //we can only check the parameters after loading the file, so do this now
        mp_CheckParameters();

        //if there's no error, it's safe to set all parameters now
        /*        if( !mv_bErrorOccured )
        mp_SetAllParameters();*/
    }
}

void ClarionDevice::mp_CheckParameters()
{
  Q_ASSERT( m_Params );

  //create parameter name array
  QStringList cParams;
  Q_FOREACH (data::Parameter param, m_Params->parameters())
    cParams.append(param.type());

    //now go and check
  if( !m_pClarion->mf_bCheckParameters( cParams ) )
  {
      //not good, param not found, tell the user it's game over
    const QString sErr( "Not all required parameters were found.\nThis is the message:\n" + QString( m_pClarion->mf_cGetLastError() ) );
    sf_ShowError( sErr );
    mv_bErrorOccured = true;
  }
}

/*void ClarionDevice::mp_SetAllParameters()
{
//s_assert( m_pCurrentFileParameters );
//const ExtendedParameters& params = m_pCurrentFileParameters->GetExtParams();
//tParamInfosMapCIt itE = params.end();
//for( tParamInfosMapCIt it = params.begin() ; it != itE ; ++it )
//  m_pClarion->mp_bSetParameter( (*it).second.m_sType.toAscii().data(), (*it).second.m_sValue.toAscii().data() );
}*/

void ClarionDevice::RemoveAll()
{
    //we never  unload a file
}

void ClarionDevice::PlayAll()
{
  mv_bErrorOccured = false; //FIXME this is just for testing
    if( !mv_bErrorOccured )
    {
        if( !m_pClarion->mp_bStart() )
        {
            mv_bErrorOccured = true;
            sf_ShowError( "Playing not started " + QString( m_pClarion->mf_cGetLastError() ) );
        }
    }
    else
    {
        sf_ShowError( "Playing not started because of previous error" );
    }
}

void ClarionDevice::StopAll()
{
    if( !mv_bErrorOccured )
        m_pClarion->mp_bStop();
}

bool ClarionDevice::AllDone()
{
    if( !mv_bErrorOccured )
    {
        if( !m_pClarion->mp_bWaitForStop() )
        {
            mv_bErrorOccured = true;
            sf_ShowError( "Stop failed: " + QString( m_pClarion->mf_cGetLastError() ) );
        }
    }
    return true;
}



/*bool ClarionDevice::HasParameter( const QString& ac_ParamID )
{
//if( !m_pCurrentFileParameters )
return false;
//return m_pParameters->HasParameter( ac_ParamID );
}*/


bool ClarionDevice::SetParameter ( const QString& type, const int channel, const QVariant& value )
{
    Q_UNUSED (channel);
    if( !m_pClarion->mp_bSetParameter( type.ascii(), value.toString().ascii() ) )
    {
        mv_bErrorOccured = true;
        sf_ShowError( "Parameter not set: " + QString( m_pClarion->mf_cGetLastError() ) );

    }

    return false;
}


void ClarionDevice::Reset() {

}

void ClarionDevice::Prepare() {

}
