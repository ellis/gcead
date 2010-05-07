/**
 * Copyright (C) 2008,2009,2010  Ellis Whitehead
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

#include "Actions.h"

#include <QActionGroup>


#define MAX_RECENT_FILES 10


Actions::Actions(QObject *parent)
	: QObject(parent)
{
	fileNew = new QAction(tr("&New"), this);
	fileNew->setShortcut(QKeySequence::New);

	fileOpen = new QAction(tr("&Open..."), this);
	fileOpen->setShortcut(QKeySequence::Open);

	for (int i = 0; i < MAX_RECENT_FILES; i++)
	{
		QAction* act = new QAction(QString(), this);
		act->setVisible(false);
		act->setData(i);
		fileOpenRecentActions << act;
	}

	fileSave = new QAction(tr("&Save"), this);
	fileSave->setShortcut(QKeySequence::Save);

	fileSaveAs = new QAction(tr("Save &As..."), this);
	fileSaveAs->setShortcut(QKeySequence::SaveAs);

	fileComment = new QAction(tr("Edit File &Comment..."), this);
	fileComment->setIcon(QIcon(":/images/comment-24x24.png"));
	fileComment->setIconText(tr("Comment"));
	fileComment->setToolTip(tr("Edit the general comment for the whole file"));

	fileImport = new QAction(tr("Import Waves..."), this);
	fileImport->setToolTip(tr("Import waves from another EAD file"));

	fileExportSignalData = new QAction(tr("Export Si&gnal Data..."), this);

	fileExportAmplitudeData = new QAction(tr("Export Am&plitude Data..."), this);

	fileExportRetentionData = new QAction(tr("Export Re&tention Data..."), this);

	fileLoadSampleProject = new QAction(tr("&Load Sample Project"), this);

	fileExit = new QAction(tr("E&xit"), this);
	fileExit->setShortcut(QKeySequence::Quit);

	//
	// View Menu
	//

	QActionGroup* group;
	
	group = new QActionGroup(this);
	viewViewMode = new QAction(tr("&View Mode"), group);
	viewViewMode->setCheckable(true);
	viewViewMode->setChecked(true);
	viewViewMode->setShortcut(tr("Ctrl+V"));
	viewPublishMode = new QAction(tr("&Publish Mode"), group);
	viewPublishMode->setCheckable(true);
	viewPublishMode->setShortcut(tr("Ctrl+P"));

	group = new QActionGroup(this);
	viewChartAverages = new QAction(tr("Chart: Averages"), group);
	viewChartAverages->setCheckable(true);
	viewChartAverages->setChecked(true);
	viewChartAverages->setShortcut(tr("Ctrl+1"));
	viewChartEads = new QAction(tr("Chart: EADs"), group);
	viewChartEads->setCheckable(true);
	viewChartEads->setShortcut(tr("Ctrl+2"));
	viewChartFids = new QAction(tr("Chart: FIDs"), group);
	viewChartFids->setCheckable(true);
	viewChartFids->setShortcut(tr("Ctrl+3"));
	viewChartAll = new QAction(tr("Chart: All"), group);
	viewChartAll->setCheckable(true);
	viewChartAll->setShortcut(tr("Ctrl+4"));
	viewChartRecording = new QAction(tr("Chart: Recording"), group);
	viewChartRecording->setCheckable(true);
	viewChartRecording->setEnabled(false);
	viewChartRecording->setShortcut(tr("Ctrl+5"));

	viewZoomIn = new QAction(tr("Zoom In"), this);
	viewZoomIn->setShortcut(tr("Ctrl++"));
	viewZoomIn->setIcon(QIcon(":/images/stock_zoom-in.png"));
	viewZoomOut = new QAction(tr("Zoom Out"), this);
	viewZoomOut->setShortcut(tr("Ctrl+-"));
	viewZoomOut->setIcon(QIcon(":/images/stock_zoom-out.png"));
	viewZoomFull = new QAction(tr("Zoom Full"), this);
	viewZoomFull->setShortcut(tr("Ctrl+="));
	viewZoomFull->setIcon(QIcon(":/images/stock_zoom-page-width.png"));

	viewScrollDivLeft = new QAction(tr("Scroll Division Left"), this);
	viewScrollDivLeft->setShortcut(QKeySequence::MoveToPreviousChar);
	viewScrollDivRight = new QAction(tr("Scroll Division Right"), this);
	viewScrollDivRight->setShortcut(QKeySequence::MoveToNextChar);
	viewScrollPageLeft = new QAction(tr("Scroll Page Left"), this);
	viewScrollPageLeft->setShortcut(QKeySequence::MoveToPreviousPage);
	viewScrollPageRight = new QAction(tr("Scroll Page Right"), this);
	viewScrollPageRight->setShortcut(QKeySequence::MoveToNextPage);

	viewWaveComments = new QAction(tr("Show Wave &Comments"), this);
	viewWaveComments->setCheckable(true);

	//
	// Record menu
	//

	recordRecord = new QAction(tr("&Record..."), this);
	recordRecord->setIcon(QIcon(":/images/record-24x24.png"));
	recordRecord->setShortcut(tr("Ctrl+R"));

	recordHardwareSettings = new QAction(tr("&Hardware Settings..."), this);
	//recordOptions->setIconText("Options");

	recordSave = new QAction(tr("&Stop and Save"), this);
	
	recordDiscard = new QAction(tr("&Discard"), this);

	recordHardwareConnect = new QAction(tr("&Connect to Hardware"), this);

	//
	// Markers menu
	//

	//group = new QActionGroup(this);

	markersHide = new QAction(tr("&Hide Markers"), this);
	markersHide->setCheckable(true);
	markersHide->setIconText(tr("Hide"));
	markersHide->setToolTip(tr("Hide markers"));

	/*markersShow = new QAction(tr("&Show Markers"), group);
	markersShow->setCheckable(true);
	markersShow->setIconText(tr("Show"));
	markersShow->setToolTip(tr("Show selected markers"));*/

	markersEdit = new QAction(tr("&Edit Markers"), this);
	markersEdit->setCheckable(true);
	markersEdit->setIconText(tr("Edit"));
	markersEdit->setToolTip(tr("Activate the mode for selecting peaks and adjusting their areas"));


	markersShowTime = new QAction(tr("Show &Timestamps"), this);
	markersShowTime->setCheckable(true);
	markersShowTime->setChecked(true);
	markersShowTime->setIconText(tr("Time"));
	markersShowTime->setToolTip(tr("Show the timestamps for markers"));

	markersShowEadAmplitude = new QAction(tr("Show EAD &Amplitude"), this);
	markersShowEadAmplitude->setCheckable(true);
	markersShowEadAmplitude->setChecked(true);
	markersShowEadAmplitude->setIconText(tr("Amplitude"));
	markersShowEadAmplitude->setToolTip(tr("Show the amplitude of EAD peaks"));

	markersShowFidArea = new QAction(tr("Show &FID Area Percentage"), this);
	markersShowFidArea->setCheckable(true);
	markersShowFidArea->setChecked(true);
	markersShowFidArea->setIconText(tr("Area"));
	markersShowFidArea->setToolTip(tr("Show the area percentage of FID peaks"));
}
