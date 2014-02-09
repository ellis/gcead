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

#include "TaskReviewWidget.h"

#include <QtDebug>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>
#include <QToolTip>
#include <QVBoxLayout>

#include <Check.h>
#include <Globals.h>
#include <Utils.h>

#include <WaveInfo.h>

#include <MainScope.h>

#include "WaveEditorDialog.h"


/// Volts per Division (in mV)
const double g_anVoltsPerDivision[] =
{
	0.02, 0.05,
	0.1, 0.2, 0.5,
	1, 2, 5,
	10, 20, 50,
	100,
	0 // termination value
};


TaskReviewWidget::TaskReviewWidget(MainScope* scope, QWidget* parent)
	: TaskWidget(parent)
{
	m_scope = scope;
	//m_scope->
	connect(m_scope, SIGNAL(waveListChanged()), this, SLOT(on_scope_waveListChanged()));
	connect(m_scope, SIGNAL(peakModeChanged(EadMarkerMode)), this, SLOT(on_scope_peakModeChanged()));

	setAttribute(Qt::WA_OpaquePaintEvent);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_groups[0].sTitle = tr("Recordings");
	m_groups[1].sTitle = tr("Averages");
	m_groups[2].sTitle = tr("Extras");

	//m_sVisible = tr("Visible:");
	//m_sSens = tr("Sensitivity:");

	/*
	m_area = new QScrollArea();
	m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_area->setWidget(new QWidget());
	m_layout = new QVBoxLayout();
	//m_layout->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
	m_layout->setAlignment(Qt::AlignTop);
	m_area->widget()->setLayout(m_layout);
	m_area->setWidgetResizable(true);
	*/

	m_cmbEad = new QComboBox(this);
	connect(m_cmbEad, SIGNAL(activated(int)), this, SLOT(on_cmbEad_activated()));

	m_cmbFid = new QComboBox(this);
	connect(m_cmbFid, SIGNAL(activated(int)), this, SLOT(on_cmbFid_activated()));

	m_btnShowAll = new QPushButton(tr("Show All"), this);
	connect(m_btnShowAll, SIGNAL(clicked()), this, SLOT(on_btnShowAll_clicked()));
	m_btnHideAll = new QPushButton(tr("Hide All"), this);
	connect(m_btnHideAll, SIGNAL(clicked()), this, SLOT(on_btnHideAll_clicked()));

	layout(false);
}

void TaskReviewWidget::setupItems(EadFile* file)
{
	//qDebug() << "setupItems";
	m_file = file;

	// Clear all group items
	for (int i = 0; i < 3; i++)
		m_groups[i].items.clear();

	m_cmbEad->clear();
	m_cmbEad->addItem(tr("No EAD"), -1);
	int iCmbEad = 0;

	m_cmbFid->clear();
	m_cmbFid->addItem(tr("No FID"), -1);
	int iCmbFid = 0;

	if (m_file != NULL)
	{
		ViewInfo* view = m_file->viewInfo(m_scope->viewType());

		QList<ViewWaveInfo*> vwis;
		vwis << view->vwis() << view->vwiExtras();
		foreach (ViewWaveInfo* vwi, vwis)
		{
			CHECK_ASSERT_RET(vwi->wave() != NULL);
			const WaveInfo* wave = vwi->wave();
		
			// If this is an averaged waveform and we're not in "Averages" view:
			if (wave->recId() == 0)
			{
				if (m_scope->viewType() == EadView_Averages)
					m_groups[1].items << ItemInfo(vwi, Mode_Sens);
				else
					m_groups[1].items << ItemInfo(vwi, Mode_SensVisible);
			}
			else
				m_groups[0].items << ItemInfo(vwi, Mode_Everything);
		}

		// Add the extra waveform, if there is one
		if (view->vwiUser.wave() != NULL)
		{
			m_groups[2].items << ItemInfo(&view->vwiUser, Mode_Sens);
		}

		foreach (RecInfo* rec, m_file->recs())
		{
			foreach (WaveInfo* wave, rec->waves())
			{
				// Fill EAD combobox
				if (wave->type == WaveType_EAD)
				{
					if (m_scope->viewType() == EadView_FIDs)
					{
						m_cmbEad->addItem(wave->sName, qVariantFromValue((QObject*) wave));
						if (wave == view->vwiUser.wave())
							iCmbEad = m_cmbEad->count() - 1;
					}
				}
				// Fill FID combobox
				else if (wave->type == WaveType_FID)
				{
					if (m_scope->viewType() == EadView_EADs)
					{
						m_cmbFid->addItem(wave->sName, qVariantFromValue((QObject*) wave));
						if (wave == view->vwiUser.wave())
							iCmbFid = m_cmbFid->count() - 1;
					}
				}
			}
		}
	}

	m_groups[2].bCmbEad = false;
	m_groups[2].bCmbFid = false;

	switch (m_scope->viewType())
	{
	case EadView_EADs:
		m_groups[2].sTitle = tr("FID");
		m_groups[2].bCmbFid = true;
		break;
	case EadView_FIDs:
		m_groups[2].sTitle = tr("EAD");
		m_groups[2].bCmbEad = true;
		break;
	case EadView_Averages:
	case EadView_All:
	case EadView_Recording:
		break;
	}

	m_cmbEad->setCurrentIndex(iCmbEad);
	m_cmbEad->setVisible(m_groups[2].bCmbEad);
	m_cmbFid->setCurrentIndex(iCmbFid);
	m_cmbFid->setVisible(m_groups[2].bCmbFid);
	
	m_btnShowAll->setEnabled(m_scope->viewType() != EadView_Averages);
	m_btnHideAll->setEnabled(m_scope->viewType() != EadView_Averages);

	m_bLayout = true;
	update();
}

QSize TaskReviewWidget::sizeHint() const
{
	return QSize(m_nWidthMin, m_nHeight);
}

void TaskReviewWidget::layout(bool bLayoutItems)
{
	QString sNameA = QObject::tr("EAD AVE");
	QString sNameB = QObject::tr("FID AVE");
	QString s3a = QObject::tr("%0 mV").arg(g_anVoltsPerDivision[0]);
	QString s3b = QObject::tr("%0 mV").arg(100);

	QFontMetrics fm = fontMetrics();
	QFont fontBold;
	fontBold.setBold(true);
	QFontMetrics fmBold = QFontMetrics(fontBold);

	nBorderGroup = 15;
	m_nPixmapSize = 22;
	int x = nBorderGroup + 15;
	x0 = x;
	xVisible = x;
	xEdit = xVisible + m_nPixmapSize + 2;
	xName = xEdit + m_nPixmapSize + 5;
	xSensitivity = xName + qMax(fmBold.width(sNameA), fmBold.width(sNameB)) + 15;
	wSensitivity = qMax(fm.width(s3a), fm.width(s3b));
	xZoomIn = xSensitivity + wSensitivity + 10;
	xZoomOut = xZoomIn + m_nPixmapSize + 2;
	int xRight = xZoomOut + m_nPixmapSize;
	m_nLineHeight = fm.lineSpacing() + fm.lineSpacing() / 3;
	m_nLineHeight = qMax(m_nLineHeight, m_nPixmapSize);

	if (m_nLineHeight != m_pixZoomIn.height())
	{
		m_pixEdit = QPixmap(":/images/configure.png");
		m_pixZoomIn = QPixmap(":/images/zoom-in-22x22.png");
		m_pixZoomIn = m_pixZoomIn.scaledToHeight(m_nPixmapSize);
		m_pixZoomOut = QPixmap(":/images/zoom-out-22x22.png");
		m_pixZoomOut = m_pixZoomOut.scaledToHeight(m_nPixmapSize);
	}

	m_nWidthMin = xRight + 15 + nBorderGroup + 5;
	m_nWidthGroup = width() - nBorderGroup * 2 - 5;
	m_nWidthItem = m_nWidthGroup - 30;

	// HACK: hackish calculation for centering the items
	/*int x1New = (width() - 5 - (x5 - x1)) / 2;
	int dx = x1New - x1;
	if (dx > 0)
	{
		x0 += dx;
		x1 += dx;
		x2 += dx;
		x3 += dx;
		x4 += dx;
	}*/

	// Setup each group's rectangle
	int y = 20;
	if (bLayoutItems)
	{
		for (int i = 0; i < 3; i++)
		{
			layoutGroup(m_groups[i], y);
			if (m_groups[i].rc.height() > 0)
				y = m_groups[i].rc.bottom() + 20;
		}
	}

	int xCenter = (width() - 5) / 2;
	int nRowWidth;
	int xCol1;

	// Position show/hide buttons
	nRowWidth = m_btnShowAll->sizeHint().width() + 10 + m_btnHideAll->sizeHint().width();
	xCol1 = xCenter - nRowWidth / 2;
	m_btnShowAll->move(xCol1, y);
	m_btnHideAll->move(xCol1 + m_btnShowAll->sizeHint().width() + 10, y);
	y += m_btnShowAll->sizeHint().height() + 15;

	y += 10;

	m_nHeight = y;

	updateGeometry();
}

void TaskReviewWidget::layoutGroup(GroupInfo& group, int y)
{
	group.rc.setTop(y);
	group.rc.setLeft(nBorderGroup);
	group.rc.setWidth(m_nWidthGroup);

	y += 10 + 20 + 10;

	int xCenter = (width() - 5) / 2;

	bool bHasContents = false;

	if (group.bCmbEad)
	{
		bHasContents = true;
		int x = xCenter - m_cmbEad->sizeHint().width() / 2;
		m_cmbEad->move(x, y);
		y += m_cmbEad->height() + 10;
	}
	if (group.bCmbFid)
	{
		bHasContents = true;
		int x = xCenter - m_cmbFid->sizeHint().width() / 2;
		m_cmbFid->move(x, y);
		y += m_cmbFid->height() + 10;
	}

	for (int i = 0; i < group.items.size(); i++)
	{
		bHasContents = true;
		ItemInfo& item = group.items[i];
		layoutGroupItem(item, y);
		y = item.rc.bottom() + 20;
	}

	if (bHasContents)
		group.rc.setBottom(y);
	else
		group.rc.setHeight(0);
}

void TaskReviewWidget::layoutGroupItem(ItemInfo& item, int y)
{
	const WaveInfo* wave = item.vwi->wave();

	item.rc.setLeft(x0);
	item.rc.setTop(y);
	item.rc.setWidth(m_nWidthItem);
	// If visibility/enabled property can't be edited:
	if (item.mode == Mode_Sens)
	{
		item.bShowVisible = false;
	}
	else if (wave->recId() > 0)
	{
		item.bShowVisible = true;
	}
	else
	{
		item.bShowVisible = true;
	}
	item.bShowSens = true;
	
	// Account for height of wave name
	y += m_nLineHeight;
	//if (item.bShowSens)
	//	y += m_nLineHeight;

	item.rc.setHeight(y - item.rc.top());
}

bool TaskReviewWidget::event(QEvent* e)
{
	if (e->type() == QEvent::ToolTip)
	{
		QHelpEvent* helpEvent = static_cast<QHelpEvent*>(e);
		QPoint pt = helpEvent->pos();
		const ItemInfo* info;
		
		// Check for user clicking on "visible" checkbox
		if ((info = itemAt(m_arcVisible, pt)) != NULL)
			QToolTip::showText(helpEvent->globalPos(), tr("Show/hide this wave"));
		else if ((info = itemAt(m_arcEdit, pt)) != NULL)
			QToolTip::showText(helpEvent->globalPos(), tr("Open settings dialog"));
		else if ((info = itemAt(m_arcSensitivity, pt)) != NULL)
			QToolTip::showText(helpEvent->globalPos(), tr("Sensitivity"));
		else
			QToolTip::hideText();
	}
	return QWidget::event(e);
}

void TaskReviewWidget::resizeEvent(QResizeEvent* e)
{
	Q_UNUSED(e);
	m_bLayout = true;
}

void TaskReviewWidget::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e);

	QPainter p(this);

	if (m_bLayout)
	{
		layout();
		m_bLayout = false;
	}

	QLinearGradient grad(0, 0, width(), 0);
	//QColor clr = QColor(0xe0, 0xe0, 0xff);
	QColor clr = palette().window().color();
	grad.setColorAt(0, clr);
	grad.setColorAt(qreal(width() - 10) / width(), clr);
	grad.setColorAt(1, Qt::white);
	p.fillRect(rect(), grad);

	m_arcVisible.clear();
	m_arcEdit.clear();
	m_arcSensitivity.clear();
	m_arcZoomIn.clear();
	m_arcZoomOut.clear();
	
	for (int iGroup = 0; iGroup < 3; iGroup++)
	{
		paintGroup(p, m_groups[iGroup]);
	}
}

void TaskReviewWidget::paintGroup(QPainter& p, const GroupInfo& group)
{
	if (group.rc.height() > 0)
	{
		paintGroupBackground(p, group.sTitle, group.rc);

		for (int i = 0; i < group.items.count(); i++)
			paintGroupItem(p, group.items[i]);
	}
}

void TaskReviewWidget::paintGroupBackground(QPainter& p, const QString& sTitle, const QRect& rcGroup)
{
	QPainterPath path;
	QRect rc;
	QColor clr;

	p.setRenderHint(QPainter::Antialiasing, true);
	
	// Fill shadow
	rc = rcGroup;
	path = roundedRect(rc, 10);
	clr = QColor(0x20, 0x20, 0x60);
	p.fillPath(path, clr);

	// Path for background
	int nShadow = 5;
	rc.adjust(nShadow, nShadow, -nShadow, -nShadow);
	path = roundedRect(rc, 5);

	// Fill background
	clr = QColor(0xc0, 0xc0, 0xff);
	p.fillPath(path, clr);
	// Border around background
	clr = QColor(0xa0, 0xa0, 0xd0);
	p.setPen(clr);
	p.drawPath(path);

	// Title rectangle
	rc = QRect(rc.left() + 2, rc.top() + 2, rc.width() - 4, 20);
	path = roundedRect(rc, 3);
	p.fillPath(path, clr);

	p.setRenderHint(QPainter::Antialiasing, false);

	// Title text
	QFont fontOrig = p.font();
	QFont fontTitle = fontOrig;
	fontTitle.setBold(true);
	fontTitle.setPixelSize(16);
	rc.translate(0, -1);
	p.setPen(Qt::black);
	p.setFont(fontTitle);
	p.drawText(rc, Qt::AlignCenter, sTitle);
	p.setFont(fontOrig);
}

void TaskReviewWidget::paintGroupItem(QPainter& p, const ItemInfo& item)
{
	CHECK_PRECOND_RET(item.vwi != NULL);

	ViewWaveInfo* vwi = item.vwi;
	const WaveInfo* wave = vwi->wave();

	p.setPen(Qt::black);

	QFont font = p.font();
	QFont fontBold = font;
	fontBold.setBold(true);

	QRect rc;

	// Wave name
	rc = item.rc;
	rc.setLeft(xName);
	rc.setHeight(m_nLineHeight);
	p.setFont(fontBold);
	if (wave != NULL)
		p.drawText(rc, Qt::AlignVCenter, wave->sName);
	p.setFont(font);

	// Draw edit icon to the right of the name
	int flagsExtra = vwi->editorFlags & (WaveEditorFlag_Comment | WaveEditorFlag_Invert | WaveEditorFlag_Timeshift);
	if (flagsExtra != 0) {
		rc = paintPixmap(p, m_pixEdit, xEdit, rc.top());
		m_arcEdit[&item] = rc;
		p.drawPixmap(rc, m_pixEdit);
	}

	int y = item.rc.top();
	
	// Visible
	if (item.bShowVisible)
	{
		// Paint checkbox
		QStyleOptionButton opt;
		opt.initFrom(this);
		//opt.features = QStyleOptionButton::Flat; // NOTE: this didn't have any effect
		opt.palette = QPalette(QColor(0xc0, 0xc0, 0xff));
		opt.palette.setColor(QPalette::Window, QColor(0xc0, 0xc0, 0xff));
		opt.palette.setColor(QPalette::Base, QColor(0xc0, 0xc0, 0xff));
		opt.state = QStyle::State_Enabled;

		int nWidth = style()->pixelMetric(QStyle::PM_IndicatorWidth, &opt, this);
		int nHeight = style()->pixelMetric(QStyle::PM_IndicatorWidth, &opt, this);
		opt.rect = QRect(xVisible + (16 - nWidth) / 2, item.rc.top() + (m_nLineHeight - nHeight) / 2, nWidth, nHeight);
		m_arcVisible[&item] = opt.rect;

		bool bChecked = vwi->isVisible();
		opt.state |= (bChecked) ? QStyle::State_On : QStyle::State_Off;

		style()->drawControl(QStyle::CE_CheckBox, &opt, &p);
		
		p.setPen(Qt::black);
	}
	
	// Sensitivity
	if (item.bShowSens)
	{
		// Draw label
		rc = QRect(xSensitivity, y, wSensitivity, m_nLineHeight);
		//p.drawText(rc, Qt::AlignVCenter, m_sSens);
		// Draw sensitivity value
		QString s = QObject::tr("%0 mV").arg(vwi->voltsPerDivision());
		p.drawText(rc, Qt::AlignVCenter | Qt::AlignRight, s);
		m_arcSensitivity[&item] = rc;
		// Draw zoom-in button
		rc = paintPixmap(p, m_pixZoomIn, xZoomIn, y);
		m_arcZoomIn[&item] = rc;
		// Draw zoom-out button
		rc = paintPixmap(p, m_pixZoomOut, xZoomOut, y);
		m_arcZoomOut[&item] = rc;
		
		y += m_nLineHeight;
	}
}

QRect TaskReviewWidget::paintPixmap(QPainter& p, const QPixmap& pixmap, int x, int y)
{
	int dx = (m_nPixmapSize - pixmap.width()) / 2;
	int dy = (m_nLineHeight - pixmap.height()) / 2;
	QRect rc = QRect(x + dx, y + dy, pixmap.width(), pixmap.width());
	p.drawPixmap(rc, pixmap);
	return rc;
}

const TaskReviewWidget::ItemInfo* TaskReviewWidget::itemAt(const QMap<const ItemInfo*, QRect>& map, const QPoint& pt) const
{
	QMap<const ItemInfo*, QRect>::const_iterator it;
	
	// Check for user clicking on "visible" checkbox
	for (it = map.constBegin(); it != map.constEnd(); ++it)
	{
		QRect rc = it.value();
		if (rc.contains(pt))
			return it.key();
	}

	return NULL;
}

void TaskReviewWidget::mousePressEvent(QMouseEvent* e)
{
	QPoint pt = e->pos();

	const ItemInfo* item;

	// Check for user clicking on "visible" checkbox
	if ((item = itemAt(m_arcVisible, pt)) != NULL)
	{
		ViewWaveInfo* vwi = item->vwi;
		vwi->setVisible(!vwi->isVisible());
		QRect rcUpdate(0, item->rc.top() - 5, width(), item->rc.height() + 10);
		update(rcUpdate);
	}

	// Properties
	else if ((item = itemAt(m_arcEdit, pt)) != NULL)
	{
		// REFACTOR: get rid of item->mode and replace it with "WaveEditorFlags flags"

		QPoint ptGlobal(mapToGlobal(QPoint(0, pt.y())));

		WaveEditorDialog editor(item->vwi, item->vwi->editorFlags, this);
		editor.move(ptGlobal);
		editor.exec();
	}

	else if ((item = itemAt(m_arcZoomIn, pt)) != NULL)
	{
		changeSens(item->vwi, -1);
		QRect rcUpdate(0, item->rc.top() - 5, width(), item->rc.height() + 10);
		update(rcUpdate);
		return;
	}

	else if ((item = itemAt(m_arcZoomOut, pt)) != NULL)
	{
		changeSens(item->vwi, 1);
		QRect rcUpdate(0, item->rc.top() - 5, width(), item->rc.height() + 10);
		update(rcUpdate);
		return;
	}
}

void TaskReviewWidget::changeSens(ViewWaveInfo* vwi, int nIndexInc)
{
	CHECK_PARAM_RET(vwi != NULL);
	vwi->bAssureVisibility = true; // HACK: tell ChartPixmap to reposition this wave for visibility if necessary
	vwi->changeVoltsPerDivision(nIndexInc);
}

void TaskReviewWidget::on_cmbEad_activated()
{
	int iItem = m_cmbEad->currentIndex();
    WaveInfo* wave = (WaveInfo*) m_cmbEad->itemData(iItem).value<QObject*>();
	selectExtraWave(wave);
}

void TaskReviewWidget::on_cmbFid_activated()
{
	int iItem = m_cmbFid->currentIndex();
    WaveInfo* wave = (WaveInfo*) m_cmbFid->itemData(iItem).value<QObject*>();
	selectExtraWave(wave);
}

void TaskReviewWidget::on_btnShowAll_clicked()
{
	setAllVisible(true);
}

void TaskReviewWidget::on_btnHideAll_clicked()
{
	setAllVisible(false);
}

void TaskReviewWidget::setAllVisible(bool bVisible)
{
	for (int iGroup = 0; iGroup < 3; iGroup++)
	{
		for (int iItem = 0; iItem < m_groups[iGroup].items.count(); iItem++)
		{
			const ItemInfo& item = m_groups[iGroup].items[iItem];
			if (item.vwi->editorFlags.testFlag(WaveEditorFlag_Visible))
			{
				item.vwi->setVisible(bVisible);
			}
		}
	}
	update();
}

void TaskReviewWidget::on_scope_waveListChanged()
{
	//qDebug() << "TaskReviewWidget::on_scope_waveListChanged";
	m_bLayout = true;
	update();
}

void TaskReviewWidget::on_scope_peakModeChanged()
{
	EadMarkerMode peakMode = m_scope->peakMode();
	// TODO: Is this necessary anymore?  I removed the tip label... -- ellis, 2010-09-16
	bool bShowTip = (peakMode == EadMarkerMode_Edit && m_scope->peakModeRecId() >= 0);
	if (bShowTip)
	{
		m_bLayout = true;
		update();
	}
}

void TaskReviewWidget::selectExtraWave(WaveInfo* wave)
{
	CHECK_PRECOND_RET(m_file != NULL);

	ViewInfo* view = m_file->viewInfo(m_scope->viewType());
	view->setUserWave(wave);

	setupItems(m_file);
}
