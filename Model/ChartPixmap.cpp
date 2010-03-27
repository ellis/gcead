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

#include "ChartPixmap.h"

#include <QtDebug>
#include <QPainter>

#include "RecInfo.h"
#include "RenderData.h"
#include "ViewInfo.h"
#include "WaveInfo.h"
#include "PublisherSettings.h"

#include <Check.h>
#include <Globals.h>
#include <Utils.h>


ChartPixmap::ChartPixmap()
{
}

ChartPixmap::~ChartPixmap()
{
	qDeleteAll(m_cwis);
}

void ChartPixmap::ChartWaveInfo::clearRenderData()
{
	delete render;
	render = NULL;
	delete renderStd;
	renderStd = NULL;
}

ChartPixmap::Params::Params(const ChartPixmapParams& user)
{
	int yAxisTop = 0;
	int nBorderBottom = 0;
	int nBorderRight = 0;

	nCols = user.nCols;
	if (nCols <= 0)
		nCols = CHART_ROWS;

	// Calculate the maximum division sizes in the horizontal and vertical directions
	int nWidthAvailable = user.size.width() - (nBorderRight * 2);
	int nHeightAvailable = user.size.height() - yAxisTop - nBorderBottom;
	int nSizeC = nWidthAvailable / nCols;
	int nSizeR = nHeightAvailable / CHART_ROWS;

	// Select the smaller of the two maximum division sizes
	if (nSizeC < nSizeR)
		nDivisionSize = nSizeC;
	else
		nDivisionSize = nSizeR;

	// If the user doesn't specify number of cols, then use as many as possible.
	if (user.nCols <= 0 && nDivisionSize > 0)
		nCols = nWidthAvailable / nDivisionSize;

	int xAxisRight = nBorderRight + (nDivisionSize * nCols);
	int xAxisLeft = nBorderRight;
	int yAxisBottom = yAxisTop + nDivisionSize * CHART_ROWS;

	rcBorder = QRect(xAxisLeft, yAxisTop, xAxisRight - xAxisLeft + 1, yAxisBottom - yAxisTop + 1);

	if (rcBorder.width() > 0)
		nXToIndexFactor = EAD_SAMPLES_PER_SECOND * user.nSecondsPerDivision * nCols / double(rcBorder.width());
	else
		nXToIndexFactor = 0;
}

WaveInfo* ChartPixmap::waveOfPeaks() const
{
	return m_wavePeaks;
}

QSize ChartPixmap::sizeForAvailableArea(const QSize& size, int nCols) const
{
	ChartPixmapParams params = m_params;
	params.size = size;
	params.nCols = nCols;
	Params p(params);
	return p.rcBorder.size();
}

void ChartPixmap::draw(const ChartPixmapParams& params)
{
	QSize size = sizeForAvailableArea(params.size, params.nCols);
	if (m_pixmap.size() != size)
		m_pixmap = QPixmap(size);

	m_pixmap.fill(Qt::white);

	QPainter painter(&m_pixmap);
	paint(painter, params);
}

void ChartPixmap::draw(QPaintDevice* device, const ChartPixmapParams& params)
{
	m_pixmap = QPixmap();

	QPainter painter(device);
	paint(painter, params);
}

void ChartPixmap::draw(QPainter& painter, const ChartPixmapParams& params)
{
	m_pixmap = QPixmap();
	paint(painter, params);
}

void ChartPixmap::paint(QPainter& painter, const ChartPixmapParams& params)
{
	m_params = params;

	updateDrawingParameters();

	m_wavePeaks = NULL;
	m_arcPeaksPossible.clear();

	// Draw the grid
	drawGrid(painter);

	if (m_file == NULL || m_view == NULL)
		return;

	//QTime timeStart = QTime::currentTime();

	// Get list of relevant waves
	QList<ViewWaveInfo*> vwis;
	foreach (ViewWaveInfo* vwi, m_view->allVwis())
	{
		if (vwi->wave() != NULL && vwi->isVisible() && vwi->wave()->display.size() > 0)
			vwis << vwi;
	}

	// Create new list of relevant ChartWaveInfos
	ChartWaveInfo* cwiHilight = NULL;
	QList<ChartWaveInfo*> cwisOld = m_cwis;
	m_cwis.clear();
	foreach (ViewWaveInfo* vwi, vwis)
	{
		ChartWaveInfo* cwiFound = NULL;
		for (int i = 0; i < cwisOld.count(); i++)
		{
			ChartWaveInfo* cwi = cwisOld[i];
			if (cwi->vwi == vwi)
			{
				// Clear the variables
				cwi->rcName = QRect();
				// Add/remove it for the new/old lists
				m_cwis << cwi;
				cwisOld.removeAt(i);
				cwiFound = cwi;
				break;
			}
		}
		
		if (cwiFound == NULL)
		{
			cwiFound = new ChartWaveInfo;
			cwiFound->vwi = vwi;
			m_cwis << cwiFound;
		}
		cwiFound->arcPeaksChosen.clear();

		if (cwiFound->vwi == m_params.vwiHilight)
			cwiHilight = cwiFound;
	}
	qDeleteAll(cwisOld);

	// If we have a hilight wave, put it at the end of the list so that it's painted on top of the others
	if (cwiHilight != NULL)
	{
		m_cwis.removeAll(cwiHilight);
		m_cwis << cwiHilight;
	}

	// Set clipping
	QRect rcClip(p.rcBorder.left() + 1, p.rcBorder.top() + 1, p.rcBorder.width() - 1, p.rcBorder.height() - 1);
	painter.setClipping(true);
	painter.setClipRect(rcClip);

	// Draw all waves
	foreach (ChartWaveInfo* cwi, m_cwis)
		drawWaveform(painter, cwi);
	
	// Draw wave names
	if (m_params.elements.testFlag(ChartElement_WaveNames) || m_params.elements.testFlag(ChartElement_WaveComments))
	{
		foreach (ChartWaveInfo* cwi, m_cwis)
			drawWaveName(painter, cwi, (cwi->vwi == m_params.vwiHilight));
	}

	painter.setClipping(false);

	// HACK: This doesn't really belong here
	// If requested, assure that a wave has least 10% visibility; if not, center it around division 5
	foreach (ChartWaveInfo* cwi, m_cwis)
	{
		ViewWaveInfo* vwi = cwi->vwi;

		// Make sure that the wave is visible; if not, make it visible
		if (vwi->bAssureVisibility)
		{
			vwi->bAssureVisibility = false;

			int nPercent = calcPercentVisible(cwi);
			if (nPercent < 10)
			{
				// Calculate the average
				RenderData* render = cwi->render;
				const MinMax* pixdata = render->pixels.constData();
				double nSum = 0;
				for (int i = 0; i < render->pixels.count(); i++)
				{
					nSum += (pixdata->yBot + pixdata->yTop) / 2;
				}
				double nAve = nSum / render->pixels.count();
				double nDivisions = nAve / vwi->voltsPerDivision();
				// Center the wave
				vwi->setDivisionOffset(5 - nDivisions);
			}
		}
	}
	//qDebug() << "paint T:" << timeStart.msecsTo(QTime::currentTime());
}

void ChartPixmap::updateDrawingParameters()
{
	p = Params(m_params);
	m_params.nCols = p.nCols;
	m_file = m_params.file;
	m_view = m_params.view;
}

ViewWaveInfo* ChartPixmap::waveToViewWaveInfo(const WaveInfo* wave) const
{
	foreach (ViewWaveInfo* vwi, m_view->allVwis())
	{
		if (vwi->wave() == wave)
			return vwi;
	}

	return NULL;
}

int ChartPixmap::xToSampleOffset(int x) const
{
	int iSample = int((x - p.rcBorder.left()) * p.nXToIndexFactor) + m_params.nSampleOffset;
	if (iSample < 0)
		iSample = 0;

	return iSample;
}

int ChartPixmap::xToCenterSample(const WaveInfo* wave, int x) const
{
	CHECK_PARAM_RETVAL(wave != NULL, 0);

	x -= p.rcBorder.left();
	double i0 = (x * p.nXToIndexFactor) + m_params.nSampleOffset - wave->shift();
	double i1 = ((x + 1) * p.nXToIndexFactor) + m_params.nSampleOffset - wave->shift();
	int i = (int) ((i0 + i1) / 2);
	return i;
}

int ChartPixmap::sampleOffsetToX(int nSampleOffset) const
{
	int x = int((nSampleOffset - m_params.nSampleOffset) / p.nXToIndexFactor) + p.rcBorder.left();
	return x;
}

double ChartPixmap::yToValue(const ViewWaveInfo* vwi, int y) const
{
	CHECK_PARAM_RETVAL(vwi != NULL, 0);

	double nDataToYFactor = p.nDivisionSize / vwi->voltsPerDivision();
	int yTop = p.rcBorder.top() + int(vwi->divisionOffset() * p.nDivisionSize);
	double n = (yTop - y) / nDataToYFactor;

	return n;
}

int ChartPixmap::valueToY(const ViewWaveInfo* vwi, double n) const
{
	CHECK_PARAM_RETVAL(vwi != NULL, 0);

	double nDataToYFactor = p.nDivisionSize / vwi->voltsPerDivision();
	int yTop = p.rcBorder.top() + int(vwi->divisionOffset() * p.nDivisionSize);
	int y = yTop - int(n * nDataToYFactor);

	return y;
}

int ChartPixmap::widthToSampleCount(int nWidth) const
{
	return nWidth * p.nXToIndexFactor;
}

QColor ChartPixmap::color(ChartColor color)
{
	QColor clr;

	// Normal screen colors
	if (m_params.task != EadTask_Publish)
	{
		switch (color)
		{
		case ChartColor_Border: clr = QColor(180, 180, 255); break;
		case ChartColor_Grid: clr = QColor(180, 180, 255, 200); break;
		case ChartColor_Wave: clr = Qt::black; break;
		case ChartColor_WaveAve: clr = QColor(0, 80, 0); break;
		case ChartColor_WaveRec: clr = Qt::red; break;
		}
	}
	// Publish in color
	else if (Globals->publisherSettings()->bPublishInColor)
	{
		switch (color)
		{
		case ChartColor_Border: clr = Globals->publisherSettings()->clrPublishBorder; break;
		case ChartColor_Grid: clr = Globals->publisherSettings()->clrPublishBorder; clr.setAlpha(200); break;
		case ChartColor_Wave: clr = Qt::black; break;
		case ChartColor_WaveAve: clr = Qt::black; break;
		case ChartColor_WaveRec: clr = Qt::red; break;
		}
	}
	// Publish in monochrome
	else
	{
		switch (color)
		{
		case ChartColor_Border: clr = Qt::black; break;
		case ChartColor_Grid: clr = QColor(127, 127, 127); break;
		case ChartColor_Wave: clr = Qt::black; break;
		case ChartColor_WaveAve: clr = Qt::black; break;
		case ChartColor_WaveRec: clr = Qt::black; break;
		}
	}

	return clr;
}

void ChartPixmap::drawGrid(QPainter& painter)
{
	// Draw border
	QPen pen(color(ChartColor_Border));
	//pen.setWidth(2);
	painter.setPen(pen);
	painter.drawLine(p.rcBorder.left(), p.rcBorder.top(), p.rcBorder.left(), p.rcBorder.bottom());
	painter.drawLine(p.rcBorder.right(), p.rcBorder.top(), p.rcBorder.right(), p.rcBorder.bottom());
	painter.drawLine(p.rcBorder.left(), p.rcBorder.top(), p.rcBorder.right(), p.rcBorder.top());
	painter.drawLine(p.rcBorder.left(), p.rcBorder.bottom(), p.rcBorder.right(), p.rcBorder.bottom());

	//clr = QColor(qMax(clr.red() * 12 / 10, 255), qMax(clr.green() * 12 / 10, 255), qMax(clr.blue() * 12 / 10);
	pen = QPen(color(ChartColor_Grid), 0, Qt::DotLine);
	painter.setPen(pen);

	if (m_params.elements.testFlag(ChartElement_Grid))
	{
		// Draw vertical grid lines
		for (int i = 1; i < p.nCols; i++)
		{
			int x = p.rcBorder.left() + i * p.nDivisionSize;
			painter.drawLine(x, p.rcBorder.top() + 1, x, p.rcBorder.bottom() - 1);
		}

		// Draw horizontal grid lines
		for (int i = 1; i < CHART_ROWS; i++)
		{
			int y = p.rcBorder.top() + i * p.nDivisionSize;
			painter.drawLine(p.rcBorder.left() + 1, y, p.rcBorder.right() -1, y);
		}
	}

	// Draw the timestamps
	if (m_params.elements.testFlag(ChartElement_AxisTime))
	{
		// Time of the right edge in seconds
		double nTimeLeft = double(m_params.nSampleOffset) / EAD_SAMPLES_PER_SECOND;
		double nTimeRight = double(m_params.nSampleOffset) / EAD_SAMPLES_PER_SECOND + m_params.nSecondsPerDivision * p.nCols;

		TimestampBase base;
		QString sTimeRight = timestampString(nTimeRight, &base);
		QString sTimeLeft = timestampString(nTimeLeft, base);

		painter.setPen(color(ChartColor_Border));
		QRect rc = p.rcBorder;
		rc.adjust(3, 1, -2, -1);
		painter.drawText(rc, Qt::AlignLeft | Qt::AlignBottom, sTimeLeft);
		painter.drawText(rc, Qt::AlignRight | Qt::AlignBottom, sTimeRight);
	}

	painter.setPen(Qt::black);
}

// TEST: test display of one data point over multiple pixels
void ChartPixmap::drawWaveform(QPainter& painter, ChartWaveInfo* cwi)
{
	CHECK_PARAM_RET(cwi != NULL);
	CHECK_PARAM_RET(cwi->vwi != NULL);
	CHECK_PARAM_RET(cwi->vwi->wave() != NULL && cwi->vwi->isVisible());

	ViewWaveInfo* vwi = cwi->vwi;
	const WaveInfo* wave = vwi->wave();

	// NOTE: Make sure we have "render" allocated before returning, because other functions currently assume that it's not NULL -- ellis, 2008-06-26
	if (cwi->render == NULL)
	{
		cwi->render = new RenderData();
		// If this is an averaged wave
		if (cwi->vwi->wave()->recId() == 0)
			cwi->renderStd = new RenderData();
	}
	RenderData* render = cwi->render;

	if (wave->display.size() == 0)
		return;
	
	//
	// Setup general parameters to paint rendered data
	//

	int xWidth = p.rcBorder.right() - p.rcBorder.left();

	//
	// Render data to min/max vector
	//

	RenderData renderCheck;
	renderCheck.setup(vwi, m_params.nSampleOffset, xWidth, p.nXToIndexFactor);
	
	if (render->nSamplesPerPixel != renderCheck.nSamplesPerPixel ||
		render->didxFirst > renderCheck.didxFirst ||
		render->didxLast < renderCheck.didxLast ||
		render->nPixels != renderCheck.nPixels)
	{
		render->render(vwi, m_params.nSampleOffset, xWidth, p.nXToIndexFactor);
		if (cwi->renderStd != NULL)
			cwi->renderStd->renderStd(vwi, m_params.nSampleOffset, xWidth, p.nXToIndexFactor);
	}

	//
	// Draw wave
	//

	// Is this the FID for which we should display peaks?
	bool bShowMarkers = (m_params.task == EadTask_Publish && m_params.elements.testFlag(ChartElement_Markers)) ||
		(m_params.task == EadTask_Review && m_params.peakMode != EadMarkerMode_Hide);
	bool bPeakFid = false;
	EadMarkerMode peakMode = EadMarkerMode_Hide;
	if (bShowMarkers && wave->type == WaveType_FID)
	{
		if (m_params.task == EadTask_Publish)
		{
			bPeakFid = true;
			peakMode = EadMarkerMode_Verified;
		}
		else if (wave->recId() == m_params.nPeakModeRecId)
		{
			bPeakFid = true;
			peakMode = m_params.peakMode;
		}
	}

	// Mark possible peaks of averaged FID waves
	if (peakMode == EadMarkerMode_Edit)
		drawAreaHandles(painter, cwi);

	if (wave->type == WaveType_Digital)
		drawWaveformDigital(painter, cwi);
	else if (m_params.task == EadTask_Publish || render->nSamplesPerPixel < 1)
		drawWaveformSmooth(painter, cwi);
	else
		drawWaveformRough(painter, cwi);

	// Mark possible peaks of averaged FID waves
	if (peakMode == EadMarkerMode_Detected || peakMode == EadMarkerMode_Edit)
		drawPossiblePeaks(painter, cwi);

	if (bShowMarkers)
	{
		drawMarkerTimes(painter, cwi);
		if (wave->type == WaveType_FID)
			drawAreaLines(painter, cwi);
	}
}

void ChartPixmap::drawWaveformRough(QPainter& painter, const ChartWaveInfo* cwi)
{
	const ViewWaveInfo* vwi = cwi->vwi;
	const WaveInfo* wave = vwi->wave();
	const RenderData* render = cwi->render;

	QColor clrWave = color((wave->recId() >= 1) ? ChartColor_Wave : ChartColor_WaveAve);
	if (m_file->newRec() != NULL && wave->recId() == m_file->newRec()->id())
		clrWave = color(ChartColor_WaveRec);

	// If this is an averaged wave
	if (wave->recId() == 0)
		drawWaveformStd(painter, cwi);

	int yTop = p.rcBorder.top() + int(vwi->divisionOffset() * p.nDivisionSize);
	double nDataToYFactor = p.nDivisionSize / vwi->voltsPerDivision();

	const MinMax* pixdata = render->pixels.constData();

	painter.setPen(clrWave);

	double n = pixdata->yBot;
	int y = yTop - int(n * nDataToYFactor);
	int y0Prev = y;
	int y1Prev = y;
	
	int x = p.rcBorder.left() + render->xOffset;
	int xEnd = x + render->nPixels;
	while (x < xEnd)
	{
		int y0 = yTop - int(pixdata->yBot * nDataToYFactor); // Lower side (greater y-coordinate)
		int y1 = yTop - int(pixdata->yTop * nDataToYFactor); // Upper side (lesser y-coordinate)
		// If this data line is completely below the previous minimum
		if (y1 > y0Prev)
		{
			y1 = y0Prev + 1;
		}
		// If this data line is completely above the previous maximum
		else if (y0 < y1Prev)
		{
			y0 = y1Prev - 1;
		}

		if (y0 >= p.rcBorder.top() && y1 <= p.rcBorder.bottom())
		{
			y0 = qMin(y0, p.rcBorder.bottom());
			y1 = qMax(y1, p.rcBorder.top());

			if (y0 != y1)
				painter.drawLine(x, y0, x, y1);
			else
				painter.drawPoint(x, y0);
		}

		y0Prev = y0;
		y1Prev = y1;

		x++;
		pixdata++;
	}
}

void ChartPixmap::drawWaveformSmooth(QPainter& painter, const ChartWaveInfo* cwi)
{
	const ViewWaveInfo* vwi = cwi->vwi;
	const WaveInfo* wave = vwi->wave();
	const RenderData* render = cwi->render;
	int yTop = p.rcBorder.top() + int(vwi->divisionOffset() * p.nDivisionSize);
	double nDataToYFactor = p.nDivisionSize / vwi->voltsPerDivision();

	int xStart = p.rcBorder.left() + render->xOffset;
	int xEnd = xStart + render->nPixels;

	// NOTE: antialiasing is currently VERY expensive.  Way too expensive (.5s to draw small graphs).
	// So this check is here in case this function was only called because nSamplesPerPixel < 1.
	//  -- ellis, 2008-08-29
	if (m_params.task == EadTask_Publish)
		painter.setRenderHint(QPainter::Antialiasing, true);

	if (wave->recId() == 0)
		drawWaveformStd(painter, cwi);

	QColor clrWave = color((wave->recId() >= 1) ? ChartColor_Wave : ChartColor_WaveAve);
	if (m_file->newRec() != NULL && wave->recId() == m_file->newRec()->id())
		clrWave = color(ChartColor_WaveRec);

	QPoint* line = new QPoint[render->nPixels];
	QPoint* pt = line;
	const MinMax* pixdata = render->pixels.constData();
	int x = xStart;
	while (x < xEnd)
	{
		int y0 = yTop - int(pixdata->yBot * nDataToYFactor); // Lower side (greater y-coordinate)
		int y1 = yTop - int(pixdata->yTop * nDataToYFactor); // Upper side (lesser y-coordinate)
		*pt++ = QPoint(x, (y0 + y1) / 2);
		x++;
		pixdata++;
	}

	painter.setPen(clrWave);
	painter.drawPolyline(line, render->nPixels);

	if (m_params.task == EadTask_Publish)
		painter.setRenderHint(QPainter::Antialiasing, false);

	delete[] line;
}

void ChartPixmap::drawWaveformStd(QPainter& painter, const ChartWaveInfo* cwi)
{
	if (!m_params.elements.testFlag(ChartElement_StdDev))
		return;

	const ViewWaveInfo* vwi = cwi->vwi;
	const RenderData* render = cwi->render;
	int yTop = p.rcBorder.top() + int(vwi->divisionOffset() * p.nDivisionSize);
	double nDataToYFactor = p.nDivisionSize / vwi->voltsPerDivision();

	int xStart = p.rcBorder.left() + render->xOffset;
	int xEnd = xStart + render->nPixels;

	QPoint* line = new QPoint[render->nPixels];
	QPoint* pt = NULL;
	painter.setPen(QColor(255, 0, 0, 100));

	const MinMax* pixdata;
	int x;

	pixdata = cwi->renderStd->pixels.constData();
	x = xStart;
	pt = line;
	while (x < xEnd)
	{
		int y0 = yTop - int(pixdata->yBot * nDataToYFactor); // Lower side (greater y-coordinate)
		*pt++ = QPoint(x, y0);
		x++;
		pixdata++;
	}
	painter.drawPolyline(line, render->nPixels);

	pixdata = cwi->renderStd->pixels.constData();
	x = xStart;
	pt = line;
	while (x < xEnd)
	{
		int y1 = yTop - int(pixdata->yTop * nDataToYFactor); // Upper side (lesser y-coordinate)
		*pt++ = QPoint(x, y1);
		x++;
		pixdata++;
	}
	painter.drawPolyline(line, render->nPixels);

	delete[] line;
}

void ChartPixmap::drawWaveformDigital(QPainter& painter, ChartWaveInfo* cwi)
{
	ViewWaveInfo* vwi = cwi->vwi;
	const WaveInfo* wave = vwi->wave();
	RenderData* render = cwi->render;
	int yTop = p.rcBorder.top() + int(vwi->divisionOffset() * p.nDivisionSize);
	int xWidth = p.rcBorder.right() - p.rcBorder.left();
	double nDataToYFactor = p.nDivisionSize / vwi->voltsPerDivision();

	const MinMax* pixdata = render->pixels.constData();

	QColor clrWave = color((wave->recId() >= 1) ? ChartColor_Wave : ChartColor_WaveAve);
	if (m_file->newRec() != NULL && wave->recId() == m_file->newRec()->id())
		clrWave = color(ChartColor_WaveRec);
	painter.setPen(clrWave);

	QPen penLow(clrWave, 0, Qt::DashLine);
	QPen penHigh(clrWave);

	QVector<QPoint> line;
	line.reserve(xWidth);
	
	int x = p.rcBorder.left() + render->xOffset;
	int xEnd = x + render->nPixels;
	int yLow = (int) (yTop + nDataToYFactor / 2);
	int yHigh = (int) (yTop - nDataToYFactor / 2);

	// Draw rectangles on top of base line for HIGH states
	while (x < xEnd)
	{
		// If pixdata->yTop == 0, then this loop will never exit
		CHECK_ASSERT_RET(pixdata->yTop != 0);

		int xLowStart = x;

		// Skip to next HIGH-state
		while (pixdata->yTop < 0 && x < xEnd)
		{
			x++;
			pixdata++;
		}

		int xHighStart = x;

		// Draw the LOW dashed line
		if (xHighStart > xLowStart)
		{
			painter.setPen(penLow);
			painter.drawLine(xLowStart, yLow, xHighStart, yLow);
		}

		// Skip to next LOW-state
		while (pixdata->yTop > 0 && x < xEnd)
		{
			x++;
			pixdata++;
		}

		int xHighEnd = x;

		if (xHighEnd > xHighStart)
		{
			QVector<QPoint> line(4);
			line[0] = QPoint(xHighStart, yLow);
			line[1] = QPoint(xHighStart, yHigh);
			line[2] = QPoint(xHighEnd, yHigh);
			line[3] = QPoint(xHighEnd, yLow);
			painter.setPen(penHigh);
			painter.drawPolyline(line.constData(), line.size());
		}
	}
}

void ChartPixmap::drawWaveName(QPainter& painter, ChartWaveInfo* cwi, bool bHilight)
{
	ViewWaveInfo* vwi = cwi->vwi;
	int yTop = p.rcBorder.top() + int(vwi->divisionOffset() * p.nDivisionSize);
	double nDataToYFactor = p.nDivisionSize / vwi->voltsPerDivision();
	const WaveInfo* wave = vwi->wave();
	RenderData* render = cwi->render;
	// Can't draw the name if there are no pixels to draw
	if (render->nPixels == 0)
		return;

	// "Name: Comment" or just "Name" or just "Comment"
	QStringList parts;
	if (m_params.elements.testFlag(ChartElement_WaveNames))
		parts << vwi->wave()->sName;
	if (m_params.elements.testFlag(ChartElement_WaveComments) && !vwi->wave()->sComment.isEmpty())
		parts << vwi->wave()->sComment;
	QString sLabel = parts.join(": ");
	
	int yName = yTop - int(render->pixels[0].yTop * nDataToYFactor); // Upper side (lesser y-coordinate)
	yName -= 20 + painter.fontMetrics().ascent();
	int xName = p.rcBorder.left() + 10;
	QRect rc = painter.boundingRect(xName, yName, 100, 100, 0, sLabel);

	// REFACTOR: this is duplicate code (see drawWaveform_rough())
	QColor clrWave = color((wave->recId() >= 1) ? ChartColor_Wave : ChartColor_WaveAve);
	if (m_file->newRec() != NULL && wave->recId() == m_file->newRec()->id())
		clrWave = color(ChartColor_WaveRec);
	painter.setPen(clrWave);

	if (bHilight)
	{
		QBrush brushOld = painter.brush();
		rc.adjust(-5, -5, 5, 5);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(QColor(255, 255, 255, 200));
		painter.drawEllipse(rc);
		painter.setRenderHint(QPainter::Antialiasing, false);
		painter.setBrush(brushOld);
	}
	else
	{
		painter.fillRect(rc, QColor(255, 255, 255, 127));
	}

	painter.drawText(rc, Qt::AlignCenter, sLabel);

	cwi->rcName = rc;
}

void ChartPixmap::drawPossiblePeaks(QPainter& painter, ChartWaveInfo* cwi)
{
	CHECK_PRECOND_RET(m_wavePeaks == NULL);

	ViewWaveInfo* vwi = cwi->vwi;

	painter.setPen(Qt::red);
	for (int iPeak = 0; iPeak < vwi->wave()->peaks0.size(); iPeak++)
	{
		const WavePeakInfo& info = vwi->wave()->peaks0.at(iPeak);

		if (!info.bEnabled)
			continue;

		int i = info.middle.i + vwi->shift();
		int x = sampleOffsetToX(i);
		int y = valueToY(vwi, info.middle.n);
		painter.drawLine(x, y, x, y - 10);

		// Decide what text to draw on top of the peak
		QString sText;
		QRect rcText;
		if (m_params.peakMode == EadMarkerMode_Edit)
			sText = QObject::tr("Add");
		else
		{
			double nSeconds = double(i) / EAD_SAMPLES_PER_SECOND;
			sText = timestampString(nSeconds);
		}

		// Draw the text
		QRect rc(x - 100, y - 112, 201, 100);
		painter.drawText(rc, Qt::AlignHCenter | Qt::AlignBottom, sText, &rcText);
		rcText.adjust(-3, -3, 3, 3);

		// Save pointer to the wave with peaks
		m_wavePeaks = vwi->waveInfo();

		// Save rectangle where text was drawn
		m_arcPeaksPossible << QPair<QRect, int>(rcText, info.middle.i);
	}
}

void ChartPixmap::drawMarkerTimes(QPainter& painter, ChartWaveInfo* cwi)
{
	ViewWaveInfo* vwi = cwi->vwi;

	painter.setPen(Qt::blue);
	for (int iPeak = 0; iPeak < vwi->wave()->peaksChosen.size(); iPeak++)
	{
		const WavePeakChosenInfo& info = vwi->wave()->peaksChosen.at(iPeak);

		int i = info.didxMiddle + vwi->shift();
		int x = sampleOffsetToX(i);
		int y = valueToY(vwi, vwi->wave()->display[info.didxMiddle]);
		painter.drawLine(x, y, x, y - 10);

		double nSeconds = double(i) / EAD_SAMPLES_PER_SECOND;
		QString sTime = timestampString(nSeconds);
		QRect rc(x - 100, y - 112, 201, 100);
		QRect rcTime;
		painter.drawText(rc, Qt::AlignHCenter | Qt::AlignBottom, sTime, &rcTime);
		rcTime.adjust(-3, -3, 3, 3);

		cwi->arcPeaksChosen << QPair<QRect, int>(rcTime, iPeak);
		cwi->arcPeaksChosen << QPair<QRect, int>(QRect(x - 2, y - 10, 5, 11), iPeak);
	}
}

void ChartPixmap::drawAreaLines(QPainter& painter, ChartWaveInfo* cwi)
{
	ViewWaveInfo* vwi = cwi->vwi;
	QPen penOld = painter.pen();
	painter.setPen(Qt::blue);

	const double* data = vwi->wave()->display.constData();

	// Draw stuff
	foreach (WavePeakChosenInfo peak, vwi->wave()->peaksChosen)
	{
		int i0 = peak.didxLeft + vwi->shift();
		int x0 = sampleOffsetToX(i0);
		int y0 = valueToY(vwi, data[peak.didxLeft]);

		int i2 = peak.didxRight + vwi->shift();
		int x2 = sampleOffsetToX(i2);
		int y2 = valueToY(vwi, data[peak.didxRight]);

		QColor clr(0, 50, 255);
		painter.setPen(clr);
		painter.drawLine(x0, y0, x2, y2);

		// Draw area percentage
		int tidx1 = peak.didxMiddle + vwi->shift();
		int x1 = sampleOffsetToX(tidx1);
		int y1 = qMax(y0, y2) + 5;
		int nPercent = int(peak.nPercent * 100 + 0.5);
		QString s = QObject::tr("%0%").arg(nPercent);
		QRect rc(x1 - 100, y1, 201, 100);
		painter.drawText(rc, Qt::AlignHCenter | Qt::AlignTop, s);
	}

	painter.setPen(penOld);
}

void ChartPixmap::drawAreaHandles(QPainter& painter, ChartWaveInfo* cwi)
{
	ViewWaveInfo* vwi = cwi->vwi;

	QPen penOld = painter.pen();
	painter.setPen(QColor(0, 50, 255));
	QColor clr(0, 50, 255, 80);

	// Draw stuff
	foreach (WavePeakChosenInfo peak, vwi->wave()->peaksChosen)
	{
		QRect rc;
		
		rc = rectOfAreaHandle(vwi, peak.didxLeft);
		painter.drawRect(rc);
		painter.fillRect(rc, clr);

		rc = rectOfAreaHandle(vwi, peak.didxRight);
		painter.drawRect(rc);
		painter.fillRect(rc, clr);
	}

	painter.setPen(penOld);
}

QRect ChartPixmap::rectOfAreaHandle(ViewWaveInfo* vwi, int didx) const
{
	double n = vwi->wave()->display.at(didx);
	int i = didx + vwi->shift();
	int x = sampleOffsetToX(i);
	int y = valueToY(vwi, n);

	QRect rc(x - 5, y - 5, 11, 11);
	return rc;
}


int ChartPixmap::calcPercentVisible(ChartWaveInfo* cwi)
{
	// REFACTOR: This is basically just COPIED from drawWaveformRough()

	ViewWaveInfo* vwi = cwi->vwi;
	RenderData* render = cwi->render;

	int yTop = p.rcBorder.top() + int(vwi->divisionOffset() * p.nDivisionSize);
	double nDataToYFactor = p.nDivisionSize / vwi->voltsPerDivision();

	const MinMax* pixdata = render->pixels.constData();

	// Count the number of visible pixels
	int x = p.rcBorder.left() + render->xOffset;
	int xEnd = x + render->nPixels;
	int nVisible = 0;
	while (x < xEnd)
	{
		double nAve = (pixdata->yBot + pixdata->yTop) / 2;
		int y = yTop - int(nAve * nDataToYFactor);

		if (y > p.rcBorder.top() && y < p.rcBorder.bottom())
			nVisible++;

		x++;
		pixdata++;
	}

	int nPercent = int(double(nVisible * 100) / render->nPixels + 0.5);
	return nPercent;
}

void ChartPixmap::clearRenderData()
{
	foreach (ChartWaveInfo* cwi, m_cwis)
		cwi->clearRenderData();
}

QPoint ChartPixmap::screenToBoxPoint(const QPoint& pt) const
{
	int x = pt.x() - p.rcBorder.left();
	int y = pt.y() - p.rcBorder.top();
	if (x <= p.rcBorder.right() && y <= p.rcBorder.bottom())
	{
		x /= 10;
		y /= 10;
		return QPoint(x, y);
	}

	return QPoint();
}

int ChartPixmap::firstSample() const
{
	return m_params.nSampleOffset;
}

int ChartPixmap::centerSample() const
{
	return m_params.nSampleOffset + int(m_params.nSecondsPerDivision * EAD_SAMPLES_PER_SECOND * p.nCols / 2) - 1;
}

int ChartPixmap::lastSample() const
{
	return m_params.nSampleOffset + int(m_params.nSecondsPerDivision * EAD_SAMPLES_PER_SECOND * p.nCols) - 1;
}

void ChartPixmap::fillChartPointInfo(const QPoint& ptPixmap, ChartPointInfo* info) const
{
	CHECK_PRECOND_RET(info != NULL);

	info->vwi = NULL;
	info->didxPossiblePeak = -1;
	info->iChosenPeak = -1;
	info->iLeftAreaHandle = -1;
	info->iRightAreaHandle = -1;

	int xCenter = ptPixmap.x() - p.rcBorder.left();
	int yCenter = ptPixmap.y() - p.rcBorder.top();

	if (xCenter < 0 || yCenter < 0 || ptPixmap.x() > p.rcBorder.right() || ptPixmap.y() > p.rcBorder.bottom())
		return;

	//bool bPeaks = (m_params.task == EadTask_Review && m_params.peakMode == EadMarkerMode_Edit);

	//if (bPeaks)
	{
		// Check whether mouse is over a CHOSEN peak label
		foreach (ChartWaveInfo* cwi, m_cwis)
		{
			typedef QPair<QRect, int> Pair;
			foreach (const Pair& pair, cwi->arcPeaksChosen)
			//for (int iRect = 0; iRect < cwi->arcPeaksChosen.size(); iRect++)
			{
				//QRect rc = cwi->arcPeaksChosen[iRect].first;
				const QRect& rc = pair.first;
				if (rc.contains(ptPixmap))
				{
					//int didx = cwi->arcPeaksChosen[iRect].second;
					//info->iChosenPeak = cwi->vwi->wave()->indexOfChosenPeakAtDidx(didx);
					info->iChosenPeak = pair.second;
					info->vwi = cwi->vwi;
					return;
				}
			}
		}

		// Check whether mouse is over a POSSIBLE peak label
		for (int iRect = 0; iRect < m_arcPeaksPossible.size(); iRect++)
		{
			QRect rc = m_arcPeaksPossible[iRect].first;
			if (rc.contains(ptPixmap))
			{
				info->didxPossiblePeak = m_arcPeaksPossible[iRect].second;
				info->vwi = waveToViewWaveInfo(m_wavePeaks);
				return;
			}
		}
	}

	// Check whether mouse is over a name
	for (int iCwi = m_cwis.count() - 1; iCwi >= 0; iCwi--)
	{
		ChartWaveInfo* cwi = m_cwis[iCwi];
		ViewWaveInfo* vwi = cwi->vwi;
		if (vwi == NULL || vwi->wave() == NULL)
			continue;
		
		if (cwi->rcName.contains(ptPixmap))
		{
			info->vwi = vwi;
			return;
		}
	}

	// Check whether mouse is over a wave
	int x0 = qMax(xCenter - 5, 0);
	int x1 = qMin(xCenter + 5, p.rcBorder.width() - 2);
	double nDistMin = 1000;
	ViewWaveInfo* vwiWave = NULL;
	for (int iCwi = m_cwis.count() - 1; iCwi >= 0; iCwi--)
	{
		ChartWaveInfo* cwi = m_cwis[iCwi];
		ViewWaveInfo* vwi = cwi->vwi;
		if (vwi == NULL || vwi->wave() == NULL || cwi->render == NULL)
			continue;
		RenderData* render = cwi->render;

		int yTop = p.rcBorder.top() + int(vwi->divisionOffset() * p.nDivisionSize);
		double nDataToYFactor = p.nDivisionSize / vwi->voltsPerDivision();

		const MinMax* minmax = render->pixels.constData();

		int i0 = qMax(x0 - render->xOffset, 0);
		int i1 = qMin(x1 - render->xOffset, render->nPixels - 1);
		minmax += i0;

		int nClosePoints = 0;
		double nDist = 0;
		for (int i = i0; i <= i1; i++)
		{
			int yMax = yTop - int(minmax->yTop * nDataToYFactor);
			int yMin = yTop - int(minmax->yBot * nDataToYFactor);
			
			// Find distance from wave at this 'x'
			int dy = 1000;
			if (yCenter >= yMax && yCenter <= yMin)
				dy = 0;
			else
			{
				int dyTop = qAbs(yMax - yCenter);
				int dyBot = qAbs(yMin - yCenter);
				dy = qMin(dyTop, dyBot);
			}

			// If we're close enough
			if (dy <= 5)
			{
				nClosePoints++;
				nDist += dy;
			}

			minmax++;
		}

		if (nClosePoints > 0)
		{
			nDist /= nClosePoints;
			if (nDist < nDistMin)
			{
				vwiWave = vwi;
				nDistMin = nDist;
			}
		}
	}

	if (vwiWave != NULL)
	{
		info->vwi = vwiWave;

		// Check whether we're over an area handle
		bool bPeakEdit = (m_params.task == EadTask_Review && m_params.peakMode == EadMarkerMode_Edit);
		if (bPeakEdit)
		{
			const WaveInfo* wave = vwiWave->wave();
			for (int iPeak = 0; iPeak < wave->peaksChosen.size(); iPeak++)
			{
				const WavePeakChosenInfo& peak = wave->peaksChosen[iPeak];
				
				if (rectOfAreaHandle(vwiWave, peak.didxLeft).contains(ptPixmap))
				{
					info->iLeftAreaHandle = iPeak;
					break;
				}
				else if (rectOfAreaHandle(vwiWave, peak.didxRight).contains(ptPixmap))
				{
					info->iRightAreaHandle = iPeak;
					break;
				}
			}
		}
	}
}
