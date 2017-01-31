/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
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

#include "calibrationdialog.h"
#include "ui_calibrationdialog.h"

#include <QPushButton>

namespace apex
{

class CalibrationDialogPrivate : public QObject
{
    Q_OBJECT
public:
    CalibrationDialogPrivate (CalibrationDialog *pub);

    void setTargetAmplitude (const QString &amplitude);
    void setExperimentAmplitude (const QString &amplitude);
    QString targetAmplitude() const;
    void setMeasuredAmplitude (const QString &amplitude);
    QString measuredAmplitude() const;
    void setOutputParameter (const QString &value);
    QString outputParameter() const;
    void setHardwareSetups (QStringList value);
    void setStimuli (QStringList value);
    void setParameterNames (QStringList value);

    void setMinimumOutputParameter (double value);
    void setMaximumOutputParameter (double value);

    void setDefaultMutedParameters();
    void setMutedParameters (const QStringList &value);

    bool isGlobal() const;
    void setGlobal(bool value);
    void setReadOnly();

    Ui::CalibrationDialog ui;
    double minimum, maximum;

private Q_SLOTS:
    void on_outputSlider_valueChanged (int value);
    void on_output_textChanged();
    void on_target_textChanged();
    void on_measured_textChanged();
    void on_correct_clicked();
    void on_autoCalibrateSingle_clicked();
    void on_hardware_currentIndexChanged (int index);
    void on_stimulus_currentIndexChanged (const QString &value);
    void on_parameter_currentIndexChanged (QListWidgetItem *item);
    void on_loop_stateChanged (int state);
    void on_muted_itemSelectionChanged();

private:
    void updateSlider();
    void updateWidgets();

private:
    CalibrationDialog * const p;

private:
    unsigned ignoreSlider;
    unsigned ignoreInput;
    unsigned ignoreHardware;
    unsigned ignoreStimulus;
    unsigned ignoreParameter;
    unsigned ignoreLoop;
    unsigned ignoreMuted;
    QString oldHardwareSetup;
    QString oldStimulus;
    QString oldParameter;
    QSet<QString> oldMuted;
    bool enabled;
    QString unit;
    QStringList setups;
    bool playing;
    bool autoCalibrateEnabled;
    bool global;

    QPushButton *advanced;
    QPushButton *start;
    QPushButton *stop;
    QPushButton *correct;
    QPushButton *apply;
    QPushButton *autoCalibrateAll;
    QPushButton *autoCalibrateSingle;

    friend class CalibrationDialog;
};

// CalibrationDialogPrivate ====================================================

CalibrationDialogPrivate::CalibrationDialogPrivate (CalibrationDialog *pub) :
    minimum (0),
    maximum (1),
    p (pub),
    ignoreSlider (0),
    ignoreInput (0),
    ignoreHardware (0),
    ignoreStimulus (0),
    ignoreParameter (0),
    ignoreLoop (0),
    ignoreMuted (0),
    enabled (true),
    unit ("dB"),
    playing (false),
    autoCalibrateEnabled(false)
{
    ui.setupUi (pub);

    start = ui.buttonBox->addButton (tr ("Start"),
            QDialogButtonBox::ActionRole);
    stop = ui.buttonBox->addButton (tr ("Stop"),
            QDialogButtonBox::ActionRole);
    advanced = ui.buttonBox->addButton (tr ("Advanced..."),
            QDialogButtonBox::ActionRole);
    advanced->setCheckable (true);
    autoCalibrateAll = ui.buttonBox->addButton (tr ("Calibrate Automatically"),
                                             QDialogButtonBox::ActionRole);
    QDialogButtonBox *buttons = ui.calibrationButtons;
    correct = buttons->addButton (tr ("Correct Output Parameter"),
            QDialogButtonBox::ActionRole);
    autoCalibrateSingle = buttons->addButton (tr ("Autocalibrate"),
                                  QDialogButtonBox::ActionRole);
    autoCalibrateSingle->setEnabled(autoCalibrateEnabled);
    autoCalibrateSingle->setVisible(autoCalibrateEnabled);
    autoCalibrateAll->setEnabled(autoCalibrateEnabled);
    autoCalibrateAll->setVisible(autoCalibrateEnabled);

    apply = buttons->button (QDialogButtonBox::Apply);

    connect (apply, SIGNAL (clicked()),
            pub, SIGNAL (saveCalibration()));
    connect (correct, SIGNAL (clicked()),
            this, SLOT (on_correct_clicked()));
    connect (autoCalibrateSingle, SIGNAL (clicked()),
             this, SLOT (on_autoCalibrateSingle_clicked()));
    connect (start, SIGNAL (clicked()),
            pub, SIGNAL (startOutput()));
    connect (stop, SIGNAL (clicked()),
            pub, SIGNAL (stopOutput()));
    connect (advanced, SIGNAL (toggled(bool)),
            pub, SLOT (showAdvanced(bool)));
    connect (autoCalibrateAll, SIGNAL(clicked()),
             pub, SIGNAL (autoCalibrateAll()));
    connect (ui.outputSlider, SIGNAL (valueChanged(int)),
            this, SLOT (on_outputSlider_valueChanged(int)));
    connect (ui.output, SIGNAL (textChanged(QString)),
            this, SLOT (on_output_textChanged()));
    connect (ui.target, SIGNAL (textChanged(QString)),
            this, SLOT (on_target_textChanged()));
    connect (ui.measured, SIGNAL (textChanged(QString)),
            this, SLOT (on_measured_textChanged()));
    connect (ui.hardware, SIGNAL (currentIndexChanged(int)),
            this, SLOT (on_hardware_currentIndexChanged(int)));
    connect (ui.stimulus, SIGNAL (currentIndexChanged(QString)),
            this, SLOT (on_stimulus_currentIndexChanged(QString)));
    connect (ui.parameter,SIGNAL (currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT (on_parameter_currentIndexChanged(QListWidgetItem*)));
    connect (ui.loop, SIGNAL (stateChanged(int)),
            this, SLOT (on_loop_stateChanged(int)));
    connect (ui.muted, SIGNAL (itemSelectionChanged()),
            this, SLOT (on_muted_itemSelectionChanged()));
        disconnect(ui.buttonBox->button(QDialogButtonBox::Cancel), 0, 0, 0);
        connect(ui.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
                        pub, SIGNAL(beforeCancel()));


    ui.advanced->hide();
    ui.clippingStack->setCurrentWidget (ui.noClipping);
    stop->setEnabled (false);

    setTargetAmplitude ("100");
    setOutputParameter ("0");

    updateWidgets();
}

void CalibrationDialogPrivate::updateWidgets()
{
    const QString measured = measuredAmplitude();
    correct->setEnabled (enabled && !measured.isEmpty());
    correct->setDefault (enabled && !measured.isEmpty());
    apply->setEnabled (enabled && measured.isEmpty() &&
            !targetAmplitude().isEmpty() && !outputParameter().isEmpty());
    ui.target->setEnabled (enabled);
    ui.output->setEnabled (enabled);
    ui.outputSlider->setEnabled (enabled);
    ui.measured->setEnabled (enabled);
    ui.parameter->setEnabled (enabled);
    ui.stimulus->setEnabled (enabled);
    ui.loop->setEnabled (enabled);
    ui.muted->setEnabled (enabled);
    ui.targetLabel->setText (tr ("Target SPL during calibration (%1)").arg (unit));
    ui.targetExperimentLabel->setText (tr ("Target SPL during experiment (%1)").arg (unit));
    ui.measuredLabel->setText (tr ("Measured SPL (%1)").arg (unit));
        autoCalibrateSingle->setEnabled(enabled && !playing);
    start->setEnabled (enabled && !playing);
    stop->setEnabled (playing);
        autoCalibrateAll->setEnabled(enabled && !playing);
        ui.buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(!playing);
}

void CalibrationDialogPrivate::on_outputSlider_valueChanged (int value)
{
    if (ignoreSlider)
        return;

    ++ignoreInput;
    setOutputParameter (QString::number
            (value * (maximum - minimum) / 1000 + minimum));
    --ignoreInput;
}

void CalibrationDialogPrivate::on_output_textChanged()
{
    emit p->outputParameterChanged (outputParameter());
    ui.outputDisplay->setText (ui.output->text());

    if (ignoreInput)
        return;

    updateSlider();
}

void CalibrationDialogPrivate::on_target_textChanged()
{
    emit p->targetAmplitudeChanged (targetAmplitude());
}

void CalibrationDialogPrivate::on_loop_stateChanged (int state)
{
    emit p->loopingChanged (state == Qt::Checked);
}

void CalibrationDialogPrivate::on_measured_textChanged()
{
    updateWidgets();
}

void CalibrationDialogPrivate::on_correct_clicked()
{
    const QString measured = measuredAmplitude();
    if (!measured.isEmpty())
        emit p->correctOutputParameter (measured.toDouble());
}


void CalibrationDialogPrivate::on_autoCalibrateSingle_clicked() {
    emit p->autoCalibrateSingle ( p->parameterName() );
}


void CalibrationDialogPrivate::on_hardware_currentIndexChanged
    (int index)
{
    if (ignoreHardware)
        return;

    if (ui.hardware->itemData (index) == true) {
        ++ignoreHardware;
        p->setHardwareSetup (oldHardwareSetup);
        --ignoreHardware;
        emit p->manageHardwareSetups();
    } else {
        const QString value = ui.hardware->itemText(index);
        if (oldHardwareSetup != value) {
            emit p->hardwareSetupChanged (value);
            oldHardwareSetup = value;
            // We reload the whole dialog, this should do the trick
            setParameterNames (p->parameterNames());
        }
    }
}

void CalibrationDialogPrivate::on_muted_itemSelectionChanged()
{
    const QSet<QString> muted = QSet<QString>::fromList (p->mutedParameters());
    QSet<QString> toMute (muted);
    toMute -= oldMuted;
    QSet<QString> toUnmute (oldMuted);
    toUnmute -= muted;

    if (!ignoreMuted) {
        Q_FOREACH (const QString &name, toMute)
            emit p->mutingChanged (name, true);
        Q_FOREACH (const QString &name, toUnmute)
            emit p->mutingChanged (name, false);
    }

    oldMuted = muted;
}

void CalibrationDialogPrivate::on_stimulus_currentIndexChanged
    (const QString &value)
{
    if (oldStimulus != value && !ignoreStimulus) {
        emit p->stimulusChanged (value);
        oldStimulus = value;
    }
}

void CalibrationDialogPrivate::on_parameter_currentIndexChanged
    (QListWidgetItem *item)
{
    const QString value (item ? item->text() : QString());
    if (oldParameter != value && !ignoreParameter) {
        emit p->parameterNameChanged (value);
        oldParameter = value;
        setDefaultMutedParameters();
    }
}

QString CalibrationDialogPrivate::targetAmplitude() const
{
    bool ok;
    const QString text = ui.target->text();
    text.toDouble (&ok);
    return ok ? text : QString();
}

void CalibrationDialogPrivate::setTargetAmplitude (const QString &amplitude)
{
    ui.target->setText (amplitude);
}

void CalibrationDialogPrivate::setExperimentAmplitude (const QString &amplitude)
{
    ui.targetExperimentValueLabel->setText (amplitude);
}


QString CalibrationDialogPrivate::outputParameter() const
{
    bool ok;
    const QString text = ui.output->text();
    const double result = text.toDouble (&ok);
    return ok && result >= minimum && result <= maximum ? text : QString();
}

void CalibrationDialogPrivate::setOutputParameter (const QString &value)
{
    ui.output->setText (value);
}

QString CalibrationDialogPrivate::measuredAmplitude() const
{
    bool ok;
    const QString text = ui.measured->text();
    text.toDouble (&ok);
    return ok ? text : QString();
}

void CalibrationDialogPrivate::setMeasuredAmplitude (const QString &amplitude)
{
    ui.measured->setText (amplitude);
}

void CalibrationDialogPrivate::setMinimumOutputParameter (double value)
{
    minimum = value;
    updateSlider();
}

void CalibrationDialogPrivate::setMaximumOutputParameter (double value)
{
    maximum = value;
    updateSlider();
}

void CalibrationDialogPrivate::setHardwareSetups (QStringList value)
{
    setups = value;
    ++ignoreHardware;
    const QString current = p->hardwareSetup();
    ui.hardware->clear();
    ui.hardware->addItems (value);
    ui.hardware->addItem (tr ("<Manage setups...>"), true);
    p->setHardwareSetup (current);
    --ignoreHardware;
    // The handler will only emit if the old and the new setup are different
    on_hardware_currentIndexChanged (ui.hardware->currentIndex());
}

void CalibrationDialogPrivate::setStimuli (QStringList value)
{
    ++ignoreStimulus;
    const QString current = p->stimulus();
    ui.stimulus->clear();
    ui.stimulus->addItems (value);
    p->setStimulus (current);
    --ignoreStimulus;
    // The handler will only emit if the old and the new setup are different
    on_stimulus_currentIndexChanged (p->stimulus());
}

void CalibrationDialogPrivate::setParameterNames (QStringList value)
{
    ++ignoreParameter;
    const QString current = p->parameterName();
    ui.parameter->clear();
    ui.parameter->addItems (value);
    p->setParameterName (current);
    --ignoreParameter;

    ++ignoreMuted;
    ui.muted->clear();
    ui.muted->addItems (value);
    // The handler will only emit if the old and the new setup are different
    on_parameter_currentIndexChanged (ui.parameter->currentItem());
    setDefaultMutedParameters();
    --ignoreMuted;
    for (unsigned i = 0; i < unsigned (ui.muted->count()); ++i) {
        const QListWidgetItem * const item = ui.muted->item (i);
        emit p->mutingChanged (item->text(), item->isSelected());
    }
}

void CalibrationDialogPrivate::setDefaultMutedParameters()
{
    QStringList parameters = p->parameterNames();
    parameters.removeAll(p->parameterName());
    setMutedParameters (parameters);
}

void CalibrationDialogPrivate::setMutedParameters (const QStringList &value)
{
    for (unsigned i = 0; i < unsigned (ui.muted->count()); ++i) {
        QListWidgetItem * const item = ui.muted->item (i);
        item->setSelected (value.contains (item->text()));
    }
}

void CalibrationDialogPrivate::updateSlider()
{
    ui.minimum->setText (QString::number (minimum));
    ui.maximum->setText (QString::number (maximum));
    ui.average->setText (QString::number ((minimum + maximum) / 2));

    // outputParameter() not used because we do not care for maximum/minimum
    bool ok;
    const QString text = ui.output->text();
    const double value = text.toDouble (&ok);

    if (ok) {
        ++ignoreSlider;
        ui.outputSlider->setValue
            (int ((value - minimum) / (maximum - minimum) * 1000));
        --ignoreSlider;
    }
}

bool CalibrationDialogPrivate::isGlobal() const {
    return global;
}

void CalibrationDialogPrivate::setGlobal(bool value) {
    global = value;
}

void CalibrationDialogPrivate::setReadOnly() {
    ui.measured->setDisabled(true);
    ui.outputSlider->setDisabled(true);
    ui.output->setDisabled(true);
    apply->setDisabled(true);
}

// CalibrationDialog ===========================================================

CalibrationDialog::CalibrationDialog (QWidget *parent) :
    QDialog (parent),
    d (new CalibrationDialogPrivate (this))
{
}

CalibrationDialog::~CalibrationDialog()
{
}

void CalibrationDialog::setGlobal(bool global) {
    d->setGlobal(global);
}

void CalibrationDialog::setHardwareSetups (const QStringList &value)
{
    d->setHardwareSetups (value);
}

QStringList CalibrationDialog::hardwareSetups() const
{
    return d->setups;
}

QString CalibrationDialog::hardwareSetup() const
{
    return d->ui.hardware->currentText();
}

void CalibrationDialog::setHardwareSetup (const QString &value)
{
    d->ui.hardware->setCurrentIndex (d->setups.indexOf (value));
}

void CalibrationDialog::setStimuli (QStringList value)
{
    d->setStimuli (value);
}

QStringList CalibrationDialog::stimuli() const
{
    QStringList result;
    for (int i = 0; i < d->ui.stimulus->count(); ++i)
        result << d->ui.stimulus->itemText (i);
    return result;
}

QString CalibrationDialog::stimulus() const
{
    return d->ui.stimulus->currentText();
}

void CalibrationDialog::setStimulus (const QString &value)
{
    d->ui.stimulus->setCurrentIndex (d->ui.stimulus->findText (value));
}

void CalibrationDialog::setParameterNames (const QStringList &value)
{
    d->setParameterNames (value);
}

QStringList CalibrationDialog::parameterNames() const
{
    QStringList result;
    for (int i = 0; i < d->ui.parameter->count(); ++i)
        result << d->ui.parameter->item (i)->text();
    return result;
}

QString CalibrationDialog::parameterName() const
{
    QListWidgetItem *currentItem = d->ui.parameter->currentItem();
    if (!currentItem)
        return QString();

    return currentItem->text();
}

void CalibrationDialog::setParameterName (const QString &value)
{
    const QList<QListWidgetItem*> items = d->ui.parameter->findItems (value,
            Qt::MatchExactly);
    if (items.isEmpty())
        d->ui.parameter->setCurrentItem (NULL);
    else
        d->ui.parameter->setCurrentItem (items.first());
}

void CalibrationDialog::setDatabaseStatus (const QString &status)
{
    d->ui.status->setText (status);
}

void CalibrationDialog::setMinimumOutputParameter (double value)
{
    d->setMinimumOutputParameter (value);
}

double CalibrationDialog::minimumOutputParameter() const
{
    return d->minimum;
}

void CalibrationDialog::setMaximumOutputParameter (double value)
{
    d->setMaximumOutputParameter (value);
}

double CalibrationDialog::maximumOutputParameter() const
{
    return d->maximum;
}

void CalibrationDialog::setOutputParameter (const QString &value)
{
    d->setOutputParameter (value);
}

QString CalibrationDialog::outputParameter() const
{
    return d->outputParameter();
}

void CalibrationDialog::setTargetAmplitude (const QString &amplitude)
{
    d->setTargetAmplitude (amplitude);
}

void CalibrationDialog::setExperimentAmplitude (const QString &amplitude)
{
    d->setExperimentAmplitude (amplitude);
}

QString CalibrationDialog::targetAmplitude() const
{
    return d->targetAmplitude();
}

void CalibrationDialog::setMeasuredAmplitude (const QString &amplitude)
{
    d->setMeasuredAmplitude (amplitude);
}

QString CalibrationDialog::measuredAmplitude() const
{
    return d->measuredAmplitude();
}

void CalibrationDialog::enable()
{
    d->enabled = true;
    d->updateWidgets();

    if(d->isGlobal()) {
        d->setReadOnly();
    }
}


void CalibrationDialog::disable()
{
    d->enabled = false;
    d->updateWidgets();
}

void CalibrationDialog::showAdvanced (bool advanced)
{
    d->ui.advanced->setVisible (advanced);
    d->advanced->setChecked (advanced);
}

void CalibrationDialog::setLooping (bool value)
{
    d->ui.loop->setCheckState (value ? Qt::Checked : Qt::Unchecked);
}

bool CalibrationDialog::isLooping() const
{
    return d->ui.loop->checkState() == Qt::Checked;
}

void CalibrationDialog::setComplete (bool value)
{
    d->ui.buttonBox->button (QDialogButtonBox::Ok)->setEnabled (value);
}

void CalibrationDialog::setPlaying (bool playing)
{
    d->playing = playing;
    d->updateWidgets();
}

void CalibrationDialog::setUnit (const QString &unit)
{
    d->unit = unit;
    d->updateWidgets();
}

QString CalibrationDialog::unit() const
{
    return d->unit;
}

void CalibrationDialog::setClipping (bool value)
{
    d->ui.clippingStack->setCurrentWidget (value ? d->ui.clipping :
            d->ui.noClipping);
}

void CalibrationDialog::setMutedParameters (const QStringList &value)
{
    d->setMutedParameters (value);
}

QStringList CalibrationDialog::mutedParameters() const
{
    QStringList result;

    Q_FOREACH (const QListWidgetItem *item, d->ui.muted->selectedItems())
        if (item->isSelected())
            result.append (item->text());
    return result;
}

void CalibrationDialog::finalize(bool save)
{
        if(save)
                accept();
        else
                reject();
}

void CalibrationDialog::setAutoCalibrateEnabled (bool value) {
    d->autoCalibrateEnabled=value;
    d->autoCalibrateAll->setEnabled(value);
    d->autoCalibrateAll->setVisible(value);
    d->autoCalibrateSingle->setEnabled(value);
    d->autoCalibrateSingle->setVisible(value);
}



} // namespace apex

#include "calibrationdialog.moc"
