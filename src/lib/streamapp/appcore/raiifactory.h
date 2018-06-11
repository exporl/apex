/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef __RAII_FACTORY_IMP_H_
#define __RAII_FACTORY_IMP_H_

#include <QtGlobal>

#include <algorithm>
#include <vector>

namespace appcore
{

/**
  * RaiiFactoryImp
  *   reusable implementation class template for RAII factories
  *   see google for what RAII is.
  ************************************************************* */
template <typename T>
class RaiiFactoryImp
{
public:
    /**
      * Constructor.
      * Prepares the array.
      */
    RaiiFactoryImp()
    {
        expand();
    }

    /**
      * Destructor.
      * Delete created objects.
      */
    virtual ~RaiiFactoryImp()
    {
        disposeAll();
    }

    /**
      * Take over an object.
      * @param p pointer to the object
      */
    T *keep(T *p)
    {
        Q_ASSERT(p);
        container.push_back(p);
        expand();
        return p;
    }

    /**
      * Delete object.
      * The object must have been created by the factory that uses this
     * RaiiFactoryImp.
      * @param p pointer to the object
      */
    void dispose(const T *p)
    {
        Q_ASSERT(p);
        typedef std::vector<const void *>::reverse_iterator reverse_iterator;
        reverse_iterator rpos =
            std::find(container.rbegin(), container.rend(), p);
        if (rpos != container.rend()) {
            container.erase((++rpos).base()); // see: Scott Meyers -
            // http://www.aristeia.com/Papers/CUJ_June_2001.pdf
            // - Guideline 3:
            doDelete(p);
        } else {
            Q_ASSERT(0 &&
                     "unknown pointer: object not created by this factory");
        }
    }

    /**
      * Delete all objects in reverse order of takeover.
      */
    void disposeAll()
    {
        while (!container.empty()) {
            const void *p = container.back();
            container.pop_back();
            doDelete(p);
        }
    }

    /**
      * Get the current number of owned objects.
      * @return the size
      */
    size_t size() const
    {
        return container.size();
    }

    /**
      * Get an object.
      * @param n the index
      * @return the object at n
      */
    T *operator[](size_t n)
    {
        Q_ASSERT(n < size());
        return const_cast<T *>(static_cast<const T *>(container[n]));
    }

    /**
      * Get a const object.
      * @param n the index
      * @return the object at n
      */
    const T *operator[](size_t n) const
    {
        Q_ASSERT(n < size());
        return static_cast<const T *>(container[n]);
    }

private:
    /**
      * Expand container size.
      */
    void expand()
    {
        if (container.capacity() < container.size() + 1) {
            if (container.size() == 0) {
                container.reserve(16);
            } else if (container.size() < 8192) {
                container.reserve(container.capacity() * 2);
            } else {
                container.reserve(container.capacity() / 2 * 3);
            }
        }
    }

    /**
      * Delete an object.
      * @param p pointer to the object
      */
    void doDelete(const void *p) const
    {
        try {
            delete static_cast<const T *>(p);
        } catch (...) {
            Q_ASSERT(0);
        }
    }

    std::vector<const void *> container;

    //!< non-copyable
    RaiiFactoryImp(const RaiiFactoryImp &);
    RaiiFactoryImp &operator=(const RaiiFactoryImp &);
};
}

#endif //#ifndef __RAII_FACTORY_IMP_H_
