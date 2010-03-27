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

class ChartScope;
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

// Overrides for QWidget
protected:
	void resizeEvent(QResizeEvent* e);
	void paintEvent(QPaintEvent* e);
	void leaveEvent(QEvent* e);
	void mousePressEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mouseDoubleClickEvent(QMouseEvent* e);
	void wheelEvent(QWheelEvent* e);
	void contextMenuEvent(QContextMenuEvent* e);

private:
	void setupWidgets();
	void layoutRecordingLabel();

	QSize calcPixmapSize() const;

	void addPeak(ViewWaveInfo* vwi, int x);
	void movePeakHandle(WaveInfo* wave, int iPeak, int didx, bool bLeft);
	void openWaveEditorDialog(ViewWaveInfo* vwi, const QPoint& ptGlobal);
	void updateStatus();

private slots:
	//void on_timerUpdate_timeout();
	void on_scope_timebaseChanged(const QString& s);
	void on_scope_scrollMaxChanged(int i);
	void on_scope_scrollPageStepChanged(int n);
	void on_scope_scrollSingleStepChanged(int n);
	void on_scope_recordingLabelVisibleChanged(bool b);

private:
	MainScope* m_mainS;
	ChartScope* m_chartS;
	const ChartPixmap* m_pixmap;
	QStatusBar* m_statusbar;
	QLabel* m_lblStatus;

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
	/// Original division offset of m_dragChannel before vertical dragging started
	double m_nDragOrigDivisionOffset;
	/// Data for timeline dragging (middle-click)
	int m_nClickSampleOffset;

	bool m_bForceStatusUpdate;

	WaveInfo* m_recordingEadWave;
	WaveInfo* m_recordingFidWave;
};

#endif
