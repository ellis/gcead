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

#ifndef RECORDDIALOG_H
#define RECORDDIALOG_H

#include <QDialog>
#include "ui_RecordDialog.h"

class IdacProxy;
class RecordHandler;
class WaveInfo;


class RecordDialog : public QDialog
{
	Q_OBJECT

public:
	RecordDialog(IdacProxy* idac, QWidget *parent = NULL);
	~RecordDialog();

// QDialog overrides
public:
	void done(int r);

private:
	void setupWidgets();
	void loadSettings();
	void saveSettings();
	void updateSens();

private slots:
	void updateStatus();
	void settingsChanged();
	void getData();
	void on_btnSensDec_clicked();
	void on_btnSensInc_clicked();
	void on_spnWindow_valueChanged();
	void on_btnRecord_clicked();
	void on_btnConnect_clicked();
	void on_btnOptions_clicked();
	void on_btnCancel_clicked();

private:
	Ui::RecordDialogClass ui;

	IdacProxy* m_idac;

	double m_nVoltsPerDivision;

	QTimer* m_timer;
	RecordHandler* m_handler;

	bool m_bDataReceived;
	/// True when RecordSettingsDialog is open, in order to prevent triggers from initiating recording
	bool m_bOptionsDialogOpen;
};

#endif // RECORDDIALOG_H
