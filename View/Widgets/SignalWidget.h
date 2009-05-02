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

#ifndef __SIGNALWIDGET_H
#define __SIGNALWIDGET_H

#include <QList>
#include <QWidget>


class SignalWidget : public QWidget
{
	Q_OBJECT
public:
	SignalWidget(QWidget* parent = NULL);

	void clear();
	void addSample(double n);

// Overrides for QWidget
protected:
	void paintEvent(QPaintEvent* e);

private:
	double m_nSubSampleSum;
	double m_nSubSamples;
	QList<double> m_data;
	double m_nMax;
	double m_nMin;
};

#endif
