/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sanne.raymaekers@gmail.com>           *
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

#include "commonguitest.h"

#include "common/global.h"
#include "common/testutils.h"

#include "commongui/listpicker.h"

#include <QListWidgetItem>
#include <QPushButton>
#include <QTest>
#include <QTimer>

using namespace cmn;

void CommonGuiTest::listPickerTest()
{
    TEST_EXCEPTIONS_TRY

    ListPicker picker(QSL("someTitle"));
    picker.addItem(QSL("item1"), QVariant(QL1S("item1-data")));
    picker.addItem(QSL("item2"), QVariant(QL1S("item2-data")));
    QTimer::singleShot(1000, [&]() { picker.reject(); });
    QTimer::singleShot(200, [&]() {
        QListWidget *listWidget =
            picker.findChild<QListWidget *>(QL1S("listWidget"));
        QListWidgetItem *item = listWidget->item(1);
        QRect rect = listWidget->visualItemRect(item);
        QTest::mouseClick(listWidget->viewport(), Qt::LeftButton, 0,
                          rect.center());
#ifndef Q_OS_ANDROID
        QTest::mouseClick(picker.findChild<QPushButton *>(QL1S("selectButton")),
                          Qt::LeftButton);
#endif
    });
    QCOMPARE(picker.exec(), (int)QDialog::Accepted);
    QCOMPARE(picker.selectedItem(), QSL("item2"));
    QCOMPARE(picker.selectedData().toString(), QSL("item2-data"));

    TEST_EXCEPTIONS_CATCH
}
