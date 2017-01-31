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
  *      -- v3tov2 plugin --
  * Do not modify this file by hand,
  * your changes will be overwritten.
  * <^>
  * [ prog ] apex
  * [ file ] streamapp_namespace.h
  * [ fmod ] default
  * <^>
  */

  /**
    * @file streamapp_namespace.h
    * Defines main namespace macros.
    * We use a macro to allow having no
    * main namespace at all.
    */

#ifndef __STR_STREAMAPP_NAMESPACE_H__
#define __STR_STREAMAPP_NAMESPACE_H__

#ifdef streamapp_main_namespace

  #define S_NAMESPACE_BEGIN       namespace streamapp_main_namespace{
  #define S_NAMESPACE_END         }
  #define S_NAMESPACE_USING       using namespace streamapp_main_namespace;
  #define S_CLASS_USING( Class )  using streamapp_main_namespace::Class;
  #define smn                     streamapp_main_namespace

#else

  #define S_NAMESPACE_BEGIN
  #define S_NAMESPACE_END
  #define S_NAMESPACE_USING
  #define S_CLASS_USING( Class )  using ::Class;

#endif

#endif //#ifndef __STR_STREAMAPP_NAMESPACE_H__
