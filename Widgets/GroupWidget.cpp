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

#include "GroupWidget.h"

#include <QPainter>

#include "Utils.h"


GroupWidget::GroupWidget(const QColor& clr, QWidget* parent)
	: QWidget(parent)
{
	init(clr);
}

GroupWidget::GroupWidget(const QColor& clr, const QString& sTitle, QWidget* parent)
	: QWidget(parent)
{
	init(clr);
	m_sTitle = sTitle;
}

void GroupWidget::init(const QColor& clr)
{
	m_clr = clr;

	//setAttribute(Qt::WA_NoSystemBackground);
	setContentsMargins(15, 25, 15, 15);
}

void GroupWidget::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e);
	
	QPainterPath path;
	QRect rc;
	QColor clr;
	
	QPainter p(this);

	p.setRenderHint(QPainter::Antialiasing, true);
	
	// Fill shadow
	rc = rect();
	path = roundedRect(rc, 10);
	clr = m_clr.dark(300);
	p.fillPath(path, clr);

	// Path for background
	int nShadow = 5;
	rc.adjust(nShadow, nShadow, -nShadow, -nShadow);
	path = roundedRect(rc, 5);

	// Fill background
	clr = m_clr;
	p.fillPath(path, clr);
	// Border around background
	clr = clr.dark(115);
	p.setPen(clr);
	p.drawPath(path);

	// Title rectangle
	rc = QRect(rc.left() + 2, rc.top() + 2, rc.width() - 4, 20);
	path = roundedRect(rc, 3);
	p.fillPath(path, clr);

	//p.setRenderHint(QPainter::Antialiasing, false);

	// Title text
	QFont fontOrig = p.font();
	QFont fontTitle = fontOrig;
	fontTitle.setBold(true);
	fontTitle.setPixelSize(16);
	rc.translate(0, -1);
	p.setPen(Qt::black);
	p.setFont(fontTitle);
	p.drawText(rc, Qt::AlignCenter, m_sTitle);
	p.setFont(fontOrig);
}
