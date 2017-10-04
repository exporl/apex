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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_PA5DEVICE_PA5_ACTX_H_
#define _EXPORL_SRC_LIB_APEXMAIN_PA5DEVICE_PA5_ACTX_H_

#include <qstring.h>

class PA5Wnd;
class QWidget;

/**
  * Wrapper around the MFC ActiveX wrapper
  * only reason for wrapper is that xercesc dosn't allow forward declaring
  *properly,
  * so we cannot #include <axwin.h> because it defines DOMDocument. shame...
  * FIXME this is fixed by now probably.
  **********************************************************************************
  */
class Pa5ActX
{
public:
    /**
      * Constructor.
      * Tries to create the ActiveX instance.
      * Throws a ApexStringException on failure.
      */
    Pa5ActX();

    /**
      * Destructor.
      */
    ~Pa5ActX();

    /**
      * Connect to a Pa5
      * @param Interface the interface used ( usb or zbus )
      * @param DevNum the Pa5 id ( starts at 0 )
      * @return true if succeeded, else query GetError()
      */
    bool Connect(long Interface, long DevNum);

    /**
      * Set the attenuation.
      * @param AttVal the value in dB
      * @return true if ok
      */
    bool SetAtten(float AttVal);

    /**
      * Get the current attenuation set.
      * @return the attenuation in dB
      */
    float GetAtten();

    /**
      * Reset the connection.
      * @return false if failed
      */
    bool Reset();

    /**
      * Set user data
      * @see Pa5 docs.
      * @param ParCode ??
      * @param Val ??
      * @return true if ok
      */
    bool SetUser(long ParCode, float Val);

    /**
      * Display a text on the LCD screen
      * @param Text the text
      * @param Position the position in characters
      * @return true if ok
      */
    bool Display(char *Text, long Position);

    /**
      * Connect to a PA5
      * @param IntName the interface name ( "usb" or "zbus" )
      * @param DevNum the Pa5 id ( starts at 0 )
      * @return true if ok
      */
    long ConnectPA5(char *IntName, long DevNum);

    /**
      * Get the error description.
      * Use this if a method returned false to see what's going on.
      * @return the error as QString
      */
    QString GetError();

private:
    QWidget *m_pContainer;
    PA5Wnd *m_pObj;
};

#endif //__PA5DEVICEACTX_H__
