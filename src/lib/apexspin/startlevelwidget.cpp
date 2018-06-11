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

#include "apextools/apextools.h"

#include "noisestartpointdialog.h"
#include "startlevelwidget.h"
#include "ui_startlevelwidget.h"

#include <QDebug>

using namespace spin;
using namespace spin::gui;

StartLevelWidget::StartLevelWidget(QWidget *parent)
    : QWidget(parent),
      widgets(new Ui::StartLevelWidget),
      dBVal(this),
      noiseJump(RANDOM)
{
    dBVal.setDecimals(2);

    setupUi();
    setupConnections();
    setupValidators();
}

StartLevelWidget::~StartLevelWidget()
{
    delete widgets;
}

void StartLevelWidget::setupUi()
{
    widgets->setupUi(this);
    updateNoiseStartPoint(noiseJump);
}

void StartLevelWidget::setupConnections()
{
    connect(widgets->speechCheck, SIGNAL(toggled(bool)),
            widgets->speechStartLine, SLOT(setEnabled(bool)));
    connect(widgets->noiseCheck, SIGNAL(toggled(bool)), widgets->noiseStartLine,
            SLOT(setEnabled(bool)));
    connect(widgets->speechCheck, SIGNAL(toggled(bool)),
            widgets->speechStartLine, SLOT(setFocus()));
    connect(widgets->noiseCheck, SIGNAL(toggled(bool)), widgets->noiseStartLine,
            SLOT(setFocus()));

    connect(widgets->speechCheck, SIGNAL(toggled(bool)), this,
            SIGNAL(speechUsedChanged(bool)));
    connect(widgets->noiseCheck, SIGNAL(toggled(bool)), this,
            SIGNAL(noiseUsedChanged(bool)));

    connect(widgets->speechCheck, SIGNAL(toggled(bool)), this,
            SIGNAL(contentsChanged()));
    connect(widgets->noiseCheck, SIGNAL(toggled(bool)), this,
            SIGNAL(contentsChanged()));
    connect(widgets->speechStartLine, SIGNAL(textChanged(QString)), this,
            SLOT(speechlevelChanged(QString)));
    connect(widgets->noiseStartLine, SIGNAL(textChanged(QString)), this,
            SLOT(noiselevelChanged(QString)));
    connect(this, SIGNAL(noiselevelChanged(double)), this,
            SIGNAL(contentsChanged()));
    connect(this, SIGNAL(speechlevelChanged(double)), this,
            SIGNAL(contentsChanged()));

    connect(widgets->showStartPointLbl, SIGNAL(linkActivated(QString)), this,
            SLOT(showNoiseStartPointDialog()));

    /*    connect(widgets->noiseStartLine, SIGNAL(textChanged(QString)),
                this, SLOT(printChange(QString)));
        connect(widgets->speechStartLine, SIGNAL(textChanged(QString)),
                this, SLOT(printChange(QString)));*/
}

void StartLevelWidget::setupValidators()
{
    widgets->speechStartLine->setValidator(&dBVal);
    widgets->noiseStartLine->setValidator(&dBVal);
}

void StartLevelWidget::showAngle(uint angle)
{
    QString label(tr("Start level (%1%2)"));
    QString degree(tr("°"));
    widgets->label->setText(label.arg(angle).arg(degree));
}

bool StartLevelWidget::hasSpeechLevel() const
{
    return widgets->speechCheck->isChecked();
}

bool StartLevelWidget::hasNoiseLevel() const
{
    return widgets->noiseCheck->isChecked();
}

bool StartLevelWidget::hasLevels() const
{
    return hasSpeechLevel() || hasNoiseLevel();
}

double StartLevelWidget::speechLevel() const
{
    // Q_ASSERT(hasSpeechLevel());

    if (widgets->speechStartLine->text().isEmpty())
        return 0.0;

    return widgets->speechStartLine->text().toDouble();
}

double StartLevelWidget::noiseLevel() const
{
    // Q_ASSERT(hasNoiseLevel());

    if (widgets->noiseStartLine->text().isEmpty())
        return 0.0;

    return widgets->noiseStartLine->text().toDouble();
}

data::SpeakerLevels StartLevelWidget::speakerLevels(bool *hasLevels) const
{
    data::SpeakerLevels levels;
    *hasLevels = false;

    levels.speech = speechLevel();

    if (hasSpeechLevel()) {
        levels.hasSpeech = true;
        *hasLevels = true;
    }

    levels.noise = noiseLevel();

    if (hasNoiseLevel()) {
        levels.hasNoise = true;
        *hasLevels = true;
    }

    return levels;
}

void StartLevelWidget::setSpeakerLevels(const data::SpeakerLevels &levels)
{
    clear();

    if (levels.hasSpeech) {
        widgets->speechCheck->setChecked(true);
        //         widgets->speechStartLine->setText(QString::number(levels.speech));
        widgets->speechStartLine->setLevel(levels.speech);
    }
    if (levels.hasNoise) {
        widgets->noiseCheck->setChecked(true);
        //         widgets->noiseStartLine->setText(QString::number(levels.noise));
        widgets->noiseStartLine->setLevel(levels.noise);
    }
}

void StartLevelWidget::clear()
{
    widgets->speechCheck->setChecked(false);
    widgets->noiseCheck->setChecked(false);
    widgets->speechStartLine->clear();
    widgets->noiseStartLine->clear();
}

void StartLevelWidget::setUncorrelatedNoisesVisible(bool visible)
{
    widgets->startNoiseAtLbl->setVisible(visible);
    widgets->showStartPointLbl->setVisible(visible);

    if (visible)
        updateNoiseStartPoint(-1);
}

void StartLevelWidget::setNoiseStartlevel(double level)
{
    widgets->noiseStartLine->setLevel(level);
}

void StartLevelWidget::setSpeechStartlevel(double level)
{
    widgets->speechStartLine->setLevel(level);
}

double StartLevelWidget::noiseStartPoint() const
{
    if (noiseJump < 0)
        return RANDOM;

    return noiseJump;
}

void StartLevelWidget::updateNoiseStartPoint(double to)
{
    QString link = "<a href=\"foo\">%1</a>";

    if (to < 0)
        link = link.arg("random");
    else
        link = link.arg(QString::number(to) + "s");

    widgets->showStartPointLbl->setText(link);

    if (to != noiseJump) {
        noiseJump = to;
        Q_EMIT contentsChanged();
    }
}

void StartLevelWidget::showNoiseStartPointDialog()
{
    NoiseStartpointDialog dlg(parentWidget(), this, noiseJump);
    dlg.exec();
}

void StartLevelWidget::noiselevelChanged(QString level)
{
    Q_EMIT noiselevelChanged(level.toDouble());
}

void StartLevelWidget::speechlevelChanged(QString level)
{
    Q_EMIT speechlevelChanged(level.toDouble());
}

bool StartLevelWidget::hasAllObligedFields(QWidget **w)
{
    if (hasNoiseLevel() && widgets->noiseStartLine->text().isEmpty()) {
        *w = widgets->noiseStartLine;
        return false;
    }
    if (hasSpeechLevel() && widgets->speechStartLine->text().isEmpty()) {
        *w = widgets->speechStartLine;
        return false;
    }

    return true;
}

void StartLevelWidget::printChange(QString to)
{
    qCDebug(APEX_RS) << objectName() << ": changed to " << to;
}
