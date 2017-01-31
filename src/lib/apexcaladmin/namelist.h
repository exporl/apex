/******************************************************************************
 * Copyright (C) 2014  Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>  *
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
#ifndef _EXPORL_SRC_LIB_APEXCALADMIN_NAMELIST_H_
#define _EXPORL_SRC_LIB_APEXCALADMIN_NAMELIST_H_

#include "apextools/global.h"

#include <QCoreApplication>
#include <QList>
#include <QString>

#include <stdexcept>

/*!
 * \brief The NameList class.
 * A list where one can get the elements by name. The name is a function of the item in the list.
 */
template <typename T>
class NameList : public QList<T>{
    Q_DECLARE_TR_FUNCTIONS(NameList)
public:
    NameList() {}

    bool contains(const QString& name) const {
        for(int i=0; i<this->count(); ++i){
            const T e = this->at(i);
            if(e.name() == name) {
                return true;
            }
        }
        return false;
    }

    T get(const QString& name) const {
        for(int i=0; i<this->count(); ++i){
            const T e = this->at(i);
            if(e.name() == name) {
                return e;
            }
        }
        throw std::range_error(tr("No element with name %1").arg(name).toStdString());
    }
};

#endif // _EXPORL_SRC_LIB_APEXCALADMIN_NAMELIST_H_
