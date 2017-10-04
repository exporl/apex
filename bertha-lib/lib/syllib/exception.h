/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#ifndef _BERTHA_SRC_LIB_SYLLIB_EXCEPTION_H_
#define _BERTHA_SRC_LIB_SYLLIB_EXCEPTION_H_

/** @file
 * Exception class declaration.
 */

#include "global.h"

#include <QByteArray>
#include <QString>

namespace syl
{

/** Exception that provides a simple error message. All derived classes must be
 * marked with SYL_EXPORT, otherwise exceptions will just terminate()!
 */
class SYL_EXPORT SylError : public std::exception
{
public:
    /** Creates a new instance with the given error message. Please use a
     * meaningful description that makes it possible to isolate the error
     * position. Converts the QString to the local encoding with
     * QString#toLocal8Bit() before saving it as a char array that will be
     * returned by #what().
     *
     * @param message error message
     *
     * @see what()
     */
    SylError(const QString &message) throw() : message(message.toLocal8Bit())
    {
    }

    /** Virtual destructor to make the compiler happy. */
    virtual ~SylError() throw()
    {
    }

    /** Returns the error message.
     *
     * @return error message
     */
    virtual const char *what() const throw()
    {
        return message;
    }

private:
    const QByteArray message;
};

} // namespace syl

#endif
