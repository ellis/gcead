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

#ifndef __GROUPWIDGET_H
#define __GROUPWIDGET_H

#include <QWidget>


class GroupWidget : public QWidget
{
public:
	GroupWidget(const QColor& clr, QWidget* parent = NULL);
	GroupWidget(const QColor& clr, const QString& sTitle, QWidget* parent = NULL);

	void setTitle(const QString& sTitle);

// QWidget overrides
protected:
	void paintEvent(QPaintEvent* e);

private:
	void init(const QColor& clr);

private:
	QColor m_clr;
	QString m_sTitle;
};

#endif
