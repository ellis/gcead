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

#ifndef __TASKPANEL_H
#define __TASKPANEL_H

#include <QWidget>


class QScrollBar;

class TaskWidget;


class TaskPanel : public QWidget
{
	Q_OBJECT
public:
	TaskPanel(TaskWidget* viewport, QWidget* parent = NULL);

	//TaskWidget* viewport() { return m_viewport; }

// QWidget overrides
public:
	QSize sizeHint() const;

// QWidget overrides
protected:
	bool event(QEvent* e);
	void resizeEvent(QResizeEvent* e);
	void paintEvent(QPaintEvent* e);

private:
	void performLayout();

private slots:
	void on_scrollbar_actionTriggered();

private:
	QScrollBar* m_scrollbar;
	TaskWidget* m_viewport;
	int m_nPreferredWidth;
};

#endif
