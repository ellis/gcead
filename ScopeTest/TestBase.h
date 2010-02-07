/**
 * Copyright (C) 2010  Ellis Whitehead
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

#ifndef __TEST_BASE_H
#define __TEST_BASE_H

#include <QtDebug>
#include <QSettings>

#include <AppDefines.h>
#include <Globals.h>

#include <Scope/MainScope.h>
#include <Scope/MainScopeUi.h>


class TestUi : public MainScopeUi
{
public:
	QString s;
	QMessageBox::StandardButton btn;

	QString sInformation, sWarning, sError, sStatus;
	QString sLastDir, sCurrentFilename, sComment;

	void clear() {
		s = sInformation = sWarning = sError = sStatus = QString();
	}

	virtual QString getFileOpenFilename(const QString& sLastDir) { this->sLastDir = sLastDir; return s; }
	virtual QString getFileSaveAsFilename(const QString& sCurrentFilename) { this->sCurrentFilename = sCurrentFilename; return s; }
	/// Let user edit the file comment
	virtual QString getComment(const QString& sComment) { this->sComment = sComment; return s; }
	/// Return QMessageBox::Save, QMessageBox::Discard, or QMessageBox::Cancel
	virtual QMessageBox::StandardButton warnAboutUnsavedChanged() { return btn; }
	/// Pose a question to the user in dialog-box form and get an answer back
	virtual QMessageBox::StandardButton question(const QString&, const QString& /*text*/, QMessageBox::StandardButtons, QMessageBox::StandardButton) { return btn; }
	/// Show user this information and prompt for acknowledgement
	virtual void showInformation(const QString& sTitle, const QString& sInformation) { Q_UNUSED(sTitle); this->sInformation = sInformation; }
	/// Show user this warning message and prompt for acknowledgement
	virtual void showWarning(const QString& sWarning) { this->sWarning = sWarning; }
	/// Show user an error message and prompt for acknowledgement
	virtual void showError(const QString& sTitle, const QString& sError) { Q_UNUSED(sTitle); this->sError = sError; }
	/// Show status message (does not require user acknowledgement)
	virtual void showStatusMessage(const QString& sStatus) { this->sStatus = sStatus; }

	/// Wait for hardware to become available before proceeding.
	/// @param bCloseOnAvailable if true, the dialog will automatically close as soon as the hardware becomes available.
	/// @returns true if the hardware is now available, false if user clicks on "Cancel" while waiting.
	virtual bool waitForHardware(IdacProxy* /*idac*/, bool /*bCloseOnAvailable*/) { return false; }
	/// Let the user configure/setup recording
	/// @returns true if recording should be initiated
	virtual bool showRecordPreview(IdacProxy* idac) { Q_UNUSED(idac); return false; }
	/// Let the user set the recording options
	virtual void showRecordOptions(IdacProxy* idac) { Q_UNUSED(idac); }
};


class TestBase
{
public:
	TestBase();
	~TestBase();

protected:
	/// Construct a filename
	QString getFilename(const QString& sLabel);
	/// Get a chart snapshot
	QImage getImage();
	/// Save chart snapshot to disk and return the filename
	QString snap(const QString& sLabel);
	QString snapAndContrast(const QString& sLabel, const QString& sFilenameContrast);
	bool compare(const QImage& image, const QString& sFilename, const QString& sFilenameContrast, bool bExpectEqual);
	void compare(const QString& sLabel, const QString& sFilenameContrast, bool bExpectEqual = true);

	void contrast(const QString& sLabel, const QString& sFilename) { compare(sLabel, sFilename, false); }

protected:
	TestUi* ui;
	MainScope* scope;
	QSize sz;
	int iStep;
};

#endif
