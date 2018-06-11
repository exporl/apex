/******************************************************************************
 * Copyright (C) 2014  Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>  *
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

#include "apexcaladmin/calibrationdatabase.h"

#include "mainwindow.h"

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>

struct Ui {
    QGridLayout *layout;
    QPushButton *makeLocal;
    QPushButton *makeGlobal;

    QLabel *local;
    QListWidget *localSetups;

    QLabel *global;
    QListWidget *globalSetups;

    QLabel *info;

    QWidget *centralWidget;

    ~Ui()
    {
        delete layout;
        delete makeLocal;
        delete makeGlobal;
        delete local;
        delete localSetups;
        delete global;
        delete globalSetups;
        delete info;
        delete centralWidget;
    }
};

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent)
{
    ui = new Ui();
    ui->layout = new QGridLayout();
    ui->makeLocal = new QPushButton("<-");
    ui->makeGlobal = new QPushButton("->");
    ui->localSetups = new QListWidget();
    ui->globalSetups = new QListWidget();
    ui->info = new QLabel();
    ui->info->setAlignment(Qt::AlignTop);
    ui->local = new QLabel(tr("Local setups"));
    ui->global = new QLabel(tr("Global setups"));
    ui->centralWidget = new QWidget();

    connect(ui->localSetups, SIGNAL(itemClicked(QListWidgetItem *)), this,
            SLOT(showInfo(QListWidgetItem *)));
    connect(ui->globalSetups, SIGNAL(itemClicked(QListWidgetItem *)), this,
            SLOT(showInfo(QListWidgetItem *)));

    connect(ui->makeLocal, SIGNAL(clicked()), this, SLOT(makeLocal()));
    connect(ui->makeGlobal, SIGNAL(clicked()), this, SLOT(makeGlobal()));

    int localColumn = 0;
    int buttonColumn = 1;
    int globalColumn = 2;
    int infoColumn = 3;
    int labelRow = 0;
    int setupRow = 1;

    ui->layout->addWidget(ui->local, labelRow, localColumn);
    ui->layout->addWidget(ui->localSetups, setupRow, localColumn, 5, 1);

    ui->layout->addWidget(ui->makeLocal, 1, buttonColumn);
    ui->layout->addWidget(ui->makeGlobal, 3, buttonColumn);

    ui->layout->addWidget(ui->global, labelRow, globalColumn);
    ui->layout->addWidget(ui->globalSetups, setupRow, globalColumn, 5, 1);

    ui->layout->addWidget(ui->info, 0, infoColumn, 6, 1);

    fillSetups();

    ui->centralWidget->setLayout(ui->layout);
    setCentralWidget(ui->centralWidget);

    CalibrationDatabase cd;
    if (!cd.isWritable()) {
        QMessageBox msg;
        msg.setText(tr("Can't write the settings, are you running this program "
                       "as administrator?\nYou are in read only mode now."));
        msg.exec();

        ui->makeGlobal->setDisabled(true);
        ui->makeLocal->setDisabled(true);
        this->statusBar()->showMessage(tr("You are in read only mode."));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fillSetups()
{
    fillLocalSetups();
    fillGlobalSetups();
}

void MainWindow::fillLocalSetups()
{
    ui->localSetups->clear();
    CalibrationDatabase cd;
    HardwareSetups setups = cd.localHardwareSetups();
    foreach (HardwareSetup setup, setups) {
        ui->localSetups->addItem(setup.name());
    }
}

void MainWindow::fillGlobalSetups()
{
    ui->globalSetups->clear();
    CalibrationDatabase cd;
    HardwareSetups setups = cd.globalHardwareSetups();
    foreach (HardwareSetup setup, setups) {
        ui->globalSetups->addItem(setup.name());
    }
}

void MainWindow::makeLocal()
{
    QList<QListWidgetItem *> items = ui->globalSetups->selectedItems();
    if (items.count() != 1) {
        QMessageBox msg;
        msg.setText(tr("Please select 1 item form the global column"));
        msg.exec();
        return;
    }

    CalibrationDatabase cd;
    HardwareSetup setup = cd.setup(items.first()->text());

    try {
        cd.makeLocal(setup);
        statusBar()->showMessage(
            tr("Hardware setup %1 is made local.").arg(setup.name()));
    } catch (const std::domain_error &err) {
        QMessageBox msg;
        msg.setText(err.what());
        msg.exec();
        statusBar()->showMessage(
            tr("Couldn't make hardware setup %1 local.").arg(setup.name()));
    }

    fillSetups();
}

void MainWindow::makeGlobal()
{
    QList<QListWidgetItem *> items = ui->localSetups->selectedItems();
    if (items.count() != 1) {
        QMessageBox msg;
        msg.setText(tr("Please select 1 item form the local column"));
        msg.exec();
        return;
    }

    CalibrationDatabase cd;
    HardwareSetup setup = cd.setup(items.first()->text());
    try {
        cd.makeGlobal(setup);
        statusBar()->showMessage(
            tr("Hardware setup %1 is made global.").arg(setup.name()));
    } catch (const std::domain_error &err) {
        QMessageBox msg;
        msg.setText(err.what());
        msg.exec();
        statusBar()->showMessage(
            tr("Couldn't make hardware setup %1 global.").arg(setup.name()));
    }

    fillSetups();
}

void MainWindow::showInfo(QListWidgetItem *item)
{
    if (item->listWidget() == ui->localSetups) {
        ui->globalSetups->setCurrentItem(0);
    } else {
        ui->localSetups->setCurrentItem(0);
    }

    CalibrationDatabase cd;
    HardwareSetup setup = cd.setup(item->text());
    QStringList profiles = setup.profiles();

    QString text = "Setup: " + setup.name();
    foreach (QString profile, profiles) {
        text += "\n";
        text += "\t" + profile;
    }

    ui->info->setText(text);
}
