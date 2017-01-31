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

#ifndef APEX_ERRORLOGGER_H
#define APEX_ERRORLOGGER_H

#include "statusdispatcher.h"
#include "global.h"

#include <QCoreApplication>

#include <memory>

namespace apex
{

/// Derive from this class to get some basic error logging
class APEXTOOLS_EXPORT ErrorLogger
{
        Q_DECLARE_TR_FUNCTIONS(ErrorLogger)

    public:

        virtual ~ErrorLogger();

        /**
         * After a call to this method, all errors will also be reported
         * to the given StatusReporter.
         *
         * @note We don't take ownership so it's the callers responsibility
         *       to keep the pointer valid until destruction of this object.
         */
        void addErrorListener(StatusReporter* listener);

        /**
         * Returns a read-only reference to the StatusReporter.
         */
        const StatusReporter& logger() const;

        /**
         * Clear the StatusReporter by calling StatusReporter::clear()
         */
        void clearLog();

    protected:

        ErrorLogger();

        /**
         * Returns a writable reference to the StatusReporter to be used by
         * the subclass.
         */
        StatusReporter& log() const;

    private:

        mutable std::auto_ptr<StatusDispatcher> errors;
};
}


#endif
