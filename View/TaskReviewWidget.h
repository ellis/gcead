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

#ifndef __TASKREVIEWWIDGET_H
#define __TASKREVIEWWIDGET_H

#include <QList>
#include <QMap>
#include <QPointer>
#include <QWidget>

#include <EadEnums.h>

#include "TaskWidget.h"


class QComboBox;
class QLabel;
class QPushButton;
class QScrollArea;
class QVBoxLayout;

class EadFile;
class MainScope;
class ViewWaveInfo;
class WaveInfo;


class TaskReviewWidget : public TaskWidget
{
	Q_OBJECT
public:
	TaskReviewWidget(MainScope* scope, QWidget* parent = NULL);

	void setupItems(EadFile* file);

// QWidget overrides
public:
	QSize sizeHint() const;

// QWidget overrides
protected:
	bool event(QEvent* e);
	void resizeEvent(QResizeEvent* e);
	void paintEvent(QPaintEvent* e);
	void mousePressEvent(QMouseEvent* e);

private:
	enum ItemMode
	{
		Mode_Sens,
		Mode_SensVisible,
		Mode_Everything
	};

	struct ItemInfo
	{
		ViewWaveInfo* vwi;
		ItemMode mode;
		WaveEditorFlags flags;
		QRect rc;

		bool bShowVisible;
		bool bShowSens;

		ItemInfo()
			: vwi(NULL)
		{
		}

		ItemInfo(ViewWaveInfo* vwi, ItemMode mode, WaveEditorFlags flags)
			: vwi(vwi), mode(mode), flags(flags)
		{
		}
	};

	struct GroupInfo
	{
		bool bVisible;
		QString sTitle;
		bool bCmbEad;
		bool bCmbFid;
		QList<ItemInfo> items;
		QRect rc;

		GroupInfo()
		:	bVisible(true),
			bCmbEad(false),
			bCmbFid(false)
		{
		}
	};

private slots:
	void on_cmbEad_activated();
	void on_cmbFid_activated();
	void on_btnShowAll_clicked();
	void on_btnHideAll_clicked();
	void on_scope_peakModeChanged();
	void on_cmbPeakFid_activated();

private:
	/// @param bLayoutItems If this is false, only the widget's width is determined, but not the height
	void layout(bool bLayoutItems = true);
	void layoutGroup(GroupInfo& group, int y);
	void layoutGroupItem(ItemInfo& item, int y);

	void paintGroup(QPainter& p, const GroupInfo& group);
	void paintGroupBackground(QPainter& p, const QString& sTitle, const QRect& rc);
	void paintGroupItem(QPainter& p, const ItemInfo& item);
	QRect paintPixmap(QPainter& p, const QPixmap& pixmap, int x, int y);

	const ItemInfo* itemAt(const QMap<const ItemInfo*, QRect>& map, const QPoint& pt) const;

	void changeSens(ViewWaveInfo* vwi, int nIndexInc);

	void selectExtraWave(WaveInfo* wave);

	void setAllVisible(bool bVisible);

private:
	MainScope* m_scope;

	QPointer<EadFile> m_file;

	//QScrollBar* m_scrollbar;
	QComboBox* m_cmbEad;
	QComboBox* m_cmbFid;

	QPushButton* m_btnShowAll;
	QPushButton* m_btnHideAll;

	QLabel* m_lblPeakFid;
	QComboBox* m_cmbPeakFid;
	QLabel* m_lblPeakTip;

	GroupInfo m_groups[3];

	bool m_bLayout;

	QPixmap m_pixEdit;
	QPixmap m_pixZoomIn;
	QPixmap m_pixZoomOut;
	int nBorderGroup;
	int m_nPixmapSize;
	int x0;
	int xVisible;
	int xEdit;
	int xName;
	int xSensitivity;
	int wSensitivity;
	int xZoomIn;
	int xZoomOut;
	int m_nLineHeight;

	int m_nHeight;
	int m_nWidthMin;
	int m_nWidthGroup;
	int m_nWidthItem;

	QMap<const ItemInfo*, QRect> m_arcVisible;
	QMap<const ItemInfo*, QRect> m_arcEdit;
	QMap<const ItemInfo*, QRect> m_arcSensitivity;
	QMap<const ItemInfo*, QRect> m_arcZoomIn;
	QMap<const ItemInfo*, QRect> m_arcZoomOut;
};

#endif
