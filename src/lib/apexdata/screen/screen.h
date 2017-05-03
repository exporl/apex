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

#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_SCREEN_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_SCREEN_H_

#include "apextools/apextypedefs.h"
#include "apextools/global.h"

#include <QString>

#include <vector>

class QWidget;

namespace apex
{

namespace data
{

class ScreenElement;
class ButtonGroup;

using gui::ScreenElementMap;

/**
 * The Screen class represents a single screen.  It contains only
 * data about how the screen will look, and does not know anything
 * about any on-screen representation with QWidgets and stuff.  If
 * that is what you are looking for, you should be looking at the
 * ScreenRunDelegate class.
 *
 * So: this class contains the data as parsed from the xml.  It
 * knows about how the screen looks on screen, with the containing
 * ScreenElement's, it has a id-to-ScreenElement map, it can
 * contain a buttongroup ( list of id's grouped together ), and
 * knows about a default answer element.
 */
class APEXDATA_EXPORT Screen
{
    public:
        /**
         * Create a new screen.  rootElement and buttonGroup must have
         * been allocated with new, and this Screen takes ownership of
         * them.  That is, it will delete them in its destructor.
         * Consistency between the idToElementMap and the real
         * ScreenElements beneath rootElement is assumed.  It is also
         * assumed that the rootElement is some kind of layout element.
         *
         * The root element is the base of the ScreenElement tree.
         *
         * Normally this is only called by the parser.
         */
        Screen(const QString& id, ScreenElement* rootElement,
               const ScreenElementMap& idToElementMap,
               ButtonGroup* buttonGroup = 0,
               const QString& defaultAnswer = QString());
        virtual ~Screen();

        const ScreenElementMap& idToElementMap() const;
        const QString getDefaultAnswerElement() const;
        ScreenElement* getRootElement();
        const ScreenElement* getRootElement() const;
        void setRootElement(ScreenElement* e);
        ButtonGroup* getButtonGroup();
        const ButtonGroup* getButtonGroup() const;
        const QString getID() const;
        void setElementID(ScreenElement* e, const QString& id);
        const ScreenElement* elementById(const QString& id) const;
        QString elementTextById(const QString& id) const;
        /**
         * Let the Screen know about a newly created ScreenElement.
         */
        void addElement(ScreenElement* e);
        void deleteElement(ScreenElement* e);

        bool operator==(const Screen& other) const;
        bool operator!=(const Screen& other) const;

    private:
        ScreenElementMap m_idToElementMap;
        ScreenElement* rootElement;
        ButtonGroup* buttonGroup;
        QString defaultAnswer;
        QString id;
};

}
}

#endif
