/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "snrwidget.h"

using namespace spin::gui;

SnrWidget::SnrWidget(QWidget *parent) : QWidget(parent), updatingSNR(false)
{
    setupUi();
    setupConnections();

    widgets.snrNoiseRadio->setChecked(true); // FIXME
}

void SnrWidget::setupUi()
{
    widgets.setupUi(this);

    // center everything in the grid layout
    for (int i = 0; i < widgets.snrLayout->count(); i++)
        widgets.snrLayout->itemAt(i)->setAlignment(Qt::AlignCenter);
}

void SnrWidget::setupConnections()
{
    // connections for enabling/disabling input fields
    connect(widgets.snrSpeechRadio, SIGNAL(toggled(bool)),
            widgets.speechlevelLine, SLOT(setEnabled(bool)));
    connect(widgets.snrSpeechRadio, SIGNAL(toggled(bool)),
            widgets.snrSpeechLine, SLOT(setEnabled(bool)));
    connect(widgets.snrNoiseRadio, SIGNAL(toggled(bool)),
            widgets.noiselevelLine, SLOT(setEnabled(bool)));
    connect(widgets.snrNoiseRadio, SIGNAL(toggled(bool)), widgets.snrNoiseLine,
            SLOT(setEnabled(bool)));

    // connection to lock the levels
    connect(widgets.lockLevelsBtn, SIGNAL(clicked()), this,
            SIGNAL(lockButtonClicked()));

    // update connections
    connect(widgets.speechlevelLine, SIGNAL(levelChanged(double)), this,
            SLOT(updateSnr()));
    connect(widgets.noiselevelLine, SIGNAL(levelChanged(double)), this,
            SLOT(updateSnr()));
    connect(widgets.snrSpeechLine, SIGNAL(levelChanged(double)), this,
            SLOT(updateSnr()));
    connect(widgets.snrNoiseLine, SIGNAL(levelChanged(double)), this,
            SLOT(updateSnr()));

    // connections between corresponding level fields
    connect(widgets.snrSpeechLine, SIGNAL(levelChanged(double)),
            widgets.snrNoiseLine, SLOT(setLevel(double)));
    connect(widgets.snrNoiseLine, SIGNAL(levelChanged(double)),
            widgets.snrSpeechLine, SLOT(setLevel(double)));
    connect(widgets.speechlevelLine, SIGNAL(levelChanged(double)),
            widgets.speechlevelLbl, SLOT(setLevel(double)));
    connect(widgets.noiselevelLine, SIGNAL(levelChanged(double)),
            widgets.noiselevelLbl, SLOT(setLevel(double)));
}

void SnrWidget::showSnr()
{
    widgets.snrStack->setCurrentIndex(SNR);
}

void SnrWidget::showWarning()
{
    widgets.snrStack->setCurrentIndex(WARNING);
}

void SnrWidget::setSpeechlevel(double level)
{
    widgets.speechlevelLine->setLevel(level);
}

void SnrWidget::setNoiselevel(double level)
{
    widgets.noiselevelLine->setLevel(level);
}

double SnrWidget::calculateSnr(double speech, double noise)
{
    return speech - noise;
}

double SnrWidget::calculateSpeechlevel(double snr, double noise)
{
    return snr + noise;
}

double SnrWidget::calculateNoiselevel(double snr, double speech)
{
    return speech - snr;
}

void SnrWidget::updateSnr()
{
    // bool is set to true when updating snr. this to prevent this method to be
    // called when updating (because of the connections)
    if (updatingSNR)
        return;

    // check if we have enough data
    bool enough = (widgets.snrSpeechRadio->isChecked() &&
                   widgets.snrSpeechLine->hasLevel() &&
                   widgets.speechlevelLine->hasLevel()) ||
                  (widgets.snrNoiseRadio->isChecked() &&
                   widgets.snrNoiseLine->hasLevel() &&
                   widgets.noiselevelLine->hasLevel());

    if (!enough)
        return;

    // ok we start updating
    updatingSNR = true;

    double speech, noise, snr;

    if (widgets.snrSpeechRadio->isChecked()) {
        snr = widgets.snrSpeechLine->level();
        speech = widgets.speechlevelLine->level();
        noise = calculateNoiselevel(snr, speech);

        widgets.noiselevelLbl->setLevel(noise);
        widgets.noiselevelLine->setLevel(noise);
    } else {
        snr = widgets.snrNoiseLine->level();
        noise = widgets.noiselevelLine->level();
        speech = calculateSpeechlevel(snr, noise);

        widgets.speechlevelLbl->setLevel(speech);
        widgets.speechlevelLine->setLevel(speech);
    }
    /*    qCDebug(APEX_RS, "Setting snr to %f", snr);
        widgets.snrSpeechLine->setLevel(snr);
        widgets.snrNoiseLine->setLevel(snr);*/

    Q_EMIT levelsChanged(speech, noise);

    // ok done updating
    updatingSNR = false;
}
