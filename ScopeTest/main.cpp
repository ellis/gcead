/**
 * Copyright (C) 2008, 2009, 2010  Ellis Whitehead
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

#include <QtDebug>
#include <QApplication>
#include <QMutex>
#include <QSettings>

#include <AppDefines.h>
#include <Globals.h>
#include <Idac/IdacFactory.h>

#include <Scope/MainScope.h>
#include <Scope/MainScopeUi.h>


class TestUi : public MainScopeUi
{
public:
	QString s;
	QMessageBox::StandardButton btn;

	QString sInformation, sWarning, sError, sStatus;

	void clear() {
		s = sInformation = sWarning = sError = sStatus = QString();
	}

	virtual QString getFileOpenFilename(const QString& sLastDir) { return s; }
	virtual QString getFileSaveAsFilename(const QString& sCurrentFilename) { return s; }
	/// Let user edit the file comment
	virtual QString getComment(const QString& sComment) { return s; }
	/// Return QMessageBox::Save, QMessageBox::Discard, or QMessageBox::Cancel
	virtual QMessageBox::StandardButton warnAboutUnsavedChanged() { return btn; }
	/// Pose a question to the user in dialog-box form and get an answer back
	virtual QMessageBox::StandardButton question(const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton) { return btn; }
	/// Show user this information and prompt for acknowledgement
	virtual void showInformation(const QString& sTitle, const QString& sInformation) { this->sInformation = sInformation; }
	/// Show user this warning message and prompt for acknowledgement
	virtual void showWarning(const QString& sWarning) { this->sWarning = sWarning; }
	/// Show user an error message and prompt for acknowledgement
	virtual void showError(const QString& sTitle, const QString& sError) { this->sError = sError; }
	/// Show status message (does not require user acknowledgement)
	virtual void showStatusMessage(const QString& sStatus) { this->sStatus = sStatus; }

	/// Wait for hardware to become available before proceeding.
	/// @param bCloseOnAvailable if true, the dialog will automatically close as soon as the hardware becomes available.
	/// @returns true if the hardware is now available, false if user clicks on "Cancel" while waiting.
	virtual bool waitForHardware(IdacProxy* idac, bool bCloseOnAvailable) { return false; }
	/// Let the user configure/setup recording
	/// @returns true if recording should be initiated
	virtual bool showRecordPreview(IdacProxy* idac) { return false; }
	/// Let the user set the recording options
	virtual void showRecordOptions(IdacProxy* idac) {}
};


class Test1
{
public:
	Test1()
	{
		TestUi* ui = new TestUi;
		//IdacProxy* idac = IdacFactory::getProxy();
		scope = new MainScope(ui, NULL);
		sz = scope->chart()->pixmap()->sizeForAvailableArea(QSize(300, 400), 10);
		iStep = 0;

		Actions* actions = scope->actions();
		QString sFilename;

		// Load sample project
		actions->fileLoadSampleProject->trigger();
		QString sFilenameAves = snap("AVEs");
		// Chart: EADs
		actions->viewChartEads->trigger();
		snap("EADs");
		// Chart: FIDs
		actions->viewChartFids->trigger();
		snap("FIDs");
		// Chart: All
		actions->viewChartAll->trigger();
		QString sFilenameAll = snap("ALL");
		// Chart: AVEs
		actions->viewChartAverages->trigger();
		compare("AVEs", sFilenameAves);

		//
		// ZOOMING AND SCROLLING
		//

		// Zoom in three times
		actions->viewChartAll->trigger();
		actions->viewZoomIn->trigger();
		actions->viewZoomIn->trigger();
		actions->viewZoomIn->trigger();
		snap("ZoomIn");
		// Scroll page left (should be at start now)
		actions->viewScrollPageLeft->trigger();
		sFilename = snap("ScrollPageLeft");
		// Try to scroll further to the left (should fail)
		actions->viewScrollPageLeft->trigger();
		compare("ScrollPageLeft", sFilename);
		actions->viewScrollDivLeft->trigger();
		compare("ScrollPageLeft", sFilename);
		// Scroll division right
		actions->viewScrollDivRight->trigger();
		snap("ScrollDivRight");
		// Scroll page right 3 times
		actions->viewScrollPageRight->trigger();
		actions->viewScrollPageRight->trigger();
		actions->viewScrollPageRight->trigger();
		sFilename = snap("ScrollRight");
		// Try to scroll further to the right (should fail)
		actions->viewScrollDivRight->trigger();
		compare("ScrollRight", sFilename);
		actions->viewScrollPageRight->trigger();
		compare("ScrollRight", sFilename);
		// Zoom out 4 times
		actions->viewZoomOut->trigger();
		actions->viewZoomOut->trigger();
		actions->viewZoomOut->trigger();
		actions->viewZoomOut->trigger();
		snap("ZoomOut");
		// Zoom full (should be same as sFilenameAves)
		actions->viewZoomFull->trigger();
		compare("ALL", sFilenameAll);

		//
		// Wave comments
		//

		actions->viewChartAll->trigger();
		actions->viewWaveComments->setChecked(true);
		compare("ALL", sFilenameAll);

		ViewWaveInfo* vwiEad1All = scope->file()->viewInfo(EadView_All)->allVwis()[0];

		// Add a comment
		vwiEad1All->setComment("sample comment");
		QString sFilenameComment = snapAndContrast("WaveComment", sFilenameAll);
		// Hide comments
		actions->viewWaveComments->toggle();
		compare("WaveCommentsOff", sFilenameAll);
		// Turn comments on again
		actions->viewWaveComments->toggle();
		compare("WaveCommentsOn", sFilenameComment);
		// Hide comments again
		actions->viewWaveComments->toggle();
		compare("WaveCommentsOff", sFilenameAll);

		//
		// PEAKS
		//

		actions->viewChartAverages->trigger();

		// Hide peaks (shouldn't change anything visually)
		actions->viewHidePeaks->trigger();
		compare("AVEs", sFilenameAves);
		// Show detected peaks
		actions->viewDetectedPeaks->trigger();
		snapAndContrast("PeaksDetected", sFilenameAves);
		// Edit peaks
		actions->viewEditPeaks->trigger();
		snap("PeaksEdit");
		// Validate a detected peak on the FID wave
		ViewWaveInfo* vwi = scope->file()->viewInfo(EadView_Averages)->vwis()[1];
		vwi->choosePeakAtDidx(vwi->wave()->peaks0[1].middle.i);
		snap("PeaksEdit1");
		// Hide peaks
		actions->viewHidePeaks->trigger();
		compare("AVEs", sFilenameAves);
		// Verified peaks
		actions->viewVerifiedPeaks->trigger();
		snap("PeaksVerified1");
		// Show detected peaks
		actions->viewDetectedPeaks->trigger();
		snap("PeaksDetected1");
	}


private:
	/// Construct a filename
	QString getFilename(const QString& sLabel)
	{
		iStep++;
		QString sFilename = QString("1-%0-%1.png").arg(iStep, 3, 10, QChar('0')).arg(sLabel);
		return sFilename;
	}

	/// Get a chart snapshot
	QImage getImage()
	{
		const ChartPixmap* cp = scope->chart()->pixmap();
		scope->chart()->draw(sz);

		QRect rc = cp->borderRect();
		rc.adjust(-1, -1, 1, 1);
		QImage image = cp->pixmap().toImage().copy(rc);
		return image;
	}

	/// Save chart snapshot to disk and return the filename
	QString snap(const QString& sLabel)
	{
		QString sFilename = getFilename(sLabel);
		QImage image = getImage();
		image.save(sFilename);

		return sFilename;
	}

	QString snapAndContrast(const QString& sLabel, const QString& sFilenameContrast)
	{
		QString sFilename = getFilename(sLabel);
		QImage image = getImage();
		image.save(sFilename);

		compare(image, sFilename, sFilenameContrast, false);

		return sFilename;
	}

	bool compare(const QImage& image, const QString& sFilename, const QString& sFilenameContrast, bool bExpectEqual)
	{
		QImage orig(sFilenameContrast);
		bool bEqual = (image == orig);
		if (bEqual != bExpectEqual) {
			qDebug() << "Comparison to" << sFilenameContrast << "failed.  See" << sFilename;
			return false;
		}
		return true;
	}

	void compare(const QString& sLabel, const QString& sFilenameContrast, bool bExpectEqual = true)
	{
		QString sFilename = getFilename(sLabel);
		QImage image = getImage();

		if (!compare(image, sFilename, sFilenameContrast, bExpectEqual))
			image.save(sFilename);
	}

	void contrast(const QString& sLabel, const QString& sFilename) { compare(sLabel, sFilename, false); }

private:
	MainScope* scope;
	QSize sz;
	int iStep;
};

static QString sImagePrefix;



void checkFailure(const char* sFile, int iLine)
{
	qDebug() << "CHECK FAILURE: " << sFile << ", line " << iLine;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Globals = new GlobalVars();

	Test1();

	//a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	//int ret = a.exec();

	//IdacFactory::exitIdacThreads();

	delete Globals;

	return 0;
}
