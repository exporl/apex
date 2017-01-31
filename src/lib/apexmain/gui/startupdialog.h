/******************************************************************************
 * Copyright (C) 2010  Michael Hofmann <mh21@piware.de>                       *
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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_STARTUPDIALOG_H_
#define _EXPORL_SRC_LIB_APEXMAIN_GUI_STARTUPDIALOG_H_

#include <QDialog>

namespace apex
{

class StartupDialogPrivate;

class StartupDialog : public QDialog
{
    Q_OBJECT
public:
    StartupDialog(const QStringList &recent, QWidget *parent = NULL);

    /** Destroys the information dialog. */
    ~StartupDialog();

    QString selectedFile() const;

private:
    StartupDialogPrivate * const d;
};

} // namespace apex

#endif
