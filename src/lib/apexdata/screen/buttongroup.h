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

#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QList>
#include <QString>
#include "factoryelement.h"

#include "global.h"

namespace apex
{
namespace data
{
/**
 * ButtonGroup
 *   a list containing the ID's of the ScreenElements that can
 *   accept user input.
 */
class APEXDATA_EXPORT ButtonGroup : /*public std::list<QString>,*/ public FactoryElement
{
    public:
        /**
         * Constructor.
         * @param ac_sID a unique identifier
         */
        ButtonGroup(const QString& ac_sID);

        /**
         * Used to iterate the button ids. I know it looks strange but M$VC
         * cannot handle template inheritance across libraries...
         */
        struct APEXDATA_EXPORT const_iterator
        {
            const_iterator(const QList<QString>::const_iterator& it);
            const_iterator() {}
            
            const_iterator& operator=(const QList<QString>::const_iterator& it);
            const_iterator& operator++(); //prefix++
            const_iterator  operator++(int); //postfix++
            bool operator==(const const_iterator& other) const;
            bool operator!=(const const_iterator& other) const;
            const QString& operator*() const;

            private:

                QList<QString>::const_iterator it;
        };

        QList<QString>::const_iterator begin() const;
        QList<QString>::const_iterator end() const;
        int size() const;
        void append(const QString& id);

        const QString getID() const
        {
            return id;
        }

        /**
         * Check whether the buttongroup contains an element with
         * the given ID
         * @param ac_sID
         */
        bool IsElement(const QString& ac_sID) const;
        
        bool operator==(const ButtonGroup& other) const;
    private:

        QList<QString> buttonIds;
        
        const QString id;
};

}
}

#endif
