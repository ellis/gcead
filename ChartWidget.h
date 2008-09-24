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

#ifndef __CHARTWIDGET_H
#define __CHARTWIDGET_H

#include <QMap>
#include <QWidget>

#include "ChartPixmap.h"
#include "EadEnums.h"


class QLabel;
class QScrollBar;
class QStatusBar;
class QToolButton;

class EadFile;
class MainScope;
class ViewInfo;
class WaveInfo;


class ChartWidget : public QWidget
{
	Q_OBJECT
public:
	ChartWidget(MainScope* scope, QWidget* parent = NULL);
	~ChartWidget();

	void setStatusBar(QStatusBar* statusbar);

	EadFile* file() { return m_file; }

	//double timebase() const;
	QString timebaseString() const;
	int sampleOffset() const { return m_nSampleOffset; }
	double secondsPerDivision() const { return m_nSecondsPerDivision; }
	const ChartPixmap* pixmap() const { return m_pixmap; }

	void setSecondsPerDivisionIndex(int i);
	/// Set seconds per division using the minimum value @param nMin
	bool setSecondsPerDivisionMin(double nMin);

public slots:
	void setSampleOffset(int nSampleOffset);
	void zoomOut();
	void zoomIn();
	void zoomFull();

	/// Force a repaint of the chart
	void repaintChart();
	void updateRecordings();

// Overrides for QWidget
protected:
	void resizeEvent(QResizeEvent* e);
	void paintEvent(QPaintEvent* e);
	void leaveEvent(QEvent* e);
	void mousePressEvent(QMouseEvent* e);
	//void mouseDoubleClickEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);

private:
	void setupWidgets();
	void setup(EadFile* file, ViewInfo* view, EadTask task);

	QSize calcPixmapSize() const;
	int sampleCount();
	//void updateDrawingParameters();
	//int xToSample(int x) const;
	//int xToMiddleSample(int x, const WaveInfo* wave) const;

	/// Center the waveform display around iSample
	void center(int iSample);

	void setHilight(ViewWaveInfo* vwi);

	void updateStatus();

	/// Convert a screen coordinate to box coordinates
	//QPoint screenToBoxPoint(const QPoint& pt) const;
	//void mouseOverWave(const QPoint& pt, int* piWave, bool* pbPeak);

//private slots:
	//void on_globals_taskChanged(EadTask task, EadTask oldTask);
	//void on_globals_viewChanged(EadView view, EadView oldView);
private slots:
	void scope_fileChanged();
	void scopeChanged();
	void showRecordingLabel(bool bShow);
	void on_view_changed(ViewChangeEvents events);
	void on_timerUpdate_timeout();

private:
	MainScope* m_scope;
	QStatusBar* m_statusbar;
	QLabel* m_lblStatus;

	QPointer<EadFile> m_file;
	QPointer<ViewInfo> m_view;
	EadTask m_task;

	QWidget* m_buttons;
	QToolButton* m_btnZoomIn;
	QToolButton* m_btnZoomOut;
	QToolButton* m_btnZoomFull;
	QLabel* m_lblSecondsPerDivision;
	QScrollBar* m_scrollbar;
	QLabel* m_lblRecording;

	/// Maximum area that the pixmap is allowed to use
	QRect m_rcPixmapMax;
	QRect m_rcPixmap;
	ChartPixmap* m_pixmap;
	bool m_bRedraw;

	/// Scroll offset
	int m_nSampleOffset;
	/// Index into array of valid seconds per division settings
	int m_iSecondsPerDivision;
	/// Seconds per division, for convenience
	double m_nSecondsPerDivision;

	/// The currently hilighted wave
	QPointer<ViewWaveInfo> m_vwiHilight;

	/// Whether the user is currently dragging the mouse (click+drag)
	bool m_bDragging;
	/// Whether the user is selecting an area to zoom in on
	bool m_bSelecting;
	/// Coordinate where dragging started
	QPoint m_ptClickWidget;
	QPoint m_ptClickPixmap;
	QPoint m_ptMouseWidget;
	QPoint m_ptMousePixmap;
	/// Info about the point where the user pressed the mouse
	ChartPointInfo m_clickInfo;
	/// Pointer to the channel we're currently dragging, or NULL if not dragging.
	WaveInfo* m_waveDrag;
	/// Original division offset of m_dragChannel before dragging started
	double m_nDragOrigDivisionOffset;

	QTimer* m_timerUpdate;
	int m_nRecordingUpdates;
	WaveInfo* m_recordingEadWave;
	WaveInfo* m_recordingFidWave;
};

#endif
