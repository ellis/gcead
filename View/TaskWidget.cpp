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

#include "TaskWidget.h"

#include <QLinearGradient>
#include <QPainter>


TaskWidget::TaskWidget(QWidget* parent)
	: QWidget(parent)
{
	setContentsMargins(15, 20, 20, 10);
}

void TaskWidget::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e);

	QPainter p(this);

	QColor clr = palette().window().color();

	QLinearGradient grad(0, 0, width(), 0);
	//grad.setColorAt(0, m_clr);
	grad.setColorAt(0, clr);
	//grad.setColorAt(qreal(width() - 10) / width(), m_clr);
	grad.setColorAt(qreal(width() - 10) / width(), clr);
	grad.setColorAt(1, Qt::white);
	p.fillRect(rect(), grad);
}
