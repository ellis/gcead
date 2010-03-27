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

#ifndef __CHARTPIXMAP_H
#define __CHARTPIXMAP_H

#include <QHash>
#include <QPixmap>
#include <QPointer>
#include <QVector>

#include "EadEnums.h"
#include "EadFile.h" // Need to include this because 'moc' complains without it... --ellis, 2008-06-30


class QPainter;

class EadFile;
class RenderData;
class ViewInfo;
class ViewWaveInfo;
class WaveInfo;
class WavePos;


/// Number of columns on a chart
#define CHART_ROWS 10


/// Parameters for chart drawing
class ChartPixmapParams
{
public:
	QPointer<EadFile> file;
	QPointer<ViewInfo> view;
	EadTask task;
	EadMarkerMode peakMode;
	int nPeakModeRecId;
	QSize size;
	int nCols;
	int nSampleOffset;
	double nSecondsPerDivision;
	ChartElements elements;
	
	/// This wave show be drawn on top of all others
	QPointer<ViewWaveInfo> vwiHilight;
	bool bSelectedWaveIsActive;

	ChartPixmapParams()
	{
		file = NULL;
		view = NULL;
		nCols = CHART_ROWS;
		nSampleOffset = 0;
		nSecondsPerDivision = 50;
		elements = ChartElement_AxisTime | ChartElement_Grid | ChartElement_WaveNames | ChartElement_WaveComments;
		bSelectedWaveIsActive = false;
	}
};


/// Information about what's displayed at the chart at a given point
class ChartPointInfo
{
public:
	QPointer<ViewWaveInfo> vwi;
	int didxPossiblePeak;
	int iChosenPeak;
	int iLeftAreaHandle;
	int iRightAreaHandle;
};


class ChartPixmap
{
public:
	ChartPixmap();
	~ChartPixmap();

	const QPixmap& pixmap() const { return m_pixmap; }

	const ChartPixmapParams& params() const { return m_params; }
	//const QList<ViewWaveInfo>& chartWaveInfo() const { return m_waves; }
	int height() const { return m_pixmap.height(); }
	int width() const { return m_pixmap.width(); }
	const QRect& borderRect() const { return p.rcBorder; }

	/// First sample index which fits onto the pixmap
	int firstSample() const;
	/// Middle sample index on pixmap
	int centerSample() const;
	/// Last sample index which fits onto the pixmap
	int lastSample() const;

	/// Calculate the size of the chart given the available area and number of columns
	QSize sizeForAvailableArea(const QSize& size, int nCols) const;
	
	void draw(const ChartPixmapParams& params);
	void draw(QPaintDevice* device, const ChartPixmapParams& params);
	void draw(QPainter& painter, const ChartPixmapParams& params);

	void clearRenderData();

	int xToSampleOffset(int x) const;
	/// Returns the sample index at 'x' for 'wave'.
	/// If more than one sample is drawn at x, then it returns the center
	int xToCenterSample(const WaveInfo* wave, int x) const;
	int sampleOffsetToX(int nSampleOffset) const;
	ViewWaveInfo* waveToViewWaveInfo(const WaveInfo* wave) const;
	double yToValue(const ViewWaveInfo* cwi, int y) const;
	int valueToY(const ViewWaveInfo* cwi, double n) const;
	int widthToSampleCount(int nWidth) const;
	void fillChartPointInfo(const QPoint& pt, ChartPointInfo* info) const;

	WaveInfo* waveOfPeaks() const;

private:
	enum ChartColor
	{
		ChartColor_Border,
		ChartColor_Grid,
		ChartColor_Wave,
		/// Averaged waveform
		ChartColor_WaveAve,
		/// Color for recording waves
		ChartColor_WaveRec,
	};

	struct Params
	{
		/// Width/height of a division in pixels
		int nDivisionSize;
		double nXToIndexFactor;
		QRect rcBorder;
		int nCols;

		Params()
		{
		};

		Params(const ChartPixmapParams& user);
	};

	struct ChartWaveInfo
	{
		QPointer<ViewWaveInfo> vwi;
		/// List of y-coordinates where pixels were drawn for each wave
		RenderData* render;
		/// List of y-coordinates for standard deviation (only used on averaged waves)
		RenderData* renderStd;
		/// List of rects where wave names are drawn
		QRect rcName;
		/// List of rects where user can click on ADD to choose a peak
		//QList< QPair<QRect, int> > arcPeaksPossible;
		/// List of rects where user can click to remove a chosen peak
		QList< QPair<QRect, int> > arcPeaksChosen;

		ChartWaveInfo()
			: render(NULL), renderStd(NULL)
		{
		}

		~ChartWaveInfo()
		{
			clearRenderData();
		}

		void clearRenderData();
	};

private:
	void paint(QPainter& painter, const ChartPixmapParams& params);
	void updateDrawingParameters();
	void createWaveList();

	QColor color(ChartColor color);
	void drawGrid(QPainter& painter);
	void drawWaveform(QPainter& painter, ChartWaveInfo* vwi);
	void drawWaveformRough(QPainter& painter, const ChartWaveInfo* vwi);
	void drawWaveformSmooth(QPainter& painter, const ChartWaveInfo* vwi);
	void drawWaveformStd(QPainter& painter, const ChartWaveInfo* cwi);
	void drawWaveformDigital(QPainter& painter, ChartWaveInfo* vwi);
	void drawWaveName(QPainter& painter, ChartWaveInfo* vwi, bool bHilight);
	void drawPossiblePeaks(QPainter& painter, ChartWaveInfo* vwi);
	void drawMarkerTimes(QPainter& painter, ChartWaveInfo* vwi);
	void drawAreaLines(QPainter& painter, ChartWaveInfo* vwi);
	void drawAreaHandles(QPainter& painter, ChartWaveInfo* vwi);
	QRect rectOfAreaHandle(ViewWaveInfo* vwi, int didx) const;
	int calcPercentVisible(ChartWaveInfo* cwi);

	/// Convert a screen coordinate to box coordinates
	QPoint screenToBoxPoint(const QPoint& pt) const;

private:
	QPixmap m_pixmap;
	/// Drawing parameters specified by caller
	ChartPixmapParams m_params;
	/// Calculated drawing parameters, based on m_params
	Params p;
	/// Convenience pointer to our data structure (supplied by m_params)
	QPointer<EadFile> m_file;
	/// Convenience pointer to structure with wave and drawing information passed by the user (supplied by m_params)
	QPointer<ViewInfo> m_view;
	/// Wave which we are showing the peaks for
	QPointer<WaveInfo> m_wavePeaks;
	/// List of ChartWaveInfo objects containing drawing information for this chart
	QList<ChartWaveInfo*> m_cwis;
	/// List of rects where user can click on ADD to choose a peak
	QList< QPair<QRect, int> > m_arcPeaksPossible;
	/// List of rects where user can click to remove a chosen peak
	//QList< QPair<QRect, int> > m_arcPeaksChosen;
};

#endif
