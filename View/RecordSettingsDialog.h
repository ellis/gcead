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

#ifndef RECORDSETTINGSDIALOG_H
#define RECORDSETTINGSDIALOG_H

#include <QDialog>

#include "ui_RecordSettingsDialog.h"


class IdacProxy;


class RecordSettingsDialog : public QDialog
{
	Q_OBJECT

public:
	/// @param bSendChanges true if changes the user makes should be 
	/// immediately sent to the hardware.
	RecordSettingsDialog(IdacProxy* idac, bool bSendChanges, QWidget *parent = NULL);

signals:
	void settingsChanged();

private:
	int convOffsetSamplesToMicrovolts(int nSamples) const;
	int convOffsetMicrovoltsToSamples(int nMicrovolts) const;
	int convOffsetSamplesToSlider(int nSamples) const;

private:
	void on_cmbLowcut_activated(int iChan, int i);
	void on_cmbHighcut_activated(int iChan, int i);
	void on_chkInvert_clicked(int iChan, bool bInvert);
	void on_cmbRange_activated(int iChan, int i);
	int on_edtOffset_valueChanged(int iChan, int n);
	int on_sliderOffset_sliderMoved(int n);
	void on_edtExternalAmplification_editingFinished(int iChan, int n);

private slots:
	// Recording duration
	//void on_chkRecordingDuration_clicked();
	void on_edtRecordingDuration_editingFinished();
	void on_cmbRange_activated(int n);

	// EAD
	void on_cmbLowcut_1_activated(int i);
	void on_cmbHighcut_1_activated(int i);
	void on_chkInvert_1_clicked();
	void on_cmbRange_1_activated(int i);
	void on_edtOffset_1_valueChanged(int n);
	void on_sliderOffset_1_sliderMoved(int n);
	void on_edtExternalAmplification_1_editingFinished();

	// FID
	void on_cmbLowcut_2_activated(int i);
	void on_cmbHighcut_2_activated(int i);
	void on_chkInvert_2_clicked();
	void on_cmbRange_2_activated(int i);
	void on_edtOffset_2_valueChanged(int n);
	void on_sliderOffset_2_sliderMoved(int n);
	void on_edtExternalAmplification_2_editingFinished();
	void on_edtGcDelay_valueChanged(int n);

	// Trigger
	void on_chkRecordOnTrigger_clicked();
	void on_chkInvert_3_clicked();

	// Digital Signal
	void on_chkEnabled_4_clicked();
	void on_chkInvert_4_clicked();

private:
	Ui::RecordSettingsDialogClass ui;
	IdacProxy* m_idac;
	bool m_bSliderMoved_1;
	bool m_bSliderMoved_2;
};

#endif // RECORDSETTINGSDIALOG_H
