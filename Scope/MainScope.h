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

#ifndef __MAINSCOPE_H
#define __MAINSCOPE_H

#include <QObject>

#include <Actions.h>
#include <EadEnums.h>
#include <EadFile.h>

#include "ChartScope.h"


class IdacProxy;
class MainScopeUi;
class RecordHandler;


class MainScope : public QObject
{
	Q_OBJECT

	Q_PROPERTY(EadTask taskType READ taskType WRITE setTaskType)
	Q_PROPERTY(EadView viewType READ viewType WRITE setViewType)
	Q_PROPERTY(bool isRecording READ isRecording WRITE setIsRecording)
	Q_PROPERTY(QString windowTitle READ windowTitle)

public:
	MainScope(MainScopeUi* ui, IdacProxy* idac, QObject* parent = NULL);
	~MainScope();

// Properties
public:
	EadTask taskType() const { return m_taskType; }
	void setTaskType(EadTask taskType);

	EadView viewType() const { return m_viewType; }
	void setViewType(EadView viewType);

	const QString& windowTitle() const { return m_sWindowTitle; }

	bool isWindowModified() const { return m_bWindowModified; }

	bool isRecording() const { return m_bRecording; }

public:
	Actions* actions() { return m_actions; }
	
	EadFile* file() { return m_file; }
	void setFile(EadFile* file);

	ChartScope* chart() { return m_chart; }

	bool isRecentFilesMenuEnabled() const { return m_bRecentFilesMenuEnabled; }

	/// User comment for this file
	const QString& comment() { return m_file->comment(); }
	void setComment(const QString& s);

	/// Current peak mode
	EadMarkerMode peakMode() const { return m_chart->params().peakMode; }
	void setPeakMode(EadMarkerMode peakMode);
	
	/// The rec ID for which the peak mode applies (-1 if no record is selected)
	int peakModeRecId() const { return m_chart->params().nPeakModeRecId; }
	void setPeakModeRecId(int id);

	bool checkSaveAndContinue();
	void open(const QString& sFilename);
	bool save(const QString& sFilename);

public slots:
	void updateActions();

signals:
	//void propertyChanged(const QString& sName);
	void fileChanged(EadFile* file);
	void isRecentFilesMenuEnabledChanged(bool bEnabled);
	void waveListChanged();
	void commentChanged(const QString& sComment);
	void taskTypeChanged(EadTask taskType);
	void viewTypeChanged(EadView viewType);
	void peakModeChanged(EadMarkerMode peakMode);
	void windowTitleChanged(const QString& sWindowTitle);
	void isWindowModifiedChanged(bool bWindowModified);
	void isRecordingChanged(bool bRecording);

	void viewSettingChanged(const QString& sName);

	void updateRecordings();

private:
	void setIsRecentFilesMenuEnabled(bool bEnabled);
	void setIsWindowModified(bool bWindowModified);
	void setIsRecording(bool bRecording);
	void updateWindowTitle();
	void addRecentFile(const QString& sFilename);
	void updateRecentFileActions();
	bool checkHardware();

private slots:
	void on_idac_isAvailable();
	void on_file_dirtyChanged();

	void on_actions_fileNew_triggered();
	void on_actions_fileOpen_triggered();
	void on_actions_fileOpenRecentActions_triggered();
	bool on_actions_fileSave_triggered();
	bool on_actions_fileSaveAs_triggered();
	void on_actions_fileComment_triggered();
	void on_actions_fileLoadSampleProject_triggered();

	void on_actions_viewViewMode_triggered();
	void on_actions_viewPublishMode_triggered();
	void on_actions_viewChartAverages_triggered();
	void on_actions_viewChartEads_triggered();
	void on_actions_viewChartFids_triggered();
	void on_actions_viewChartAll_triggered();
	void on_actions_viewChartRecording_triggered();
	void on_actions_viewWaveComments_triggered();
	void on_actions_viewHidePeaks_triggered();
	void on_actions_viewDetectedPeaks_triggered();
	void on_actions_viewVerifiedPeaks_triggered();
	void on_actions_viewEditPeaks_triggered();

	void on_actions_recordRecord_triggered();
	void on_actions_recordHardwareSettings_triggered();
	void on_actions_recordSave_triggered();
	void on_actions_recordDiscard_triggered();
	void on_actions_recordHardwareConnect_triggered();
	void on_recTimer_timeout();
	void stopRecording(bool bSave, bool bAutoStop);

private:
	MainScopeUi* m_ui;
	IdacProxy* m_idac;

	Actions* m_actions;
	EadFile* m_file;
	ChartScope* m_chart;

	EadTask m_taskType;
	EadView m_viewType;


	bool m_bRecentFilesMenuEnabled;

	QString m_sWindowTitle;
	bool m_bWindowModified;

	bool m_bRecording;
	ViewWaveInfo* m_vwiEad;
	ViewWaveInfo* m_vwiFid;
	ViewWaveInfo* m_vwiDig;
	RecordHandler* m_recHandler;
	QTimer* m_recTimer;
};

#endif
