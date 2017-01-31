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
 
#ifndef RUNDELEGATEDEFINES_H
#define RUNDELEGATEDEFINES_H

#include <map>
#include <list>

#include <QMap>

namespace apex
{
  namespace data
  {
    class ScreenElement;
  }

  namespace rundelegates
  {
    using data::ScreenElement;
    class ScreenElementRunDelegate;
    class SpinBoxRunDelegate;
	class CheckBoxRunDelegate;

    typedef QMap<const ScreenElement*, ScreenElementRunDelegate*> ElementToRunningMap;
      
    typedef std::list<const SpinBoxRunDelegate*> spinBoxListT;
	typedef std::list<const CheckBoxRunDelegate*> checkBoxListT;

  }
}

#endif
