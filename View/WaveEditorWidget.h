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

#ifndef __WAVEEDITORWIDGET_H
#define __WAVEEDITORWIDGET_H

#include <QPointer>

#include "DataListItem.h"

#include <EadEnums.h>


class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QToolButton;

class ViewWaveInfo;


class WaveEditorWidget : public DataListItem
{
	Q_OBJECT
public:
	WaveEditorWidget(QWidget* parent = NULL);

	void setWave(ViewWaveInfo* vwi, WaveEditorFlags flags);

private:
	void setupWidgets();
	void updateFromInfo();
	void updateVisibility();

private slots:
	void on_btnSensDec_clicked();
	void on_btnSensInc_clicked();
	void on_btnShiftDec_clicked();
	void on_btnShiftInc_clicked();
	void on_chkVisible_clicked();
	void on_edtComment_textChanged();
	void on_btnInvert_clicked();

private:
	QPointer<ViewWaveInfo> m_vwi;
	WaveEditorFlags m_flags;

	QLabel* m_lblName;
	QLabel* m_lblSensLabel;
	QLabel* m_lblSens;
	QPushButton* m_btnSensInc;
	QPushButton* m_btnSensDec;
	QLabel* m_lblShiftLabel;
	QLabel* m_lblShift;
	QPushButton* m_btnShiftInc;
	QPushButton* m_btnShiftDec;
	QLabel* m_lblVisibleLabel;
	QCheckBox* m_chkVisible;
	QLabel* m_lblCommentLabel;
	QLineEdit* m_edtComment;
	QToolButton* m_btnInvert;

	class GraphicsWaveInfoItem* m_graphics;
};

#endif
