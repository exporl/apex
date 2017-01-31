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

#include "../data/spindataenums.h"
#include "../data/spindefaultsettings.h"
#include "../data/spinusersettingsdatabase.h"

#include "../spincalibrator.h"
#include "../spinexperimentcreator.h"

#include "../xml/spinconfigfileparser.h"

#include "spindialog.h"

#include <QCloseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

using namespace spin;
using namespace spin::gui;

SpinDialog::SpinDialog(QString conf, QString schema) : QDialog(),
        configFile(conf),
        schemaFile(schema),
        dBVal(this),
        posdBVal(0, 1000, 2, this),
        uintVal(1, 1000, this)
{
    readConfig();
    setupUi();
    setupConnections();
    setupValidators();
    setupDefaults();
    dialogContentsChanged = false;
}

void SpinDialog::setupUi()
{
    widgets.setupUi(this);

    widgets.tabWidget->setCurrentIndex(0);

    updateSavedSettings();

    //setup speechmaterial combo box
    widgets.speechmaterialCmb->clear();
    widgets.speechmaterialCmb->addItems(configuration.speechmaterials().keys());
    widgets.speechmaterialCmb->setCurrentIndex(0);

    //setup resources
    widgets.hpPicLbl->setPixmap(QPixmap(":images/headphone.svg"));

    //setup free field widget
    widgets.freeFieldWidget->setSpeakerSetup(configuration.speaker_setup());

    setUncorrelatedNoisesVisible(true);

    updateMaterials();
    updateSpeakersStack();
    updateProcedure();

    //setup the custom screens combo
    widgets.customScreenCombo->addItems(configuration.customScreensDescriptions());
    widgets.customScreenCombo->setVisible(widgets.customScreenRadio->isChecked());

    widgets.soundCardCombo->addItem(tr("RME Multiface"), data::RmeMultiface);
    widgets.soundCardCombo->addItem(tr("RME Fireface UC"), data::RmeFirefaceUc);
    widgets.soundCardCombo->addItem(tr("LynxOne"), data::LynxOne);
}

void SpinDialog::setupConnections()
{
    //main tab connections
     connect(widgets.saveSettingsBtn, SIGNAL(clicked()),
            this, SLOT(saveSettings()));
    connect(widgets.removeSettingsBtn, SIGNAL(clicked()),
            this, SLOT(removeSettings()));
    connect(widgets.renameSettingsBtn, SIGNAL(clicked()),
            this, SLOT(renameSettings()));
    connect(widgets.settingsList, SIGNAL(currentItemChanged(
            QListWidgetItem*, QListWidgetItem*)), this, SLOT(checkSettingsChange(
                QListWidgetItem*, QListWidgetItem*)));
    connect(widgets.createExperimentBtn, SIGNAL(clicked()),
            this, SLOT(createExperiment()));

    //resources tab connections
    connect(widgets.speechmaterialCmb, SIGNAL(activated(QString)),
            this, SLOT(updateMaterials()));
    connect(widgets.noisematerialCmb, SIGNAL(activated(QString)),
            this, SLOT(updateNoisematerial()));
    connect(widgets.speechcategoryCmb, SIGNAL(activated(QString)),
            this, SLOT(updateNoiseCombo()));
    connect(widgets.speechcategoryCmb, SIGNAL(activated(QString)),
            this, SLOT(updateListCombo()));

    //speakers tab connections
    connect(widgets.freeFieldRadio, SIGNAL(toggled(bool)),
            this, SLOT(updateSpeakersStack()));
    connect(widgets.lockNoiseCheck, SIGNAL(toggled(bool)),
            this, SLOT(setLockNoiselevels(bool)));
    connect(widgets.lockSpeechCheck, SIGNAL(toggled(bool)),
            this, SLOT(setLockSpeechlevels(bool)));
    connect(widgets.lockNoiseCheck, SIGNAL(toggled(bool)),
            this, SLOT(updateSnrWidget()));
    connect(widgets.lockSpeechCheck, SIGNAL(toggled(bool)),
            this, SLOT(updateSnrWidget()));
    connect(widgets.snrWidget, SIGNAL(lockButtonClicked()),
            this, SLOT(lockLevels()));
    connect(widgets.snrWidget, SIGNAL(levelsChanged(double,double)),
            this, SLOT(updateAllSpeakerLevels(double,double)));

    //procedure tab connections
    connect(widgets.constProcRadio, SIGNAL(toggled(bool)),
            this, SLOT(updateProcedure()));
    connect(widgets.addStepsizeBtn, SIGNAL(clicked()),
            this, SLOT(insertInStepsizeTable()));
    connect(widgets.adaptNoiseRadio, SIGNAL(toggled(bool)),
            this, SLOT(updateStepsizeBoxTitle()));
    connect(widgets.stepsizeTable, SIGNAL(cellChanged(int, int)),
            this, SLOT(removeFromStepsizeTable(int, int)));

    //options tab connections
    connect(widgets.customScreenRadio, SIGNAL(toggled(bool)),
            widgets.customScreenCombo, SLOT(setVisible(bool)));

    //contents changed connections
    connect(widgets.freeFieldWidget, SIGNAL(contentsChanged()),
            this, SLOT(setContentsChanged()));
    connect(widgets.leftHpStartLevels, SIGNAL(contentsChanged()),
            this, SLOT(setContentsChanged()));
    connect(widgets.rightHpStartLevels, SIGNAL(contentsChanged()),
            this, SLOT(setContentsChanged()));
    connect(widgets.nameLine, SIGNAL(textChanged(QString)),
            this, SLOT(setContentsChanged()));
    connect(widgets.freeFieldRadio, SIGNAL(toggled(bool)),
            this, SLOT(setContentsChanged()));
    connect(widgets.constProcRadio, SIGNAL(toggled(bool)),
            this, SLOT(setContentsChanged()));
    connect(widgets.adaptNoiseRadio, SIGNAL(toggled(bool)),
            this, SLOT(setContentsChanged()));
    connect(widgets.initStepsizeLine, SIGNAL(textChanged(QString)),
            this, SLOT(setContentsChanged()));
    connect(widgets.addStepsizeBtn, SIGNAL(clicked()),
            this, SLOT(setContentsChanged()));
    connect(widgets.repeatFirstCheck, SIGNAL(toggled(bool)),
            this, SLOT(setContentsChanged()));
    connect(widgets.exitAfterCheck, SIGNAL(toggled(bool)),
            this, SLOT(setContentsChanged()));
    connect(widgets.expSitsRadio, SIGNAL(toggled(bool)),
            this, SLOT(setContentsChanged()));
    connect(widgets.subSitsRadio, SIGNAL(toggled(bool)),
            this, SLOT(setContentsChanged()));
    connect(widgets.customScreenRadio, SIGNAL(toggled(bool)),
            this, SLOT(setContentsChanged()));
    connect(widgets.customScreenCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setContentsChanged()));
    connect(widgets.soundCardCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setContentsChanged()));

    //calibrating tab connections
    connect (widgets.calibrateBtn, SIGNAL(clicked()),
             this, SLOT(startCalibration()));
}

void SpinDialog::setupValidators()
{
    widgets.initStepsizeLine->setValidator(&posdBVal);
    widgets.stepsizeLine->setValidator(&posdBVal);
    widgets.trialLine->setValidator(&uintVal);
    widgets.nbResponsesLine->setValidator(&uintVal);
    widgets.timeBeforeLine->setValidator(&posdBVal);
}

void SpinDialog::setupDefaults()
{
    widgets.repeatFirstCheck->
            setChecked(data::defaults::REPEAT_FIRST_UNTIL_CORRECT);
    widgets.timeBeforeLine->
            setText(QString::number(data::defaults::TIME_BEFORE_FIRST_STIMULUS));
    widgets.reinforcementCheck->setChecked(data::defaults::REINFORCEMENT);
    widgets.nbResponsesLine->
            setText(QString::number(data::defaults::NB_RESPONSES_THAT_COUNT));
    widgets.initStepsizeLine->setText(QString::number(
                                      data::defaults::INITIAL_STEPSIZE));
    widgets.lockNoiseCheck->setChecked(true);
    widgets.lockSpeechCheck->setChecked(true);

    saveSettings(data::SpinUserSettingsDatabase::DEFAULT);
}

void SpinDialog::readConfig()
{
        configuration = parser::SpinConfigFileParser::parse(configFile,
                                                            schemaFile);
}

void SpinDialog::clearWidgets()
{
    widgets.nameLine->clear();
    widgets.initStepsizeLine->clear();
    widgets.trialLine->clear();
    widgets.stepsizeLine->clear();
    widgets.stepsizeTable->clearContents();

    widgets.freeFieldWidget->clear();
    widgets.leftHpStartLevels->clear();
    widgets.rightHpStartLevels->clear();
}

void SpinDialog::setFocusTo(QWidget *widget)
{
    if (widget == NULL)
        return;

    qDebug("setting focus");

    //search for the tab of the given widget
    int i;
    for (i = 0; i < widgets.tabWidget->count(); i++)
    {
        if (widgets.tabWidget->widget(i)->isAncestorOf(widget))
            break;
    }

    Q_ASSERT(i < widgets.tabWidget->count());

    widgets.tabWidget->setCurrentIndex(i);
    widget->setFocus(Qt::OtherFocusReason);
}

QString SpinDialog::getFileName()
{
    return experimentFile;
}

const data::SpinUserSettings SpinDialog::currentSettings()
{
    data::SpinUserSettings settings;

    settings.setSubjectName(widgets.nameLine->text());

    settings.setSpeechmaterial(widgets.speechmaterialCmb->currentText());
    settings.setSpeechcategory(currentCategory());
    settings.setNoisematerial(widgets.noisematerialCmb->currentText());
    settings.setList(widgets.listCmb->currentText());
    settings.setLockSpeechlevels(widgets.lockSpeechCheck->isChecked());
    settings.setLockNoiselevels(widgets.lockNoiseCheck->isChecked());

    if (widgets.freeFieldRadio->isChecked())
    {
        settings.setSpeakerType(data::FREE_FIELD);

        //take every speaker widget from the free field widget
        QVector<data::Speaker> speakers = configuration.speaker_setup();

        QVector<data::Speaker>::const_iterator it;
        for (it = speakers.begin(); it != speakers.end(); it++)
        {
            const StartLevelWidget *slw =
                widgets.freeFieldWidget->startLevelWidget(it->angle);

            Q_ASSERT(slw != NULL);

            bool hasLevels = false;
            data::SpeakerLevels levels = slw->speakerLevels(&hasLevels);

            // [Tom] also add levels if there is no speech or noise
            /*if (hasLevels)
            {*/
                settings.addLevels(levels, it->angle);

                if (levels.hasNoise)
                    settings.setNoiseJump(it->angle, slw->noiseStartPoint());
            //}
        }
    }
    else
    {
        settings.setSpeakerType(data::HEADPHONE);
        data::SpeakerLevels levels;
        bool hasLevels = false;

        levels = widgets.leftHpStartLevels->speakerLevels(&hasLevels);

        if (hasLevels)
        {
            settings.addLevels(levels, data::Headphone::LEFT);

            if (levels.hasNoise)
            {
                settings.setNoiseJump(data::Headphone::LEFT,
                                  widgets.leftHpStartLevels->noiseStartPoint());
            }

            hasLevels = false;
        }

        levels = widgets.rightHpStartLevels->speakerLevels(&hasLevels);

        if (hasLevels)
        {
            settings.addLevels(levels, data::Headphone::RIGHT);

            if (levels.hasNoise)
            {
                settings.setNoiseJump(data::Headphone::RIGHT,
                                 widgets.rightHpStartLevels->noiseStartPoint());
            }
        }
    }

    if (widgets.constProcRadio->isChecked())
        settings.setProcedureType(data::CONSTANT);
    else
    {
        settings.setProcedureType(data::ADAPTIVE);

        if (widgets.adaptSpeechRadio->isChecked())
            settings.setAdaptingMaterial(data::SPEECH);
        else
            settings.setAdaptingMaterial(data::NOISE);

        //set initial stepsize
        settings.addStepsize(widgets.initStepsizeLine->text().toDouble());

        //and all other stepsizes
        for (int row = 0; row < widgets.stepsizeTable->rowCount(); row++)
        {
            settings.addStepsize(
                widgets.stepsizeTable->item(
                    row, INDEX_STEPSIZE)->text().toDouble(),
                widgets.stepsizeTable->item(
                    row, INDEX_TRIAL)->text().toUInt());
        }

        settings.setRepeatFirst(widgets.repeatFirstCheck->isChecked());
    }

    settings.setNoiseStopsBetweenTrials(widgets.noiseStopsCheck->isChecked());

    if (widgets.customScreenRadio->isChecked())
        settings.setCustomScreen(widgets.customScreenCombo->currentText());
    else
    {
        settings.setPersonBeforeScreen(widgets.expSitsRadio->isChecked() ?
                data::EXPERIMENTER : data::SUBJECT );
    }
    settings.setTimeBeforeFirstStimulus(widgets.timeBeforeLine->
            text().toDouble());
    settings.setReinforcement(widgets.reinforcementCheck->isChecked());
    settings.setShowResults(widgets.showResultsCheck->isChecked());
    settings.setExitAfter(widgets.exitAfterCheck->isChecked());
    settings.setNbResponsesThatCount(widgets.nbResponsesLine->text().toUInt());
    settings.setTrialOrder(widgets.randTrialsCheck->isChecked() ?
            data::ORDER_RANDOM : data::ORDER_SEQUENTIAL);
    settings.setSoundCard(spin::data::SoundCard(widgets.soundCardCombo->itemData
            (widgets.soundCardCombo->currentIndex()).toUInt()));

    return settings;
}

//slots

void SpinDialog::createExperiment()
{
    if (!hasAllObligedFields())
        return;

    data::SpinUserSettings settings = currentSettings();

    //create the experiment creator
    SpinExperimentCreator creator(configuration, settings);

    //ask user where to save the file
    QString apxExtension = tr("Apex Experiment files (*.apx)");
    QString xmlExtension = tr("XML files (*.xml)");
    QString extensions = apxExtension + ";;" + xmlExtension;
    QString selectedExtension;
    QString dir = data::SpinUserSettingsDatabase::lastSavePath();
    QString suggestedFilename(QLatin1String("SPIN"));
    if (! settings.subjectName().isEmpty())
        suggestedFilename += "-" + settings.subjectName();
    suggestedFilename += "-" + settings.speechmaterial();
    if (! settings.speechcategory().isEmpty())
        suggestedFilename += "-" + settings.speechcategory();
    suggestedFilename += "_" + settings.list();
    // check whether there is noise on any speaker
    if (settings.hasNoise())
        suggestedFilename += "-" + settings.noisematerial();
    suggestedFilename += ".apx";

    QString file = QFileDialog::getSaveFileName(this, tr("Save file"),
                   dir + "/" + suggestedFilename, extensions, &selectedExtension);

    if (file.isEmpty()) //user pressed cancel
        return;

    //save the path the where the user saved the file
    QFileInfo fileInfo(file);
    data::SpinUserSettingsDatabase::setLastSavePath(fileInfo.absolutePath());

    //check if the file has an extension and if not, add it
    QString extension;
    if (selectedExtension == xmlExtension)
        extension = ".xml";
    else
    {
        Q_ASSERT(selectedExtension == apxExtension);
        extension = ".apx";
    }

    if (!file.endsWith(extension, Qt::CaseInsensitive))
        file += extension;

    //write the file
    creator.createExperimentFile(file);
    experimentFile = file;

    //save settings as previous
    saveSettings(data::SpinUserSettingsDatabase::PREVIOUS);

    //close the dialog
    accept();
}

bool SpinDialog::saveSettings(QString saveName)
{
    bool ok = true;

    if (saveName.isEmpty())
    {
        saveName = QInputDialog::getText(this, tr("Name to save"),
                        tr("Please give the name for the settings."),
                        QLineEdit::Normal, QString(), &ok);
    }

    if (!ok)//user clicked cancel
        return false;

    data::SpinUserSettingsDatabase::save(saveName, currentSettings());
    updateSavedSettings();

    dialogContentsChanged = false;

    return true;
}

void SpinDialog::checkSettingsChange(QListWidgetItem *to, QListWidgetItem *from)
{
    static bool resettedSelection = false;

    if (to == 0) //happens when slection changes to nothing
        return;

    if (resettedSelection)
    {
        resettedSelection = false;
        return;
    }

    if (dialogContentsChanged)
    {
        QString question(tr("There are unsaved changed in the SPIN dialog "
                             "which will be lost if you load new settings.\n"
                             "Do you wish to save your settings now?"));

        if (QMessageBox::question(this, tr("Unsaved changes"), question,
                                  QMessageBox::Save | QMessageBox::Discard,
                                  QMessageBox::Save) ==
                QMessageBox::Save)
        {

            if (!saveSettings() && from != 0)
            {
                resettedSelection = true;

                //TODO find a way to actually select the previous item
                widgets.settingsList->setCurrentItem(from);
                widgets.settingsList->clearSelection();
            }

            return;
        }
    }

    loadSettings(to->text());
}

void SpinDialog::loadSettings(const QString& name)
{
    if (name.isEmpty())
        return; //happens sometimes...

    clearWidgets();

    data::SpinUserSettings settings =
        data::SpinUserSettingsDatabase::load(name);

    //main
    widgets.nameLine->setText(settings.subjectName());

    //resources
    int speechmatIndex = widgets.speechmaterialCmb->
                         findText(settings.speechmaterial());

    Q_ASSERT(speechmatIndex != -1);

    widgets.speechmaterialCmb->setCurrentIndex(speechmatIndex);
    updateMaterials();

    int speechcatIndex = widgets.speechcategoryCmb->
                         findText(settings.speechcategory());


    if (speechcatIndex != -1) //material has category
    {
        widgets.speechcategoryCmb->setCurrentIndex(speechcatIndex);
        updateNoiseCombo();
    }

    int listIndex = widgets.listCmb->findText(settings.list());

    Q_ASSERT(listIndex != -1);

    widgets.listCmb->setCurrentIndex(listIndex);

    int noisematIndex = widgets.noisematerialCmb->
                        findText(settings.noisematerial());

    Q_ASSERT(noisematIndex != -1);

    widgets.noisematerialCmb->setCurrentIndex(noisematIndex);
    updateNoisematerial();

    //speakers
    //first unlock levels so no levels get overwritten when setting levels
    //on another speaker
    unlockLevels();

    //TODO load noise jumps
    switch (settings.speakerType())
    {
        case data::HEADPHONE:
            widgets.headphoneRadio->setChecked(true);
            widgets.leftHpStartLevels->setSpeakerLevels(
                settings.speakerLevels(data::Headphone::LEFT));
            widgets.rightHpStartLevels->setSpeakerLevels(
                settings.speakerLevels(data::Headphone::RIGHT));

            if (settings.lockSpeechlevels() && settings.lockNoiselevels())
            {
                widgets.snrWidget->setSpeechlevel(widgets.leftHpStartLevels->
                        speechLevel());
                widgets.snrWidget->setNoiselevel(widgets.leftHpStartLevels->
                        noiseLevel());
            }

            break;
        case data::FREE_FIELD:
            widgets.freeFieldRadio->setChecked(true);

            Q_FOREACH(uint angle, settings.speakerAngles())
            {
                widgets.freeFieldWidget->startLevelWidget(angle)->
                    setSpeakerLevels(settings.speakerLevels(angle));
            }

            if (settings.lockSpeechlevels() && settings.lockNoiselevels())
            {
                widgets.snrWidget->setSpeechlevel(widgets.freeFieldWidget->
                        totalSpeechlevel());
                widgets.snrWidget->setNoiselevel(widgets.freeFieldWidget->
                        totalNoiselevel());
            }

            break;
        default:
            qFatal("unknown speaker type while loading settings");
    }

    widgets.lockSpeechCheck->setChecked(settings.lockSpeechlevels());
    widgets.lockNoiseCheck->setChecked(settings.lockNoiselevels());

    //procedure
    switch (settings.procedureType())
    {
        case data::INVALID_PROCEDURE:
            qFatal("Procedure not defined");
        case data::CONSTANT:
            widgets.constProcRadio->setChecked(true);
            break;
        case data::ADAPTIVE:
            widgets.adapProcRadio->setChecked(true);

            if (settings.adaptingMaterial() == data::SPEECH)
                widgets.adaptSpeechRadio->setChecked(true);
            else
                widgets.adaptNoiseRadio->setChecked(true);

            widgets.repeatFirstCheck->setChecked(settings.repeatFirst());

            QMap<uint, double> stepsizes = settings.stepsizes();
            bool hasInitial = false;

            QMap<uint, double>::const_iterator it;
            for (it = stepsizes.begin(); it != stepsizes.end(); it++)
            {
                if (it.key() == 0)
                {
                    hasInitial = true;
                    widgets.initStepsizeLine->
                    setText(QString::number(it.value()));
                    continue;
                }

                widgets.trialLine->setText(QString::number(it.key()));
                widgets.stepsizeLine->setText(QString::number(it.value()));
                insertInStepsizeTable();
            }

            Q_ASSERT(hasInitial);
    }

    //options
    widgets.noiseStopsCheck->setChecked(settings.noiseStopsBetweenTrials());

    if (settings.personBeforeScreen() == data::EXPERIMENTER )
        widgets.expSitsRadio->setChecked(true);
    else if (settings.personBeforeScreen() == data::SUBJECT )
        widgets.subSitsRadio->setChecked(true);
    else //custom screen
    {
        widgets.customScreenRadio->setChecked(true);

        int index = widgets.customScreenCombo->findText(settings.customScreen());

        if (index == -1)
        {
            QMessageBox::warning(this, tr("Unknown custom screen"),
                   tr("Your settings refer to the custom screen \"%1\" "
                      "but this screen does not seem to exist anymore.\n"
                      "The subject screen will be used.")
                           .arg(settings.customScreen()));

            widgets.subSitsRadio->setChecked(true);
        }
        else
            widgets.customScreenCombo->setCurrentIndex(index);
    }

    widgets.timeBeforeLine->setText(QString::number(
                                    settings.timeBeforeFirstStimulus()));
    widgets.reinforcementCheck->setChecked(settings.reinforcement());
    widgets.showResultsCheck->setChecked(settings.showResults());
    widgets.exitAfterCheck->setChecked(settings.exitAfter());
    widgets.nbResponsesLine->setText(QString::number(
                                     settings.nbResponsesThatCount()));

    dialogContentsChanged = false;
}

void SpinDialog::removeSettings()
{
    if (widgets.settingsList->currentItem() == 0)
        return;

    // Get currently selected item
    QString name(widgets.settingsList->currentItem()->text());
    data::SpinUserSettingsDatabase::remove(name);
    updateSavedSettings();
}

void SpinDialog::renameSettings()
{
    if (widgets.settingsList->currentItem() == 0)
        return;

    // Get currently selected item
    QString oldName(widgets.settingsList->currentItem()->text());

    bool ok;
    QString newName = QInputDialog::getText(this, tr("New name"),
                              tr("Please give the new name for the settings."),
                              QLineEdit::Normal, QString(), &ok);

    if (!ok) //use pressed cancel
        return;

    if (newName == oldName)
        return;

    if (data::SpinUserSettingsDatabase::savedSettings().contains(newName))
    {
        QMessageBox::warning(this, tr("Name exists"), tr("The requested name "
                    "already exists in the database. Please choose another name "
                    "or remove the existing settings first." ));
        renameSettings();
        return;
    }

    data::SpinUserSettingsDatabase::rename(oldName, newName);
    updateSavedSettings();
}

bool SpinDialog::hasAllObligedFields()
{
    QString title(tr("Missing field"));
    QString message(tr("Please fill in the %1."));

    //speakers tab
    QWidget *w;
    QString levelMessage(tr("required start levels"));

    if (widgets.headphoneRadio->isChecked())
    {
        //check if a least one speaker is used
        if (!widgets.leftHpStartLevels->hasLevels() &&
            !widgets.rightHpStartLevels->hasLevels())
        {
            QMessageBox::warning(this, title,
                                 tr("You have to use at least one speaker."));
            setFocusTo(widgets.leftHpStartLevels);
            return false;
        }

        if (!widgets.leftHpStartLevels->hasAllObligedFields(&w))
        {
            Q_ASSERT(w != 0);

            QMessageBox::warning(this, title, message.arg(levelMessage));
            setFocusTo(w);
            return false;
        }
        if (!widgets.rightHpStartLevels->hasAllObligedFields(&w))
        {
            Q_ASSERT(w != 0);

            QMessageBox::warning(this, title, message.arg(levelMessage));
            setFocusTo(w);
            return false;
        }
    }
    else
    {
        //check if a least one speaker is used
        if (!widgets.freeFieldWidget->hasLevels())
        {
            QMessageBox::warning(this, title,
                                 tr("You have to use at least one speaker."));
            setFocusTo(widgets.freeFieldWidget);
            return false;
        }

        if (!widgets.freeFieldWidget->hasAllObligedFields(&w))
        {
            Q_ASSERT(w != 0);

            QMessageBox::warning(this, title, message.arg(levelMessage));
            setFocusTo(w);
            return false;
        }
    }

    //procedure tab
    if (widgets.adapProcRadio->isChecked())
    {
        if (widgets.initStepsizeLine->text().isEmpty())
        {
            QMessageBox::warning(this, title, message.arg(tr("initial stepsize")));
            setFocusTo(widgets.initStepsizeLine);
            return false;
        }
    }

    //options tab
    if (widgets.timeBeforeLine->text().isEmpty())
    {
        QMessageBox::warning(this, title, message.arg(
                             tr("time before first stimulus")));
        setFocusTo(widgets.timeBeforeLine);
        return false;
    }

    if (widgets.nbResponsesLine->text().isEmpty())
    {
        QMessageBox::warning(this, title, message.arg(
                             tr("number of responses that count")));
        setFocusTo(widgets.nbResponsesLine);
        return false;
    }

    //check if levels are locked when creating adaptive procedure
    //FIXME
    if (!widgets.lockNoiseCheck->isChecked() ||
        !widgets.lockSpeechCheck->isChecked())
    {
        QMessageBox::warning(this, tr("Levels not locked"),
                             tr("Both speech and noise levels have to be "
                                "locked when creating an adaptive procedure."));
        setFocusTo(widgets.snrWidget);
        return false;
    }

    return true;
}

void SpinDialog::updateSavedSettings()
{
    widgets.settingsList->clear();
    QStringList savedSettings = data::SpinUserSettingsDatabase::savedSettings();
    qSort(savedSettings);
    bool hasDefault = false, hasPrevious = false;

    Q_FOREACH(QString setting, savedSettings)
    {
        if (setting == data::SpinUserSettingsDatabase::DEFAULT)
            hasDefault = true;
        else if (setting == data::SpinUserSettingsDatabase::PREVIOUS)
            hasPrevious = true;
        else
            widgets.settingsList->addItem(setting);
    }

    if (hasPrevious)
        widgets.settingsList->insertItem(0,
                                         data::SpinUserSettingsDatabase::PREVIOUS);
    if (hasDefault)
        widgets.settingsList->insertItem(0,
                                         data::SpinUserSettingsDatabase::DEFAULT);
}

void SpinDialog::loadExperiment()
{
}

void SpinDialog::updateMaterials()
{
    //clear all widgets on the material tab but the speechmaterial combo so
    //they can all be filled from scratch
    widgets.speechcategoryCmb->clear();
    widgets.noisematerialCmb->clear();

    QString speechmatId = widgets.speechmaterialCmb->currentText();

    if (speechmatId.isNull() || speechmatId.isEmpty())   //nothing selected
    {
        widgets.listCmb->hide();
        return;
    }

    //something selected
    data::Speechmaterial speechmat = configuration.speechmaterial(speechmatId);

    //fill category combo if needed, hide otherwise
    if (speechmat.hasCategories())
    {
        widgets.speechcategoryCmb->show();
        widgets.speechcategoryLbl->show();

        widgets.speechcategoryCmb->addItems(speechmat.categories());
    }
    else
    {
        widgets.speechcategoryCmb->hide();
        widgets.speechcategoryLbl->hide();
    }

    updateNoiseCombo();
    //updateNoisematerial();
    updateListCombo();
    updateOptions();
}

void SpinDialog::updateNoiseCombo()
{
    widgets.noisematerialCmb->clear();

    QString speechmatId = widgets.speechmaterialCmb->currentText();
    QString speechcatId = widgets.speechcategoryCmb->currentText();

    QVector<spin::data::Noise> noises =
        configuration.noisesBySpeechmaterial(speechmatId, speechcatId);

    QVector<spin::data::Noise>::const_iterator it;
    uint index = 0, indexToSet = 0;
    for (it = noises.begin(); it != noises.end(); it++, index++)
    {
        widgets.noisematerialCmb->addItem(it->id);

        if (!it->speechmaterial.isEmpty())   //noise has a material
        {
            //don't set the index if the materials don't match
            if (it->speechmaterial != speechmatId)
                continue;

            //don't set the index if the noise has a category and
            //it doesn't match with the current selected category
            if (!it->category.isEmpty() && it->category != speechcatId)
                continue;

            //ok material and category match so set the index to set
            //but only set if it hasn't been set before
            if (indexToSet == 0)
                indexToSet = index;
        }
    }

    widgets.noisematerialCmb->setCurrentIndex(indexToSet);

    updateNoisematerial();
}

void SpinDialog::updateNoisematerial()
{
    widgets.noisedescriptionLbl->clear();

    QString noisematId = widgets.noisematerialCmb->currentText();

    if (noisematId.isNull() || noisematId.isEmpty())   //nothing selected
        return;

    data::Noise noise = configuration.noise(noisematId);

    QString noisedescription;

    if (!noise.name.isEmpty())
        noisedescription += QString(tr("Name: %1\n")).arg(noise.name);
    if (!noise.description.isEmpty())
        noisedescription += QString("\n%1").arg(noise.description);

    widgets.noisedescriptionLbl->setText(noisedescription);

    setContentsChanged();
}

void SpinDialog::updateListCombo()
{
    widgets.listCmb->clear();

    data::Speechmaterial speechmat = currentSpeechmaterial();
    QList<data::List> lists = speechmat.lists(currentCategory());

    QStringList listIds;
    QList<data::List>::const_iterator it;
    for (it = lists.begin(); it != lists.end(); it++)
        listIds << it->id;

    //put the ids in a QMap to sort them by number if they are all ints
    QMap<int, QString> idMap;
    bool ok;
    QStringList::const_iterator it2;
    for (it2 = listIds.begin(); it2 != listIds.end(); it2++)
    {
        idMap.insert(it2->toUInt(&ok), *it2);

        if (!ok)
            break;
    }

    if (!ok)   //a conversion of an id to uint failed so we sort alphabetically
    {
        listIds.sort();
        widgets.listCmb->addItems(listIds);
    }
    else //all conversions to uint where ok so sort numerically
    {
        QMap<int, QString>::const_iterator it;
        for (it = idMap.begin(); it != idMap.end(); it++)
            widgets.listCmb->addItem(*it);
    }

    setContentsChanged();
}

void SpinDialog::setNoiseDisabled(bool disable)
{
    widgets.noisematerialLbl->setEnabled(!disable);
    widgets.noisematerialCmb->setEnabled(!disable);
    widgets.noisedescriptionLbl->setEnabled(!disable);
}

void SpinDialog::updateSpeakersStack()
{
    if (widgets.freeFieldRadio->isChecked())
        widgets.speakersStack->setCurrentIndex(INDEX_FF);
    else
        widgets.speakersStack->setCurrentIndex(INDEX_HP);
}

void SpinDialog::updateAllSpeakerLevels(double speech, double noise)
{
    //free field
    widgets.freeFieldWidget->setTotalSpeechlevel(speech);
    widgets.freeFieldWidget->setTotalNoiselevel(noise);

    //headphone
    widgets.leftHpStartLevels->setSpeechStartlevel(speech);
    widgets.leftHpStartLevels->setNoiseStartlevel(noise);
    widgets.rightHpStartLevels->setSpeechStartlevel(speech);
    widgets.rightHpStartLevels->setNoiseStartlevel(noise);
}

void SpinDialog::setUncorrelatedNoisesVisible(bool visible)
{
    widgets.leftHpStartLevels->setUncorrelatedNoisesVisible(visible);
    widgets.rightHpStartLevels->setUncorrelatedNoisesVisible(visible);
    widgets.freeFieldWidget->setUncorrelatedNoisesVisible(visible);
}

void SpinDialog::lockLevels()
{
    widgets.lockNoiseCheck->setChecked(true);
    widgets.lockSpeechCheck->setChecked(true);
}

void SpinDialog::unlockLevels()
{
    widgets.lockNoiseCheck->setChecked(false);
    widgets.lockSpeechCheck->setChecked(false);
}

void SpinDialog::updateSnrWidget()
{
    if (widgets.lockNoiseCheck->isChecked() &&
        widgets.lockSpeechCheck->isChecked())
    {
        widgets.snrWidget->showSnr();

        //connect headphone levels signals to snr
        connect(widgets.leftHpStartLevels, SIGNAL(speechlevelChanged(double)),
                widgets.snrWidget, SLOT(setSpeechlevel(double)));
        connect(widgets.leftHpStartLevels, SIGNAL(noiselevelChanged(double)),
                widgets.snrWidget, SLOT(setNoiselevel(double)));
        connect(widgets.rightHpStartLevels, SIGNAL(speechlevelChanged(double)),
                widgets.snrWidget, SLOT(setSpeechlevel(double)));
        connect(widgets.rightHpStartLevels, SIGNAL(noiselevelChanged(double)),
                widgets.snrWidget, SLOT(setNoiselevel(double)));

        //connect free field levels signals to snr
        connect(widgets.freeFieldWidget, SIGNAL(totalSpeechlevelChanged(double)),
                widgets.snrWidget, SLOT(setSpeechlevel(double)));
        connect(widgets.freeFieldWidget, SIGNAL(totalNoiselevelChanged(double)),
                widgets.snrWidget, SLOT(setNoiselevel(double)));
    }
    else
    {
        widgets.snrWidget->showWarning();

        widgets.leftHpStartLevels->disconnect(widgets.snrWidget);
        widgets.rightHpStartLevels->disconnect(widgets.snrWidget);
        widgets.freeFieldWidget->disconnect(widgets.snrWidget);
    }
}

void SpinDialog::setLockNoiselevels(bool lock)
{
    widgets.freeFieldWidget->setLockNoiselevels(lock);

    if (lock)
    {
        connect(widgets.leftHpStartLevels, SIGNAL(noiselevelChanged(double)),
                widgets.rightHpStartLevels, SLOT(setNoiseStartlevel(double)));
        connect(widgets.rightHpStartLevels, SIGNAL(noiselevelChanged(double)),
                widgets.leftHpStartLevels, SLOT(setNoiseStartlevel(double)));
    }
    else
    {
        widgets.leftHpStartLevels->disconnect(SIGNAL(noiselevelChanged(double)),
                                              widgets.rightHpStartLevels);
        widgets.rightHpStartLevels->disconnect(SIGNAL(noiselevelChanged(double)),
                                               widgets.leftHpStartLevels);
    }
}

void SpinDialog::setLockSpeechlevels(bool lock)
{
    widgets.freeFieldWidget->setLockSpeechlevels(lock);

    if (lock)
    {
        connect(widgets.leftHpStartLevels, SIGNAL(speechlevelChanged(double)),
                widgets.rightHpStartLevels, SLOT(setSpeechStartlevel(double)));
        connect(widgets.rightHpStartLevels, SIGNAL(speechlevelChanged(double)),
                widgets.leftHpStartLevels, SLOT(setSpeechStartlevel(double)));
    }
    else
    {
        widgets.leftHpStartLevels->disconnect(SIGNAL(speechlevelChanged(double)),
                                              widgets.rightHpStartLevels);
        widgets.rightHpStartLevels->disconnect(SIGNAL(speechlevelChanged(double)),
                                               widgets.leftHpStartLevels);
    }
}

void SpinDialog::updateOptions()
{
    widgets.nbResponsesLine->setText(QString::number(
            configuration.speechmaterial(widgets.speechmaterialCmb->
            currentText()).valuesForMean));
}

void SpinDialog::startCalibration()
{
    qDebug() << "calibrating from SPIN";
    SpinCalibrator calibrator(configuration, currentSettings(), this);
    bool changesMade;

    if (!calibrator.calibrate(&changesMade))
    {
        QMessageBox::critical(this, tr("Calibration error"),
                              tr("Calibration failed, please try again."));
        return;
    }

    if (changesMade)
    {
        if (QMessageBox::question(this, tr("Propagate changes"),
                                  tr("Do you want to apply the changes to "
                                     "all SPIN-related noises?"))
                == QMessageBox::Yes)
        {
            calibrator.propagateChanges();
        }
    }
}

void SpinDialog::updateProcedure()
{
    if (widgets.constProcRadio->isChecked())
        setToConstant();
    else
        setToAdaptive();
}

void SpinDialog::setToConstant()
{
    //hide everything related to adaptive procedures
    widgets.adaptationBox->hide();
    widgets.stepsizeBox->hide();
}

void SpinDialog::setToAdaptive()
{
    //show everything related to adaptive procedures
    widgets.adaptationBox->show();
    widgets.stepsizeBox->show();
}

void SpinDialog::insertInStepsizeTable()
{
    QString warning(tr("You have to fill in the %1 field."));
    QString warningTitle(tr("Missing field"));

    QString trial = widgets.trialLine->text();

    if (trial.isEmpty())
    {
        QMessageBox::warning(this, warningTitle, warning.arg(tr("trial")));
        return;
    }

    QString stepsize = widgets.stepsizeLine->text();

    if (stepsize.isEmpty())
    {
        QMessageBox::warning(this, warningTitle, warning.arg(tr("stepsize")));
        return;
    }

    int row = widgets.stepsizeTable->rowCount();
    widgets.stepsizeTable->insertRow(row);

    QTableWidgetItem *item = new QTableWidgetItem(trial);
    item->setTextAlignment(Qt::AlignCenter);
    widgets.stepsizeTable->setItem(row, INDEX_TRIAL, item);

    item = new QTableWidgetItem(stepsize);
    item->setTextAlignment(Qt::AlignCenter);
    widgets.stepsizeTable->setItem(row, INDEX_STEPSIZE, item);

    widgets.trialLine->clear();
    widgets.stepsizeLine->clear();

    widgets.stepsizeTable->sortByColumn(INDEX_TRIAL, Qt::AscendingOrder);
}

void SpinDialog::removeFromStepsizeTable(int row, int col)
{
    if (widgets.stepsizeTable->item(row, col)->text().isEmpty())
        widgets.stepsizeTable->removeRow(row);
}

void SpinDialog::updateStepsizeBoxTitle()
{
    QString title(tr("Stepsize (for %1):"));
    QString arg;

    if (widgets.adaptNoiseRadio->isChecked())
        arg = tr("noise");
    else
        arg = tr("speech");

    widgets.stepsizeBox->setTitle(title.arg(arg));
}

void SpinDialog::setContentsChanged()
{
    dialogContentsChanged = true;
}

void SpinDialog::closeEvent(QCloseEvent *event)
{
    if (dialogContentsChanged)
    {
        QString question(tr("There are unsaved changed in the SPIN dialog "
                "which will be lost if you close it.\n"
                "Do you wish to save your settings now?"));
        if (QMessageBox::question(this, tr("Unsaved changes"), question,
            QMessageBox::Save | QMessageBox::Discard, QMessageBox::Save) ==
            QMessageBox::Discard)
        {
            event->accept();
        }
        else
        {
            saveSettings();
            event->accept();
        }
    }

    saveSettings(data::SpinUserSettingsDatabase::PREVIOUS);
    event->accept();
}

//some private helper methods

data::Speechmaterial SpinDialog::currentSpeechmaterial()
{
    QString speechmatId = widgets.speechmaterialCmb->currentText();
    return configuration.speechmaterial(speechmatId);
}

QString SpinDialog::currentCategory()
{
    return widgets.speechcategoryCmb->currentText();
}













