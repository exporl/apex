/******************************************************************************
 * Copyright (C) 2014 Michael Hofmann <mh21@mh21.de>                          *
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

#include "apexmain/calibration/soundlevelmeter.h"

#include "apextools/apexpluginloader.h"
#include "apextools/global.h"

#include "common/pluginloader.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QMainWindow>
#include <QTimer>

#include <QtDebug>

using namespace apex;
using namespace cmn;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(const QString &plugin, QMainWindow *parent = 0);
    ~MainWindow();

private Q_SLOTS:
    void update();

private:
    QString style;
    QLabel *label;
    QScopedPointer<SoundLevelMeter> slm;
};

MainWindow::MainWindow(const QString &plugin, QMainWindow *parent)
    : QMainWindow(parent),
      style(QString::fromLatin1("font: %1px; color: black;")
                .arg(QApplication::desktop()->availableGeometry().width() / 5 *
                     2))
{
    this->setWindowTitle(tr("Sound Level Meter"));

    this->label = new QLabel(QL1S("big"), this);
    this->label->setAlignment(Qt::AlignCenter);
    this->label->setStyleSheet(style);
    setCentralWidget(this->label);

    QList<SoundLevelMeterPluginCreator *> available =
        PluginLoader().availablePlugins<SoundLevelMeterPluginCreator>();
    Q_FOREACH (SoundLevelMeterPluginCreator *creator, available) {
        qCDebug(APEX_RS, "Available plugins: %s",
                qPrintable(creator->availablePlugins().join(QL1S(", "))));
    }
    QString name(plugin.length() > 0 ? plugin : QL1S("dummyslmslave"));

    SoundLevelMeterPluginCreator *creator =
        createPluginCreator<SoundLevelMeterPluginCreator>(name);
    this->slm.reset(creator->createSoundLevelMeter(name));
    this->slm->setTransducer("4196"); // TODO
    this->slm->startMeasurement(SoundLevelMeter::SPL_Measurement,
                                SoundLevelMeter::A_Weighting,
                                SoundLevelMeter::FastWeighting);

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

void MainWindow::update()
{
    if (this->slm->measure()) {
        double value = this->slm->result();
        this->label->setText(QString::fromLatin1("%1").arg(value, 0, 'f', 1));
        this->label->setStyleSheet(
            QString::fromLatin1("%1; background-color: %2;")
                .arg(this->style,
                     value > 100
                         ? QL1S("lightred")
                         : value > 80 ? QL1S("yellow") : QL1S("lightgreen")));
    }
}

MainWindow::~MainWindow()
{
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window(app.arguments().value(1));
    window.show();
    return app.exec();
}

#include "largeslm.moc"
