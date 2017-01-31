/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                             *
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

/** \file
 * Singleton support with ordered destruction,
 * termed Services and ServiceManager.
 */

#include "servicemanager.h"

#include <iostream>
#include <memory>

using namespace apex;

ServiceManager::ServiceManager() :
    argc (0),
    argv (NULL)
{
}

/**
 * Destructor.
 * Removes any pending services.
 */
ServiceManager::~ServiceManager()
{
    RemoveServices();
}

/**
 * Destroy the registered singletons in the right order.
 * This depends on std::multimap<unsigned> being sorted
 * in the right way.
 */
void ServiceManager::RemoveServices()
{
    // This should assure the reverse order of destruction
    while (services.begin() != services.end()) {
        // Creates problems with testbench output
        //qCDebug(APEX_RS, "Removing Service %s", services.back()->Name());
        delete services.back();
        services.pop_back();
    }
}

/** Manage a singleton instance.
 * The instance is registered and is destroyed
 * when the program finishes after all other Services that have been
 * constructed later have been destructed.
 * \param singleton Instance pointer
 * \return Copy of instance pointer
 */

ServiceBase* ServiceManager::Manage (ServiceBase* singleton)
{
    services.push_back (singleton);
    return singleton;
}

/** Get the ServiceManager instance.
 * \return Reference to the one and only SingletonRegistry
 */

ServiceManager& ServiceManager::Get()
{
    static QScopedPointer<ServiceManager> instance(new ServiceManager());
    return *instance;
}


/** Saves the command line parameters.
 */
void ServiceManager::SetCommandLine (int argc, char** argv)
{
    this->argc = argc;
    this->argv = argv;
}

/** Retrieves the number of command line parameters that have been previously
 * set with SetCommandLine().
 */
int& ServiceManager::GetCommandLineCount()
{
    return argc;
}

/** Retrieves the values of command line parameters that have been previously
 * set with SetCommandLine().
 */
char** ServiceManager::GetCommandLineValues()
{
    return argv;
}
