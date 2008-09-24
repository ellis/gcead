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

#ifndef __PUBLISHERWIDGET_H
#define __PUBLISHERWIDGET_H

#include <QWidget>

#include "TaskWidget.h"


class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;

class ChartPixmapParams;
class ChartWidget;
class EadFile;
class GroupWidget;
class MainScope;


class TaskPublishWidget : public TaskWidget
{
	Q_OBJECT
public:
	TaskPublishWidget(MainScope* scope, QWidget* parent = NULL);

	void setChartWidget(ChartWidget* chart);

signals:
	void settingsChanged();

private:
	void setupWidgets();
	void updateWidgets();
	void setupChartPixmapParams(ChartPixmapParams& params, const QSize& size);

private slots:
	void on_chkGrid_clicked();
	void on_chkTime_clicked();
	void on_chkNames_clicked();
	void on_chkComments_clicked();
	void on_chkPeaks_clicked();
	void on_chkWidth_clicked();
	void on_edtWidth_valueChanged(int n);
	void on_chkTimebase_clicked();
	void on_edtTimebase_valueChanged(int n);

	void on_chkColor_clicked();
	void on_btnBorderColor_clicked();

	void on_rdoOutputType_clicked();

	void on_chkSize_clicked();
	void on_edtSize_valueChanged(int n);
	void on_btnSave_clicked();
	void on_btnClipboard_clicked();

	void on_btnPreview_clicked();
	void on_btnPrint_clicked();

	void on_preview_paintRequested(QPrinter* printer);

private:
	MainScope* m_scope;

	ChartWidget* m_chart;

	QCheckBox* m_chkGrid;
	QCheckBox* m_chkTime;
	QCheckBox* m_chkNames;
	QCheckBox* m_chkComments;
	QCheckBox* m_chkPeaks;
	QCheckBox* m_chkWidth;
	QSpinBox* m_edtWidth;
	QCheckBox* m_chkTimebase;
	QSpinBox* m_edtTimebase;

	QCheckBox* m_chkColor;
	QPushButton* m_btnBorderColor;

	QRadioButton* m_rdoOutputFile;
	QRadioButton* m_rdoOutputPrinter;

	GroupWidget* m_grpFile;
	QCheckBox* m_chkSize;
	QSpinBox* m_edtSize;
	QPushButton* m_btnSave;
	QPushButton* m_btnClipboard;

	GroupWidget* m_grpPrinter;
	QLineEdit* m_edtTitle;
};

#endif
