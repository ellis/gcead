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

#ifndef __MAINWINDOWUI_H
#define __MAINWINDOWUI_H

#include "MainScopeUi.h"


class QMainWindow;

class IdacProxy;


class MainWindowUi : public MainScopeUi
{
public:
	/// @param parent is used for displaying message boxes
	MainWindowUi(QMainWindow* parent);
	
// MainScopeUi overrides
public:
	QString getFileOpenFilename(const QString& sLastDir);
	QString getFileSaveAsFilename(const QString& sCurrentFilename);
	QString getComment(const QString& sComment);
	QMessageBox::StandardButton warnAboutUnsavedChanged();
	QMessageBox::StandardButton question(const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton);
	void showInformation(const QString& sTitle, const QString& sInformation);
	void showWarning(const QString& sWarning);
	void showError(const QString& sTitle, const QString& sError);
	void showStatusMessage(const QString& sStatus);

	bool waitForHardware(IdacProxy* idac, bool bCloseOnAvailable);
	bool showRecordPreview(IdacProxy* idac);
	void showRecordOptions(IdacProxy* idac);

private:
	QMainWindow* m_widget;
};

#endif
