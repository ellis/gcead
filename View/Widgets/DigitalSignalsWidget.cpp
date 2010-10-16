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

#include "DigitalSignalsWidget.h"

#include <QtDebug>
#include <QPainter>

#include "EadEnums.h"


/// Number of digital channels
#define CHANNELS 2


DigitalSignalsWidget::DigitalSignalsWidget(QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_OpaquePaintEvent);
	setMinimumHeight(50);
	setMaximumHeight(100);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	clear();
}

void DigitalSignalsWidget::clear()
{
	m_data.clear();
	m_iEnd = 0;
	update();
}

void DigitalSignalsWidget::setSampleCount(int nSamples)
{
	m_nSamples = nSamples;
	//m_points.reserve(m_nSamples);
	//m_points.resize(m_nSamples);
	clear();
}

void DigitalSignalsWidget::addSample(uchar n)
{
	if (m_data.size() < m_nSamples)
		m_data << n;
	else
		m_data[m_iEnd] = n;

	m_iEnd++;
	if (m_iEnd >= m_nSamples)
		m_iEnd = 0;

	update();
}

void DigitalSignalsWidget::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e);

	QPainter p(this);
	p.fillRect(rect(), Qt::white);

	// X-value for the end of the data ring
	double nSampleToXFactor = double(width()) / m_nSamples;
	double xEnd = m_iEnd * nSampleToXFactor;
	double xStart = xEnd + 20;
	int iStart = (int) (xStart / nSampleToXFactor);

	// blank area is in the middle, resulting in two line segments to draw
	QPen pen(Qt::red, 2);
	p.setPen(pen);
	if (xStart < width())
	{
		paintChannels(p, 0, m_iEnd);
		paintChannels(p, iStart, m_nSamples);
	}
	else
	{
		iStart %= m_nSamples;
		paintChannels(p, iStart, m_iEnd);
	}
}

void DigitalSignalsWidget::paintChannels(QPainter& p, int i0, int i1)
{
	i1 = qMin(i1, m_data.size());

	paintChannel(p, 0x01, 20, QColor(0x00, 0x80, 0x20), i0, i1);
	paintChannel(p, 0x02, 40, Qt::red, i0, i1);
}

void DigitalSignalsWidget::paintChannel(QPainter& p, uchar mask, int y, const QColor& clr, int i0, int i1)
{
	int xMax = width() - 1;
	int xPrev = int(xMax * double(i0) / (m_nSamples - 1)) + 1;
	for (int i = i0 + 1; i < i1; i++)
	{
		bool b = ((m_data[i] & mask) != 0);
		double nXPerc = double(i) / (m_nSamples - 1);
		int x = int(xMax * nXPerc) + 1;
		int nHeight = (b) ? 10 : 2;

		QRect rc(xPrev, y - nHeight + 1, x - xPrev + 1, nHeight);
		p.fillRect(rc, clr);

		xPrev = x;
	}
}
