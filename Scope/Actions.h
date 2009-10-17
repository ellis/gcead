/**
 * Copyright (C) 2008,2009  Ellis Whitehead
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

#ifndef __ACTIONS_H
#define __ACTIONS_H

#include <QAction>
#include <QList>
#include <QObject>


class Actions : public QObject
{
	Q_OBJECT
public:
	Actions(QObject* parent = NULL);

	QAction* fileNew;
	QAction* fileOpen;
	QList<QAction*> fileOpenRecentActions;
	QAction* fileSave;
	QAction* fileSaveAs;
	QAction* fileComment;
	QAction* fileExportSignalData;
	QAction* fileExportRetentionData;
	QAction* fileLoadSampleProject;
	QAction* fileExit;

	QAction* viewViewMode;
	QAction* viewPublishMode;
	QAction* viewChartAverages;
	QAction* viewChartEads;
	QAction* viewChartFids;
	QAction* viewChartAll;
	QAction* viewChartRecording;
	QAction* viewWaveComments;
	QAction* viewHidePeaks;
	QAction* viewDetectedPeaks;
	QAction* viewVerifiedPeaks;
	QAction* viewEditPeaks;

	QAction* recordRecord;
	QAction* recordHardwareSettings;
	QAction* recordSave;
	QAction* recordDiscard;
	QAction* recordHardwareConnect;
};

#endif
