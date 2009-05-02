/**
 * Copyright (C) 2008  Ellis Whitehead
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "RecordDialog.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include <AppDefines.h>
#include <Check.h>
#include <Globals.h>
#include <Utils.h>

#include <Idac/IdacProxy.h>
#include <IdacDriver/IdacSettings.h>
#include <Widgets/DigitalSignalsWidget.h>
#include <Widgets/SignalWidget.h>
#include <Widgets/SweepWidget.h>
#include <RecordHandler.h>
#include <RecordSettingsDialog.h>


RecordDialog::RecordDialog(IdacProxy* idac, QWidget* parent)
	: QDialog(parent)
{
	m_idac = idac;

	ui.setupUi(this);
	setWindowFlags((windowFlags() | Qt::WindowMaximizeButtonHint) & ~Qt::WindowContextHelpButtonHint);

	// TODO: this doesn't really work -- I probably need to take the border/margin/frame size into account too -- ellis, 2009-05-01
	ui.lblSens->setMinimumWidth(ui.lblSens->fontMetrics().width(tr("%0 mV").arg(0.05)));
	ui.eadSignal->setDivisionCount(10);

	// Create timer to collect data from IDAC
	m_timer = new QTimer(this);
	m_timer->setInterval(200); // 200ms
	connect(m_timer, SIGNAL(timeout()), this, SLOT(getData()));

	m_handler = new RecordHandler(m_idac);

	m_bDataReceived = false;
	m_bOptionsDialogOpen = false;

	loadSettings();
	on_spnWindow_valueChanged();
	// This forces the time-axis to be placed in the middle (the value passed is arbitrary)
	ui.eadSignal->setRange(m_nVoltsPerDivision * 10);

	connect(m_idac, SIGNAL(stateChanged(IdacState)), this, SLOT(updateStatus()));
	updateStatus();

	m_idac->startSampling(Globals->idacSettings()->channels);
}

RecordDialog::~RecordDialog()
{
	delete m_handler;
	saveSettings();
}

void RecordDialog::loadSettings()
{
	QSettings settings("Syntech", APPSETTINGSKEY);

	settings.beginGroup("RecordPreview");
	m_nVoltsPerDivision = settings.value("EadSensitivity", 0.05).toDouble();
	int nWindow = settings.value("WindowWidth", 10).toInt();

	ui.spnWindow->setValue(nWindow);

	updateSens();
}

void RecordDialog::saveSettings()
{
	QSettings settings("Syntech", APPSETTINGSKEY);

	settings.beginGroup("RecordPreview");
	settings.setValue("EadSensitivity", m_nVoltsPerDivision);
	settings.setValue("WindowWidth", ui.spnWindow->value());
}

void RecordDialog::updateSens()
{
	ui.lblSens->setText(tr("%0 mV").arg(m_nVoltsPerDivision));
}

void RecordDialog::done(int r)
{
	QDialog::done(r);
	if (result() != QDialog::Accepted)
		m_idac->stopSampling();
}

void RecordDialog::updateStatus()
{
	//QColor clrStartSave = ui.btnStartSave->palette().window().color();
	switch (m_idac->state())
	{
	case IdacState_Searching:
	case IdacState_Initializing:
		ui.btnConnect->setEnabled(false);
		ui.btnRecord->setEnabled(false);
		break;

	case IdacState_None:
	case IdacState_NotPresent:
	case IdacState_Present:
	case IdacState_InitError:
		ui.btnConnect->setEnabled(true);
		ui.btnRecord->setEnabled(false);
		break;

	case IdacState_Ready:
		ui.btnConnect->setEnabled(true);
		ui.btnRecord->setEnabled(false);
		m_handler->updateRawToVoltageFactors();
		break;
	
	case IdacState_Sampling:
		// Receiving state AND we've actually received data
		if (m_bDataReceived)
		{
			ui.btnConnect->setEnabled(true);
			ui.btnRecord->setEnabled(true);
			// Focus on the [Record] button
			ui.btnRecord->setFocus();
		}
		// Receiving state, BUT we haven't received any data yet
		else
		{
			ui.btnConnect->setEnabled(true);
			ui.btnRecord->setEnabled(false);
			// We need to call m_handler->updateRawToVoltageFactors() 
			// before m_handle->convert() gets called in this->getData()
			settingsChanged();
		}
		break;
	}

	if (m_idac->state() == IdacState_Sampling)
		ui.btnConnect->setText(tr("Re&connect"));
	else
		ui.btnConnect->setText(tr("&Connect"));

	if (m_idac->state() == IdacState_Sampling)
		m_timer->start();
	else
		m_timer->stop();
}

void RecordDialog::settingsChanged()
{
	m_handler->updateRawToVoltageFactors();
}

void RecordDialog::getData()
{
	if (!m_handler->check() || !m_handler->convert())
		return;

	if (!m_bDataReceived)
	{
		m_bDataReceived = true;
		updateStatus();
	}

	// Process digital signals (and handle trigger)
	const QVector<short>& digital = m_handler->digitalRaw();
	int iRecording = -1;
	for (int i = 0; i < digital.size(); i++)
	{
		short n = digital[i];
		ui.digitalSignals->addSample(n);

		bool bTrigger = ((n & 0x01) > 0);
		// Start recording if the settings dialog isn't open, etc.
		if (
			!m_bOptionsDialogOpen && 
			Globals->idacSettings()->bRecordOnTrigger && 
			bTrigger)
		{
			iRecording = i;
		}
	}

	// Display EAD and FID data
	ui.eadSignal->addSamples(m_handler->eadDisplay());
	ui.fidSignal->addSamples(m_handler->fidDisplay());

	if (iRecording >= 0)
		accept();
}

void RecordDialog::on_btnSensDec_clicked()
{
	m_nVoltsPerDivision = changeVoltsPerDivision(m_nVoltsPerDivision, 1);
	ui.eadSignal->setRange(m_nVoltsPerDivision * 10);
	updateSens();
}

void RecordDialog::on_btnSensInc_clicked()
{
	m_nVoltsPerDivision = changeVoltsPerDivision(m_nVoltsPerDivision, -1);
	ui.eadSignal->setRange(m_nVoltsPerDivision * 10);
	updateSens();
}

void RecordDialog::on_spnWindow_valueChanged()
{
	int nSamples = ui.spnWindow->value() * EAD_SAMPLES_PER_SECOND;
	ui.eadSignal->setSampleCount(nSamples);
	ui.fidSignal->setSampleCount(nSamples);
	ui.digitalSignals->setSampleCount(nSamples);
}

void RecordDialog::on_btnRecord_clicked()
{
	accept();
}

void RecordDialog::on_btnConnect_clicked()
{
	m_idac->stopSampling();
	m_idac->startSampling(Globals->idacSettings()->channels);
}

void RecordDialog::on_btnOptions_clicked()
{
	m_bOptionsDialogOpen = true;
	RecordSettingsDialog dlg(m_idac, true, this);
	connect(&dlg, SIGNAL(settingsChanged()), this, SLOT(settingsChanged()));
	dlg.exec();
	m_bOptionsDialogOpen = false;
}

void RecordDialog::on_btnCancel_clicked()
{
	reject();
}
