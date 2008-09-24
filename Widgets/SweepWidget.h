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

#ifndef __SWEEPWIDGET_H
#define __SWEEPWIDGET_H

#include <QFrame>
#include <QList>
#include <QVector>


class SweepWidget : public QWidget
{
	Q_OBJECT
public:
	SweepWidget(QWidget* parent = NULL);

	/// Set the number of samples to be displayed
	void setSampleCount(int nSamples);
	/// Set to true for the range to automatically span the height of the data
	void setAutoRange(bool b);
	/// Set the range of values to display between -n and n
	void setRange(double n);

	/// Clear the graph's data
	/// This method calls update().
	void clear();
	/// Add the given samples to the graph.
	/// This method calls update().
	void addSamples(const QVector<double>& data);

// Overrides for QWidget
protected:
	//void resizeEvent(QResizeEvent* e);
	void paintEvent(QPaintEvent* e);

private:
	void updateMinMax();
	void calcDrawingParameters();
	void drawPoints(QPainter& p, int i0, int i1);

private:
	bool m_bAutoRange;
	QList<double> m_data;
	QVector<QPointF> m_points;
	int m_nSamples;
	double m_nRange;
	/// Minimum value in m_data
	double m_nMin;
	/// Maximum value in m_data
	double m_nMax;
	/// Index to the end of the data ring, where the next sample will be placed.
	int m_iEnd;

	// Drawing parameters
	double m_nSampleToXFactor;
	double m_nValueToYFactor;
	double m_yOffset;
};

#endif
