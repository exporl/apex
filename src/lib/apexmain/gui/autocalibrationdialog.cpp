/******************************************************************************
 * Copyright (C) 2007  Pieter Verlinde										  *
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

#include "autocalibrationdialog.h"
#include "ui_autocalibrationdialog.h"

#include <QtGui>
#include <QPushButton>
#include <QTreeWidget>
#include <QModelIndex>
#include <QSize>
#include <QCheckBox>
#include <QRegExp>


namespace apex
{

class AutoCalibrationDialogPrivate : public QObject
{
    Q_OBJECT
public:
    AutoCalibrationDialogPrivate (AutoCalibrationDialog *pub);
	
	void setParameters(const QList<QStringList> &values); 
	void setSlmParameters(const QStringList &values);
	void setHardwareSetups (QStringList value);

    Ui::AutoCalibrationDialog ui;
    QPushButton *advanced;    
	QPushButton *calibrateAll;
	QPushButton *calibrateManually;
	QPushButton *stop;
private:
	void updateWidgets();

private Q_SLOTS:
	void on_hardware_currentIndexChanged (int index);

private:
    AutoCalibrationDialog * const p;
	QList<QTreeWidgetItem*> parameterWidgets;
	QList<QTreeWidgetItem*> slmParameterWidgets;


	unsigned ignoreHardware;

	QString oldHardwareSetup;

    QStringList setups;

	bool enabled;
	bool calibrating;

	friend class AutoCalibrationDialog;
};

// AutoCalibrationDialogPrivate ====================================================

AutoCalibrationDialogPrivate::AutoCalibrationDialogPrivate (AutoCalibrationDialog *pub) :
    p (pub),
	ignoreHardware (0),
	enabled (true),
	calibrating (false)
{
    ui.setupUi (pub);
	

	calibrateAll = ui.buttonBox->addButton (tr ("Calibrate Selected"),
		QDialogButtonBox::ActionRole);
	stop = ui.buttonBox->addButton(tr ("Stop"),
		QDialogButtonBox::ActionRole);
	advanced = ui.buttonBox->addButton (tr ("Advanced.."),
	QDialogButtonBox::ActionRole);
	calibrateManually = ui.buttonBox->addButton(tr ("Calibrate Manually"),
		QDialogButtonBox::ActionRole);
	
    advanced->setCheckable (true);
	//ui.parameterTree->resizeColumnToContents(1);
	//ui.parameterTree->resizeColumnToContents(2);
	ui.parameterTree->setRootIsDecorated(false);
	ui.parameterTree->setSelectionMode(QAbstractItemView::MultiSelection);
	//ui.parameterTree->setEditFocus(false);
	
	ui.amplitudeTxt->setEnabled(false);
	ui.amplitudeLbl->setEnabled(false);
	ui.amplitudeTxt->setValidator(new QDoubleValidator(ui.amplitudeTxt));
	QRegExp exp("0\\.[0-9]\\d{1,1}");
	ui.percTxt->setValidator(new QRegExpValidator(exp, ui.percTxt));
	ui.timeTxt->setValidator(new QIntValidator(1, 100, ui.timeTxt));

	connect (calibrateAll, SIGNAL(clicked()),
			pub, SIGNAL (calibrateAllParameters())); 
	connect (stop, SIGNAL(clicked()),
			pub, SIGNAL(stopAutoCalibration()));
	//Make sure the dialog doesn't hide when receiving the accepted/rejected signal
	disconnect(ui.buttonBox->button(QDialogButtonBox::Cancel), 0, 0, 0); 
	
	connect(ui.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
			pub, SIGNAL(beforeCancel()));
	connect (ui.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
			pub, SIGNAL(stopAutoCalibration()));
    connect (calibrateManually, SIGNAL(clicked()),
			pub, SIGNAL(calibrateManually()));
	connect (advanced, SIGNAL (toggled(bool)),
            pub, SLOT (showAdvanced(bool)));
	connect(ui.typeCbox, SIGNAL(currentIndexChanged(QString)),
			pub, SLOT(slmTypeChanged(QString)));
	connect(ui.freqCbox, SIGNAL(currentIndexChanged(QString)),
			pub, SLOT(slmFreqWeighChanged(QString)));
	connect(ui.timeCbox, SIGNAL(currentIndexChanged(QString)),
			pub, SLOT(slmTimeWeighChanged(QString)));
	connect(ui.percTxt, SIGNAL(textChanged(QString)),
			pub, SLOT(slmPercChanged(QString)));
	connect(ui.timeTxt, SIGNAL(textChanged(QString)),
			pub, SLOT(slmTimeChanged(QString)));
    connect (ui.hardware, SIGNAL (currentIndexChanged(int)),
            this, SLOT (on_hardware_currentIndexChanged(int)));
	connect(ui.amplitudeCbox, SIGNAL(stateChanged(int)),
			pub, SLOT(enableUserDefinedCalibrationAmplitude(int)));
	ui.advanced->hide();

	ui.progressBar->setValue(0);

	updateWidgets();
}

void AutoCalibrationDialogPrivate::updateWidgets()
{
	ui.parameterTree->setEnabled(enabled);
	calibrateManually->setEnabled(!calibrating);
    calibrateAll->setEnabled (enabled && !calibrating);
    stop->setEnabled (enabled && calibrating);
	ui.typeCbox->setEnabled(enabled && !calibrating);
	ui.freqCbox->setEnabled(enabled && !calibrating);
	ui.timeCbox->setEnabled(enabled && !calibrating);
	ui.percTxt->setEnabled(enabled && !calibrating);
	ui.timeTxt->setEnabled(enabled && !calibrating);
	ui.label->setEnabled(enabled && !calibrating);
	ui.label3->setEnabled(enabled && !calibrating);
	ui.label4->setEnabled(enabled && !calibrating);
	ui.label5->setEnabled(enabled && !calibrating);
	ui.label6->setEnabled(enabled && !calibrating);
	ui.amplitudeTxt->setEnabled(enabled && !calibrating && ui.amplitudeCbox->checkState() == Qt::Checked);
	ui.amplitudeLbl->setEnabled(enabled && !calibrating 
		&& ui.amplitudeCbox->checkState() == Qt::Checked);
	ui.amplitudeCbox->setEnabled(enabled && !calibrating);
	ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enabled && !calibrating);
}


void AutoCalibrationDialogPrivate::on_hardware_currentIndexChanged
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
        const QString value = ui.hardware->text (index);
        if (oldHardwareSetup != value) {
            emit p->hardwareSetupChanged (value);
            oldHardwareSetup = value;
        }
    }
}

void AutoCalibrationDialogPrivate::setHardwareSetups (QStringList value)
{
    setups = value;
    ++ignoreHardware;
    const QString current = p->hardwareSetup();
    ui.hardware->clear();
    ui.hardware->addItems (value);
    ui.hardware->addItem (tr ("<Manage profiles...>"), true);
    p->setHardwareSetup (current);
    --ignoreHardware;
    // The handler will only emit if the old and the new setup are different
    on_hardware_currentIndexChanged (ui.hardware->currentIndex());
}

void AutoCalibrationDialogPrivate::setParameters(const QList<QStringList> &values)
{
	QTreeWidget *tree = ui.parameterTree;
	tree->clear();
	parameterWidgets.clear();
	for(int i = 0; i < values.size(); ++i){
		QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*) 0, values.at(i));
		parameterWidgets.append(item);
	}	
	tree->insertTopLevelItems(0, parameterWidgets);
	for(int i = 0; i < tree->topLevelItemCount(); ++i){
		QCheckBox *cb = new QCheckBox(tree);
		cb->setCheckState(Qt::Unchecked);
		cb->setEnabled(false);
		tree->setItemWidget(tree->topLevelItem(i), tree->columnCount() - 1, cb);
	}
}

void AutoCalibrationDialog::updateParameter(const QString &parameter, const QString &value, const QString &difference)
{ 
	QList<QTreeWidgetItem*> items = d->ui.parameterTree->findItems(parameter, Qt::MatchFixedString);
	items.at(0)->setData(2, 0, QVariant(value));
	items.at(0)->setData(3, 0, QVariant(difference));
	((QCheckBox*) d->ui.parameterTree->itemWidget(items.at(0),
		d->ui.parameterTree->columnCount() - 1))->setCheckState(Qt::Checked);
	((QCheckBox*) d->ui.parameterTree->itemWidget(items.at(0),
		d->ui.parameterTree->columnCount() - 1))->setEnabled(true);
}

void AutoCalibrationDialog::setClipped(const QString &parameter)
{
	QList<QTreeWidgetItem*> items = d->ui.parameterTree->findItems(parameter, Qt::MatchFixedString);
	items.at(0)->setBackground(2, QBrush(QColor(255, 0, 0)));
	items.at(0)->setForeground(2, QBrush(QColor(255, 255, 255)));
	items.at(0)->setData(2, 0, QVariant("SIGNAL HAS CLIPPED"));
	((QCheckBox*) d->ui.parameterTree->itemWidget(items.at(0),
		d->ui.parameterTree->columnCount() - 1))->setCheckState(Qt::Unchecked);
	((QCheckBox*) d->ui.parameterTree->itemWidget(items.at(0),
		d->ui.parameterTree->columnCount() - 1))->setEnabled(false);
}

void AutoCalibrationDialog::newMeasurement()
{
	for(int i = 0; i < d->ui.parameterTree->topLevelItemCount(); ++i){
		if(d->ui.parameterTree->topLevelItem(i)->foreground(2).color() == QColor(255, 255, 255)){
			d->ui.parameterTree->topLevelItem(i)->setBackground(2, QBrush(QColor(255, 255, 255)));
			d->ui.parameterTree->topLevelItem(i)->setForeground(2, QBrush(QColor(0, 0, 0)));
			d->ui.parameterTree->topLevelItem(i)->setData(2, 0, QVariant("-")); //TODO
		}
	}
}

void AutoCalibrationDialogPrivate::setSlmParameters(const QStringList &values)
{
	ui.typeCbox->setCurrentIndex(values.at(0).toInt());
	ui.freqCbox->setCurrentIndex(values.at(1).toInt());
	ui.timeCbox->setCurrentIndex(values.at(2).toInt());
	ui.percTxt->setText(values.at(3));
	ui.timeTxt->setText(values.at(4));
}

// AutoCalibrationDialog ===========================================================

AutoCalibrationDialog::AutoCalibrationDialog (QWidget *parent) :
    QDialog (parent),
    d (new AutoCalibrationDialogPrivate (this))
{
}

AutoCalibrationDialog::~AutoCalibrationDialog()
{
}

void AutoCalibrationDialog::setHardwareSetups (const QStringList &value)
{
    d->setHardwareSetups (value);
}

QStringList AutoCalibrationDialog::hardwareSetups() const
{
    return d->setups;
}

QString AutoCalibrationDialog::hardwareSetup() const
{
    return d->ui.hardware->currentText();
}

void AutoCalibrationDialog::setHardwareSetup (const QString &value)
{
    d->ui.hardware->setCurrentIndex (d->setups.indexOf (value));
	if(value.isEmpty())
		disable();
	else 
		enable();
}

void AutoCalibrationDialog::setParameterList(const QList<QStringList> &values)
{
	d->setParameters(values);
}

void AutoCalibrationDialog::setSlmParameters(const QStringList &values)
{
	d->setSlmParameters(values);
}

void AutoCalibrationDialog::showAdvanced (bool advanced)
{
    d->ui.advanced->setVisible (advanced);
    d->advanced->setChecked (advanced);
}

void AutoCalibrationDialog::setProgressBar(int min, int max, int pos)
{
	if(!(min == 0 && max == 0)) 
		d->ui.progressBar->setRange(min, max);
	d->ui.progressBar->setValue(pos);
}

void AutoCalibrationDialog::setCalibrating (bool calibrating)
{
    d->calibrating = calibrating;
    d->updateWidgets();
}

void AutoCalibrationDialog::setStatus(const QString &status)
{
	d->ui.status->setText(status);
}

void AutoCalibrationDialog::enable()
{
    d->enabled = true;
    d->updateWidgets();
}

void AutoCalibrationDialog::disable()
{
    d->enabled = false;
    d->updateWidgets();
}

void AutoCalibrationDialog::setCurrentParameterCalibrated(const QString &parameter)
{
	QFont font = d->ui.parameterTree->topLevelItem(0)->font(0);
	font.setBold(false);
	for(int i = 0; i < d->ui.parameterTree->topLevelItemCount(); ++i)
		d->ui.parameterTree->topLevelItem(i)->setFont(0, font);
	if(parameter.size() < 1) 
		return;
	QList<QTreeWidgetItem*> items = d->ui.parameterTree->findItems(parameter, Qt::MatchFixedString);
	font.setBold(true);
	items.at(0)->setFont(0, font);
}

QStringList AutoCalibrationDialog::getSelectedParametersToSave()
{
	QStringList checkedParameters;
	for(int i = 0; i < d->ui.parameterTree->topLevelItemCount(); ++i)
	{
		if(((QCheckBox*)d->ui.parameterTree->itemWidget(
			d->ui.parameterTree->topLevelItem(i),
			d->ui.parameterTree->columnCount() - 1))->checkState() == Qt::Checked)
		{
			checkedParameters << d->ui.parameterTree
				->topLevelItem(i)->data(0, 0).toString();
		}
	}
	return checkedParameters;
}

QStringList AutoCalibrationDialog::getSelectedParametersToCalibrate()
{
	QStringList ret;
	QList<QTreeWidgetItem*> list = d->ui.parameterTree->selectedItems();
	for (int i = 0; i < list.size(); ++i) 
		ret << list.at(i)->data(0, 0).toString();
	return ret;
}

void AutoCalibrationDialog::slmTypeChanged(const QString &text)
{
	emit slmParameterChanged("type",text);
}

void AutoCalibrationDialog::slmFreqWeighChanged(const QString &text)
{
	emit slmParameterChanged("frequency_weighting",text);
}

void AutoCalibrationDialog::slmTimeWeighChanged(const QString &text)
{
	emit slmParameterChanged("time_weighting",text);
}

void AutoCalibrationDialog::slmPercChanged(const QString &text)
{
	bool ok;
	double percentile = text.toDouble(&ok);
	if(ok && percentile >= 0 && percentile <= 1)
		emit slmParameterChanged("percentile",QString::number(percentile));
	else {
		setStatus(tr("<span style=\"color:#fff;background-color:#f00\">"
		"Invalid percentile parameter.</span>"));
	}
}

void AutoCalibrationDialog::slmTimeChanged(const QString &text)
{
		emit slmParameterChanged("time",text);
}

QString AutoCalibrationDialog::targetAmplitude() const
{
	return d->ui.amplitudeTxt->text();
}

void AutoCalibrationDialog::setTargetAmplitude(const QString &text)
{
	d->ui.amplitudeTxt->setText(text);
}

void AutoCalibrationDialog::selectAllParamersByDefault()
{
	d->ui.parameterTree->selectAll();
}

void AutoCalibrationDialog::enableUserDefinedCalibrationAmplitude(int checkState)
{
	if(checkState == Qt::Checked)
	{
		d->ui.amplitudeTxt->setEnabled(true);
		d->ui.amplitudeLbl->setEnabled(true);
	}
	else
	{
		d->ui.amplitudeTxt->setEnabled(false);
		d->ui.amplitudeLbl->setEnabled(false);
		emit advancedCalibrationAmplitudeUnchecked();
	}
}

void AutoCalibrationDialog::finalize(bool save)
{
	if(save)
		accept();
	else
		reject();
}

} // namespace apex

#include "autocalibrationdialog.moc"
