/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
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

#ifndef _BERTHA_SRC_LIB_BERTHA_GLOBAL_H_
#define _BERTHA_SRC_LIB_BERTHA_GLOBAL_H_

#include "common/global.h"

#include <QtGlobal>

#define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__ * 10 + __GNUC_PATCHLEVEL)

#if defined(Q_CC_MSVC) || defined(Q_OS_WIN32)
#define BERTHA_EXPORT_DECL __declspec(dllexport)
#define BERTHA_IMPORT_DECL __declspec(dllimport)
#else
// Test for GCC >= 4.0.0
#if GCC_VERSION >= 400
#define BERTHA_EXPORT_DECL __attribute__((visibility("default")))
#define BERTHA_IMPORT_DECL
#else
#define BERTHA_EXPORT_DECL
#define BERTHA_IMPORT_DECL
#endif
#endif

#ifdef BERTHA_MAKEDLL
#define BERTHA_EXPORT BERTHA_EXPORT_DECL
#else
#define BERTHA_EXPORT BERTHA_IMPORT_DECL
#endif

#define BERTHA_D(Class)                                                        \
    Class##Private *const d = static_cast<Class##Private *>(this->d);

#define BERTHA_SD(Class)                                                       \
    Class##Private *const d = static_cast<Class##Private *>(this->d.data());

#endif

// Q_DECLARE_LOGGING_CATEGORY
extern BERTHA_EXPORT const QLoggingCategory &EXPORL_BERTHA();
