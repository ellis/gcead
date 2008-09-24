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

#ifndef __DIGITALSIGNALSWIDGET_H
#define __DIGITALSIGNALSWIDGET_H

#include <QList>
#include <QWidget>


class DigitalSignalsWidget : public QWidget
{
	Q_OBJECT
public:
	DigitalSignalsWidget(QWidget* parent = NULL);

	/// Set the number of samples to be displayed
	void setSampleCount(int nSamples);
	/// Clear the graph's data
	/// This method calls update().
	void clear();
	/// Add the given sample to the graph.
	/// This method calls update().
	void addSample(uchar n);

// Overrides for QWidget
protected:
	void paintEvent(QPaintEvent* e);

private:
	void clearSubSamples();
	void paintChannels(QPainter& p, int i0, int i1);
	void paintChannel(QPainter& p, uchar mask, int y, const QColor& clr, int i0, int i1);

private:
	QList<uchar> m_data;
	//QVector<QPointF> m_points;
	int m_nSamples;
	/// Index to the end of the data ring, where the next sample will be placed.
	int m_iEnd;
};

#endif
