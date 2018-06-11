/******************************************************************************
 * Copyright (C) 2018  Sanne Raymaekers <sanne.raymaekers@gmail.com>         *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
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

#ifndef _EXPORL_COMMON_LIB_COMMONGUI_LISTPICKER_H_
#define _EXPORL_COMMON_LIB_COMMONGUI_LISTPICKER_H_

#include "common/global.h"

#include <QDialog>
#include <QVariant>

namespace cmn
{
class ListPickerPrivate;

class COMMON_EXPORT ListPicker : public QDialog
{
    Q_OBJECT
public:
    ListPicker(const QString &title);
    virtual ~ListPicker();

    void addItem(const QString &item, const QVariant &data = QVariant());

    const QString &selectedItem() const;
    const QVariant &selectedData() const;

private:
    QScopedPointer<ListPickerPrivate> d;
};
}

#endif
