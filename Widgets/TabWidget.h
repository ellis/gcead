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

#ifndef __TABWIDGET_H
#define __TABWIDGET_H

#include <QWidget>


class TabWidget : public QWidget
{
	Q_OBJECT
public:
	TabWidget(QWidget* parent = NULL);

	//void setup(const QStringList& asTabs, Qt::Alignment alignment);
	void addTab(const QString& sName, const QColor& clr = Qt::transparent);
	void setEnabled(int iTab, bool bEnabled);
	//void setColors(const QList<QColor>& aclrTabs);

public slots:
	void setCurrentIndex(int i);

signals:
	/// Emitted when the user selects a tab
	void tabClicked(int i);

// QWidget overrides
public:
	QSize sizeHint() const;

// QWidget overrides
protected:
	void resizeEvent(QResizeEvent* e);
	void paintEvent(QPaintEvent* e);
	void mousePressEvent(QMouseEvent* e);

private:
	struct TabInfo
	{
		QString s;
		QColor clr;
		bool bEnabled;
		QRect rc;
	};

private:
	void layout();
	void paintTab(QPainter& p, int iTab);

private:
	QList<TabInfo> m_tabs;
	/// Index of currently selected tab
	int m_iTab;

	// Calculated in layout()
	int m_nHeight;
	int m_nWidthMin;

	/// True if we need to perform a new layout before the next paint event
	bool m_bLayout;
};

#endif
