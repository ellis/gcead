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

#include "SweepWidget.h"

#include <QtDebug>
#include <QPainter>
#include <QTime>

#include <Check.h>


SweepWidget::SweepWidget(QWidget* parent)
	: QWidget(parent)
{
	m_bAutoRange = true;
	m_iEnd = 0;
	m_nRange = 1;
	m_nMin = 0;
	m_nMax = 0;
	setSampleCount(100);
}

void SweepWidget::clear()
{
	m_data.clear();
	m_iEnd = 0;
	m_nMin = 0;
	m_nMax = 0;
	update();
}

void SweepWidget::setSampleCount(int nSamples)
{
	m_nSamples = nSamples;
	m_points.reserve(m_nSamples);
	m_points.resize(m_nSamples);
	clear();
}

void SweepWidget::setAutoRange(bool b)
{
	m_bAutoRange = b;
	if (m_bAutoRange)
		updateMinMax();
	update();
}

void SweepWidget::setRange(double n)
{
	m_bAutoRange = false;
	m_nRange = n;
	update();
}

void SweepWidget::addSamples(const QVector<double>& data)
{
	for (int i = 0; i < data.size(); i++)
	{
		double n = data[i];
		if (m_data.size() < m_nSamples)
			m_data << n;
		else
			m_data[m_iEnd] = n;

		m_iEnd++;
		if (m_iEnd >= m_nSamples)
			m_iEnd = 0;
	}

	//qDebug() << "add:" << QTime::currentTime().msec() << data.size() << m_data.size();

	if (m_bAutoRange)
		updateMinMax();

	update();
}

void SweepWidget::updateMinMax()
{
	m_nMin = m_nMax = m_data[0];
	foreach (double n, m_data)
	{
		if (n > m_nMax)
			m_nMax = n;
		else if (n < m_nMin)
			m_nMin = n;
	}
}

void SweepWidget::calcDrawingParameters()
{
	m_nSampleToXFactor = double(width()) / m_nSamples;

	int nHeight = height();

	if (m_bAutoRange)
	{
		double nMin = m_nMin;
		double nMax = m_nMax;
		double nRange = nMax - nMin;
		if (nRange <= 0)
		{
			nRange = 2;
			nMax += 1;
			nMin -= 1;
		}
		// Always show the zero level
		if (nMax > 0)
			nMin = qMin(nMin, -nMax / 15);
		if (nMin < 0)
			nMax = qMax(nMax, -nMin / 15);
		nRange = nMax - nMin;

		m_nValueToYFactor = -nHeight / nRange;
		m_yOffset = -(nMax * m_nValueToYFactor);
	}
	else
	{
		m_nValueToYFactor = -nHeight / (2 * m_nRange);
		m_yOffset = double(nHeight) / 2;
	}
}

/*void SweepWidget::resizeEvent(QResizeEvent* e)
{
	update();
}*/

// Four cases to consider:
// - blank area is in the middle, resulting in two line segments to draw
// - blank area is at the very beginning, resulting in one line segment
// - blank area is at the very end, resulting in one line segment
// - blank area wraps around the end to the beginning, resulting in one line segment
void SweepWidget::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e);
	
	QPainter p(this);

	calcDrawingParameters();

	// Draw the time-axis
	p.setPen(Qt::blue);
	p.drawLine(QPointF(0, m_yOffset), QPointF(width(), m_yOffset));

	// X-value for the end of the data ring
	double xEnd = m_iEnd * m_nSampleToXFactor;
	double xStart = xEnd + 20;
	int iStart = (int) (xStart / m_nSampleToXFactor);

	// blank area is in the middle, resulting in two line segments to draw
	QPen pen(Qt::red, 2);
	p.setPen(pen);
	if (xStart < width())
	{
		drawPoints(p, 0, m_iEnd);
		drawPoints(p, iStart, m_nSamples);
	}
	else
	{
		iStart %= m_nSamples;
		drawPoints(p, iStart, m_iEnd);
	}
}

void SweepWidget::drawPoints(QPainter& p, int i0, int i1)
{
	i1 = qMin(i1, m_data.size());

	// Setup the QPoints array
	for (int i = i0; i < i1; i++)
	{
		double n = m_data[i];
		double x = i * m_nSampleToXFactor;
		double y = m_yOffset + n * m_nValueToYFactor;
		m_points[i] = QPointF(x, y);
	}
	// Draw the line
	if (i0 < i1)
		p.drawPolyline(m_points.data() + i0, i1 - i0);
}
