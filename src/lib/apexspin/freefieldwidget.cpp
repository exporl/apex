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

#include "freefieldwidget.h"

#include <cmath>
#include <limits>

#include <QDebug>

using namespace spin;
using namespace spin::gui;

FreeFieldWidget::FreeFieldWidget(QWidget *parent)
    : QWidget(parent),
      grid(new QGridLayout(this)),
      totalNoise(0.0),
      totalSpeech(0.0),
      totalNoiseSet(false),
      totalSpeechSet(false)
{
    /*arc = new ArcLayout(ArcLayout::ARC_FULL, this);
    setLayout(arc);*/
    // setting up ui is postponed till speaker setup is set
}

FreeFieldWidget::~FreeFieldWidget()
{
    /*if (arc != NULL)      // not necessary, QWidget takes ownership
        delete arc;*/
}

void FreeFieldWidget::setSpeakerSetup(QVector<data::Speaker> speakers)
{
    setupUi(speakers);
}

void FreeFieldWidget::setupUi(QVector<data::Speaker> speakers)
{
    QList<uint> angles;

    QVector<data::Speaker>::const_iterator it;
    for (it = speakers.begin(); it != speakers.end(); it++)
        angles << it->angle;

    std::sort(angles.begin(), angles.end());

    // an empty widget in the center of the arc
    // TODO replace with image-6.02
    // arc->addWidget(new QWidget());

    // add widgets for all speakers
    // TODO make a beter layout than the arc
    QList<uint>::const_iterator it2;
    int count = 0;
    for (it2 = angles.begin(); it2 != angles.end(); it2++) {
        StartLevelWidget *w = new StartLevelWidget(this);
        w->showAngle(*it2);
        // arc->addWidget(w);   //layout takes ownership
        if (angles.size() == 4) {
            int x = 0;
            int y = 0;
            switch (*it2) {
            case 0:
                x = 0;
                y = 1;
                break;
            case 90:
                x = 1;
                y = 2;
                break;
            case 180:
                x = 2;
                y = 1;
                break;
            case 270:
                x = 1;
                y = 0;
                break;
            default:
                x = 0;
                y = 0;
                qCDebug(APEX_RS, "Angle %u not implemented", *it2);
            }
            qCDebug(APEX_RS, "x=%u, y=%u", x, y);
            grid->addWidget(w, x, y);
        } else {
            grid->addWidget(w, floor((double)count / 2), count % 2);
        }
        angleMap.insert(*it2, w);

        connect(w, SIGNAL(contentsChanged()), this, SIGNAL(contentsChanged()));

        connect(w, SIGNAL(speechlevelChanged(double)), this,
                SLOT(emitSpeechChanged()));
        connect(w, SIGNAL(noiselevelChanged(double)), this,
                SLOT(emitNoiseChanged()));

        connect(w, SIGNAL(noiseUsedChanged(bool)), this,
                SLOT(updateNoiselevels()));
        connect(w, SIGNAL(speechUsedChanged(bool)), this,
                SLOT(updateSpeechlevels()));
        ++count;
    }
}

StartLevelWidget *FreeFieldWidget::startLevelWidget(uint angle) const
{
    if (!angleMap.contains(angle))
        return 0;

    return angleMap[angle];
}

void FreeFieldWidget::clear()
{
    QMap<uint, StartLevelWidget *>::const_iterator it;
    for (it = angleMap.begin(); it != angleMap.end(); it++)
        it.value()->clear();
}

void FreeFieldWidget::setUncorrelatedNoisesVisible(bool visible)
{
    QMap<uint, StartLevelWidget *>::const_iterator it;
    for (it = angleMap.begin(); it != angleMap.end(); it++)
        it.value()->setUncorrelatedNoisesVisible(visible);
}

void FreeFieldWidget::setLockNoiselevels(bool lock)
{
    noiseLocked = lock;

    if (lock) {
        QMap<uint, StartLevelWidget *>::const_iterator it1, it2;
        for (it1 = angleMap.begin(); it1 != angleMap.end(); it1++) {
            for (it2 = angleMap.begin(); it2 != angleMap.end(); it2++) {
                if (it1.value() != it2.value()) {
                    connect(it1.value(), SIGNAL(noiselevelChanged(double)),
                            it2.value(), SLOT(setNoiseStartlevel(double)));
                }
            }
        }

    } else {
        QMap<uint, StartLevelWidget *>::const_iterator it;
        for (it = angleMap.begin(); it != angleMap.end(); it++)
            it.value()->disconnect(SIGNAL(noiselevelChanged(double)));
    }
}

void FreeFieldWidget::setLockSpeechlevels(bool lock)
{
    speechLocked = lock;

    if (lock) {
        QMap<uint, StartLevelWidget *>::const_iterator it1, it2;
        for (it1 = angleMap.begin(); it1 != angleMap.end(); it1++) {
            for (it2 = angleMap.begin(); it2 != angleMap.end(); it2++) {
                if (it1.value() != it2.value()) {
                    connect(it1.value(), SIGNAL(speechlevelChanged(double)),
                            it2.value(), SLOT(setSpeechStartlevel(double)));
                }
            }
        }
    } else {
        QMap<uint, StartLevelWidget *>::const_iterator it;
        for (it = angleMap.begin(); it != angleMap.end(); it++)
            it.value()->disconnect(SIGNAL(speechlevelChanged(double)));
    }
}

void FreeFieldWidget::setTotalSpeechlevel(double level)
{
    if (!speechLocked)
        return;

    totalSpeech = level;
    totalSpeechSet = true;
    updateSpeechlevels();
}

void FreeFieldWidget::setTotalNoiselevel(double level)
{
    if (!noiseLocked)
        return;

    totalNoise = level;
    totalNoiseSet = true;
    updateNoiselevels();
}

void FreeFieldWidget::updateSpeechlevels()
{
    if (!speechLocked || !totalSpeechSet || !anySpeechEnabled())
        return;

    double channelLevel =
        this->channelLevel(totalSpeech, speechlevels().size());

    // only set speech in one start level widget because speechlevels are locked
    angleMap.values().first()->setSpeechStartlevel(channelLevel);
}

void FreeFieldWidget::updateNoiselevels()
{
    if (!noiseLocked || !totalNoiseSet || !anyNoiseEnabled())
        return;

    double channelLevel = this->channelLevel(totalNoise, noiselevels().size());

    // only set noise in one start level widget because noiselevels are locked
    angleMap.values().first()->setNoiseStartlevel(channelLevel);
}

bool FreeFieldWidget::hasAllObligedFields(QWidget **w)
{
    QMap<uint, StartLevelWidget *>::const_iterator it;
    for (it = angleMap.begin(); it != angleMap.end(); it++) {
        if (!it.value()->hasAllObligedFields(w))
            return false;
    }

    return true;
}

bool FreeFieldWidget::hasLevels() const
{
    QMap<uint, StartLevelWidget *>::const_iterator it;
    for (it = angleMap.begin(); it != angleMap.end(); it++) {
        if (it.value()->hasLevels())
            return true;
    }

    return false;
}

void FreeFieldWidget::emitSpeechChanged()
{
    double speech = totalSpeechlevel();

    if (speech > -std::numeric_limits<double>::infinity())
        Q_EMIT totalSpeechlevelChanged(speech);
}

void FreeFieldWidget::emitNoiseChanged()
{
    double noise = totalNoiselevel();

    if (noise > -std::numeric_limits<double>::infinity())
        Q_EMIT totalNoiselevelChanged(noise);
}

double FreeFieldWidget::totalSpeechlevel() const
{
    return totalLevel(speechlevels());
}

double FreeFieldWidget::totalNoiselevel() const
{
    return totalLevel(noiselevels());
}

double FreeFieldWidget::totalLevel(QList<double> levels) const
{
    if (levels.isEmpty())
        return -std::numeric_limits<double>::infinity();

    double totalLinear = 0.0;

    Q_FOREACH (double db, levels)
        totalLinear += pow(10, db / 10);

    return 10 * log10(totalLinear);
}

double FreeFieldWidget::channelLevel(double totalLevel, uint nbOfChannels)
{
    return 10 * log10(pow(10, totalLevel / 10) / nbOfChannels);
}

QList<double> FreeFieldWidget::noiselevels() const
{
    QList<double> levels;

    Q_FOREACH (StartLevelWidget *slw, angleMap.values()) {
        if (slw->hasNoiseLevel())
            levels.append(slw->noiseLevel());
    }

    return levels;
}

QList<double> FreeFieldWidget::speechlevels() const
{
    QList<double> levels;

    Q_FOREACH (StartLevelWidget *slw, angleMap.values()) {
        if (slw->hasSpeechLevel())
            levels.append(slw->speechLevel());
    }

    return levels;
}

bool FreeFieldWidget::anyNoiseEnabled() const
{
    Q_FOREACH (StartLevelWidget *slw, angleMap.values()) {
        if (slw->hasNoiseLevel())
            return true;
    }

    return false;
}

bool FreeFieldWidget::anySpeechEnabled() const
{
    Q_FOREACH (StartLevelWidget *slw, angleMap.values()) {
        if (slw->hasSpeechLevel())
            return true;
    }

    return false;
}
