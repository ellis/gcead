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

#include "SignalWidget.h"

#include <QPainter>

#include "EadEnums.h"


/// Number of seconds to hold and draw
#define SECONDS 5
/// Number of samples to hold and draw
#define SAMPLES 50
/// Number of samples to average together into one pixel
#define SUBSAMPLES (EAD_SAMPLES_PER_SECOND * SECONDS / SAMPLES);


SignalWidget::SignalWidget(QWidget* parent)
	: QWidget(parent)
{
	setMinimumHeight(50);
	setMaximumHeight(100);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	clear();
}

void SignalWidget::clear()
{
	m_nSubSampleSum = 0;
	m_nSubSamples = SUBSAMPLES;
	m_nMax = 0;
	m_nMin = 0;
	m_data.clear();
	update();
}

void SignalWidget::addSample(double n)
{
	m_nSubSampleSum += n;
	if (--m_nSubSamples > 0)
		return;

	m_nSubSamples = SUBSAMPLES;
	n = m_nSubSampleSum / SUBSAMPLES;
	m_nSubSampleSum = 0;

	m_data << n;
	while (m_data.size() > SAMPLES)
		m_data.removeFirst();

	m_nMin = m_nMax = m_data[0];
	foreach (double n, m_data)
	{
		if (n > m_nMax)
			m_nMax = n;
		else if (n < m_nMin)
			m_nMin = n;
	}

	update();
}

void SignalWidget::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e);

	QPainter p(this);

	QColor clrBorder = palette().window().color().darker(150);
	p.setPen(clrBorder);
	p.drawRect(0, 0, width() - 1, height() - 1);

	QPen penWave(Qt::red, 2);

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
	if (nMin > 0)
	{
		nMin = -(nMax * 2 / 30);
	}
	else if (nMax < 0)
	{
		nMax = -(nMin * 2 / 30);
	}
	nRange = nMax - nMin;

	int xMax = width() - 2 - 1; // "2" accounts for the borders
	int yMax = height() - 2 - 1;

	// Draw the 0-value axis
	int yZero = yMax - int(yMax * -nMin / nRange) + 1;
	p.setPen(Qt::blue);
	p.drawLine(1, yZero, width() - 2, yZero);

	p.setPen(penWave);
	int xPrev = 0, yPrev = 0;
	for (int i = 0; i < m_data.size(); i++)
	{
		double n = m_data[i];
		double nXPerc = double(i) / (SAMPLES - 1);
		double nYPerc = (n - nMin) / nRange;
		int x = int(xMax * nXPerc) + 1;
		int y = yMax - int(yMax * nYPerc) + 1;

		p.drawPoint(x, y);
		if (i > 0)
		{
			p.drawLine(xPrev, yPrev, x, y);
		}

		xPrev = x;
		yPrev = y;
	}
}
