/******************************************************************************
 * Copyright (C) 2011  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
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

/** @file
 * Debug function declaration.
 */

#ifndef _EXPORL_COMMON_LIB_COMMON_COMMONAPPLICATION_H_
#define _EXPORL_COMMON_LIB_COMMON_COMMONAPPLICATION_H_

#include "global.h"

#include <QCoreApplication>

namespace cmn
{

COMMON_EXPORT void registerCommonMetaTypes();

class COMMON_EXPORT CommonCoreApplication : public QCoreApplication
{
public:
    CommonCoreApplication(int &argc, char **argv);

    virtual bool notify(QObject *receiver, QEvent *event);
};

} // namespace cmn

#endif
