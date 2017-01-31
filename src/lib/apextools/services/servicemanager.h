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

#ifndef _SERVICEMANAGER_H
#define _SERVICEMANAGER_H

/** \file
 * Singleton support with ordered destruction,
 * termed Services and ServiceManager.
 */

#include "global.h"
#include <qglobal.h>

//#include <QApplication>

#include <vector>
#include <typeinfo>

namespace apex {

    /** Base class for singletons which is noncopyable.
     */

    class ServiceBase
    {
    private:
        ServiceBase (const ServiceBase& other);
        ServiceBase& operator= (const ServiceBase& other);
    protected:
        ServiceBase() {};
    public:
            /** Needed virtual destructor to be able to destruct the derived
         * singletons with a base pointer.
         */
        virtual ~ServiceBase()
        {
        };

           /** Provide an implementation that returns the service name as a string.
         */
        virtual const char* Name() = 0;
    };

    /** Class for singleton destruction management, termed the ServiceManager.
     */

    class APEXTOOLS_EXPORT ServiceManager
    {
    public:
        ~ServiceManager();
        static ServiceManager& Get();
        ServiceBase* Manage (ServiceBase* singleton);

        void SetCommandLine (int argc, char** argv);
        int& GetCommandLineCount();
        char** GetCommandLineValues();
        void RemoveServices();

    private:
        ServiceManager();
        typedef std::vector<ServiceBase*> ServiceList;
        ServiceList services;
        int argc;
        char** argv;
    };

    /** Base type for singletons, called the Service.
     * Derive from this template to get destruction managment for free.
     * The template parameter \c ServiceClass specifies the name of the singelton
     * class.
     * \warning There is no locking done, so it is not thread-save!
     */

    template<class ServiceClass>
    class Service: public ServiceBase
    {
    public:
        static ServiceClass& Get();
    };

    /** Return the instance of the singleton class.
     * \return Reference to the singleton instance
     */

    template<class ServiceClass>
    ServiceClass& Service<ServiceClass>::Get()
    {
        static ServiceClass* instance = 0;
        if (instance)
            return *instance;

        static bool initializing = false;
        if (initializing) {
            qWarning("Service dependency loop of Service %s...", typeid (ServiceClass).name());
            return *static_cast<ServiceClass*>(0);
        }
        initializing = true;
        instance = static_cast<ServiceClass*> (
            ServiceManager::Get().Manage (new ServiceClass()));
        if (!instance)
            qDebug ("... in Service %s", typeid (ServiceClass).name());
        else
            qDebug ("Instantiated Service %s", instance->Name());
        return *instance;
    }
}

#endif /* _SERVICEMANAGER_H */
