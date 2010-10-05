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

#include "TaskPanel.h"

#include <QEvent>
#include <QLinearGradient>
#include <QPainter>
#include <QScrollBar>

#include "TaskWidget.h"


TaskPanel::TaskPanel(TaskWidget* viewport, QWidget* parent)
	: QWidget(parent)
{
	Q_ASSERT(viewport != NULL);

	setMinimumHeight(100);
	
	m_nPreferredWidth = 0;

	m_viewport = viewport;
	m_viewport->setParent(this);

	m_scrollbar = new QScrollBar(Qt::Vertical, this);
	m_scrollbar->setSingleStep(10);
	connect(m_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(on_scrollbar_actionTriggered()));
}

QSize TaskPanel::sizeHint() const
{
	QSize sz(m_nPreferredWidth, 200);
	return sz;
}

bool TaskPanel::event(QEvent* e)
{
	if (e->type() == QEvent::LayoutRequest)
	{
		performLayout();
		e->accept();
		return true;
	}
	return QWidget::event(e);
}

void TaskPanel::resizeEvent(QResizeEvent* e)
{
	Q_UNUSED(e);
	performLayout();
}

void TaskPanel::paintEvent(QPaintEvent* e)
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

void TaskPanel::performLayout()
{
	int x = 0;
	int nPreferredWidth = 0;

	int nViewportHeight = m_viewport->sizeHint().height();

	// Check whether we need the scrollbar
	bool bScrollbar = (nViewportHeight > height());
	int yScroll = 0;
	if (bScrollbar)
	{
		QRect rcScrollbar(x, 0, m_scrollbar->sizeHint().width(), height());
		m_scrollbar->setGeometry(rcScrollbar);
		nPreferredWidth += rcScrollbar.width();
		x = rcScrollbar.right();

		m_scrollbar->setMaximum(nViewportHeight - height());
		m_scrollbar->setPageStep(height());

		// Start at the top if we're just now showing the scrollbar
		if (!m_scrollbar->isVisible())
			m_scrollbar->setValue(0);
		else
			yScroll = m_scrollbar->value();
	}
	m_scrollbar->setVisible(bScrollbar);

	// Size the TaskWidget
	QRect rcViewport(x, -yScroll, width() - x, nViewportHeight);
	m_viewport->setGeometry(rcViewport);
	nPreferredWidth += m_viewport->sizeHint().width();

	// Let the system know if our sizeHint() has changed
	if (nPreferredWidth != m_nPreferredWidth)
	{
		m_nPreferredWidth = nPreferredWidth;
		updateGeometry();
	}
}

void TaskPanel::on_scrollbar_actionTriggered()
{
	performLayout();
}
