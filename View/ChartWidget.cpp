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

#include "ChartWidget.h"

#include <math.h>

#include <QtDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStatusBar>
#include <QTimer>
#include <QToolButton>

#include <WaveInfo.h>
#include "Actions.h"
#include "ChartPixmap.h"
#include "Check.h"
#include "EadFile.h"
#include "Globals.h"
#include "MainScope.h"
#include "PublisherSettings.h"
#include "Utils.h"
#include "ViewSettings.h"
#include "WaveEditorDialog.h"


// REFACTOR: duplicated in ChartWidget.cpp
const double g_anSecondsPerDivision[] =
{
	0.1, 0.2, 0.3, 0.5, 0.7,
	1, 1.5, 2, 3, 5, 7,
	10, 15, 20, 30, 40, 50,
	60, 90, 2*60, 3*60, 5*60, 7*60,
	10*60, 15*60, 20*60, 30*60,
	0 // termination value
};


ChartWidget::ChartWidget(MainScope* scope, QWidget* parent)
	: QWidget(parent)
{
	Q_ASSERT(scope != NULL);

	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);

	m_scope = scope;
	connect(m_scope, SIGNAL(fileChanged(EadFile*)), this, SLOT(scope_fileChanged()));
	connect(m_scope, SIGNAL(waveListChanged()), this, SLOT(scopeChanged()));
	connect(m_scope, SIGNAL(taskTypeChanged(EadTask)), this, SLOT(scopeChanged()));
	connect(m_scope, SIGNAL(viewTypeChanged(EadView)), this, SLOT(scopeChanged()));
	connect(m_scope, SIGNAL(peakModeChanged(EadPeakMode)), this, SLOT(scopeChanged()));
	connect(m_scope, SIGNAL(isRecordingChanged(bool)), this, SLOT(showRecordingLabel(bool)));
	connect(m_scope, SIGNAL(updateRecordings()), this, SLOT(updateRecordings()));
	connect(m_scope, SIGNAL(viewSettingChanged(const QString&)), this, SLOT(scopeChanged()));

	m_statusbar = NULL;
	m_lblStatus = NULL;

	m_view = NULL;
	m_task = EadTask_Review;

	m_pixmap = new ChartPixmap();
	m_nSampleOffset = 0;
	m_bDragging = false;
	m_bSelecting = false;

	m_recordingEadWave = NULL;
	m_recordingFidWave = NULL;

	setupWidgets();

	setSecondsPerDivisionIndex(16); // 50s/div

	m_timerUpdate = new QTimer(this);
	connect(m_timerUpdate, SIGNAL(timeout()), this, SLOT(on_timerUpdate_timeout()));
}

ChartWidget::~ChartWidget()
{
	delete m_pixmap;
}

void ChartWidget::setupWidgets()
{
	m_buttons = new QWidget(this);

	QHBoxLayout* tblayout = new QHBoxLayout;
	tblayout->addItem(new QSpacerItem(15, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
	tblayout->addWidget(new QLabel(tr("Timebase:")));
	tblayout->addItem(new QSpacerItem(5, 5, QSizePolicy::Fixed, QSizePolicy::Fixed));
	m_lblSecondsPerDivision = new QLabel();
	tblayout->addWidget(m_lblSecondsPerDivision);

	tblayout->addSpacing(15);

	QToolButton* tbtn;

	tbtn = new QToolButton;
	tbtn->setDefaultAction(m_scope->actions()->viewZoomIn);
	tbtn->setFocusPolicy(Qt::NoFocus);
	tblayout->addWidget(tbtn);
	// REFACTOR: the zoom functions should perhaps be handled in MainScope
	connect(m_scope->actions()->viewZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
	m_btnZoomIn = tbtn;

	tbtn = new QToolButton;
	tbtn->setDefaultAction(m_scope->actions()->viewZoomOut);
	tbtn->setFocusPolicy(Qt::NoFocus);
	tblayout->addWidget(tbtn);
	connect(m_scope->actions()->viewZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
	m_btnZoomOut = tbtn;

	tbtn = new QToolButton;
	tbtn->setDefaultAction(m_scope->actions()->viewZoomFull);
	tbtn->setFocusPolicy(Qt::NoFocus);
	tblayout->addWidget(tbtn);
	connect(m_scope->actions()->viewZoomFull, SIGNAL(triggered()), this, SLOT(zoomFull()));
	tblayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
	m_btnZoomFull = tbtn;

	m_buttons->setLayout(tblayout);

	m_scrollbar = new QScrollBar(Qt::Horizontal, this);
	m_scrollbar->setSingleStep(EAD_SAMPLES_PER_SECOND);
	connect(m_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(setSampleOffset(int)));

	m_buttons->setCursor(Qt::ArrowCursor);
	m_scrollbar->setCursor(Qt::ArrowCursor);

	// Set fixed size for m_lblSecondsPerDivision
	int nWidth = 0;
	for (int i = 0; g_anSecondsPerDivision[i] > 0; i++)
	{
		QString s = timestampString(g_anSecondsPerDivision[i]);
		nWidth = qMax(nWidth, m_lblSecondsPerDivision->fontMetrics().width(s));
	}
	m_lblSecondsPerDivision->setMinimumWidth(nWidth);

	m_lblRecording = new QLabel(tr("RECORDING"), this);
	m_lblRecording->setStyleSheet(
		//"background-color: rgba(255, 127, 127, 127);"
		"background-color: #fdd;"
		"border-style: outset;"
		"border-width: 1px;"
		"border-color: #500;"
		//"border-radius: 7px;"
		"font-weight: bold;"
		"padding: 5px 10px;");
	m_lblRecording->setVisible(false);
}

void ChartWidget::setStatusBar(QStatusBar* statusbar)
{
	if (m_lblStatus != NULL)
	{
		delete m_lblStatus;
		m_lblStatus = NULL;
	}

	m_statusbar = statusbar;

	if (m_statusbar != NULL)
	{
		m_lblStatus = new QLabel();
		m_statusbar->insertPermanentWidget(0, m_lblStatus);
	}
}

void ChartWidget::scope_fileChanged()
{
	CHECK_PRECOND_RET(m_scope != NULL);
	CHECK_PRECOND_RET(m_scope->file() != NULL);

	scopeChanged();

	// Zoom full if data available
	bool bHasData = (m_scope->file()->recs().count() > 1);
	if (bHasData)
		zoomFull();
	// Otherwise
	else
		setSecondsPerDivisionMin(50);
}

void ChartWidget::scopeChanged()
{
	CHECK_PRECOND_RET(m_scope != NULL);
	CHECK_PRECOND_RET(m_scope->file() != NULL);

	EadFile* file = m_scope->file();
	setup(file, file->viewInfo(m_scope->viewType()), m_scope->taskType());
}

void ChartWidget::setup(EadFile* file, ViewInfo* view, EadTask task)
{
	if (m_view != NULL)
	{
		m_view->disconnect(this);
	}

	m_file = file;
	m_view = view;
	m_task = task;

	if (m_view != NULL)
	{
		connect(m_view, SIGNAL(changed(ViewChangeEvents)), this, SLOT(on_view_changed(ViewChangeEvents)));
	}

	repaintChart();
}

/*
double ChartWidget::timebase() const
{
	return g_anSecondsPerDivision[m_iSecondsPerDivision];
}
*/

QString ChartWidget::timebaseString() const
{
	return timestampString(m_nSecondsPerDivision);
}

void ChartWidget::setSampleOffset(int nSampleOffset)
{
	m_nSampleOffset = nSampleOffset;
	repaintChart();
}

void ChartWidget::setSecondsPerDivisionIndex(int i)
{
	if (i >= 0 && g_anSecondsPerDivision[i] > 0)
	{
		m_iSecondsPerDivision = i;
		m_nSecondsPerDivision = g_anSecondsPerDivision[i];
		repaintChart();
		m_lblSecondsPerDivision->setText(timebaseString());
	}
	updateStatus();
}

bool ChartWidget::setSecondsPerDivisionMin(double nMin)
{
	// Find next larger valid value:
	for (int i = 0; g_anSecondsPerDivision[i] > 0; i++)
	{
		if (g_anSecondsPerDivision[i] >= nMin)
		{
			setSecondsPerDivisionIndex(i);
			return true;
		}
	}

	return false;
}

void ChartWidget::showRecordingLabel(bool bShow)
{
	m_lblRecording->setVisible(bShow);
	// HACK: This is done because sometime when we [Discard] a recording and then [Record] a new one,
	// we might get the same WaveInfo address allocated to us again, which would make ChartPixmap think
	// that it could still use the old RenderData. -- ellis, 2008-06-23
	m_pixmap->clearRenderData();
}

int ChartWidget::sampleCount()
{
	int nSamples = 0;

	// First look in the current view
	if (m_view != NULL)
	{
		foreach (ViewWaveInfo* vwi, m_view->vwis())
		{
			const WaveInfo* wave = vwi->wave();
			int n = wave->display.size() + wave->shift();
			if (n > nSamples)
				nSamples = n;
		}
	}
	// If the current view is empty, look in the whole file
	if (nSamples == 0 && m_file != NULL)
	{
		foreach (RecInfo* rec, m_file->recs())
		{
			foreach (const WaveInfo* wave, rec->waves())
			{
				int n = wave->display.size() + wave->shift();
				if (n > nSamples)
					nSamples = n;
			}
		}
	}

	return nSamples;
}

void ChartWidget::zoomOut()
{
	int iSample = m_pixmap->centerSample();

	setSecondsPerDivisionIndex(m_iSecondsPerDivision + 1);
	center(iSample);

	repaintChart();
}

void ChartWidget::zoomIn()
{
	if (m_iSecondsPerDivision > 0)
	{
		int iSample = m_pixmap->centerSample();

		setSecondsPerDivisionIndex(m_iSecondsPerDivision - 1);

		if (iSample < sampleCount())
			center(iSample);
	}
}

void ChartWidget::zoomFull()
{
	double nSamples = sampleCount();
	double nSeconds = nSamples / EAD_SAMPLES_PER_SECOND;
	double nSecondsPerDivision = nSeconds / m_pixmap->params().nCols;
	setSecondsPerDivisionMin(nSecondsPerDivision);

	setSampleOffset(0);

	repaintChart();
}

void ChartWidget::repaintChart()
{
	m_bRedraw = true;
	update();
	//qDebug() << "repaintChart:" << QTime::currentTime();
}

void ChartWidget::updateRecordings()
{
	if (m_scope->viewType() == EadView_Recording)
	{
		if (!m_timerUpdate->isActive())
		{
			repaintChart();
			// REFACTOR: adjust timing so that fewer updates are done when we're zoomed out
			//  enough that many data samples are drawn in a single pixel
			// Update 10 times per second
			m_timerUpdate->start(100);
			m_nRecordingUpdates = 0;
		}
		else
			m_nRecordingUpdates++;
	}
}

void ChartWidget::on_view_changed(ViewChangeEvents events)
{
	if ((events & ViewChangeEvent_Render) != 0)
		m_pixmap->clearRenderData();
	
	repaintChart();
}

void ChartWidget::on_timerUpdate_timeout()
{
	if (m_nRecordingUpdates > 0)
	{
		m_nRecordingUpdates = 0;
		repaintChart();
	}
	else
	{
		m_timerUpdate->stop();
	}
}

QSize ChartWidget::calcPixmapSize() const
{
	QRect rcPixmapMax(20, m_buttons->height(), width() - 35, height() - m_buttons->height() - 20);

	QSize sz = m_rcPixmapMax.size();
	int nCols = 0;
	if (m_task == EadTask_Publish)
	{
		if (Globals->publisherSettings()->bPublishCols)
			nCols = Globals->publisherSettings()->nPublishCols;
		if (Globals->publisherSettings()->bPublishHeight && Globals->publisherSettings()->nPublishSize > 0)
		{
			sz = QSize(Globals->publisherSettings()->nPublishSize, Globals->publisherSettings()->nPublishSize);
		}
	}

	sz = m_pixmap->sizeForAvailableArea(sz, 0);
	return sz;
}

void ChartWidget::resizeEvent(QResizeEvent* e)
{
	Q_UNUSED(e);
	
	m_bRedraw = true;

	m_rcPixmapMax = QRect(20, m_buttons->height(), width() - 35, height() - m_buttons->height() - 20);
	QSize sz = calcPixmapSize();
	
	QRect rcScroll(m_rcPixmapMax.left(), m_rcPixmapMax.top() + sz.height(), sz.width(), 20);
	
	m_scrollbar->setGeometry(rcScroll);

	m_lblRecording->move(width() - m_lblRecording->width() - 20, 5);
}

void ChartWidget::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e);

	QPainter painter(this);

	painter.fillRect(rect(), Qt::white);

	if (m_bRedraw)
	{
		m_bRedraw = false;
		//int n = Globals->publisherSettings()->nPublisherPercentSize;
		int nCols = 0;
		QSize sz = calcPixmapSize();
		if (m_task == EadTask_Publish)
		{
			if (Globals->publisherSettings()->bPublishCols)
				nCols = Globals->publisherSettings()->nPublishCols;
		}
		
		PublisherSettings* pub = Globals->publisherSettings();
		bool bOverrideTimebase = (m_task == EadTask_Publish && pub->bPublishTimebase);

		ChartPixmapParams params;
		params.file = m_file;
		params.view = m_view;
		params.task = m_task;
		params.peakMode = m_scope->peakMode();
		params.nPeakModeRecId = m_scope->peakModeRecId();
		params.nCols = nCols;
		params.size = sz;
		params.nSampleOffset = m_nSampleOffset;
		params.vwiHilight = m_vwiHilight;
		if (m_task == EadTask_Publish)
		{
			params.elements = pub->publisherChartElements;
		}
		// Check whether we need to hide the wave comments
		else
		{
			if (!Globals->viewSettings()->bShowWaveComments)
				params.elements &= ~ChartElement_WaveComments;
		}
		if (bOverrideTimebase)
		{
			params.nSecondsPerDivision = pub->nPublishTimebase * 60;
			m_nSecondsPerDivision = params.nSecondsPerDivision;
		}
		else
		{
			m_nSecondsPerDivision = g_anSecondsPerDivision[m_iSecondsPerDivision];
		}
		params.nSecondsPerDivision = m_nSecondsPerDivision;
		m_pixmap->draw(params);

		m_btnZoomIn->setEnabled(!bOverrideTimebase);
		m_btnZoomOut->setEnabled(!bOverrideTimebase);
		m_btnZoomFull->setEnabled(!bOverrideTimebase);
		m_lblSecondsPerDivision->setText(timebaseString());


		int nSamplesPerPage = int(m_nSecondsPerDivision * EAD_SAMPLES_PER_SECOND * m_pixmap->params().nCols);
		m_scrollbar->setPageStep(nSamplesPerPage);
		m_scrollbar->setSingleStep(nSamplesPerPage / m_pixmap->params().nCols);

		// Total number of samples in the dataset
		int nSamples = sampleCount();

		int nMaxScroll = nSamples - nSamplesPerPage;
		if (nMaxScroll < 0)
			nMaxScroll = m_nSampleOffset;
		m_scrollbar->setMaximum(nMaxScroll);
		m_scrollbar->setValue(m_nSampleOffset);

		QRect rcScroll(m_rcPixmapMax.left(), m_rcPixmapMax.top() + sz.height(), sz.width(), 20);
		if (rcScroll != m_scrollbar->geometry())
			m_scrollbar->setGeometry(rcScroll);
	}

	m_rcPixmap = m_pixmap->borderRect();
	m_rcPixmap.moveTo(m_rcPixmapMax.topLeft());
	painter.drawPixmap(m_rcPixmap.topLeft(), m_pixmap->pixmap());

	// If the user is currently selecting an area to zoom in on:
	if (m_bDragging && m_bSelecting)
	{
		int x0 = m_ptClickWidget.x();
		int x1 = m_ptMouseWidget.x();
		if (x0 > x1)
			qSwap(x0, x1);
		x0 = qMax(x0, m_rcPixmap.left());
		x1 = qMin(x1, m_rcPixmap.right());
		
		QRect rcBorder = m_rcPixmap;
		QRect rc(QPoint(x0, rcBorder.top() + 1), QPoint(x1, rcBorder.bottom() - 1));
		QColor clr(0, 0, 255, 100);
		painter.fillRect(rc, clr);
	}
}

/*
void ChartWidget::paintAreaHandles(QPainter& painter, ViewWaveInfo* vwi)
{
	painter.setPen(QColor(0, 50, 255));

	// Draw stuff
	foreach (WavePeakInfo peak, vwi->wave()->peaksChosen)
	{
		{
			int i = peak.left.i + vwi->shift();
			int x = sampleOffsetToX(i);
			int y = valueToY(vwi, peak.left.n);

			QRect rcStart(x - 5, y - 5, 11, 11);
			QColor clr(0, 50, 255, 80);
			painter.drawRect(rcStart);
			painter.fillRect(rcStart, clr);
		}

		{
			int i = peak.right.i + vwi->shift();
			int x = sampleOffsetToX(i);
			int y = valueToY(vwi, peak.right.n);

			QRect rcStart(x - 5, y - 5, 11, 11);
			QColor clr(0, 50, 255, 80);
			painter.drawRect(rcStart);
			painter.fillRect(rcStart, clr);
		}
	}

	painter.setPen(Qt::black);
}
*/

void ChartWidget::center(int iSample)
{
	int nSamplesToCenter = int(m_nSecondsPerDivision * m_pixmap->params().nCols * EAD_SAMPLES_PER_SECOND / 2);
	int n = qMax(iSample - nSamplesToCenter, 0);
	setSampleOffset(n);
}

void ChartWidget::setHilight(ViewWaveInfo* vwi)
{
	if (m_vwiHilight != vwi)
	{
		m_vwiHilight = vwi;
		repaintChart();
	}
}

void ChartWidget::leaveEvent(QEvent* e)
{
	Q_UNUSED(e);
	setHilight(NULL);
}

void ChartWidget::mousePressEvent(QMouseEvent* e)
{
	QRect rcWaveforms = m_rcPixmap;

	ChartPointInfo info;
	QPoint ptPixmap = e->pos() - m_rcPixmap.topLeft();
	m_pixmap->fillChartPointInfo(ptPixmap, &info);
	m_clickInfo = info;
	ViewWaveInfo* vwi = info.vwi;

	m_ptMouseWidget = e->pos();
	m_ptMousePixmap = ptPixmap;
	m_ptClickWidget = m_ptMouseWidget;
	m_ptClickPixmap = m_ptMousePixmap;
	
	const WaveInfo* wave = NULL;
	if (vwi != NULL)
		wave = vwi->wave();

	if (e->button() == Qt::LeftButton)
	{
		// Clicked on a chosen peak:
		if (info.iChosenPeak >= 0)
		{
			//vwi->unchoosePeakAtDidx(info.didxChosenPeak);
			m_bDragging = true;
		}
		// Clicked on a detected peak:
		else if (info.didxPossiblePeak >= 0)
		{
			vwi->choosePeakAtDidx(info.didxPossiblePeak);
		}
		// Clicked on a peak area handle:
		else if (info.iLeftAreaHandle >= 0 || info.iRightAreaHandle >= 0)
		{
			m_bDragging = true;
		}
		// Clicked on a wave:
		else if (wave != NULL)
		{
			// If the user Ctrl+clicks on the selected FID wave while in peak editing mode:
			if (e->modifiers() == Qt::ControlModifier && m_scope->peakMode() == EadPeakMode_Edit && wave->recId() == m_scope->peakModeRecId())
			{
				addPeak(vwi, m_ptClickPixmap.x());
			}
			else
			{
				m_bDragging = true;
				// REFACTOR: remove m_waveDrag and use m_clickInfo instead
				//m_waveDrag = vwi->waveInfo();
				m_nDragOrigDivisionOffset = vwi->divisionOffset();
			}
		}
		else if (rcWaveforms.contains(e->pos()))
		{
			m_bDragging = true;
			m_bSelecting = true;
			updateStatus();
			update();
		}
		else
		{
			m_bDragging = false;
		}
	}
	/*
	else if (e->button() == Qt::RightButton)
	{
		if (wave != NULL)
		{
			int iMouse = m_pixmap->xToCenterSample(wave, e->pos().x());
			int i0 = qMax(iMouse - 25, 0);
			int i1 = qMin(iMouse + 25, wave->display.size() - 1);
			
			for (int i = i0; i <= i1; i++)
			{
				
			}
		}
	}
	*/

	updateStatus();
}

void ChartWidget::mouseReleaseEvent(QMouseEvent* e)
{
	Q_UNUSED(e);

	m_ptMouseWidget = e->pos();
	m_ptMousePixmap = m_ptMouseWidget - m_rcPixmap.topLeft();

	if (m_bDragging)
	{
		const ChartPointInfo& info = m_clickInfo;
		ViewWaveInfo* vwi = info.vwi;

		if (m_bSelecting)
		{
			int dx = qAbs(m_ptClickPixmap.x() - m_ptMousePixmap.x()) + 1;

			// Require minimal zoom width to avoid acting on mistaken mouse clicks
			if (dx > 10)
			{
				int iSampleStart = m_pixmap->xToSampleOffset(m_ptClickPixmap.x());
				int iSampleEnd = m_pixmap->xToSampleOffset(m_ptMousePixmap.x());
				int iSampleCenter = (iSampleStart + iSampleEnd) / 2;
				int nSamples = qAbs(iSampleEnd - iSampleStart);
				double nSeconds = double(nSamples) / EAD_SAMPLES_PER_SECOND;
				
				// New seconds per division
				double nSecondsPerDivision = nSeconds / 10;
				setSecondsPerDivisionMin(nSecondsPerDivision);
				center(iSampleCenter);
			}

			update();
		}
		// If we were dragging a wave
		else if (vwi != NULL && info.iLeftAreaHandle < 0 && info.iRightAreaHandle < 0)
		{
			vwi->bAssureVisibility = true; // HACK: tell ChartPixmap to reposition this wave for visibility if necessary
		}
	}
	m_bDragging = false;
	m_bSelecting = false;
	//m_waveDrag = NULL;
	
	updateStatus();
}

void ChartWidget::mouseMoveEvent(QMouseEvent* e)
{
	m_ptMouseWidget = e->pos();
	m_ptMousePixmap = m_ptMouseWidget - m_rcPixmap.topLeft();

	if (m_bDragging)
	{
		const ChartPointInfo& info = m_clickInfo;
		ViewWaveInfo* vwi = info.vwi;
		WaveInfo* wave = (vwi != NULL) ? vwi->waveInfo() : NULL;

		if (m_bSelecting)
		{
			// While selecting, don't let the mouse move out of the pixmap
			if (m_ptMousePixmap.x() < 0)
				m_ptMousePixmap.setX(0);
			else if (m_ptMousePixmap.x() >= m_rcPixmap.width())
				m_ptMousePixmap.setX(m_rcPixmap.width() - 1);
			update();
		}
		else if (info.iLeftAreaHandle >= 0)
		{
			int x = e->pos().x() - m_rcPixmap.left();
			int i = m_pixmap->xToCenterSample(info.vwi->wave(), x);
			info.vwi->waveInfo()->peaksChosen[info.iLeftAreaHandle].didxLeft = i;
			info.vwi->waveInfo()->calcPeakArea(info.iLeftAreaHandle);
			info.vwi->waveInfo()->calcAreaPercents();
			repaintChart();
		}
		else if (info.iRightAreaHandle >= 0)
		{
			int x = e->pos().x() - m_rcPixmap.left();
			int i = m_pixmap->xToCenterSample(info.vwi->wave(), x);
			info.vwi->waveInfo()->peaksChosen[info.iRightAreaHandle].didxRight = i;
			info.vwi->waveInfo()->calcPeakArea(info.iRightAreaHandle);
			info.vwi->waveInfo()->calcAreaPercents();
			repaintChart();
		}
		else if (info.iChosenPeak >= 0)
		{
			WavePeakChosenInfo& marker = wave->peaksChosen[info.iChosenPeak];
			int x = e->pos().x() - m_rcPixmap.left();
			int didx = m_pixmap->xToCenterSample(info.vwi->wave(), x);
			int d = didx - marker.didxMiddle;
			marker.didxLeft += d;
			marker.didxMiddle += d;
			marker.didxRight += d;
			info.vwi->waveInfo()->calcPeakArea(info.iRightAreaHandle);
			info.vwi->waveInfo()->calcAreaPercents();
			repaintChart();
		}
		else if (wave != NULL)
		{
			if (e->modifiers() == Qt::ControlModifier && m_scope->peakMode() == EadPeakMode_Edit && wave == m_pixmap->waveOfPeaks())
			{
				setCursor(Qt::PointingHandCursor);
			}
			else
			{
				// How much has the mouse been moved in total during this drag?
				double nStart = m_pixmap->yToValue(vwi, m_ptClickPixmap.y());
				double nEnd = m_pixmap->yToValue(vwi, m_ptMousePixmap.y());
				double nVolts = nEnd - nStart;
				double nOffset = nVolts / vwi->voltsPerDivision();
				// Set the channel offset appropriately
				vwi->setDivisionOffset(m_nDragOrigDivisionOffset - nOffset);

				repaintChart();
			}
		}
	}
	else
	{
		ChartPointInfo info;
		QPoint ptPixmap = e->pos() - m_rcPixmap.topLeft();
		m_pixmap->fillChartPointInfo(ptPixmap, &info);
		setHilight(info.vwi);
		
		if (info.didxPossiblePeak >= 0 || info.iChosenPeak >= 0 || info.iLeftAreaHandle >= 0 || info.iRightAreaHandle >= 0)
		{
			setCursor(Qt::PointingHandCursor);
		}
		else if (info.vwi != NULL)
		{
			Qt::CursorShape shape = Qt::OpenHandCursor;

			if (e->modifiers() == Qt::ControlModifier && m_scope->peakMode() == EadPeakMode_Edit && info.vwi->wave() == m_pixmap->waveOfPeaks())
				shape = Qt::PointingHandCursor;

			setCursor(shape);
		}
		else if (m_rcPixmap.contains(e->pos()))
		{
			setCursor(Qt::CrossCursor);
		}
		else
			unsetCursor();
	}

	updateStatus();
}

void ChartWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
	QRect rcWaveforms = m_rcPixmap;

	ChartPointInfo info;
	QPoint ptPixmap = e->pos() - m_rcPixmap.topLeft();
	m_pixmap->fillChartPointInfo(ptPixmap, &info);
	m_clickInfo = info;
	ViewWaveInfo* vwi = info.vwi;

	m_ptMouseWidget = e->pos();
	m_ptMousePixmap = ptPixmap;
	m_ptClickWidget = m_ptMouseWidget;
	m_ptClickPixmap = m_ptMousePixmap;

	const WaveInfo* wave = NULL;
	if (vwi != NULL)
		wave = vwi->wave();

	if (e->button() == Qt::LeftButton)
	{
		// Clicked on a chosen peak:
		if (info.iChosenPeak >= 0)
		{
		}
		// Clicked on a detected peak:
		else if (info.didxPossiblePeak >= 0)
		{
		}
		// Clicked on a peak area handle:
		else if (info.iLeftAreaHandle >= 0 || info.iRightAreaHandle >= 0)
		{
		}
		// Clicked on a wave:
		else if (wave != NULL)
		{
			QPoint ptGlobal = mapToGlobal(e->pos());
			openWaveEditorDialog(vwi, ptGlobal);
		}
		else if (rcWaveforms.contains(e->pos()))
		{
		}
		else
		{
			m_bDragging = false;
		}
	}

	updateStatus();
}

void ChartWidget::wheelEvent(QWheelEvent* e)
{
	// Each 'click' of the scroll wheel produces a delta of 120
	int nDivs = e->delta() / 120;
	int nSamples = -nDivs * m_nSecondsPerDivision * EAD_SAMPLES_PER_SECOND;
	int i = m_scrollbar->value() + nSamples;
	if (i < 0)
		i = 0;
	m_scrollbar->setValue(i);
}

void ChartWidget::contextMenuEvent(QContextMenuEvent* e)
{
	QRect rcWaveforms = m_rcPixmap;

	ChartPointInfo info;
	QPoint ptPixmap = e->pos() - m_rcPixmap.topLeft();
	m_pixmap->fillChartPointInfo(ptPixmap, &info);
	m_clickInfo = info;
	ViewWaveInfo* vwi = info.vwi;

	m_ptMouseWidget = e->pos();
	m_ptMousePixmap = ptPixmap;
	m_ptClickWidget = m_ptMouseWidget;
	m_ptClickPixmap = m_ptMousePixmap;

	const WaveInfo* wave = NULL;
	if (vwi != NULL)
		wave = vwi->wave();

	QMenu menu(this);
	QAction* actSettings = NULL;
	QAction* actAddMarker = NULL;
	QAction* actRemoveMarker = NULL;
	{
		// Clicked on a chosen peak:
		if (info.iChosenPeak >= 0)
		{
			actRemoveMarker = new QAction(tr("Remove Marker"), &menu);
			menu.addAction(actRemoveMarker);
		}
		// Clicked on a detected peak:
		else if (info.didxPossiblePeak >= 0)
		{
			actAddMarker = new QAction(tr("Add Peak Marker"), &menu);
			menu.addAction(actAddMarker);
		}
		// Clicked on a peak area handle:
		else if (info.iLeftAreaHandle >= 0 || info.iRightAreaHandle >= 0)
		{
			actRemoveMarker = new QAction(tr("Remove Marker"), &menu);
			menu.addAction(actRemoveMarker);
		}
		// Clicked on a wave:
		else if (wave != NULL)
		{
			actSettings = new QAction(tr("Settings..."), &menu);
			menu.addAction(actSettings);
			if (wave->type == WaveType_FID)
				actAddMarker = new QAction(tr("Add Peak Marker"), &menu);
			else
				actAddMarker = new QAction(tr("Add Time Marker"), &menu);
			menu.addAction(actAddMarker);
		}
		else if (rcWaveforms.contains(e->pos()))
		{
			menu.addAction(m_scope->actions()->viewZoomIn);
			menu.addAction(m_scope->actions()->viewZoomOut);
			menu.addAction(m_scope->actions()->viewZoomFull);
		}
		else
		{
		}
	}
	if (menu.actions().size() > 0)
	{
		QPoint ptGlobal = mapToGlobal(e->pos());
		QAction* act = menu.exec(ptGlobal);
		if (act == NULL)
			;
		else if (act == actSettings)
		{
			openWaveEditorDialog(vwi, ptGlobal);
		}
		else if (act == actAddMarker)
		{
			if (info.didxPossiblePeak >= 0)
				vwi->choosePeakAtDidx(info.didxPossiblePeak);
			else
				addPeak(vwi, ptPixmap.x());
		}
		else if (act == actRemoveMarker)
		{
			CHECK_ASSERT_NORET(info.iChosenPeak >= 0);
			if (info.iChosenPeak >= 0)
				vwi->unchoosePeakAtIndex(info.iChosenPeak);
		}
	}

	e->accept();
	updateStatus();
}

void ChartWidget::addPeak(ViewWaveInfo* vwi, int x)
{
	int didx1 = m_pixmap->xToCenterSample(vwi->wave(), x);
	int didx0 = m_pixmap->xToCenterSample(vwi->wave(), x - 10);
	int didx2 = m_pixmap->xToCenterSample(vwi->wave(), x + 10);

	if (didx0 >= 0 && didx2 < vwi->wave()->display.size())
	{
		WavePeakChosenInfo peak;
		peak.didxLeft = didx0;
		peak.didxMiddle = didx1;
		peak.didxRight = didx2;
		vwi->choosePeak(peak);
	}
}

void ChartWidget::openWaveEditorDialog(ViewWaveInfo* vwi, const QPoint& ptGlobal)
{
	WaveEditorDialog editor(vwi, vwi->editorFlags, this);
	editor.move(ptGlobal);
	editor.exec();
}

static int calcPrecision(double nMinutes)
{
	int nPrecision;
	if (nMinutes >= 1)
		nPrecision = 1;
	else if (nMinutes >= 0.1)
		nPrecision = 2;
	else
		nPrecision = 3;
	return nPrecision;
}

void ChartWidget::updateStatus()
{
	if (m_statusbar == NULL)
		return;

	QString s;
	if (m_bDragging && m_bSelecting)
	{
		// REFACTOR: Duplicate code
		int iSampleStart = m_pixmap->xToSampleOffset(m_ptClickPixmap.x());
		int iSampleEnd = m_pixmap->xToSampleOffset(m_ptMousePixmap.x());
		int nSamples = qAbs(iSampleEnd - iSampleStart);
		
		double nMinutes0 = double(iSampleStart) / (EAD_SAMPLES_PER_SECOND * 60);
		double nMinutes1 = double(iSampleEnd) / (EAD_SAMPLES_PER_SECOND * 60);
		double nMinutes = double(nSamples) / (EAD_SAMPLES_PER_SECOND * 60);
		
		int nPrecision = calcPrecision(nMinutes / 10);
		s = tr("Selecting: %0 to %1").arg(nMinutes0, 0, 'f', nPrecision).arg(nMinutes1, 0, 'f', nPrecision);;
	}
	else
	{
		int iSample = m_pixmap->xToSampleOffset(m_ptMousePixmap.x());
		double nMinutes = double(iSample) / (EAD_SAMPLES_PER_SECOND * 60);
		int nPrecision = calcPrecision(m_nSecondsPerDivision / 60);
		s = tr("Pos: %0").arg(nMinutes, 0, 'f', nPrecision);
	}
	m_lblStatus->setText(s);
}
