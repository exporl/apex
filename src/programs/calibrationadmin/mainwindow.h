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
#ifndef _EXPORL_SRC_PROGRAMS_CALIBRATIONADMIN_MAINWINDOW_H_
#define _EXPORL_SRC_PROGRAMS_CALIBRATIONADMIN_MAINWINDOW_H_

#include <QMainWindow>

struct Ui;
class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QMainWindow *parent = 0);
    ~MainWindow();
private slots:
    void showInfo(QListWidgetItem *item);
    void makeLocal();
    void makeGlobal();
private:
    Ui *ui;

    void fillSetups();
    void fillLocalSetups();
    void fillGlobalSetups();
};

#endif // _EXPORL_SRC_PROGRAMS_CALIBRATIONADMIN_MAINWINDOW_H_
