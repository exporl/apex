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

#ifndef _APEX_SRC_LIB_APEXSPIN_SPINDIALOG_H_
#define _APEX_SRC_LIB_APEXSPIN_SPINDIALOG_H_

#include "freefieldwidget.h"
#include "spinconfig.h"
#include "spinusersettings.h"
#include "startlevelwidget.h"

#include <QWizard>

class QListWidgetItem;

namespace Ui
{
class SpinMainWizard;
}

namespace spin
{
namespace gui
{

/**
 * @author Job Noorman <jobnoorman@gmail.com>
 */
class APEXSPIN_EXPORT SpinDialog : public QWizard
{
    Q_OBJECT

public:
    SpinDialog();
    virtual ~SpinDialog();

    QString getFileName();
    const data::SpinUserSettings currentSettings();

    virtual bool validateCurrentPage();

private:
    void setupUi();
    void setupConnections();

    /**
     * Initializes validators for QLineEdits.
     */
    void setupValidators();
    void setupDefaults();
    void readConfig();

    /**
     * Clears the widgets that can be changed by the user.
     * This includes:
     *  * All QLineEdits
     *  * The QTableWidget
     *  * All StartLevelWidgets (including those inside the FreeFieldWidget)
     */
    void clearWidgets();

    /**
     * Sets focus to the given widget.
     */
    void setFocusTo(QWidget *widget);

    /**
     * Checks whether all obliged fields are filled in.
     */
    bool hasAllObligedFields();

    // some helper methods
    data::Speechmaterial currentSpeechmaterial();
    QString currentCategory();

    Ui::SpinMainWizard *widgets;
    data::SpinConfig configuration;
    QString experimentFile;

    QMap<QWizardPage *, int> pageMap;

    // validators
    QDoubleValidator dBVal;
    QDoubleValidator posdBVal;
    QIntValidator uintVal;

    // indicates whether the content of the dialog has changed
    // since the last save
    bool dialogContentsChanged;

    enum { INDEX_HP, INDEX_FF };
    enum { INDEX_TRIAL = 0, INDEX_STEPSIZE = 1 };

protected:
    void closeEvent(QCloseEvent *event);

public slots:

    void loadExperiment();

private slots:

    // slots used for main tab
    /**
     * @return  True if the settings where actually saved, false if not
     *          (user pressed cancel when entering settings name).
     */
    bool saveSettings(QString saveName = "");
    void checkSettingsChange(QListWidgetItem *to, QListWidgetItem *from);
    void loadSettings(const QString &name);
    void removeSettings();
    void renameSettings();
    void updateSavedSettings();
    bool createExperiment();
    void updateMaterials();
    void updateNoisematerial();
    void updateNoiseCombo();
    void updateListCombo();
    void setNoiseDisabled(bool disnable);

    // slots used for the speakers tab
    void updateSpeakersStack();
    void updateAllSpeakerLevels(double speech, double noise);
    void setUncorrelatedNoisesVisible(bool visible);
    void setLockSpeechlevels(bool lock);
    void setLockNoiselevels(bool lock);
    void lockLevels();
    void unlockLevels();
    void updateSnrWidget();
    void toggleWarning();

    // slots for the procedure tab
    void updateProcedure();
    void updateStepsizeBoxTitle();
    void setToAdaptive();
    void setToConstant();
    void insertInStepsizeTable();
    void removeFromStepsizeTable(int row, int col);

    // slots for the options tab
    void updateOptions();

    void setContentsChanged();
};

} // ns gui
} // ns spin

#endif
