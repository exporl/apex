/******************************************************************************
 * Copyright (C) 2011  Michael Hofmann <mh21@piware.de>                       *
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

#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"

#include "common/utils.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>

using namespace bertha;
using namespace cmn;

class MainWindowPrivate : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void on_lowerFrequency_pressed();
    void on_higherFrequency_pressed();

    void on_lessIntensity_pressed();
    void on_moreIntensity_pressed();

    void on_left_toggled(bool checked);
    void on_stop_pressed();
    void on_right_toggled(bool checked);

public:
    void createProcessor();
    void updateUi();
    void updateProcessor();
    void stopPlayback();
    QVariant gainMatrix(double left, double right);

public:
    MainWindow *p;

    QScopedPointer<ExperimentProcessor> processor;

    Ui::MainWindow ui;

    QList<unsigned> frequencies;
    unsigned frequencyIndex;

    double intensityStep;
    double intensity;
    double calibrationAmplitude; // corresponding to 0dB
    bool playing;
};

void MainWindowPrivate::createProcessor()
{
    RETURN_IF_FAIL(!processor);

    ExperimentData data;

#ifdef Q_OS_WIN32
    DeviceData deviceData(QLatin1String("device"), QLatin1String("Asio"));
#else
    DeviceData deviceData(QLatin1String("device"), QLatin1String("Jack"));
#endif
    deviceData.setInputPorts(2);
    deviceData.setOutputPorts(0);
    deviceData.setSampleRate(48000);
    deviceData.setBlockSize(2048);
    data.setDevice(deviceData);

    BlockData block(QLatin1String("sine"), QLatin1String("SineGenerator"));
    block.setParameter(QLatin1String("frequency"), 0);
    block.setParameter(QLatin1String("amplitude"), calibrationAmplitude);
    data.addBlock(block);

    BlockData gainBlock(QLatin1String("mixer"), QLatin1String("Mixer"));
    gainBlock.setOutputPorts(2);
    gainBlock.setParameter(QLatin1String("logarithmicGain"), true);
    gainBlock.setParameter(QLatin1String("matrix"), gainMatrix(-120, -120));
    data.addBlock(gainBlock);

    data.addConnection(
        ConnectionData(QLatin1String("sine"), QLatin1String("output-1"),
                       QLatin1String("mixer"), QLatin1String("input-1")));

    data.addConnection(
        ConnectionData(QLatin1String("mixer"), QLatin1String("output-1"),
                       QLatin1String("device"), QLatin1String("playback-1")));
    data.addConnection(
        ConnectionData(QLatin1String("mixer"), QLatin1String("output-2"),
                       QLatin1String("device"), QLatin1String("playback-2")));

    processor.reset(new ExperimentProcessor(data));
    processor->prepare();
    processor->start();
    // TODO: Handle exceptions
}

QVariant MainWindowPrivate::gainMatrix(double left, double right)
{
    return QVariant::fromValue(QVector<QVector<double>>()
                               << (QVector<double>() << left)
                               << (QVector<double>() << right));
}

void MainWindowPrivate::updateUi()
{
    ui.frequency->setText(QString::number(frequencies.value(frequencyIndex)));
    ui.intensity->setText(QString::number(intensity));
}

void MainWindowPrivate::updateProcessor()
{
    RETURN_IF_FAIL(processor);

    processor->setParameterValue(QLatin1String("sine"),
                                 QLatin1String("frequency"),
                                 frequencies.value(frequencyIndex));
    processor->setParameterValue(
        QLatin1String("mixer"), QLatin1String("matrix"),
        gainMatrix(playing && ui.left->isChecked() ? intensity : -120,
                   playing && ui.right->isChecked() ? intensity : -120));
    // TODO: Handle exceptions
}

void MainWindowPrivate::stopPlayback()
{
    RETURN_IF_FAIL(processor);

    processor.reset();
    p->releaseKeyboard();
    ui.left->setChecked(false);
    ui.right->setChecked(false);
    ui.stop->setEnabled(false);
}

void MainWindowPrivate::on_lowerFrequency_pressed()
{
    RETURN_IF_FAIL(frequencyIndex > 0);
    --frequencyIndex;

    updateUi();
    updateProcessor();
}

void MainWindowPrivate::on_higherFrequency_pressed()
{
    RETURN_IF_FAIL(frequencyIndex + 1 < unsigned(frequencies.count()));

    ++frequencyIndex;

    updateUi();
    updateProcessor();
}

void MainWindowPrivate::on_lessIntensity_pressed()
{
    intensity -= intensityStep;

    updateUi();
    updateProcessor();
}

void MainWindowPrivate::on_moreIntensity_pressed()
{
    intensity += intensityStep;

    updateUi();
    updateProcessor();
}

void MainWindowPrivate::on_left_toggled(bool checked)
{
    if (checked) {
        if (!processor) {
            createProcessor();
            p->grabKeyboard();
        }
        ui.right->setChecked(false);
        ui.stop->setEnabled(true);
        updateProcessor();
    } else if (!ui.right->isChecked()) {
        stopPlayback();
    }
}

void MainWindowPrivate::on_right_toggled(bool checked)
{
    if (checked) {
        if (!processor) {
            createProcessor();
            p->grabKeyboard();
        }
        ui.left->setChecked(false);
        ui.stop->setEnabled(true);
        updateProcessor();
    } else if (!ui.left->isChecked()) {
        stopPlayback();
    }
}

void MainWindowPrivate::on_stop_pressed()
{
    RETURN_IF_FAIL(processor);

    stopPlayback();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), d(new MainWindowPrivate)
{
    d->p = this;

    d->frequencies << 250 << 500 << 750 << 1000 << 1500 << 2000 << 3000 << 4000
                   << 6000 << 8000;
    d->frequencyIndex = 3;
    d->intensity = 40;
    d->intensityStep = 5;
    d->calibrationAmplitude = 0.002;
    d->playing = false;

    d->ui.setupUi(this);

    connectSlotsByNameToPrivate(this, d);

    d->updateUi();
}

MainWindow::~MainWindow()
{
    delete d;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        if (event->isAutoRepeat()) {
            event->ignore();
            return;
        }

        event->accept();

        d->playing = true;
        d->updateProcessor();

        return;
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        if (event->isAutoRepeat()) {
            event->ignore();
            return;
        }

        event->accept();

        d->playing = false;
        d->updateProcessor();

        return;
    }

    QMainWindow::keyReleaseEvent(event);
}

#include "mainwindow.moc"
