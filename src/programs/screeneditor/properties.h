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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_PROPERTIES_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_PROPERTIES_H_

/**
 * This enumeration contains a list of property types.  These are used
 * by \ref ScreenElementEditorDelegate classes and determine how the
 * \ref PropertiesEditor class will handle a certain property of a
 * certain ScreenElementEditorDelegate.  The \ref PropertiesEditor
 * needs to support a certain PropertyType in order for it to be
 * useful (editable).
 */
enum PropertyType {
    StringPropertyType,   // any QString
    IntPropertyType,      // any integer
    FontSizePropertyType, // basically an integer, but restricted to sane font sizes
    PositiveIntPropertyType, // positive integer
    BooleanPropertyType,  // boolean
    IDPropertyType,       // a valid XML id
    ColorPropertyType,    // a color
    ShortcutPropertyType, // a shortcut
    FontPropertyType,              // a font
    ImagePropertyType,             // an image
    FlashPropertyType,             // a flash movie
    ArcLayoutTypePropertyType,     // the direction of an arc layout
    InputMaskPropertyType,         // an input mask for a text edit..
    Double01PropertyType,          // a double between 0 and 1
    TwoPartLayoutDirectionPropertyType
};

#endif
