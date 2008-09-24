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

#ifndef __MAINSCOPEUI_H
#define __MAINSCOPEUI_H

#include <QMessageBox>


class Idac;


class MainScopeUi
{
public:
	virtual ~MainScopeUi()
	{
	}

	virtual QString getFileOpenFilename(const QString& sLastDir) = 0;
	virtual QString getFileSaveAsFilename(const QString& sCurrentFilename) = 0;
	/// Let user edit the file comment
	virtual QString getComment(const QString& sComment) = 0;
	/// Return QMessageBox::Save, QMessageBox::Discard, or QMessageBox::Cancel
	virtual QMessageBox::StandardButton warnAboutUnsavedChanged() = 0;
	/// Pose a question to the user in dialog-box form and get an answer back
	virtual QMessageBox::StandardButton question(const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton) = 0;
	/// Show user this information and prompt for acknowledgement
	virtual void showInformation(const QString& sTitle, const QString& sInformation) = 0;
	/// Show user this warning message and prompt for acknowledgement
	virtual void showWarning(const QString& sWarning) = 0;
	/// Show user an error message and prompt for acknowledgement
	virtual void showError(const QString& sTitle, const QString& sError) = 0;
	/// Show status message (does not require user acknowledgement)
	virtual void showStatusMessage(const QString& sStatus) = 0;

	/// Let the user configure/setup recording
	/// @returns true if recording should be initiated
	virtual bool showRecordPreview(Idac* idac) = 0;
	/// Let the user set the recording options
	virtual void showRecordOptions(Idac* idac) = 0;
};

#endif
