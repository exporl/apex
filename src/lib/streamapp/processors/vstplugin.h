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
 
  /**
    * @file vstplugin.h
    * Contains implementation of IStreamProcessor for vst plugins.
    * Is known to work in Streamapp v1, with gui et all, but needs updating to new source.
    */

#ifndef VSTPLUGINN_H
#define VSTPLUGINN_H

  //vst sdk
#include <windows.h>
#include "aeffectx.h"
#include "aeffeditor.hpp"

  //streampp
#include "streamprocessor.h"
#include "containers/matrix.h"
using namespace streamapp;

  //default values; set in ctor
static int blocksize = 512;
static float samplerate = 44100.0f;
static long sampleframes = 512;

  //host callback function
  //this is called directly by the plug-in!!
static long VSTCALLBACK host(AEffect *effect, long opcode, long index, long value, void *ptr, float opt)
{
long retval=0;

switch (opcode)
{
//VST 1.0 opcodes
case audioMasterVersion:
//Input values:
//none

//Return Value:
//0 or 1 for old version
//2 or higher for VST2.0 host?
cout << "plug called audioMasterVersion" << endl;
retval=2;
break;

case audioMasterAutomate:
//Input values:
//<index> parameter that has changed
//<opt> new value

//Return value:
//not tested, always return 0

//NB - this is called when the plug calls
//setParameterAutomated

cout << "plug called audioMasterAutomate" << endl;
break;

case audioMasterCurrentId:
//Input values:
//none

//Return Value
//the unique id of a plug that's currently loading
//zero is a default value and can be safely returned if not known
cout << "plug called audioMasterCurrentId" << endl;
break;

case audioMasterIdle:
//Input values:
//none

//Return Value
//not tested, always return 0

//NB - idle routine should also call effEditIdle for all open editors
Sleep(1);
cout << "plug called audioMasterIdle" << endl;
break;

case audioMasterPinConnected:
//Input values:
//<index> pin to be checked
//<value> 0=input pin, non-zero value=output pin

//Return values:
//0=true, non-zero=false
cout << "plug called audioMasterPinConnected" << endl;
break;

//VST 2.0 opcodes
case audioMasterWantMidi:
//Input Values:
//<value> filter flags (which is currently ignored, no defined flags?)

//Return Value:
//not tested, always return 0
cout << "plug called audioMasterWantMidi" << endl;
break;

case audioMasterGetTime:
//Input Values:
//<value> should contain a mask indicating which fields are required
//...from the following list?
//kVstNanosValid
//kVstPpqPosValid
//kVstTempoValid
//kVstBarsValid
//kVstCyclePosValid
//kVstTimeSigValid
//kVstSmpteValid
//kVstClockValid

//Return Value:
//ptr to populated const VstTimeInfo structure (or 0 if not supported)

//NB - this structure will have to be held in memory for long enough
//for the plug to safely make use of it
cout << "plug called audioMasterGetTime" << endl;
break;

case audioMasterProcessEvents:
//Input Values:
//<ptr> Pointer to a populated VstEvents structure

//Return value:
//0 if error
//1 if OK
cout << "plug called audioMasterProcessEvents" << endl;
break;

case audioMasterSetTime:
//IGNORE!
break;

case audioMasterTempoAt:
//Input Values:
//<value> sample frame location to be checked

//Return Value:
//tempo (in bpm * 10000)
cout << "plug called audioMasterTempoAt" << endl;
break;

case audioMasterGetNumAutomatableParameters:
//Input Values:
//None

//Return Value:
//number of automatable parameters
//zero is a default value and can be safely returned if not known

//NB - what exactly does this mean? can the host set a limit to the
//number of parameters that can be automated?
cout << "plug called audioMasterGetNumAutomatableParameters" << endl;
break;

case audioMasterGetParameterQuantization:
//Input Values:
//None

//Return Value:
//integer value for +1.0 representation,
//or 1 if full single float precision is maintained
//in automation.

//NB - ***possibly bugged***
//Steinberg notes say "parameter index in <value> (-1: all, any)"
//but in aeffectx.cpp no parameters are taken or passed
cout << "plug called audioMasterGetParameterQuantization" << endl;
break;

case audioMasterIOChanged:
//Input Values:
//None

//Return Value:
//0 if error
//non-zero value if OK
cout << "plug called audioMasterIOChanged" << endl;
break;

case audioMasterNeedIdle:
//Input Values:
//None

//Return Value:
//0 if error
//non-zero value if OK

//NB plug needs idle calls (outside its editor window)
//this means that effIdle must be dispatched to the plug
//during host idle process and not effEditIdle calls only when its
//editor is open
//Check despatcher notes for any return codes from effIdle
cout << "plug called audioMasterNeedIdle" << endl;
break;

case audioMasterSizeWindow:
//Input Values:
//<index> width
//<value> height

//Return Value:
//0 if error
//non-zero value if OK
cout << "plug called audioMasterSizeWindow" << endl;
break;

case audioMasterGetSampleRate:
//Input Values:
//None

//Return Value:
//not tested, always return 0

//NB - Host must despatch effSetSampleRate to the plug in response
//to this call
//Check despatcher notes for any return codes from effSetSampleRate
cout << "plug called audioMasterGetSampleRate" << endl;
effect->dispatcher(effect,effSetSampleRate,0,0,NULL,samplerate);
break;

case audioMasterGetBlockSize:
//Input Values:
//None

//Return Value:
//not tested, always return 0

//NB - Host must despatch effSetBlockSize to the plug in response
//to this call
//Check despatcher notes for any return codes from effSetBlockSize
cout << "plug called audioMasterGetBlockSize" << endl;
effect->dispatcher(effect,effSetBlockSize,0,blocksize,NULL,0.0f);

break;

case audioMasterGetInputLatency:
//Input Values:
//None

//Return Value:
//input latency (in sampleframes?)
cout << "plug called audioMasterGetInputLatency" << endl;
break;

case audioMasterGetOutputLatency:
//Input Values:
//None

//Return Value:
//output latency (in sampleframes?)
cout << "plug called audioMasterGetOutputLatency" << endl;
break;

case audioMasterGetPreviousPlug:
//Input Values:
//None

//Return Value:
//pointer to AEffect structure or NULL if not known?

//NB - ***possibly bugged***
//Steinberg notes say "input pin in <value> (-1: first to come)"
//but in aeffectx.cpp no parameters are taken or passed
cout << "plug called audioMasterGetPreviousPlug" << endl;
break;

case audioMasterGetNextPlug:
//Input Values:
//None

//Return Value:
//pointer to AEffect structure or NULL if not known?

//NB - ***possibly bugged***
//Steinberg notes say "output pin in <value> (-1: first to come)"
//but in aeffectx.cpp no parameters are taken or passed
cout << "plug called audioMasterGetNextPlug" << endl;
break;

case audioMasterWillReplaceOrAccumulate:
//Input Values:
//None

//Return Value:
//0: not supported
//1: replace
//2: accumulate
cout << "plug called audioMasterWillReplaceOrAccumulate" << endl;
break;

case audioMasterGetCurrentProcessLevel:
//Input Values:
//None

//Return Value:
//0: not supported,
//1: currently in user thread (gui)
//2: currently in audio thread (where process is called)
//3: currently in 'sequencer' thread (midi, timer etc)
//4: currently offline processing and thus in user thread
//other: not defined, but probably pre-empting user thread.
cout << "plug called audioMasterGetCurrentProcessLevel" << endl;
break;

case audioMasterGetAutomationState:
//Input Values:
//None

//Return Value:
//0: not supported
//1: off
//2:read
//3:write
//4:read/write
cout << "plug called audioMasterGetAutomationState" << endl;
break;

case audioMasterGetVendorString:
//Input Values:
//<ptr> string (max 64 chars) to be populated

//Return Value:
//0 if error
//non-zero value if OK
cout << "plug called audioMasterGetVendorString" << endl;
break;

case audioMasterGetProductString:
//Input Values:
//<ptr> string (max 64 chars) to be populated

//Return Value:
//0 if error
//non-zero value if OK
cout << "plug called audioMasterGetProductString" << endl;
break;

case audioMasterGetVendorVersion:
//Input Values:
//None

//Return Value:
//Vendor specific host version as integer
cout << "plug called audioMasterGetVendorVersion" << endl;
break;

case audioMasterVendorSpecific:
//Input Values:
//<index> lArg1
//<value> lArg2
//<ptr> ptrArg
//<opt>floatArg

//Return Values:
//Vendor specific response as integer
cout << "plug called audioMasterVendorSpecific" << endl;
break;

case audioMasterSetIcon:
//IGNORE
break;

case audioMasterCanDo:
//Input Values:
//<ptr> predefined "canDo" string

//Return Value:
//0 = Not Supported
//non-zero value if host supports that feature

//NB - Possible Can Do strings are:
//"sendVstEvents",
//"sendVstMidiEvent",
//"sendVstTimeInfo",
//"receiveVstEvents",
//"receiveVstMidiEvent",
//"receiveVstTimeInfo",
//"reportConnectionChanges",
//"acceptIOChanges",
//"sizeWindow",
//"asyncProcessing",
//"offline",
//"supplyIdle",
//"supportShell"
cout << "plug called audioMasterCanDo" << endl;

if (strcmp((char*)ptr,"sendVstEvents")==0 ||
strcmp((char*)ptr,"sendVstMidiEvent")==0 ||
strcmp((char*)ptr,"supplyIdle")==0)
{
retval=1;
}
else
{
retval=0;
}

break;

case audioMasterGetLanguage:
//Input Values:
//None

//Return Value:
//kVstLangEnglish
//kVstLangGerman
//kVstLangFrench
//kVstLangItalian
//kVstLangSpanish
//kVstLangJapanese
cout << "plug called audioMasterGetLanguage" << endl;
retval=1;
break;

case audioMasterGetDirectory:
//Input Values:
//None

//Return Value:
//0 if error
//FSSpec on MAC, else char* as integer

//NB Refers to which directory, exactly?
cout << "plug called audioMasterGetDirectory" << endl;
break;

case audioMasterUpdateDisplay:
//Input Values:
//None

//Return Value:
//Unknown
cout << "plug called audioMasterUpdateDisplay" << endl;
break;
}

return retval;
};

static AEffect* sf_pLoadVstPlugin( const char* acp_sPath )
{
  AEffect* ptrPlug=NULL;

    //find and load the DLL and get a pointer to its main function
    //this has a protoype like this: AEffect *main (audioMasterCallback audioMaster)
  HINSTANCE libhandle=LoadLibrary(acp_sPath);

  if (libhandle!=NULL)
  {
    AEffect* (__cdecl* getNewPlugInstance)(audioMasterCallback);
    getNewPlugInstance=(AEffect*(__cdecl*)(audioMasterCallback))GetProcAddress(libhandle, "main");

    if (getNewPlugInstance!=NULL)
    {
      ptrPlug=getNewPlugInstance(host);

      if (ptrPlug!=NULL)
      {
        cout << "Plugin was loaded OK" << endl;

        if (ptrPlug->magic==kEffectMagic)
        {
          cout << "It's a valid VST plugin" << endl;
        }
        else
        {
          cout << "Not a VST plugin" << endl;
          FreeLibrary(libhandle);
          Sleep(5000);
          return 0;
        }
      }
      else
      {
        cout << "Plugin could not be instantiated" << endl;
        FreeLibrary(libhandle);
        Sleep(5000);
        return 0;
      }
    }
    else
    {
      cout << "Plugin main function could not be located" << endl;
      FreeLibrary(libhandle);
      Sleep(5000);
      return 0;
    }
  }
  else
  {
    cout << "Plugin DLL could not be loaded" << endl;
    Sleep(5000);
    return 0;
  }

  return ptrPlug;
}
static bool sf_bTestPlugin( const AEffect* acp_Fx )
{
  long rc=0;
  AEffect* ptrPlug = (AEffect*) acp_Fx;

  //try some dispatcher functions...
  ptrPlug->dispatcher(ptrPlug,effOpen,0,0,NULL,0.0f);

  //switch the plugin off (calls Suspend)
  ptrPlug->dispatcher(ptrPlug,effMainsChanged,0,0,NULL,0.0f);

  //set sample rate and block size
  ptrPlug->dispatcher(ptrPlug,effSetSampleRate,0,0,NULL,samplerate);
  ptrPlug->dispatcher(ptrPlug,effSetBlockSize,0,blocksize,NULL,0.0f);

  if ((ptrPlug->dispatcher(ptrPlug,effGetVstVersion,0,0,NULL,0.0f)==2) &&
  (ptrPlug->flags & effFlagsIsSynth))
  {
    //get I/O configuration for synth plugins - they will declare their
    //own output and input channels
    for (int i=0; i<ptrPlug->numInputs+ptrPlug->numOutputs;i++)
    {
      if (i<ptrPlug->numInputs)
      {
        //input pin
        VstPinProperties temp;

        if (ptrPlug->dispatcher(ptrPlug,effGetInputProperties,i,0,&temp,0.0f)==1)
        {
          cout << "Input pin " << (i+1) << " label " << temp.label << endl;
          //cout << "Input pin " << (i+1) << " shortLabel " << temp.shortLabel << endl;

          if (temp.flags & kVstPinIsActive)
          {
          cout << "Input pin " << (i+1) << " is active" << endl;
          }

          if (temp.flags & kVstPinIsStereo)
          {
            // is index even or zero?
            if (i%2==0 || i==0)
            {
              cout << "Input pin " << (i+1) << " is left channel of a stereo pair" << endl;
            }
            else
            {
              cout << "Input pin " << (i+1) << " is right channel of a stereo pair" << endl;
            }
          }
        }
      }
      else
      {
          //output pin
        VstPinProperties temp;

        if (ptrPlug->dispatcher(ptrPlug,effGetOutputProperties,i,0,&temp,0.0f)==1)
        {
        cout << "Output pin " << (i-ptrPlug->numInputs+1) << " label " << temp.label << endl;
        //cout << "Output pin " << (i-ptrPlug->numInputs+1) << " shortLabel " << temp.shortLabel << endl;

        if (temp.flags & kVstPinIsActive)
        {
        cout << "Output pin " << (i-ptrPlug->numInputs+1) << " is active" << endl;
        }
        else
        {
        cout << "Output pin " << (i-ptrPlug->numInputs+1) << " is inactive" << endl;
        }

        if (temp.flags & kVstPinIsStereo)
        {
            // is index even or zero?
          if ((i-ptrPlug->numInputs)%2==0 || (i-ptrPlug->numInputs)==0)
          {
            cout << "Output pin " << (i+1) << " is left channel of a stereo pair" << endl;
          }
          else
          {
            cout << "Output pin " << (i+1) << " is right channel of a stereo pair" << endl;
          }
        }
        else
        {
          cout <<  "Output pin " << (i+1) << " is mono" << endl;
        }
        }
      }
    }
  }//end VST2 specific

  return 0;
}

static void sf_ShowSomePluginInfo( const AEffect* acp_Fx )
{
  AEffect* ptrPlug = (AEffect*) acp_Fx;

    //switch the plugin back on (calls Resume)
  ptrPlug->dispatcher(ptrPlug,effMainsChanged,0,1,NULL,0.0f);

    //set to program zero and fetch back the name
  ptrPlug->dispatcher(ptrPlug,effSetProgram,0,0,NULL,0.0f);

  char strProgramName[24+2];
  ptrPlug->dispatcher(ptrPlug,effGetProgramName,0,0,strProgramName,0.0f);

  cout << "Set plug to program zero, name is " <<  strProgramName << endl;

    //get the parameter strings
  char strName[24];
  char strDisplay[24];
  char strLabel[24];

  ptrPlug->dispatcher(ptrPlug,effGetParamName,0,0,strName,0.0f);
  cout << "Parameter name is " << strName << endl;

  ptrPlug->dispatcher(ptrPlug,effGetParamLabel,0,0,strLabel,0.0f);
  cout << "Parameter label is " << strLabel << endl;

  ptrPlug->dispatcher(ptrPlug,effGetParamDisplay,0,0,strDisplay,0.0f);
  cout << "Parameter display is " << strDisplay << endl;

    //call the set and get parameter functions
  ptrPlug->setParameter(ptrPlug,0,0.7071f);
  float newval=ptrPlug->getParameter(ptrPlug,0);

  cout << "Parameter 0 was changed to " << newval << endl;
  ptrPlug->dispatcher(ptrPlug,effGetParamDisplay,0,0,strDisplay,0.0f);
  cout << "Parameter display is now " << strDisplay << endl;
}

static void sf_InitPluginMemory( const AEffect* ptrPlug , float** ptrOutputBuffers )
{
  if (ptrPlug->numOutputs)
  {
    ptrOutputBuffers=new float*[ptrPlug->numOutputs];

      //create the output buffers
    for (int i=0;i<ptrPlug->numOutputs;i++)
    {
      cout << "Output buffer " << (i+1) << " created" << endl;
      ptrOutputBuffers[i]=new float[blocksize];
    }
  }
}
class StreamUsesVST : public StreamProcessor
{
public:
  StreamUsesVST( const unsigned ac_nBufferSize, const unsigned long ac_nSampleRate ) :
      StreamProcessor( 2, 2, ac_nBufferSize  ),
    mcp_AEffect ( 0 ),
    mcp_Output  ( new CChanBufFloat( 2 , ac_nBufferSize , true ) ),
    mcp_Input   ( new CChanBufFloat( 2 , ac_nBufferSize , true ) ),
    mcp_dOutput ( new StreamBuf( 2 , ac_nBufferSize , true ) )
  {
    blocksize = ac_nBufferSize;
    samplerate = (float) ac_nSampleRate;
    sampleframes = ac_nBufferSize;
  }

  ~StreamUsesVST( )
  {
    delete mcp_Output;
    delete mcp_Input;
  }


  const bool mf_bOpenVSTdll( const char* acp_PathToVSTdll )
  {
    mcp_AEffect = sf_pLoadVstPlugin( acp_PathToVSTdll );
    sf_bTestPlugin( mcp_AEffect );
    sf_ShowSomePluginInfo( mcp_AEffect );
    if( !mcp_AEffect->numOutputs )
      return false;
  }

    //window size contains min size for window after call
  const bool mf_bOpenVstGuiEditor( const HWND ac_hWindow, ERect** ac_WindowSize )
  {
    long lResult = mcp_AEffect->dispatcher( (AEffect*) mcp_AEffect, effEditGetRect, 0, 0, ac_WindowSize , 0.f );
    if( ac_WindowSize )
    {
      if( ac_hWindow )
      {
        long lResult = mcp_AEffect->dispatcher( (AEffect*) mcp_AEffect, effEditOpen, 0, 0, ac_hWindow , 0.f );
        //mcp_AEffect->setParameter( (AEffect*) mcp_AEffect, 0 , 0.12f );
        if( lResult > 0 )
          return true;
        else
          return false;
      }
    }
  }

  virtual CChanStrDouble& DoDblProcessing( const CChanStrDouble& ac_dStrToProc )
  {
    if( ac_dStrToProc.mf_nGetChannelCount() > 0 )
    {
      for( unsigned i = 0 ; i < mf_nGetNumIChan() ; ++i )
      {
        for( unsigned j = 0 ; j < mf_nGetSize() ; ++j )
        {
          mcp_Input->mp_Set( i , j , (float) ac_dStrToProc( i , j ) );
        }
      }

      mcp_AEffect->processReplacing( (AEffect*) mcp_AEffect , mcp_Input->mf_pGetArray(),
                                       mcp_Output->mf_pGetArray() , mf_nGetSize() );

      for( unsigned i = 0 ; i < mf_nGetNumIChan() ; ++i )
      {
        for( unsigned j = 0 ; j < mf_nGetSize() ; ++j )
        {
          mcp_dOutput->mp_Set( i , j , (StreamType) ( mcp_Output->mf_Get( i , j ) * 65536 ) );
        }
      }
    }
    return *mcp_dOutput;
  }

private:
  const AEffect*  mcp_AEffect;
  CChanBufFloat*  const  mcp_Input;
  CChanBufFloat*  const  mcp_Output;
  StreamBuf* const  mcp_dOutput;
};

#endif //#ifndef VSTPLUGINN_H