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

#include "MainWindowUi.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QVBoxLayout>

#include "AppDefines.h"
#include "RecordDialog.h"
#include "RecordSettingsDialog.h"
#include "WaitForHardwareDialog.h"


MainWindowUi::MainWindowUi(QMainWindow* parent)
{
	m_widget = parent;
}

QString MainWindowUi::getFileOpenFilename(const QString& sLastDir)
{
	QString sFilename = QFileDialog::getOpenFileName(
		m_widget,
		QObject::tr("Open GC-EAD Project"),
		sLastDir,
		QObject::tr("GC-EAD files (*.ead)"));
	return sFilename;
}

QString MainWindowUi::getFileSaveAsFilename(const QString& sLastDir)
{
	QString sFilename = QFileDialog::getSaveFileName(
		m_widget,
		QObject::tr("Save Recordings"),
		sLastDir,
		QObject::tr("EAD recordings (*.ead)"));
	return sFilename;
}

QString MainWindowUi::getFileImportFilename(const QString& sLastDir)
{
	QString sFilename = QFileDialog::getOpenFileName(
		m_widget,
		QObject::tr("Import Wave from Another GC-EAD Project"),
		sLastDir,
		QObject::tr("GC-EAD files (*.ead)"));
	return sFilename;
}

QString MainWindowUi::getComment(const QString& sComment)
{
	QDialog dlg(m_widget);
	QVBoxLayout* layout = new QVBoxLayout;
	QPlainTextEdit* edit = new QPlainTextEdit();
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	dlg.setWindowTitle(QObject::tr("Edit File Comment"));
	edit->setPlainText(sComment);

	layout->addWidget(new QLabel(QObject::tr("Comment:")));
	layout->addWidget(edit);
	layout->addWidget(buttons);
	dlg.setLayout(layout);

	QObject::connect(buttons, SIGNAL(accepted()), &dlg, SLOT(accept()));
	QObject::connect(buttons, SIGNAL(rejected()), &dlg, SLOT(reject()));

	if (dlg.exec())
		return edit->toPlainText();
	else
		return sComment;
}

QMessageBox::StandardButton MainWindowUi::warnAboutUnsavedChanged()
{
	QMessageBox::StandardButton ret = QMessageBox::warning(m_widget, QObject::tr(APPNAME),
			QObject::tr("The project has been modified.\n"
			   "Do you want to save your changes?"),
			   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
			   QMessageBox::Save);
	return ret;
}

QMessageBox::StandardButton MainWindowUi::question(const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	return QMessageBox::question(m_widget, title, text, buttons, defaultButton);
}

void MainWindowUi::showInformation(const QString& sTitle, const QString& sInformation)
{
	QMessageBox::information(m_widget, sTitle, sInformation);
}

void MainWindowUi::showWarning(const QString& sWarning)
{
	QMessageBox::warning(m_widget, QObject::tr(APPNAME),
		sWarning,
		QMessageBox::Ok, QMessageBox::NoButton);
}

void MainWindowUi::showError(const QString& sTitle, const QString& sError)
{
	QMessageBox::critical(m_widget, sTitle, sError);
}

void MainWindowUi::showStatusMessage(const QString& sStatus)
{
	m_widget->statusBar()->showMessage(sStatus, 3000);
}

bool MainWindowUi::waitForHardware(IdacProxy* idac, bool bCloseOnAvailable)
{
	WaitForHardwareDialog dlg(idac, bCloseOnAvailable, m_widget);
	int res = dlg.exec();
	return (res == QDialog::Accepted);
}

bool MainWindowUi::showRecordPreview(IdacProxy* idac)
{
	RecordDialog dlg(idac, m_widget);

	// Size the dialog to mostly cover the parent widget
	QRect rc = m_widget->geometry();
	int dx = rc.width() / 10;
	int dy = rc.height() / 10;
	rc.adjust(dx, dy, -dx, -dy);
	dlg.setGeometry(rc);

	return (dlg.exec() == QDialog::Accepted);
}

void MainWindowUi::showRecordOptions(IdacProxy* idac)
{
	RecordSettingsDialog dlg(idac, false, m_widget);
	dlg.exec();
}
