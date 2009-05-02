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

// REFACTOR: Rename to RecordOptionsDialog for consistency -- ellis, 2008-09-16
#include "RecordSettingsDialog.h"

#include <math.h>

#include <Idac/IdacProxy.h>
#include <IdacDriver/IdacSettings.h>

#include "Globals.h"


RecordSettingsDialog::RecordSettingsDialog(IdacProxy* idac, bool bSendChanges, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle(tr("%0 Settings").arg(idac->hardwareName()));

	IdacSettings* settings = Globals->idacSettings();

	QStringList ranges;
	if (idac != NULL)
	{
		foreach (int n, idac->ranges())
		{
			QString s;
			double mv = n / 1000.0;
			if (mv >= 1.0)
				s = tr("%0 mV").arg(mv, 0, 'f', 1);
			else
				s = trUtf8("%0 µV").arg(mv * 1000, 0, 'f', 1);
			ranges << s;
		}
	}
	m_idac = (bSendChanges) ? idac : NULL;

	if (settings->nRecordingDuration > 0)
	{
		ui.edtRecordingDuration->setValue(settings->nRecordingDuration);
		ui.chkRecordingDuration->setChecked(true);
	}
	else
	{
		ui.edtRecordingDuration->setValue(30);
		ui.edtRecordingDuration->setEnabled(false);
		ui.chkRecordingDuration->setChecked(false);
	}

	ui.edtGcDelay->setValue(settings->nGcDelay_ms);

	IdacChannelSettings* chan;
	
	chan = &settings->channels[1];

	ui.cmbHighpass_1->addItems(idac->highpassStrings());
	ui.cmbHighpass_1->setCurrentIndex(chan->iHighpass);
	ui.cmbHighpass_1->setEnabled(ui.cmbHighpass_1->count() > 0);
	ui.cmbLowpass_1->addItems(idac->lowpassStrings());
	ui.cmbLowpass_1->setCurrentIndex(chan->iLowpass);
	ui.cmbLowpass_1->setEnabled(ui.cmbLowpass_1->count() > 0);
	ui.chkInvert_1->setChecked(chan->mInvert);
	ui.cmbRange_1->addItems(ranges);
	ui.cmbRange_1->setCurrentIndex(chan->iRange);
	ui.edtOffset_1->setValue(convOffsetSamplesToMicrovolts(chan->nOffset));
	ui.sliderOffset_1->setValue(convOffsetSamplesToSlider(chan->nOffset));
	ui.edtExternalAmplification_1->setValue(chan->nExternalAmplification);
	
	chan = &settings->channels[2];
	ui.cmbHighpass_2->addItems(idac->highpassStrings());
	ui.cmbHighpass_2->setCurrentIndex(chan->iHighpass);
	ui.cmbHighpass_2->setEnabled(ui.cmbHighpass_2->count() > 0);
	ui.cmbLowpass_2->addItems(idac->lowpassStrings());
	ui.cmbLowpass_2->setCurrentIndex(chan->iLowpass);
	ui.cmbLowpass_2->setEnabled(ui.cmbLowpass_2->count() > 0);
	ui.chkInvert_2->setChecked(chan->mInvert);
	ui.cmbRange_2->addItems(ranges);
	ui.cmbRange_2->setCurrentIndex(chan->iRange);
	ui.edtOffset_2->setValue(convOffsetSamplesToMicrovolts(chan->nOffset));
	ui.sliderOffset_2->setValue(convOffsetSamplesToSlider(chan->nOffset));
	ui.edtExternalAmplification_2->setValue(chan->nExternalAmplification);
	
	chan = &settings->channels[0];
	ui.chkRecordOnTrigger->setChecked(settings->bRecordOnTrigger);
	ui.chkInvert_3->setChecked(chan->mInvert & 0x01);

	ui.chkEnabled_4->setChecked(chan->mEnabled & 0x02);
	ui.chkInvert_4->setChecked(chan->mInvert & 0x02);

	m_bSliderMoved_1 = false;
	m_bSliderMoved_2 = false;
}

int RecordSettingsDialog::convOffsetSamplesToMicrovolts(int nSamples) const
{
	double nFactor = 1000000.0 / 0x7fff;
	int nMicrovolts = int(nSamples * nFactor + 0.5);
	return nMicrovolts;
}

int RecordSettingsDialog::convOffsetMicrovoltsToSamples(int nMicrovolts) const
{
	double nFactor = 0x7fff / 1000000.0;
	int nSamples = int(nMicrovolts * nFactor + 0.5);
	return nSamples;
}

int RecordSettingsDialog::convOffsetSamplesToSlider(int n) const
{
	int nSlider = 0;
	if (n > 0)
		nSlider = int(10 * log10((double) n) + 0.5);
	else if (n < 0)
		nSlider = -int(10 * log10((double) -n) + 0.5);
	return nSlider;
}

void RecordSettingsDialog::on_chkRecordingDuration_clicked()
{
	IdacSettings* settings = Globals->idacSettings();

	ui.edtRecordingDuration->setEnabled(ui.chkRecordingDuration->isChecked());
	settings->nRecordingDuration = (ui.chkRecordingDuration->isChecked())
		? ui.edtRecordingDuration->value()
		: 0;
}

void RecordSettingsDialog::on_edtRecordingDuration_editingFinished()
{
	IdacSettings* settings = Globals->idacSettings();
	settings->nRecordingDuration = ui.edtRecordingDuration->value();
}

//
// Helper functions
//

void RecordSettingsDialog::on_cmbHighpass_activated(int iChan, int i)
{
	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	chan.iHighpass = i;
	if (m_idac != NULL)
		m_idac->resendChannelSettings(iChan, chan);
	emit settingsChanged();
}

void RecordSettingsDialog::on_cmbLowpass_activated(int iChan, int i)
{
	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	chan.iLowpass = i;
	if (m_idac != NULL)
		m_idac->resendChannelSettings(iChan, chan);
	emit settingsChanged();
}

void RecordSettingsDialog::on_chkInvert_clicked(int iChan, bool bInvert)
{
	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings* chan = &settings->channels[iChan];
	chan->mInvert = bInvert;
	emit settingsChanged();
}

void RecordSettingsDialog::on_cmbRange_activated(int iChan, int i)
{
	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings* chan = &settings->channels[iChan];
	chan->iRange = i;
	if (m_idac != NULL)
	{
		m_idac->stopSampling();
		m_idac->startSampling(settings->channels);
	}
	emit settingsChanged();
}

int RecordSettingsDialog::on_edtOffset_valueChanged(int iChan, int n)
{
	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	// Account for the fact that edtOffset increments by 30, but we actually need more like 30.5...
	chan.nOffset = convOffsetMicrovoltsToSamples(n);

	int nSlider = convOffsetSamplesToSlider(chan.nOffset);

	if (m_idac != NULL)
		m_idac->resendChannelSettings(iChan, chan);

	emit settingsChanged();

	return nSlider;
}

int RecordSettingsDialog::on_sliderOffset_sliderMoved(int n)
{
	int nOffset = 0;
	if (n > 0)
		nOffset = int(pow(10.0, n / 10.0) + 0.5);
	else if (n < 0)
		nOffset = -int(pow(10.0, -n / 10.0) + 0.5);

	return nOffset;
}

void RecordSettingsDialog::on_edtExternalAmplification_editingFinished(int iChan, int n)
{
	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings* chan = &settings->channels[iChan];
	chan->nExternalAmplification = n;
	emit settingsChanged();
}

//
// EAD
//

void RecordSettingsDialog::on_cmbHighpass_1_activated(int i)
{
	on_cmbHighpass_activated(1, i);
}

void RecordSettingsDialog::on_cmbLowpass_1_activated(int i)
{
	on_cmbLowpass_activated(1, i);
}

void RecordSettingsDialog::on_chkInvert_1_clicked()
{
	on_chkInvert_clicked(1, ui.chkInvert_1->isChecked());
}

void RecordSettingsDialog::on_cmbRange_1_activated(int i)
{
	on_cmbRange_activated(1, i);
}

void RecordSettingsDialog::on_edtOffset_1_valueChanged(int n)
{
	int nSlider = on_edtOffset_valueChanged(1, n);

	if (!m_bSliderMoved_1)
		ui.sliderOffset_1->setValue(nSlider);
}

void RecordSettingsDialog::on_sliderOffset_1_sliderMoved(int n)
{
	int nOffset = on_sliderOffset_sliderMoved(n);

	m_bSliderMoved_1 = true;
	ui.edtOffset_1->setValue(nOffset);
	m_bSliderMoved_1 = false;
}

void RecordSettingsDialog::on_edtExternalAmplification_1_editingFinished()
{
	on_edtExternalAmplification_editingFinished(1, ui.edtExternalAmplification_1->value());
}

//
// FID
//

void RecordSettingsDialog::on_cmbHighpass_2_activated(int i)
{
	on_cmbHighpass_activated(2, i);
}

void RecordSettingsDialog::on_cmbLowpass_2_activated(int i)
{
	on_cmbLowpass_activated(2, i);
}

void RecordSettingsDialog::on_chkInvert_2_clicked()
{
	on_chkInvert_clicked(2, ui.chkInvert_2->isChecked());
}

void RecordSettingsDialog::on_cmbRange_2_activated(int i)
{
	on_cmbRange_activated(2, i);
}

void RecordSettingsDialog::on_edtOffset_2_valueChanged(int n)
{
	int nSlider = on_edtOffset_valueChanged(2, n);

	if (!m_bSliderMoved_2)
		ui.sliderOffset_2->setValue(nSlider);
}

void RecordSettingsDialog::on_sliderOffset_2_sliderMoved(int n)
{
	int nOffset = on_sliderOffset_sliderMoved(n);

	m_bSliderMoved_2 = true;
	ui.edtOffset_2->setValue(nOffset);
	m_bSliderMoved_2 = false;
}

void RecordSettingsDialog::on_edtExternalAmplification_2_editingFinished()
{
	on_edtExternalAmplification_editingFinished(2, ui.edtExternalAmplification_2->value());
}

void RecordSettingsDialog::on_edtGcDelay_valueChanged(int n)
{
	IdacSettings* settings = Globals->idacSettings();
	settings->nGcDelay_ms = n;
}

//
// Trigger
//

void RecordSettingsDialog::on_chkRecordOnTrigger_clicked()
{
	IdacSettings* settings = Globals->idacSettings();
	settings->bRecordOnTrigger = ui.chkRecordOnTrigger->isChecked();
}

void RecordSettingsDialog::on_chkInvert_3_clicked()
{
	if (ui.chkInvert_3->isChecked())
		Globals->idacSettings()->channels[0].mInvert |= 0x01;
	else
		Globals->idacSettings()->channels[0].mInvert &= ~0x01;
}

//
// Digital Siganl
//

void RecordSettingsDialog::on_chkEnabled_4_clicked()
{
	if (ui.chkEnabled_4->isChecked())
		Globals->idacSettings()->channels[0].mEnabled |= 0x02;
	else
		Globals->idacSettings()->channels[0].mEnabled &= ~0x02;
}

void RecordSettingsDialog::on_chkInvert_4_clicked()
{
	if (ui.chkInvert_4->isChecked())
		Globals->idacSettings()->channels[0].mInvert |= 0x02;
	else
		Globals->idacSettings()->channels[0].mInvert &= ~0x02;
}
