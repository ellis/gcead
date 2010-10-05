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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "ui_MainWindow.h"

#include "EadEnums.h"


class QComboBox;
class QLabel;
class QStackedLayout;

class ChartWidget;
class EadFile;
class MainScope;
class PanelTabs;
class RecordHandler;
class SuffixSpinBox;
class TaskFilterWidget;
class TaskMarkersWidget;
class TaskPublishWidget;
class TaskReviewWidget;
class ViewTabs;
class ViewWaveInfo;


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

	MainScope* scope() { return m_scope; }

	void open(const QString& sFilename);

// QWidget overrides
protected:
	void closeEvent(QCloseEvent* e);

private:
	void setupWidgets();
	void setupActions();

	void readSettings();
	void writeSettings();

	LoadSaveResult importAsc(const QString& sFilename);

private slots:
	void idac_statusErrorChanged(QString sError);

	void scope_fileChanged();
	void scope_taskTypeChanged(EadTask task);
	void scope_commentChanged();
	void actions_viewChartRecording_changed();
	void updateReview();

	void actions_fileImport_triggered();
	void actions_fileExportSignalData_triggered();
	void actions_fileExportAmplitudeData_triggered();
	void actions_fileExportRetentionData_triggered();
	void actions_fileExit_triggered();
	void on_actHelpAbout_triggered();

private:
	Ui::MainWindowClass ui;
	MainScope* m_scope;

	// Recently opened files stuff
	QMenu* m_recentFilesMenu;

	QLabel* m_lblComment;
	QLabel* m_lblIdacStatus;
	//QLabel* m_lblIdacError;

	PanelTabs* m_paneltabs;
	ViewTabs* m_viewtabs;
	/// Tool widget for Record task
	//TaskRecordWidget* m_taskRecord;
	/// Tool widget for Review task
	TaskReviewWidget* m_taskReview;
	/// Tool widget for Filter task
	TaskFilterWidget* m_taskFilter;
	/// Tool widget for Markers task
	TaskMarkersWidget* m_taskMarkers;
	/// Tool widget for Publish task
	TaskPublishWidget* m_taskPublish;
	QStackedLayout* m_taskStack;
	ChartWidget* m_chart;

	/// Seconds per division, for convenience
	int m_nSecondsPerDivision;
};

#endif
