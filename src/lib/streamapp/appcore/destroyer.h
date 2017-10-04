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

#ifndef __DESTROYER_H_
#define __DESTROYER_H_

#include <QtGlobal>

namespace appcore
{

/**
  * Destroyer
  *   the element that is set 'doomed'
  *   will be deleted when the destructor is called.
  ************************************************** */
template <class tType>
class Destroyer
{
public:
    /**
      * Constructor.
      */
    Destroyer() : m_pDoomed(0)
    {
    }

    /**
      * Destructor.
      * Deletes the doomed object, if any.
      */
    ~Destroyer()
    {
        delete m_pDoomed;
    }

    /**
      * Set the single doomed object.
      * Must be used only once.
      * @param a_pDoomed the object to be deleted
      */
    void SetDoomed(tType *a_pDoomed)
    {
        if (m_pDoomed)
            Q_ASSERT(
                0 &&
                "this destroyer object already has a resource assigned to it");
        m_pDoomed = a_pDoomed;
    }

private:
    /**
      * Prevent users from making copies of a
      * Destroyer to avoid double deletion.
      */
    Destroyer(const Destroyer<tType> &);
    void operator=(const Destroyer<tType> &);

    tType *m_pDoomed;
};
}

#endif //#ifndef __DESTROYER_H_
