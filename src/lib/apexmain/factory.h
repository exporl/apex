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

#ifndef __FACTORY_H__
#define __FACTORY_H__

#include "status/statusreporter.h"
#include "appcore/raiifactory.h"
#include "factoryelement.h"
#include <appcore/singleton.h>

#include <memory>

using namespace appcore;

namespace apex
{

    /**
      * Factory Template: Obsoleted by non-singleton ElementFactory (please delete when unused)
      *
      *   Singleton RAII Factory.
      *   Keeps all elements created and destroys itself at application exit,
      *   hereby effectively deleting all elements.
      *   Usage example with default tElement:
      *
      *       class myFactory : public Factory<myFactory>
      *       {
      *         friend class Singleton<myFactory>;
      *       public:
      *         virtual ~myFactory()
      *         {}
      *
      *         FactoryElement* mf_pCreateSome()
      *         {
      *           return keep( new FactoryElement() );
      *         }
      *
      *       private:
      *         myFactory()
      *         {}
      *       };
      *
      *       int main()
      *       {
      *         myFactory* pFactory = myFactory::sf_pInstance();
      *         FactoryElement* pNoNeedToDelete = pFactory->mf_pCreateSome();
      *       }
      *
      *   All elements created through the factory must only be deleted
      *   through calls to RaiiFactoryImp's dispose methods.
      *********************************************************************** */
  template<class tFactory, class tElement = FactoryElement>
  class Factory : public RaiiFactoryImp<tElement>, public Singleton<tFactory>
  {
  public:
      /**
        * Destructor.
        * Deletes all elements.
        */
    virtual ~Factory()
    {
      RaiiFactoryImp<tElement>::disposeAll();
    }

      /**
        * Get the errors if any.
        * @return an unmutable ApexError ref
        */
    const StatusReporter& mf_GetError() const
    { return *m_Error; }

  protected:
      /**
        * Constructor.
        */
    Factory() :
      m_Error (new StatusReporter())
    {}

      /**
        * Clear the errors.
        * Shortcut for implementations.
        */
    virtual void mp_ClearErrors()
    { m_Error->clear(); }

    mutable std::auto_ptr<StatusReporter> m_Error;
  };

}

#endif //#ifndef __FACTORY_H__
