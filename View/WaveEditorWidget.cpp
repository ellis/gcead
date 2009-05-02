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

#include "WaveEditorWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QToolButton>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainterPath>

#include <Check.h>
#include <Globals.h>
#include <ViewInfo.h>


WaveEditorWidget::WaveEditorWidget(QWidget* parent)
	: DataListItem(parent)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

	m_vwi = NULL;
	m_flags = (WaveEditorFlag) 0xff;
	setupWidgets();
}

void WaveEditorWidget::setupWidgets()
{
	QString sButtonStyle =
		//"background-color: #eeeeff;"
		"border-style: outset;"
		"border-width: 1px;"
		"border-color: white;"
		"border-radius: 7px;"
		"padding: 2px;"
		;//"min-width: 15px; height: 20px";

	m_lblName = new QLabel();
	QFont font = m_lblName->font();
	font.setBold(true);
	m_lblName->setFont(font);

	m_lblSensLabel = new QLabel(tr("Sensitivity:"));
	//m_cmbSens = new QComboBox();
	//m_cmbSens->setMinimumContentsLength(8);
	//m_cmbSens->setMaximumWidth(30);
	//m_cmbSens->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	//for (int i = 0; g_anVoltsPerDivision[i] != 0; i++)
	//	m_cmbSens->addItem(tr("%0 mV").arg(g_anVoltsPerDivision[i]));
	m_lblSens = new QLabel();
	m_lblSens->setMinimumWidth(m_lblSens->fontMetrics().width(tr("%0 mV").arg(0.05)));
	m_btnSensInc = new QPushButton();
	m_btnSensInc->setStyleSheet(sButtonStyle);
	m_btnSensInc->setIcon(QIcon(":/images/zoom-in-22x22.png"));
	connect(m_btnSensInc, SIGNAL(clicked()), this, SLOT(on_btnSensInc_clicked()));
	m_btnSensDec = new QPushButton();
	m_btnSensDec->setStyleSheet(sButtonStyle);
	m_btnSensDec->setIcon(QIcon(":/images/zoom-out-22x22.png"));
	connect(m_btnSensDec, SIGNAL(clicked()), this, SLOT(on_btnSensDec_clicked()));

	m_lblShiftLabel = new QLabel(tr("Timeshift:"));
	//m_cmbShift = new QComboBox();
	//m_cmbShift->addItems(QStringList() << "1" << "0" << "-1");
	//m_cmbSens->setMaximumWidth(30);
	//m_cmbShift->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	//m_edtShift = new QLineEdit();
	//m_edtShift->setMaximumWidth(30);
	//m_edtShift->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	m_lblShift = new QLabel();
	m_btnShiftDec = new QPushButton();
	m_btnShiftDec->setStyleSheet(sButtonStyle);
	m_btnShiftDec->setText("-");
	connect(m_btnShiftDec, SIGNAL(clicked()), this, SLOT(on_btnShiftDec_clicked()));
	m_btnShiftInc = new QPushButton();
	m_btnShiftInc->setStyleSheet(sButtonStyle);
	m_btnShiftInc->setText("+");
	connect(m_btnShiftInc, SIGNAL(clicked()), this, SLOT(on_btnShiftInc_clicked()));

	m_lblVisibleLabel = new QLabel(tr("Visible:"));
	m_chkVisible = new QCheckBox();
	connect(m_chkVisible, SIGNAL(clicked()), this, SLOT(on_chkVisible_clicked()));

	m_lblCommentLabel = new QLabel(tr("Comment:"));
	m_edtComment = new QLineEdit();
	connect(m_edtComment, SIGNAL(textChanged(const QString&)), this, SLOT(on_edtComment_textChanged()));

	m_btnInvert = new QToolButton();
	m_btnInvert->setText(tr("Invert"));
	connect(m_btnInvert, SIGNAL(clicked()), this, SLOT(on_btnInvert_clicked()));

	QGridLayout* layout = new QGridLayout();
	int iRow = 0;
	layout->addWidget(m_lblName, iRow, 1, 1, 4);
	iRow++;
	layout->addWidget(m_lblSensLabel, iRow, 1);
	layout->addWidget(m_lblSens, iRow, 2);
	layout->addWidget(m_btnSensInc, iRow, 3);
	layout->addWidget(m_btnSensDec, iRow, 4);
	iRow++;
	layout->addWidget(m_lblShiftLabel, iRow, 1);
	layout->addWidget(m_lblShift, iRow, 2);
	layout->addWidget(m_btnShiftDec, iRow, 3);
	layout->addWidget(m_btnShiftInc, iRow, 4);
	iRow++;
	layout->addWidget(m_lblVisibleLabel, iRow, 1);
	layout->addWidget(m_chkVisible, iRow, 2);
	iRow++;
	layout->addWidget(m_lblCommentLabel, iRow, 1);
	iRow++;
	layout->addWidget(m_edtComment, iRow, 1, 1, 4);
	iRow++;
	layout->addWidget(m_btnInvert, iRow, 2, 1, 3);
	
	// Make the second column stretch so that the buttons all maintain a minimum size
	layout->setColumnStretch(1, 1);

	setLayout(layout);
}

void WaveEditorWidget::setWave(ViewWaveInfo* vwi, WaveEditorFlags flags)
{
	m_vwi = vwi;
	m_flags = flags;

	updateFromInfo();

	if (m_vwi != NULL)
		m_edtComment->setText(m_vwi->waveInfo()->sComment);
	else
		m_edtComment->setText("");
}

void WaveEditorWidget::updateFromInfo()
{
	if (m_vwi != NULL)
	{
		m_lblName->setText(m_vwi->wave()->sName);
		m_lblSens->setText(tr("%0 mV").arg(m_vwi->voltsPerDivision()));
		m_lblShift->setText(tr("%0 s").arg(m_vwi->shift() / EAD_SAMPLES_PER_SECOND));
		m_chkVisible->setChecked(m_vwi->isVisible());
	}
	else
	{
		m_lblName->setText("");
		m_lblSens->setText(tr("-"));
		m_lblShift->setText(tr("-"));
		m_chkVisible->setChecked(false);
	}

	updateVisibility();
}

void WaveEditorWidget::updateVisibility()
{
	// Is visible
	bool v = (m_vwi != NULL && m_vwi->isVisible());
	// Is a data wave (NOT an averaged wave);
	bool d = (m_vwi == NULL || m_vwi->wave()->recId() > 0);

	m_lblName->setVisible(m_flags & WaveEditorFlag_Title);

	m_lblVisibleLabel->setVisible(m_flags & WaveEditorFlag_Visible);
	m_chkVisible->setVisible(m_flags & WaveEditorFlag_Visible);

	m_lblSensLabel->setVisible(m_flags & WaveEditorFlag_Sensitivity);
	m_lblSens->setVisible(m_flags & WaveEditorFlag_Sensitivity);
	m_btnSensDec->setVisible(m_flags & WaveEditorFlag_Sensitivity);
	m_btnSensInc->setVisible(m_flags & WaveEditorFlag_Sensitivity);

	m_lblSensLabel->setEnabled(v);
	m_lblSens->setEnabled(v);
	m_btnSensDec->setEnabled(v);
	m_btnSensInc->setEnabled(v);
	
	bool bVis;
	bVis = v && d && (m_flags & WaveEditorFlag_Timeshift);
	m_lblShiftLabel->setVisible(bVis);
	m_lblShift->setVisible(bVis);
	m_btnShiftDec->setVisible(bVis);
	m_btnShiftInc->setVisible(bVis);

	bVis = (d && m_flags.testFlag(WaveEditorFlag_Comment));
	m_lblCommentLabel->setVisible(bVis);
	m_edtComment->setVisible(bVis);

	bVis = (d && m_flags.testFlag(WaveEditorFlag_Invert));
	m_btnInvert->setVisible(bVis);
	m_btnInvert->setEnabled(v);
}

void WaveEditorWidget::on_btnSensDec_clicked()
{
	CHECK_PRECOND_RET(m_vwi != NULL);
	m_vwi->bAssureVisibility = true; // HACK: tell ChartPixmap to reposition this wave for visibility if necessary
	m_vwi->changeVoltsPerDivision(1);
	updateFromInfo();
}

void WaveEditorWidget::on_btnSensInc_clicked()
{
	CHECK_PRECOND_RET(m_vwi != NULL);
	m_vwi->bAssureVisibility = true; // HACK: tell ChartPixmap to reposition this wave for visibility if necessary
	m_vwi->changeVoltsPerDivision(-1);
	updateFromInfo();
}

void WaveEditorWidget::on_btnShiftDec_clicked()
{
	CHECK_PRECOND_RET(m_vwi != NULL);
	m_vwi->changeShift(-1);
	updateFromInfo();
}

void WaveEditorWidget::on_btnShiftInc_clicked()
{
	CHECK_PRECOND_RET(m_vwi != NULL);
	m_vwi->changeShift(1);
	updateFromInfo();
}

void WaveEditorWidget::on_chkVisible_clicked()
{
	CHECK_PRECOND_RET(m_vwi != NULL);
	m_vwi->setVisible(m_chkVisible->isChecked());
}

void WaveEditorWidget::on_edtComment_textChanged()
{
	CHECK_PRECOND_RET(m_vwi != NULL);
	m_vwi->setComment(m_edtComment->text());
}

void WaveEditorWidget::on_btnInvert_clicked()
{
	CHECK_PRECOND_RET(m_vwi != NULL);
	m_vwi->invert();
}
