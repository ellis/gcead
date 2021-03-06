/**
 * Copyright (C) 2008,2010  Ellis Whitehead
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
#include <IdacDriver/IdacCaps.h>
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

	if (settings->nRecordingDuration <= 0)
		settings->nRecordingDuration = 30;
	ui.edtRecordingDuration->setValue(settings->nRecordingDuration);

	ui.edtGcDelay->setValue(settings->nGcDelay_ms);

	IdacChannelSettings* chan;
	const IdacCaps* caps = idac->caps();
	
	// Setup the range editor for all channels
	ui.lblGeneralRange->setVisible(!caps->bRangePerChannel);
	ui.lblGeneralRange->setEnabled(!caps->bRangePerChannel);
	ui.cmbGeneralRange->setVisible(!caps->bRangePerChannel);
	ui.cmbGeneralRange->setEnabled(!caps->bRangePerChannel);
	ui.cmbGeneralRange->addItems(ranges);
	ui.cmbGeneralRange->setCurrentIndex(settings->channels[1].iRange);

	chan = &settings->channels[1];

	ui.cmbLowcut_1->addItems(idac->lowcutStrings());
	ui.cmbLowcut_1->setCurrentIndex(chan->iLowcut);
	ui.cmbLowcut_1->setEnabled(ui.cmbLowcut_1->count() > 0);
	ui.lblHighcut_1->setVisible(caps->bHighcut);
	ui.cmbHighcut_1->setVisible(caps->bHighcut);
	ui.cmbHighcut_1->addItems(idac->highcutStrings());
	ui.cmbHighcut_1->setCurrentIndex(chan->iHighcut);
	ui.chkInvert_1->setChecked(chan->mInvert);
	ui.lblRange_1->setVisible(caps->bRangePerChannel);
	ui.cmbRange_1->setVisible(caps->bRangePerChannel);
	ui.cmbRange_1->addItems(ranges);
	ui.cmbRange_1->setCurrentIndex(chan->iRange);
	ui.edtOffset_1->setValue(convOffsetSamplesToMicrovolts(chan->nOffset));
	ui.edtExternalAmplification_1->setValue(chan->nExternalAmplification);
	// Hide this label, but make sure it has the appropriate width
	ui.lblDelay_1->setMinimumWidth(ui.lblDelay_1->sizeHint().width());
	ui.lblDelay_1->setText("");
	
	chan = &settings->channels[2];
	ui.cmbLowcut_2->addItems(idac->lowcutStrings());
	ui.cmbLowcut_2->setCurrentIndex(chan->iLowcut);
	ui.cmbLowcut_2->setEnabled(ui.cmbLowcut_2->count() > 0);
	ui.lblHighcut_2->setVisible(caps->bHighcut);
	ui.cmbHighcut_2->setVisible(caps->bHighcut);
	ui.cmbHighcut_2->addItems(idac->highcutStrings());
	ui.cmbHighcut_2->setCurrentIndex(chan->iHighcut);
	ui.chkInvert_2->setChecked(chan->mInvert);
	ui.lblRange_2->setVisible(caps->bRangePerChannel);
	ui.cmbRange_2->setVisible(caps->bRangePerChannel);
	ui.cmbRange_2->addItems(ranges);
	ui.cmbRange_2->setCurrentIndex(chan->iRange);
    ui.cmbRange_2->setValidator(NULL);
	ui.edtOffset_2->setValue(convOffsetSamplesToMicrovolts(chan->nOffset));
	ui.edtExternalAmplification_2->setValue(chan->nExternalAmplification);
	
	chan = &settings->channels[0];
	ui.chkRecordOnTrigger->setChecked(settings->bRecordOnTrigger);
	ui.chkInvert_3->setChecked(chan->mInvert & 0x01);

	ui.chkEnabled_4->setChecked(chan->mEnabled & 0x02);
	ui.chkEnabled_4->setEnabled(false); // TODO: add this back in once we respect the setting -- ellis, 2010-11-07
	ui.chkInvert_4->setChecked(chan->mInvert & 0x02);

	m_bSliderMoved_1 = false;
	m_bSliderMoved_2 = false;
}

int RecordSettingsDialog::convOffsetSamplesToMicrovolts(int nSamples) const
{
	double nFactor = 1000000.0 / 0x7fff;
	double nRounder = (nSamples >= 0) ? 0.5 : -0.5;
	int nMicrovolts = int(nSamples * nFactor + nRounder);
	return nMicrovolts;
}

int RecordSettingsDialog::convOffsetMicrovoltsToSamples(int nMicrovolts) const
{
	double nFactor = 0x7fff / 1000000.0;
	double nRounder = (nMicrovolts >= 0) ? 0.5 : -0.5;
	int nSamples = int(nMicrovolts * nFactor + nRounder);
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

/*void RecordSettingsDialog::on_chkRecordingDuration_clicked()
{
	IdacSettings* settings = Globals->idacSettings();

	ui.edtRecordingDuration->setEnabled(ui.chkRecordingDuration->isChecked());
	settings->nRecordingDuration = (ui.chkRecordingDuration->isChecked())
		? ui.edtRecordingDuration->value()
		: 0;
}*/

void RecordSettingsDialog::on_edtRecordingDuration_editingFinished()
{
	IdacSettings* settings = Globals->idacSettings();
	settings->nRecordingDuration = ui.edtRecordingDuration->value();
}

void RecordSettingsDialog::on_cmbGeneralRange_activated(int i)
{
	IdacSettings* settings = Globals->idacSettings();
	settings->channels[1].iRange = i;
	settings->channels[2].iRange = i;
	if (m_idac != NULL)
	{
		m_idac->stopSampling();
		m_idac->startSampling(settings->channels);
	}
	emit settingsChanged();
}

//
// Helper functions
//

void RecordSettingsDialog::on_cmbLowcut_activated(int iChan, int i)
{
	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	chan.iLowcut = i;
	if (m_idac != NULL)
		m_idac->resendChannelSettings(iChan, chan);
	emit settingsChanged();
}

void RecordSettingsDialog::on_cmbHighcut_activated(int iChan, int i)
{
	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	chan.iHighcut = i;
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

void RecordSettingsDialog::on_cmbLowcut_1_activated(int i)
{
	on_cmbLowcut_activated(1, i);
}

void RecordSettingsDialog::on_cmbHighcut_1_activated(int i)
{
	on_cmbHighcut_activated(1, i);
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
	on_edtOffset_valueChanged(1, n);
}

void RecordSettingsDialog::on_edtExternalAmplification_1_editingFinished()
{
	on_edtExternalAmplification_editingFinished(1, ui.edtExternalAmplification_1->value());
}

//
// FID
//

void RecordSettingsDialog::on_cmbLowcut_2_activated(int i)
{
	on_cmbLowcut_activated(2, i);
}

void RecordSettingsDialog::on_cmbHighcut_2_activated(int i)
{
	on_cmbHighcut_activated(2, i);
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
	on_edtOffset_valueChanged(2, n);
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
