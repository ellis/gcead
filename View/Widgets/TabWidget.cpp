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

#include "TabWidget.h"

#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include <Check.h>

#include "Globals.h"
#include "Utils.h"


TabWidget::TabWidget(QWidget* parent)
	: QWidget(parent)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

	m_nWidthMin = 10;
	m_nHeight = 10;

	m_iTab = 0;
}

void TabWidget::addTab(const QString& sName, const QColor& clr)
{
	TabInfo tab;
	tab.s = sName;
	tab.clr = clr;
	tab.bEnabled = true;
	m_tabs << tab;
	layout();
	update();
}

void TabWidget::setEnabled(int iTab, bool bEnabled)
{
	CHECK_PARAM_RET(iTab >= 0 && iTab < m_tabs.size());
	
	m_tabs[iTab].bEnabled = bEnabled;
	update();
}

void TabWidget::setCurrentIndex(int iTab)
{
	CHECK_PARAM_RET(iTab >= 0 && iTab < m_tabs.size());

	m_iTab = iTab;
	update();
}

QSize TabWidget::sizeHint() const
{
	return QSize(m_nWidthMin, m_nHeight);
}

void TabWidget::layout()
{
	QFont fontBold = font();
	fontBold.setBold(true);
	QFontMetrics fm(fontBold);

	int nMarginTop = 5;
	int nMarginLeft = 5;
	int nPadding = 10;

	QList<int> anTextWidths;
	int nWidthMin = nMarginLeft;
	foreach (const TabInfo& tab, m_tabs)
	{
		int nTextWidth = fm.width(tab.s);
		anTextWidths << nTextWidth;
		nWidthMin += nTextWidth + nPadding * 2 + 1;
	}

	int nTabHeight = fm.lineSpacing() + 4 * 2;
	int nHeight = nMarginTop + nTabHeight;

	int x = nMarginLeft;
	int y = qMax(nMarginTop, height() - nTabHeight);
	for (int i = 0; i < anTextWidths.size(); i++)
	{
		int nTextWidth = anTextWidths[i];
		QRect rc(x, y, nTextWidth + nPadding * 2, nTabHeight);
		m_tabs[i].rc = rc;
		x = rc.right() + 1;
	}

	m_nWidthMin = nWidthMin;
	m_nHeight = nHeight;
	updateGeometry();
}

void TabWidget::resizeEvent(QResizeEvent* e)
{
	Q_UNUSED(e);
	m_bLayout = true;
}

void TabWidget::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e);

	if (m_bLayout)
	{
		layout();
		m_bLayout = false;
	}

	QPainter p(this);

	// Draw all non-active tabs
	p.setRenderHint(QPainter::Antialiasing, true);
	for (int i = 0; i < m_tabs.size(); i++)
	{
		if (i != m_iTab)
			paintTab(p, i);
	}

	// Draw the shadow
	QLinearGradient grad(0, height() - 4, 0, height() - 1);
	grad.setColorAt(0, Qt::transparent);
	grad.setColorAt(1, QColor(0, 0, 0, 80));
	p.fillRect(QRect(0, height() - 4, width(), 3), grad);

	// Draw the separator line
	p.setRenderHint(QPainter::Antialiasing, false);
	p.setPen(Qt::black);
	p.drawLine(0, height() - 1, width(), height() - 1);


	p.setRenderHint(QPainter::Antialiasing, true);
	paintTab(p, m_iTab);
}

void TabWidget::paintTab(QPainter& p, int iTab)
{
	const TabInfo& tab = m_tabs[iTab];
	QColor clrBackground = tab.clr;
	QColor clrText = Qt::black;
	if (!tab.bEnabled)
	{
		clrBackground.setAlpha(127);
		clrText.setAlpha(127);
	}

	QPainterPath path;
	QRect rc;

	QFont fontOrig = p.font();
	if (iTab == m_iTab)
	{
		QFont font = fontOrig;
		font.setBold(true);
		p.setFont(font);
	}

	rc = tab.rc.adjusted(0, 0, 0, 20);
	path = roundedRect(rc, 5);
	p.fillPath(path, clrBackground);

	// Draw outline of tab in a darker color
	if (iTab == m_iTab)
		p.setPen(Qt::black);
	else
		p.setPen(clrBackground.darker());
	p.drawPath(path);

	p.setPen(clrText);
	p.drawText(tab.rc, Qt::AlignCenter, tab.s);

	p.setFont(fontOrig);
}

void TabWidget::mousePressEvent(QMouseEvent* e)
{
	QPoint pt = e->pos();

	for (int i = 0; i < m_tabs.size(); i++)
	{
		const TabInfo& tab = m_tabs[i];
		if (tab.bEnabled && tab.rc.contains(pt) && i != m_iTab)
		{
			m_iTab = i;
			update();
			emit tabClicked(m_iTab);
		}
	}
}
