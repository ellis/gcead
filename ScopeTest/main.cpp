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

#include <QtDebug>
#include <QApplication>
#include <QFile>
#include <QMutex>
#include <QSettings>
#include <QThread>

#include <AppDefines.h>
#include <Globals.h>
#include <ViewSettings.h>
#include <Idac/IdacFactory.h>
#include <IdacDriver/IdacSettings.h>

#include <Scope/MainScope.h>
#include <Scope/MainScopeUi.h>

#include "TestBase.h"
#include "TestRecording.h"


class TestActions : public TestBase
{
public:
	TestActions() : TestBase(false)
	{
		Actions* actions = scope->actions();

		QString sFilename;

		// Load sample project
		actions->fileLoadSampleProject->trigger();
		QString sFilenameAves = snap("AVEs");
		// Chart: EADs
		actions->viewChartEads->trigger();
		QString sFilenameEads = snap("EADs");
		// Chart: FIDs
		actions->viewChartFids->trigger();
		snap("FIDs");
		// Chart: All
		actions->viewChartAll->trigger();
		QString sFilenameAll = snap("ALL");
		// Chart: AVEs
		actions->viewChartAverages->trigger();
		compare("AVEs", sFilenameAves);

		EadFile* file = scope->file();
		ViewInfo* viewEad = file->viewInfo(EadView_EADs);
		WaveInfo* waveFidAve = file->recs()[0]->fid();

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

		ViewWaveInfo* vwiEad1All = scope->file()->viewInfo(EadView_All)->allVwis()[2];

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
		compare("ALL", sFilenameAves);

		// Hide peaks (shouldn't change anything visually)
		actions->markersHide->trigger();
		compare("AVEs", sFilenameAves);
		// Show detected peaks
		actions->markersShowDetected->trigger();
		snapAndContrast("PeaksDetected", sFilenameAves);
		// Edit peaks
		actions->markersEditPeaks->trigger();
		snap("PeaksEdit");
		// Validate a detected peak on the FID wave
		ViewWaveInfo* vwi = scope->file()->viewInfo(EadView_Averages)->vwis()[1];
		vwi->choosePeakAtDidx(vwi->wave()->peaks0[1].middle.i);
		snap("PeaksEdit1");
		// Hide peaks
		actions->markersHide->trigger();
		compare("AVEs", sFilenameAves);
		// Verified peaks
		actions->markersShowVerified->trigger();
		snap("PeaksVerified1");
		// Show detected peaks
		actions->markersShowDetected->trigger();
		snap("PeaksDetected1");

		actions->markersHide->trigger();
		compare("AVEs", sFilenameAves);

		//
		// WAVE VISIBILITY
		//

		actions->viewChartAll->trigger();
		compare("ALL", sFilenameAll);

		// Hide all waves
		foreach (ViewWaveInfo* vwi, scope->file()->viewInfo(EadView_All)->allVwis())
			vwi->setVisible(false);
		snapAndContrast("HideAll", sFilenameAll);
		// Show them again
		foreach (ViewWaveInfo* vwi, scope->file()->viewInfo(EadView_All)->allVwis())
			vwi->setVisible(true);
		compare("ALL", sFilenameAll);

		//
		// EXTRA WAVES
		//

		actions->viewChartEads->trigger();
		compare("EADs", sFilenameEads);

		// Select FID 1
		viewEad->setUserWave(waveFidAve);
		sFilename = snapAndContrast("ShowFidAve", sFilenameEads);
		// Move the extra wave
		viewEad->vwiUser.setDivisionOffset(8);
		snapAndContrast("MoveFidAve", sFilename);
		// Remove extra wave
		viewEad->setUserWave(NULL);
		compare("HideFID1", sFilenameEads);

		//
		// INVERTING WAVES
		//

		actions->viewChartAll->trigger();
		compare("ALL", sFilenameAll);

		// Invert
		vwiEad1All->invert();
		snapAndContrast("Invert", sFilenameAll);
		// Un-invert
		vwiEad1All->invert();
		compare("Uninvert", sFilenameAll);
	}
};


class TestSaving : public TestBase
{
public:
	TestSaving() : TestBase(false)
	{
		Actions* actions = scope->actions();

		QString sFilename;

		// Load sample project
		actions->fileLoadSampleProject->trigger();

		EadFile* file = scope->file();
		ViewInfo* viewAve = file->viewInfo(EadView_Averages);
		ViewInfo* viewEad = file->viewInfo(EadView_EADs);
		ViewInfo* viewFid = file->viewInfo(EadView_FIDs);
		ViewInfo* viewAll = file->viewInfo(EadView_All);
		ViewWaveInfo* vwiAveFid = viewAve->vwis()[1];
		ViewWaveInfo* vwiEadEad1 = viewEad->vwis()[0];
		ViewWaveInfo* vwiFidFid1 = viewFid->vwis()[0];
		WaveInfo* waveFidAve = file->recs()[0]->fid();

		//
		// Chart: Aves
		//

		// Validate a detected peak on the FID wave
		vwiAveFid->choosePeakAtDidx(vwiAveFid->wave()->peaks0[1].middle.i);

		//
		// Chart: EADs
		//

		// Add comment to EAD1
		vwiEadEad1->setComment("first EAD");
		// Invert EAD1
		vwiEadEad1->invert();
		// Set FID Ave as extra wave
		viewEad->setUserWave(waveFidAve);
		// Change position of extra wave
		viewEad->vwiUser.setDivisionOffset(7.5);

		//
		// Chart: FIDs
		//

		// Move FID1 up
		vwiFidFid1->setDivisionOffset(8);

		//
		// Chart: All
		//

		// Hide half of the waves
		for (int i = 0; i < viewAll->allVwis().size(); i += 2)
			viewAll->allVwis()[i]->setVisible(false);

		actions->viewChartAverages->trigger();
		QString sFilenameAve = snap("Ave");
		actions->viewChartEads->trigger();
		QString sFilenameEad = snap("Ead");
		actions->viewChartFids->trigger();
		QString sFilenameFid = snap("Fid");
		actions->viewChartAll->trigger();
		QString sFilenameAll = snap("All");

		scope->save("test.ead");
		scope->open("test.ead");

		actions->viewChartAverages->trigger();
		compare("Ave", sFilenameAve);
		actions->viewChartEads->trigger();
		compare("Ead", sFilenameEad);
		actions->viewChartFids->trigger();
		compare("Fid", sFilenameFid);
		actions->viewChartAll->trigger();
		compare("All", sFilenameAll);
	}
};


void checkFailure(const char* sFile, int iLine)
{
	qDebug() << "CHECK FAILURE: " << sFile << ", line " << iLine;

	QFile file("ScopeTest.log");
	if (file.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&file);
		out << "CHECK FAILURE: " << sFile << ", line " << iLine << "\n";
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Globals = new GlobalVars();

	//TestActions();
	TestSaving();

	if (false) {
		TestRecording test;
		a.exec();
		IdacFactory::exitIdacThreads();
	}

	delete Globals;

	return 0;
}
