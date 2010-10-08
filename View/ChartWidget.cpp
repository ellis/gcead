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
#include <ChartScope.h>
#include "Check.h"
#include "EadFile.h"
#include "Globals.h"
#include "MainScope.h"
#include "PublisherSettings.h"
#include "Utils.h"
#include "ViewSettings.h"
#include "WaveEditorDialog.h"


ChartWidget::ChartWidget(MainScope* mainS, QWidget* parent)
	: QWidget(parent)
{
	Q_ASSERT(mainS != NULL);
	Q_ASSERT(mainS->chart() != NULL);

	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);

	m_mainS = mainS;
	m_chartS = mainS->chart();
	m_pixmap = m_chartS->pixmap();

	m_statusbar = NULL;
	m_lblStatus = NULL;

	m_bDragging = false;
	m_bSelecting = false;
	m_bForceStatusUpdate = false;

	m_recordingEadWave = NULL;
	m_recordingFidWave = NULL;

	setupWidgets();

	//m_timerUpdate = new QTimer(this);

	connect(m_chartS, SIGNAL(paramsChanged()), this, SLOT(update()));
	connect(m_chartS, SIGNAL(recordingLabelVisibleChanged(bool)), this, SLOT(on_scope_recordingLabelVisibleChanged(bool)));
	connect(m_chartS, SIGNAL(recordingLabelTextChanged(QString)), m_lblRecording, SLOT(setText(QString)));
	connect(m_chartS, SIGNAL(timebaseChanged(QString)), this, SLOT(on_scope_timebaseChanged(QString)));
	connect(m_chartS, SIGNAL(scrollMaxChanged(int)), this, SLOT(on_scope_scrollMaxChanged(int)));
	connect(m_chartS, SIGNAL(scrollPageStepChanged(int)), this, SLOT(on_scope_scrollPageStepChanged(int)));
	connect(m_chartS, SIGNAL(scrollSingleStepChanged(int)), this, SLOT(on_scope_scrollSingleStepChanged(int)));
	connect(m_chartS, SIGNAL(scrollValueChanged(int)), m_scrollbar, SLOT(setValue(int)));
	connect(m_scrollbar, SIGNAL(valueChanged(int)), m_chartS, SLOT(setSampleOffset(int)));
}

ChartWidget::~ChartWidget()
{
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
	tbtn->setDefaultAction(m_mainS->actions()->viewZoomIn);
	tbtn->setFocusPolicy(Qt::NoFocus);
	tblayout->addWidget(tbtn);
	m_btnZoomIn = tbtn;

	tbtn = new QToolButton;
	tbtn->setDefaultAction(m_mainS->actions()->viewZoomOut);
	tbtn->setFocusPolicy(Qt::NoFocus);
	tblayout->addWidget(tbtn);
	m_btnZoomOut = tbtn;

	tbtn = new QToolButton;
	tbtn->setDefaultAction(m_mainS->actions()->viewZoomFull);
	tbtn->setFocusPolicy(Qt::NoFocus);
	tblayout->addWidget(tbtn);
	tblayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
	m_btnZoomFull = tbtn;

	m_buttons->setLayout(tblayout);

	m_scrollbar = new QScrollBar(Qt::Horizontal, this);
	m_scrollbar->setSingleStep(EAD_SAMPLES_PER_SECOND);

	m_buttons->setCursor(Qt::ArrowCursor);
	m_scrollbar->setCursor(Qt::ArrowCursor);

	// Set fixed size for m_lblSecondsPerDivision
	int nWidth = 0;
	for (int i = 0; ChartScope::anSecondsPerDivision[i] > 0; i++)
	{
		QString s = timestampString(ChartScope::anSecondsPerDivision[i]);
		nWidth = qMax(nWidth, m_lblSecondsPerDivision->fontMetrics().width(s));
	}
	m_lblSecondsPerDivision->setMinimumWidth(nWidth);
	m_lblSecondsPerDivision->setText(m_chartS->timebaseString());

	m_lblRecording = new QLabel(tr("  RECORDING: 99.99 "), this);
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
		connect(m_chartS, SIGNAL(statusTextChanged(QString)), m_lblStatus, SLOT(setText(QString)));
	}
}

void ChartWidget::on_scope_timebaseChanged(const QString& s)
{
	m_lblSecondsPerDivision->setText(s);
	m_bForceStatusUpdate = true;
}

void ChartWidget::on_scope_scrollMaxChanged(int i) { m_scrollbar->setMaximum(i); }
void ChartWidget::on_scope_scrollPageStepChanged(int n) { m_scrollbar->setPageStep(n); }
void ChartWidget::on_scope_scrollSingleStepChanged(int n) { m_scrollbar->setSingleStep(n); }

void ChartWidget::on_scope_recordingLabelVisibleChanged(bool b)
{
	m_lblRecording->setVisible(b);
	if (b) {
		layoutRecordingLabel();
	}
}

QSize ChartWidget::calcPixmapSize() const
{
	QRect rcPixmapMax(20, m_buttons->height(), width() - 35, height() - m_buttons->height() - 20);

	QSize sz = m_rcPixmapMax.size();
	int nCols = 0;
	if (m_mainS->taskType() == EadTask_Publish)
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
	
	m_rcPixmapMax = QRect(20, m_buttons->height(), width() - 35, height() - m_buttons->height() - 20);
	QSize sz = calcPixmapSize();
	
	QRect rcScroll(m_rcPixmapMax.left(), m_rcPixmapMax.top() + sz.height(), sz.width(), 20);
	
	m_scrollbar->setGeometry(rcScroll);

	layoutRecordingLabel();

	m_chartS->redraw();
}

void ChartWidget::layoutRecordingLabel()
{
	m_lblRecording->move(width() - m_lblRecording->width() - 20, 5);
}

void ChartWidget::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e);

	QPainter painter(this);

	painter.fillRect(e->rect(), Qt::white);

	if (m_chartS->isRedrawRequired())
	{
		QSize sz = calcPixmapSize();
		m_chartS->draw(sz);

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

	if (m_bForceStatusUpdate)
	{
		m_bForceStatusUpdate = false;
		updateStatus();
	}
}

void ChartWidget::leaveEvent(QEvent* e)
{
	Q_UNUSED(e);
	m_chartS->setHilight(NULL);
}

void ChartWidget::keyPressEvent(QKeyEvent* e)
{
	// Escape key pressed
	if (e->modifiers() == 0 && e->key() == Qt::Key_Escape) {
		// Selecting:
		if (m_bDragging && m_bSelecting) {
			m_bDragging = false;
			m_bSelecting = false;

			// Call this to get the appropriate mouse cursor
			updateMouseCursor(e->modifiers());

			update();
		}
	}
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
	m_nClickSampleOffset = m_chartS->sampleOffset();
	
	const WaveInfo* wave = NULL;
	if (vwi != NULL)
		wave = vwi->wave();

	if (e->button() == Qt::LeftButton)
	{
		// If the user Ctrl+clicks on the selected FID wave while in peak editing mode:
		if (e->modifiers() == Qt::ControlModifier)
		{
			// If the user Ctrl+clicks on the selected FID wave while in peak editing mode:
			if (wave != NULL && m_chartS->params().peakMode == EadMarkerMode_Edit) {
				if (info.iChosenPeak < 0)
					addMarker(vwi, m_ptClickPixmap.x());
				else if (wave->peaksChosen[info.iChosenPeak].type == MarkerType_EadPeakXY) {
					addEadPeakXYEndPoint(vwi, info.iChosenPeak);
				}
			}
		}
		else {
			bool bIgnore = false;

			// Force no dragging of markers unless in edit mode:
			if (m_chartS->params().peakMode != EadMarkerMode_Edit) {
				if (m_clickInfo.iChosenPeak >= 0)
					bIgnore = true;
			}

			if (bIgnore)
			{
				;
			}
			// Clicked on a chosen peak:
			else if (info.iChosenPeak >= 0)
			{
				m_bDragging = true;
				setCursor(Qt::SizeHorCursor);
			}
			// Clicked on a detected peak:
			else if (info.didxPossiblePeak >= 0)
			{
				vwi->choosePeakAtDidx(info.didxPossiblePeak);
			}
			// Clicked on a wave:
			else if (wave != NULL)
			{
				m_bDragging = true;
				// REFACTOR: remove m_waveDrag and use m_clickInfo instead
				//m_waveDrag = vwi->waveInfo();
				m_nDragOrigDivisionOffset = vwi->divisionOffset();
				setCursor(Qt::SizeVerCursor);
			}
			else if (rcWaveforms.contains(e->pos()))
			{
				m_bDragging = true;
				m_bSelecting = true;
				setCursor(Qt::IBeamCursor);
				updateStatus();
				update();
			}
			else
			{
				m_bDragging = false;
			}
		}
	}
	else if (e->button() == Qt::MidButton)
	{
		m_clickInfo.iChosenPeak = -1;
		m_clickInfo.iMarkerDidx = -1;
		m_clickInfo.vwi = NULL;
		m_bDragging = true;
		setCursor(Qt::SizeHorCursor);
	}

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
				m_chartS->setSecondsPerDivisionMin(nSecondsPerDivision);
				m_chartS->center(iSampleCenter);
			}

			update();
		}
		// If we were dragging a wave
		else if (vwi != NULL && info.iChosenPeak < 0)
		{
			vwi->bAssureVisibility = true; // HACK: tell ChartPixmap to reposition this wave for visibility if necessary
		}
	}
	m_bDragging = false;
	m_bSelecting = false;
	
	// Call this to get the appropriate mouse cursor
	mouseMoveEvent(e);
}

void ChartWidget::mouseMoveEvent(QMouseEvent* e)
{
	const QPoint& pos = e->pos();
	m_ptMouseWidget = pos;
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
		else if (info.iChosenPeak >= 0)
		{
			int x = pos.x() - m_rcPixmap.left();
			int didx = m_pixmap->xToCenterSample(info.vwi->wave(), x);
			moveMarkerHandle(vwi, info.iChosenPeak, info.iMarkerDidx, didx);
		}
		else if (wave != NULL)
		{
			// REFACTOR: I don't know what this statement is doing here -- ellis, 2010-10-04
			if (e->modifiers() == Qt::ControlModifier && m_chartS->params().peakMode == EadMarkerMode_Edit && wave == m_pixmap->waveOfPeaks())
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

				m_chartS->redraw();
			}
		}
		else
		{
			int nDiffXOrig = m_ptMousePixmap.x() - m_ptClickPixmap.x();
			int nDiffSamplesOrig = m_pixmap->widthToSampleCount(-nDiffXOrig);
			int iSample = m_nClickSampleOffset + nDiffSamplesOrig;
			m_chartS->setSampleOffset(iSample);
		}
	}
	else
	{
		updateMouseCursor(e->modifiers());
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
	bool bDraggingWithMiddleMouseButton = false;
	if (m_bDragging) {
		const ChartPointInfo& info = m_clickInfo;
		ViewWaveInfo* vwi = info.vwi;
		WaveInfo* wave = (vwi != NULL) ? vwi->waveInfo() : NULL;
		bDraggingWithMiddleMouseButton = (wave == NULL);
	}

	// Suppress mouse wheel event if the user is already dragging the timeline by clicking and dragging the mouse wheel button
	if (!bDraggingWithMiddleMouseButton) {
		// Each 'click' of the scroll wheel produces a delta of 120
		int nDivs = e->delta() / 120;
		m_chartS->scrollDivs(-nDivs);
	}
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
	QAction* actAddPeakEndPoint = NULL;
	QAction* actAddPeakMarker = NULL;
	QAction* actAddTimeMarker = NULL;
	QAction* actRemoveMarker = NULL;
	QAction* actSettings = NULL;
	{
		// Clicked on a chosen peak:
		if (info.iChosenPeak >= 0)
		{
			if (m_chartS->params().task == EadTask_Markers) {
				actRemoveMarker = new QAction(tr("Remove Marker"), &menu);
				menu.addAction(actRemoveMarker);
				if (wave->peaksChosen[info.iChosenPeak].type == MarkerType_EadPeakXY) {
					actAddPeakEndPoint = new QAction(tr("Add End-Point to Marker"), &menu);
					menu.addAction(actAddPeakEndPoint);
				}
			}
		}
		// Clicked on a detected peak:
		else if (info.didxPossiblePeak >= 0)
		{
			actAddPeakMarker = new QAction(tr("Add Peak Marker"), &menu);
			menu.addAction(actAddPeakMarker);
		}
		// Clicked on a wave:
		else if (wave != NULL)
		{
			menu.addAction(actSettings);
			if (m_chartS->params().task == EadTask_Markers) {
				if (wave->type == WaveType_EAD || wave->type == WaveType_FID) {
					actAddPeakMarker = new QAction(tr("Add Peak Marker"), &menu);
					menu.addAction(actAddPeakMarker);
				}
				actAddTimeMarker = new QAction(tr("Add Time Marker"), &menu);
				menu.addAction(actAddTimeMarker);
			}
			actSettings = new QAction(tr("Settings..."), &menu);
		}
		else if (rcWaveforms.contains(e->pos()))
		{
			menu.addAction(m_mainS->actions()->viewZoomIn);
			menu.addAction(m_mainS->actions()->viewZoomOut);
			menu.addAction(m_mainS->actions()->viewZoomFull);
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
		else if (act == actAddPeakEndPoint)
		{
			addEadPeakXYEndPoint(vwi, info.iChosenPeak);
		}
		else if (act == actAddPeakMarker)
		{
			if (info.didxPossiblePeak >= 0)
				vwi->choosePeakAtDidx(info.didxPossiblePeak);
			else
				addMarker(vwi, ptPixmap.x());
		}
		else if (act == actAddTimeMarker)
		{
			addMarker(vwi, MarkerType_Time, ptPixmap.x());
		}
		else if (act == actRemoveMarker)
		{
			if (info.iChosenPeak >= 0)
				vwi->unchoosePeakAtIndex(info.iChosenPeak);
		}
		else if (act == actSettings)
		{
			openWaveEditorDialog(vwi, ptGlobal);
		}
	}

	e->accept();
	updateStatus();
}

void ChartWidget::updateMouseCursor(Qt::KeyboardModifiers modifiers)
{
	QPoint pos = m_ptMouseWidget;
	ChartPointInfo info;
	QPoint ptPixmap = pos - m_rcPixmap.topLeft();
	m_pixmap->fillChartPointInfo(ptPixmap, &info);
	m_chartS->setHilight(info.vwi);

	if (info.didxPossiblePeak >= 0 || info.iChosenPeak >= 0)
	{
		setCursor(Qt::PointingHandCursor);
	}
	else if (info.vwi != NULL)
	{
		Qt::CursorShape shape = Qt::OpenHandCursor;

		if (modifiers == Qt::ControlModifier && m_chartS->params().peakMode == EadMarkerMode_Edit && info.vwi->wave() == m_pixmap->waveOfPeaks())
			shape = Qt::PointingHandCursor;

		setCursor(shape);
	}
	else if (m_rcPixmap.contains(pos))
	{
		setCursor(Qt::CrossCursor);
	}
	else
		unsetCursor();
}

void ChartWidget::addMarker(ViewWaveInfo *vwi, int x)
{
	WaveInfo* wave = vwi->waveInfo();

	MarkerType markerType = MarkerType_Time;
	if (wave->type == WaveType_FID)
		markerType = MarkerType_FidPeak;
	else if (wave->type == WaveType_EAD)
		markerType = MarkerType_EadPeakXY;

	addMarker(vwi, markerType, x);
}

void ChartWidget::addMarker(ViewWaveInfo* vwi, MarkerType markerType, int x)
{
	WaveInfo* wave = vwi->waveInfo();

	WavePeakChosenInfo peak;
	peak.type = markerType;

	switch (markerType) {
	case MarkerType_EadPeakXY:
		setupMarkerType_EadPeakXY(wave, peak, x);
		break;
	case MarkerType_FidPeak:
		setupMarkerType_FidPeak(wave, peak, x);
		break;
	default:
		peak.type = MarkerType_Time;
		peak.didxs << m_pixmap->xToCenterSample(wave, x);
		break;
	}

	vwi->choosePeak(peak);
}

void ChartWidget::addEadPeakXYEndPoint(ViewWaveInfo *vwi, int iPeak)
{
	WavePeakChosenInfo& marker = vwi->waveInfo()->peaksChosen[iPeak];
	CHECK_ASSERT_RET(marker.didxs.size() == 2);

	int d = marker.didxs[1] - marker.didxs[0];
	int didx2 = marker.didxs[1] + d;

	vwi->setMarkerPoint(iPeak, 2, didx2);
	vwi->setMarkerType(iPeak, MarkerType_EadPeakXYZ);
}

void ChartWidget::setupMarkerType_EadPeakXY(const WaveInfo* wave, WavePeakChosenInfo& marker, int x)
{
	int didxLeft = m_pixmap->xToCenterSample(wave, x);
	int didxMiddle = m_pixmap->xToCenterSample(wave, x + 20);
	int didxEnd = m_pixmap->xToCenterSample(wave, m_pixmap->borderRect().right() - 1);

	int didxMiddle2 = wave->findNextEadMin(didxLeft, didxEnd);
	if (didxMiddle2 >= 0)
		didxMiddle = didxMiddle2;

	marker.didxs << didxLeft << didxMiddle;
}

void ChartWidget::setupMarkerType_FidPeak(const WaveInfo* wave, WavePeakChosenInfo& marker, int x)
{
	int didxLeft = m_pixmap->xToCenterSample(wave, x);
	int didxMiddle = m_pixmap->xToCenterSample(wave, x + 20);
	int radius = didxMiddle - didxLeft;
	int didxRight = didxMiddle + radius;
	int didxEnd = m_pixmap->xToCenterSample(wave, m_pixmap->borderRect().right() - 1);

	WavePeakInfo peak0;
	if (wave->findFidPeak(didxLeft, didxEnd, &peak0)) {
		didxMiddle = peak0.middle.i;
		didxRight = peak0.right.i;
	}

	marker.didxs << didxLeft << didxMiddle << didxRight;
}

void ChartWidget::moveMarkerHandle(ViewWaveInfo* vwi, int iPeak, int iDidx, int didx)
{
	vwi->setMarkerPoint(iPeak, iDidx, didx);
}

void ChartWidget::openWaveEditorDialog(ViewWaveInfo* vwi, const QPoint& ptGlobal)
{
	WaveEditorDialog editor(vwi, vwi->editorFlags, this);
	editor.move(ptGlobal);
	editor.exec();
}

void ChartWidget::updateStatus()
{
	if (m_statusbar == NULL)
		return;

	const ChartPixmap* pixmap = m_chartS->pixmap();

	QString s;
	if (m_bDragging && m_bSelecting)
	{
		int iSampleStart = pixmap->xToSampleOffset(m_ptClickPixmap.x());
		int iSampleEnd = pixmap->xToSampleOffset(m_ptMousePixmap.x());
		m_chartS->setSelectionRange(iSampleStart, iSampleEnd);
	}
	else
	{
		int iSample = pixmap->xToSampleOffset(m_ptMousePixmap.x());
		m_chartS->setMousePosition(iSample);
	}
}
