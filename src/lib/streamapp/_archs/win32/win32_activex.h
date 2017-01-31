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
 
#ifndef __WIN32_ACTIVEXSTUFF_H__
#define __WIN32_ACTIVEXSTUFF_H__

#include "appcore/singleton.h"
#include <guiddef.h>

namespace appcore
{

    /**
      * GetActiveXObject
      *   class to get any ActiveX/COM/OLE object.
      *   the object must be released by the caller
      ********************************************* */
  class GetActiveXObject : public Singleton<GetActiveXObject>
  {
      /**
        * Singleton implementation.
        */
    friend class Singleton<GetActiveXObject>;
  public:
      /**
        * Destructor.
        * calls CoUnInitialize.
        */
    ~GetActiveXObject();

      /**
        * Create a COM/OLE/ActiveX object.
        * @param a_FactoryID the CLSID of the object's factory as found in the registry
        * @param a_ClassID the CLSID of the object as found in the registry
        * @param a_pTheObject pointer to an IUnknown derivate receiving the object
        * @throws StringException
        */
    void mf_pGetObject( REFCLSID a_FactoryID, REFCLSID a_ClassID, void** a_pTheObject );

  private:
      /**
        * Private Constructor.
        * calls CoInitialize.
        */
    GetActiveXObject();
  };

}

#endif //#ifndef __WIN32_ACTIVEXSTUFF_H__
