/**
 * Copyright (C) 2008, 2009  Ellis Whitehead
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

#include "MainWindow.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QStackedLayout>

#include <WaveInfo.h>
#include <Idac/IdacFactory.h>
#include <Idac/IdacProxy.h>

#include "Actions.h"
#include "AppDefines.h"
#include "ChartWidget.h"
#include "Check.h"
#include "EadFile.h"
#include "MainWindow.h"
#include "Globals.h"
#include "MainScope.h"
#include "MainWindowUi.h"
#include "PanelTabs.h"
#include "RecordHandler.h"
#include "TaskPanel.h"
#include "TaskPublishWidget.h"
#include "TaskReviewWidget.h"
#include "ViewTabs.h"


MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	// REFACTOR: This should be moved to main and the MainScope object passed to MainWindow
	IdacProxy* idac = IdacFactory::getProxy();

	m_scope = new MainScope(new MainWindowUi(this), idac, this);

	readSettings();
	setupActions();
	setupWidgets();

	connect(idac, SIGNAL(statusTextChanged(QString)), m_lblIdacStatus, SLOT(setText(QString)));
	connect(idac, SIGNAL(statusErrorChanged(QString)), this, SLOT(idac_statusErrorChanged(QString)));
	connect(m_scope, SIGNAL(fileChanged(EadFile*)), this, SLOT(scope_fileChanged()));
	connect(m_scope, SIGNAL(isRecentFilesMenuEnabledChanged(bool)), m_recentFilesMenu, SLOT(setEnabled(bool)));
	connect(m_scope, SIGNAL(waveListChanged()), this, SLOT(updateReview()));
	connect(m_scope, SIGNAL(taskTypeChanged(EadTask)), this, SLOT(scope_taskTypeChanged(EadTask)));
	connect(m_scope, SIGNAL(viewTypeChanged(EadView)), this, SLOT(updateReview()));
	connect(m_scope, SIGNAL(commentChanged(const QString&)), this, SLOT(scope_commentChanged()));
	connect(m_scope, SIGNAL(windowTitleChanged(const QString&)), this, SLOT(setWindowTitle(const QString&)));
	connect(m_scope, SIGNAL(isWindowModifiedChanged(bool)), this, SLOT(setWindowModified(bool)));

	// Setup initial values from m_scope
	setWindowTitle(m_scope->windowTitle());
	scope_fileChanged();
	m_recentFilesMenu->setEnabled(m_scope->isRecentFilesMenuEnabled());
	scope_taskTypeChanged(m_scope->taskType());
	actions_viewChartRecording_changed();

	idac->setup();
}

MainWindow::~MainWindow()
{
	writeSettings();
}

void MainWindow::setupWidgets()
{
	//
	// Setup task widgets
	//
	m_paneltabs = new PanelTabs(m_scope);
	
	m_chart = new ChartWidget(m_scope);
	m_chart->setStatusBar(statusBar());

	m_taskReview = new TaskReviewWidget(m_scope);
	m_taskReview->setupItems(NULL);

	//m_taskRecord = new TaskRecordWidget(m_idac, m_chart);
	//connect(m_taskRecord, SIGNAL(saveRecording(bool)), this, SIGNAL(saveRecording(bool)));

	m_taskPublish = new TaskPublishWidget(m_scope);
	m_taskPublish->setChartWidget(m_chart);

	m_taskStack = new QStackedLayout();
	m_taskStack->setSpacing(0);
	//m_taskStack->addWidget(new TaskPanel(m_taskRecord));
	m_taskStack->addWidget(new TaskPanel(m_taskReview));
	m_taskStack->addWidget(new TaskPanel(m_taskPublish));
	m_taskStack->setCurrentIndex(1);

	//connect(m_taskRecord, SIGNAL(updateRecordings()), m_chart, SLOT(updateRecordings()));
	connect(m_taskPublish, SIGNAL(settingsChanged()), m_chart, SLOT(repaintChart()));

	//
	// Setup view widgets
	//
	m_viewtabs = new ViewTabs(m_scope);

	QGridLayout* layout = new QGridLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(m_paneltabs, 0, 0);
	layout->addWidget(m_viewtabs, 0, 1);
	layout->addLayout(m_taskStack, 1, 0);
	layout->addWidget(m_chart, 1, 1);
	layout->setColumnStretch(1, 1);

	QWidget* w = new QWidget;
	w->setLayout(layout);
	setCentralWidget(w);

	m_lblIdacStatus = new QLabel();
	statusBar()->addPermanentWidget(m_lblIdacStatus);
}

void MainWindow::open(const QString& sFilename)
{
	m_scope->open(sFilename);
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	bool bIgnore = false;

	if (m_scope->isRecording())
	{
		QMessageBox::StandardButton btn = QMessageBox::question(
			this,
			tr("Discard"),
			tr("Are you sure you want to discard the current recording?"),
			QMessageBox::Discard | QMessageBox::Cancel,
			QMessageBox::Discard);

		if (btn == QMessageBox::Cancel)
			bIgnore = true;
	}
	
	if (!m_scope->checkSaveAndContinue())
		bIgnore = true;

	if (bIgnore)
		e->ignore();
	else
		e->accept();
}

void MainWindow::setupActions()
{
	Actions* actions = m_scope->actions();

	ui.mnuFile->addAction(actions->fileNew);
	ui.mnuFile->addSeparator();
	ui.mnuFile->addAction(actions->fileOpen);
	m_recentFilesMenu = new QMenu(tr("Open &Recent"), this);
	ui.mnuFile->addMenu(m_recentFilesMenu);
	foreach (QAction* act, m_scope->actions()->fileOpenRecentActions)
		m_recentFilesMenu->addAction(act);
	m_recentFilesMenu->setEnabled(false);
	ui.mnuFile->addSeparator();
	ui.mnuFile->addAction(actions->fileSave);
	ui.mnuFile->addAction(actions->fileSaveAs);
	ui.mnuFile->addSeparator();
	ui.mnuFile->addAction(actions->fileComment);
	ui.mnuFile->addSeparator();
	ui.mnuFile->addAction(actions->fileExportSignalData);
	ui.mnuFile->addAction(actions->fileExportRetentionData);
	ui.mnuFile->addAction(actions->fileLoadSampleProject);
	ui.mnuFile->addSeparator();
	ui.mnuFile->addAction(actions->fileExit);

	ui.mnuView->addAction(actions->viewViewMode);
	ui.mnuView->addAction(actions->viewPublishMode);
	ui.mnuView->addSeparator();
	ui.mnuView->addAction(actions->viewChartAverages);
	ui.mnuView->addAction(actions->viewChartEads);
	ui.mnuView->addAction(actions->viewChartFids);
	ui.mnuView->addAction(actions->viewChartAll);
	ui.mnuView->addAction(actions->viewChartRecording);
	ui.mnuView->addSeparator();
	ui.mnuView->addAction(actions->viewZoomIn);
	ui.mnuView->addAction(actions->viewZoomOut);
	ui.mnuView->addAction(actions->viewZoomFull);
	ui.mnuView->addSeparator();
	ui.mnuView->addAction(actions->viewWaveComments);
	ui.mnuView->addSeparator();
	ui.mnuView->addAction(actions->viewHidePeaks);
	ui.mnuView->addAction(actions->viewDetectedPeaks);
	ui.mnuView->addAction(actions->viewVerifiedPeaks);
	ui.mnuView->addAction(actions->viewEditPeaks);

	ui.mnuRecord->addAction(actions->recordRecord);
	ui.mnuRecord->addAction(actions->recordHardwareSettings);
	ui.mnuRecord->addAction(actions->recordSave);
	ui.mnuRecord->addAction(actions->recordDiscard);
	ui.mnuRecord->addSeparator();
	ui.mnuRecord->addAction(actions->recordHardwareConnect);

	ui.toolBar->addAction(actions->recordRecord);
	ui.toolBar->addSeparator();
	ui.toolBar->addWidget(new QLabel(tr("Peaks:")));
	ui.toolBar->addAction(actions->viewHidePeaks);
	ui.toolBar->addAction(actions->viewDetectedPeaks);
	ui.toolBar->addAction(actions->viewVerifiedPeaks);
	ui.toolBar->addAction(actions->viewEditPeaks);
	ui.toolBar->addSeparator();
	ui.toolBar->addAction(actions->fileComment);
	m_lblComment = new QLabel;
	ui.toolBar->addWidget(m_lblComment);

	connect(actions->fileExportSignalData, SIGNAL(triggered()), this, SLOT(actions_fileExportSignalData_triggered()));
	connect(actions->fileExportRetentionData, SIGNAL(triggered()), this, SLOT(actions_fileExportRetentionData_triggered()));
	connect(actions->fileExit, SIGNAL(triggered()), this, SLOT(actions_fileExit_triggered()));
	connect(actions->viewChartRecording, SIGNAL(changed()), this, SLOT(actions_viewChartRecording_changed()));
}

void MainWindow::readSettings()
{
	QSettings settings("Syntech", APPSETTINGSKEY);
	
	settings.beginGroup("MainWindow");

	bool bMax = settings.value("Maximized", true).toBool();
	if (bMax)
	{
		// NOTE: This is now handled in main.cpp --ellis, 2009-05-04
		//setWindowState(Qt::WindowMaximized);
	}
	// If we're maximized, don't load the size and position values, because the de-maximizing will still have the maximized height
	else
	{
		if (settings.contains("Size"))
			resize(settings.value("Size").toSize());
		if (settings.contains("Position"))
			move(settings.value("Position").toPoint());
		setWindowState(Qt::WindowActive);
	}
	
	settings.endGroup();
}

void MainWindow::writeSettings()
{
	QSettings settings("Syntech", APPSETTINGSKEY);
	
	settings.beginGroup("MainWindow");
	settings.setValue("Size", size());
	settings.setValue("Position", pos());
	settings.setValue("Maximized", isMaximized());
	settings.endGroup();

	Globals->writeSettings();
}

void MainWindow::idac_statusErrorChanged(QString sError)
{
	statusBar()->showMessage(sError);
}

void MainWindow::scope_fileChanged()
{
	CHECK_PRECOND_RET(m_scope->file() != NULL);

	updateReview();
	
	// If this is a file with data
	EadFile* file = m_scope->file();
	bool bHasData = (file->recs().count() > 1);

	// Zoom full if data available
	if (bHasData)
		m_chart->zoomFull();
	// Otherwise
	else
		m_chart->setSecondsPerDivisionMin(50);

	scope_commentChanged();
}

void MainWindow::scope_taskTypeChanged(EadTask task)
{
	m_taskStack->setCurrentIndex((int) task);
	m_paneltabs->setCurrentIndex((int) task);

	// Might need to switch out of Recording view
	if (task == EadTask_Publish && m_scope->viewType() == EadView_Recording)
		m_scope->setViewType(EadView_Averages);
	else
		updateReview();
}

void MainWindow::scope_commentChanged()
{
	m_lblComment->setText(m_scope->comment());
}

void MainWindow::actions_viewChartRecording_changed()
{
	m_viewtabs->setEnabled(EadView_Recording, m_scope->actions()->viewChartRecording->isEnabled());
}

void MainWindow::updateReview()
{
	if (m_scope->taskType() == EadTask_Review)
		m_taskReview->setupItems(m_scope->file());
}

void MainWindow::actions_fileExportSignalData_triggered()
{
	CHECK_PRECOND_RET(m_scope->file() != NULL);

	QString sFilename = QFileDialog::getSaveFileName(
		this,
		tr("Export Data"),
		Globals->lastDir(),
		tr("Comma Separated Values (*.csv)"));

	if (sFilename.isEmpty())
		return;

	QFileInfo fi(sFilename);
	if (fi.suffix().isEmpty())
		sFilename += ".csv";

	QMessageBox msg(QMessageBox::Information, tr("Exporting..."), tr("The project data is currently being exported."));
	msg.setStandardButtons(QMessageBox::NoButton);
	msg.show();
	// Let window repaint itself before we do the slow process of exporting
	QApplication::processEvents();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	m_scope->file()->exportData(sFilename /*, EadFile::CSV*/);
	QApplication::restoreOverrideCursor();
}

// REFACTOR: almost completely duplicates the above function
void MainWindow::actions_fileExportRetentionData_triggered()
{
	CHECK_PRECOND_RET(m_scope->file() != NULL);

	QString sFilename = QFileDialog::getSaveFileName(
		this,
		tr("Export Retention Data"),
		Globals->lastDir(),
		tr("Comma Separated Values (*.csv)"));

	if (sFilename.isEmpty())
		return;

	QFileInfo fi(sFilename);
	if (fi.suffix().isEmpty())
		sFilename += ".csv";

	QMessageBox msg(QMessageBox::Information, tr("Exporting..."), tr("The retention data is currently being exported."));
	msg.setStandardButtons(QMessageBox::NoButton);
	msg.show();
	// Let window repaint itself before we do the slow process of exporting
	QApplication::processEvents();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	m_scope->file()->exportRetentionData(sFilename /*, EadFile::CSV*/);
	QApplication::restoreOverrideCursor();
}

void MainWindow::actions_fileExit_triggered()
{
	close();
}

void MainWindow::on_actHelpAbout_triggered()
{
	QString s;
	s = tr(
			"<h2>%0 %1 (%2)</h2>"
			"<p>Copyright &copy; 2008 Syntech</p>"
			"<p>Data acquisition for Gas Chromatograph with EAD.</p>"
			"<p>For use with the Syntech IDAC series of Data Acquisition Systems.</p>"
			"<p>SYNTECH<br/>Equipment and Research<br/>Hansjakobstrasse 14<br/>79199 KIRCHZARTEN<br/>Germany</p>"
			"<p>More information can be found at:<br/>"
			"<a href='http://www.syntech.nl'>http://www.syntech.nl</a></p>"
			"<p>License: GPL</p>").arg(tr(APPNAME)).arg(APPVERSION).arg(APPVERSIONDATE);

	QMessageBox::about(this, tr(APPNAME), s);
}
