/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef FREEFIELDWIDGET_H
#define FREEFIELDWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QVector>

#include "startlevelwidget.h"
#include "../data/spinconfig.h"

//from libtools
#include "gui/arclayout.h"

namespace spin
{
namespace gui
{
class FreeFieldWidget : public QWidget
{
    Q_OBJECT

    public:
        FreeFieldWidget(QWidget *parent);
        ~FreeFieldWidget();

        /**
         * Returns the start level widget at the given angle.
         * Returns NULL is there is no such widget.
         */
        StartLevelWidget *startLevelWidget(uint angle) const;

         /**
         * Checks whether all obliged fields are filled in, if not, w will
         * be set to the first found widget that was not filled in.
          */
        bool hasAllObligedFields(QWidget **w);

        /**
         * Checks whether at least one of the start level widgets has levels.
         */
        bool hasLevels() const;

        /**
         * Sets the speakers to be shown in this widget.
         */
        void setSpeakerSetup(QVector<data::Speaker> speakers);

        double totalSpeechlevel() const;
        double totalNoiselevel() const;

    public slots:

        /**
         * Clears all StartLevelWidgets in this widget.
         */
        void clear();

        /**
         * Shows/hides the uncorrelated noises widgets of all StartLevelWidgets.
         */
        void setUncorrelatedNoisesVisible(bool visible);

        /**
         * If called with true, all noiselevels will be kept at the same value;
         * if one gets changed, all get changed.
         */
        void setLockNoiselevels(bool lock);

        /**
         * If called with true, all speechlevels will be kept at the same value;
         * if one gets changed, all get changed.
         */
        void setLockSpeechlevels(bool lock);

        /**
         * Sets the total speechlevel to the given value. This will not do
         * anything if the speechlevels are not locked.
         */
        void setTotalSpeechlevel(double level);

        /**
         * Sets the total noiselevel to the given value. This will not do
         * anything if the noiselevels are not locked.
         */
        void setTotalNoiselevel(double level);

    signals:

        /**
         * Emitted when something has changed in one of the widgets.
         */
        void contentsChanged();

        /**
         * Emitted when the total speechlevel changed.
         */
        void totalSpeechlevelChanged(double to);

        /**
         * Emitted when the total noiselevel changed.
         */
        void totalNoiselevelChanged(double to);

    private:
        /**
         * Return true if speech is enabled on any speaker
         */
        bool anySpeechEnabled() const;

       /**
         * Return true if noise is enabled on any speaker
         */
        bool anyNoiseEnabled() const;

        //ArcLayout *arc;
        QGridLayout *grid;
        QMap<uint, StartLevelWidget*> angleMap;

        void setupUi(QVector<data::Speaker> speakers);
        double totalLevel(QList<double> levels) const;
        double channelLevel(double totalLevel, uint nbOfChannels);
        QList<double> noiselevels() const;
        QList<double> speechlevels() const;

        bool speechLocked;
        bool noiseLocked;
        double totalNoise;
        double totalSpeech;
        bool totalNoiseSet;
        bool totalSpeechSet;

    private slots:

        /**
         * Emits the speechlevelsChanged(double) signal with the total
         * speechlevel as argument.
         */
        void emitSpeechChanged();

        /**
         * Emits the noiselevelsChanged(double) signal with the total
         * noiselevel as argument.
         */
        void emitNoiseChanged();

        /**
         * Will update the noiselevels (if they are locked) to the value of
         * totalNoise.
         */
        void updateNoiselevels();

        /**
         * Will update the speechlevels (if they are locked) to the value of
         * totalNoise.
         */
        void updateSpeechlevels();
};
}//ns gui
}//ns spin

#endif

























