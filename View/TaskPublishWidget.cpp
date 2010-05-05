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

#include "TaskPublishWidget.h"

#include <QtDebug>
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QSvgGenerator>
#include <QTextDocument>
#if QT_VERSION >= 0x040400 // Necessary until I upgrade to a linux version with Qt4.4 -- ellis, 2008-06-22
#include <QPrintPreviewDialog>
#endif

#include <Widgets/GroupWidget.h>

#include "ChartPixmap.h"
#include <ChartScope.h>
#include "ChartWidget.h"
#include "Check.h"
#include <MainScope.h>
#include "TaskWidget.h"
#include "Globals.h"
#include "PublisherSettings.h"
#include <Utils.h>


TaskPublishWidget::TaskPublishWidget(MainScope* scope, QWidget* parent)
	: TaskWidget(parent)
{
	m_scope = scope;

	m_chart = NULL;
	setupWidgets();
	updateWidgets();
}


void TaskPublishWidget::setChartWidget(ChartWidget* chart)
{
	m_chart = chart;
}

void TaskPublishWidget::setupWidgets()
{
	QGridLayout* grid;
	int iRow = 0;

	QColor clrGroup(0xc0, 0xff, 0xc0);

	//
	// Items for "Chart" group
	//
	
	m_chkGrid = new QCheckBox(tr("Grid lines"));
	m_chkGrid->setChecked(Globals->publisherSettings()->publisherChartElements.testFlag(ChartElement_Grid));
	connect(m_chkGrid, SIGNAL(clicked()), this, SLOT(on_chkGrid_clicked()));

	m_chkTime = new QCheckBox(tr("Axis time"));
	m_chkTime->setChecked(Globals->publisherSettings()->publisherChartElements.testFlag(ChartElement_AxisTime));
	connect(m_chkTime, SIGNAL(clicked()), this, SLOT(on_chkTime_clicked()));

	m_chkNames = new QCheckBox(tr("Wave names"));
	m_chkNames->setChecked(Globals->publisherSettings()->publisherChartElements.testFlag(ChartElement_WaveNames));
	connect(m_chkNames, SIGNAL(clicked()), this, SLOT(on_chkNames_clicked()));

	m_chkComments = new QCheckBox(tr("Wave comments"));
	m_chkComments->setChecked(Globals->publisherSettings()->publisherChartElements.testFlag(ChartElement_WaveComments));
	connect(m_chkComments, SIGNAL(clicked()), this, SLOT(on_chkComments_clicked()));

	m_chkMarkers = new QCheckBox(tr("Markers"));
	m_chkMarkers->setChecked(Globals->publisherSettings()->publisherChartElements.testFlag(ChartElement_Markers));
	connect(m_chkMarkers, SIGNAL(clicked()), this, SLOT(on_chkMarkers_clicked()));

	m_chkStdDev = new QCheckBox(tr("Standard deviation"));
	m_chkStdDev->setChecked(Globals->publisherSettings()->publisherChartElements.testFlag(ChartElement_StdDev));
	connect(m_chkStdDev, SIGNAL(clicked()), this, SLOT(on_chkStdDev_clicked()));

	m_chkWidth = new QCheckBox(tr("Specify width"));
	m_chkWidth->setChecked(Globals->publisherSettings()->bPublishCols);
	connect(m_chkWidth, SIGNAL(clicked()), this, SLOT(on_chkWidth_clicked()));

	m_edtWidth = new QSpinBox();
	m_edtWidth->setEnabled(m_chkWidth->isChecked());
	m_edtWidth->setRange(10, 20);
	m_edtWidth->setValue(Globals->publisherSettings()->nPublishCols);
	connect(m_edtWidth, SIGNAL(valueChanged(int)), this, SLOT(on_edtWidth_valueChanged(int)));

	m_chkTimebase = new QCheckBox(tr("Specify timebase"));
	m_chkTimebase->setChecked(Globals->publisherSettings()->bPublishTimebase);
	connect(m_chkTimebase, SIGNAL(clicked()), this, SLOT(on_chkTimebase_clicked()));

	m_edtTimebase = new QSpinBox();
	m_edtTimebase->setEnabled(m_chkTimebase->isChecked());
	m_edtTimebase->setRange(1, 20);
	m_edtTimebase->setValue(Globals->publisherSettings()->nPublishCols);
	m_edtTimebase->setSuffix("min");
	connect(m_edtTimebase, SIGNAL(valueChanged(int)), this, SLOT(on_edtTimebase_valueChanged(int)));

	// Setup groupbox for file output
	iRow = 0;
	GroupWidget* grpChart = new GroupWidget(clrGroup, tr("Chart Settings"));
	grid = new QGridLayout();
	grid->addWidget(m_chkGrid, iRow++, 0, 1, 2);
	grid->addWidget(m_chkTime, iRow++, 0, 1, 2);
	grid->addWidget(m_chkNames, iRow++, 0, 1, 2);
	grid->addWidget(m_chkComments, iRow++, 0, 1, 2);
	grid->addWidget(m_chkMarkers, iRow++, 0, 1, 2);
	grid->addWidget(m_chkStdDev, iRow++, 0, 1, 2);
	grid->addWidget(m_chkWidth, iRow, 0);
	grid->addWidget(m_edtWidth, iRow++, 1);
	grid->addWidget(m_chkTimebase, iRow, 0);
	grid->addWidget(m_edtTimebase, iRow++, 1);
	grpChart->setLayout(grid);

	//
	// Items for "Colors" group
	//
	
	GroupWidget* grpColor = new GroupWidget(clrGroup, tr("Colors"));
	
	m_chkColor = new QCheckBox(tr("Publish in color"));
	connect(m_chkColor, SIGNAL(clicked()), this, SLOT(on_chkColor_clicked()));
	
	m_btnBorderColor = new QPushButton();
	connect(m_btnBorderColor, SIGNAL(clicked()), this, SLOT(on_btnBorderColor_clicked()));

	iRow = 0;
	grid = new QGridLayout();
	grid->addWidget(m_chkColor, iRow++, 0, 1, 2);
	grid->addWidget(new QLabel(tr("Border:")), iRow, 0);
	grid->addWidget(m_btnBorderColor, iRow, 1);
	grpColor->setLayout(grid);

	//
	// Output type selection
	//

	QLabel* lblOutputType = new QLabel(tr("Output Type"));
	lblOutputType->setStyleSheet("font-weight: bold; font-size: 20px");
	lblOutputType->setAlignment(Qt::AlignCenter);
	//QFont fontOutputType = lblOutputType->font();
	//fontOutputType.set

	m_rdoOutputFile = new QRadioButton(tr("File"));
	m_rdoOutputFile->setChecked(true);
	m_rdoOutputFile->setIcon(QIcon(":/images/filesave-32x32.png"));
	connect(m_rdoOutputFile, SIGNAL(toggled(bool)), this, SLOT(on_rdoOutputType_clicked()));

	m_rdoOutputPrinter = new QRadioButton(tr("Printer"));
	m_rdoOutputPrinter->setIcon(QIcon(":/images/fileprint-32x32.png"));

	//
	// Items for "File output" group
	//

	m_chkSize = new QCheckBox(tr("Specify size (pixels)"));
	m_chkSize->setChecked(Globals->publisherSettings()->bPublishHeight);
	connect(m_chkSize, SIGNAL(clicked()), this, SLOT(on_chkSize_clicked()));

	m_edtSize = new QSpinBox();
	m_edtSize->setEnabled(m_chkSize->isChecked());
	m_edtSize->setRange(100, 2000);
	m_edtSize->setSingleStep(50);
	m_edtSize->setValue(Globals->publisherSettings()->nPublishSize);
	connect(m_edtSize, SIGNAL(valueChanged(int)), this, SLOT(on_edtSize_valueChanged(int)));

	m_btnSave = new QPushButton(QIcon(":/images/filesave-32x32.png"), tr("Save"));
	m_btnSave->setToolTip(tr("Save image to a file"));
	connect(m_btnSave, SIGNAL(clicked()), this, SLOT(on_btnSave_clicked()));

	m_btnClipboard = new QPushButton(QIcon(":/images/editcopy-22x22.png"), tr("Copy"));
	m_btnClipboard->setToolTip(tr("Copy image to clipboard"));
	connect(m_btnClipboard, SIGNAL(clicked()), this, SLOT(on_btnClipboard_clicked()));

	// Setup groupbox for file output
	iRow = 0;
	m_grpFile = new GroupWidget(clrGroup, tr("File output"));
	grid = new QGridLayout();
	QHBoxLayout* layoutSize = new QHBoxLayout;
	layoutSize->addWidget(m_chkSize);
	layoutSize->addWidget(m_edtSize);
	grid->addLayout(layoutSize, iRow++, 0, 1, 2);
	grid->addWidget(m_btnSave, iRow, 0);
	grid->addWidget(m_btnClipboard, iRow++, 1);
	m_grpFile->setLayout(grid);

	//
	// Items for "Printer" group
	//

	QLabel* lblTitle = new QLabel(tr("Page title:"));
	m_edtTitle = new QLineEdit();

	#if QT_VERSION >= 0x040400 // Necessary until I upgrade to a linux version with Qt4.4 -- ellis, 2008-06-22
	QPushButton* btnPreview = new QPushButton(QIcon(":/images/stock_print-preview-22x22.png"), tr("Preview"));
	connect(btnPreview, SIGNAL(clicked()), this, SLOT(on_btnPreview_clicked()));
	#endif

	QPushButton* btnPrint = new QPushButton(QIcon(":/images/fileprint-32x32.png"), tr("Print"));
	connect(btnPrint, SIGNAL(clicked()), this, SLOT(on_btnPrint_clicked()));

	// Setup groupbox for file output
	iRow = 0;
	m_grpPrinter = new GroupWidget(clrGroup, tr("Printer"));
	grid = new QGridLayout();
	grid->addWidget(lblTitle, iRow++, 0, 1, 2);
	grid->addWidget(m_edtTitle, iRow++, 0, 1, 2);
	#if QT_VERSION >= 0x040400 // Necessary until I upgrade to a linux version with Qt4.4 -- ellis, 2008-06-22
	grid->addWidget(btnPreview, iRow, 0);
	grid->addWidget(btnPrint, iRow++, 1);
	#else
	grid->addWidget(btnPrint, iRow++, 0);
	#endif
	m_grpPrinter->setLayout(grid);

	//
	// Top layout
	//

	m_grpPrinter->hide();

	QGridLayout* layout = new QGridLayout();
	layout->setAlignment(Qt::AlignTop);
	layout->setMargin(0);
	iRow = 0;
	layout->addWidget(grpChart, iRow++, 0, 1, 2);
	layout->addWidget(grpColor, iRow++, 0, 1, 2);
	layout->addWidget(lblOutputType, iRow++, 0, 1, 2);
	layout->addWidget(m_rdoOutputFile, iRow, 0, Qt::AlignRight);
	layout->addWidget(m_rdoOutputPrinter, iRow++, 1);
	layout->addWidget(m_grpFile, iRow++, 0, 1, 2);
	layout->addWidget(m_grpPrinter, iRow++, 0, 1, 2);
	setLayout(layout);
}

void TaskPublishWidget::updateWidgets()
{
	QString sColorFormat = "background-color: %0";

	const PublisherSettings* settings = Globals->publisherSettings();
	m_chkColor->setChecked(settings->bPublishInColor);
	m_btnBorderColor->setStyleSheet(QString(sColorFormat).arg(settings->clrPublishBorder.name()));
	m_edtWidth->setValue(settings->nPublishCols);
	m_edtTimebase->setValue(settings->nPublishTimebase);
}

void TaskPublishWidget::setupChartPixmapParams(ChartPixmapParams& params, const QSize& size)
{
	CHECK_PRECOND_RET(m_chart != NULL);

	params = m_scope->chart()->params();
	params.size = size;
}

void TaskPublishWidget::on_chkColor_clicked()
{
	Globals->publisherSettings()->bPublishInColor = m_chkColor->isChecked();
	emit settingsChanged();
}

void TaskPublishWidget::on_btnBorderColor_clicked()
{
	QColor clr = QColorDialog::getColor(Globals->publisherSettings()->clrPublishBorder, this);
	if (clr.isValid())
	{
		Globals->publisherSettings()->clrPublishBorder = clr;
		updateWidgets();
		emit settingsChanged();
	}
}

static void setChartElement(ChartElement element, bool b)
{
	if (b)
		Globals->publisherSettings()->publisherChartElements |= element;
	else
		Globals->publisherSettings()->publisherChartElements &= ~element;
}

void TaskPublishWidget::on_chkGrid_clicked()
{
	setChartElement(ChartElement_Grid, m_chkGrid->isChecked());
	emit settingsChanged();
}

void TaskPublishWidget::on_chkTime_clicked()
{
	setChartElement(ChartElement_AxisTime, m_chkTime->isChecked());
	emit settingsChanged();
}

void TaskPublishWidget::on_chkNames_clicked()
{
	setChartElement(ChartElement_WaveNames, m_chkNames->isChecked());
	emit settingsChanged();
}

void TaskPublishWidget::on_chkComments_clicked()
{
	setChartElement(ChartElement_WaveComments, m_chkComments->isChecked());
	emit settingsChanged();
}

void TaskPublishWidget::on_chkMarkers_clicked()
{
	setChartElement(ChartElement_Markers, m_chkMarkers->isChecked());
	emit settingsChanged();
}

void TaskPublishWidget::on_chkStdDev_clicked()
{
	setChartElement(ChartElement_StdDev, m_chkStdDev->isChecked());
	emit settingsChanged();
}

void TaskPublishWidget::on_chkWidth_clicked()
{
	bool b = m_chkWidth->isChecked();
	Globals->publisherSettings()->bPublishCols = b;
	m_edtWidth->setEnabled(b);
	emit settingsChanged();
}

void TaskPublishWidget::on_edtWidth_valueChanged(int n)
{
	Globals->publisherSettings()->nPublishCols = n;
	emit settingsChanged();
}

void TaskPublishWidget::on_chkTimebase_clicked()
{
	bool b = m_chkTimebase->isChecked();
	Globals->publisherSettings()->bPublishTimebase = b;
	m_edtTimebase->setEnabled(b);
	emit settingsChanged();
}

void TaskPublishWidget::on_edtTimebase_valueChanged(int n)
{
	Globals->publisherSettings()->nPublishTimebase = n;
	emit settingsChanged();
}

void TaskPublishWidget::on_rdoOutputType_clicked()
{
	bool bFile = m_rdoOutputFile->isChecked();
	m_grpFile->setVisible(bFile);
	m_grpPrinter->setVisible(!bFile);
}

void TaskPublishWidget::on_chkSize_clicked()
{
	bool b = m_chkSize->isChecked();
	Globals->publisherSettings()->bPublishHeight = b;
	m_edtSize->setEnabled(b);
	emit settingsChanged();
}

void TaskPublishWidget::on_edtSize_valueChanged(int n)
{
	Globals->publisherSettings()->nPublishSize = n;
	emit settingsChanged();
}

void TaskPublishWidget::on_btnSave_clicked()
{
	CHECK_PRECOND_RET(m_chart != NULL);

	QString sFilename = QFileDialog::getSaveFileName(this, QString(), QString(), tr("Image files (*.png *.jpg *.svg)"));
	if (sFilename.isEmpty())
		return;

	if (sFilename.toLower().endsWith(".svg"))
	{
		QSize sz = m_chart->size();
		QRect viewBox = QRect(0, 0, sz.width(), sz.height());
		QSvgGenerator svg;
		svg.setFileName(sFilename);
		//svg.setSize(m_chart->size());
		svg.setViewBox(viewBox);

		ChartPixmapParams params;
		setupChartPixmapParams(params, m_chart->size());

		ChartPixmap cp;
		cp.draw(&svg, params);
	}
	else
	{
		int nWidth = m_chart->width();
		int nHeight = m_chart->height();
		if (Globals->publisherSettings()->bPublishHeight && Globals->publisherSettings()->nPublishSize > 0)
		{
			double nFactor = double(Globals->publisherSettings()->nPublishSize) / nHeight;
			nWidth = int(nWidth * nFactor + 0.5);
			nHeight = Globals->publisherSettings()->nPublishSize;
		}
		QSize size(nWidth, nHeight);

		ChartPixmapParams params;
		setupChartPixmapParams(params, size);

		ChartPixmap cp;
		cp.draw(params);

		QRect rc = cp.borderRect();
		rc.adjust(-1, -1, 1, 1);
		QImage image = cp.pixmap().toImage().copy(rc);
		bool b = image.save(sFilename);
		if (b == false)
		{
			// TODO: check return value and display message box upon failure
		}
	}
}

void TaskPublishWidget::on_btnClipboard_clicked()
{
	CHECK_PRECOND_RET(m_chart != NULL);

	// REFACTOR: duplicate code
	int nWidth = m_chart->width();
	int nHeight = m_chart->height();
	if (Globals->publisherSettings()->nPublishSize > 0)
	{
		nWidth = 1000000;
		nHeight = Globals->publisherSettings()->nPublishSize;
	}
	QSize size(nWidth, nHeight);

	ChartPixmapParams params;
	setupChartPixmapParams(params, size);

	ChartPixmap cp;
	cp.draw(params);

	QRect rc = cp.borderRect();
	rc.adjust(-1, -1, 1, 1);
	QImage image = cp.pixmap().toImage().copy(rc);

	QApplication::clipboard()->setImage(image);
}

void TaskPublishWidget::on_btnPreview_clicked()
{
#if QT_VERSION >= 0x040400 // Necessary until I upgrade to a linux version with Qt4.4 -- ellis, 2008-06-22
	QPrinter printer(QPrinter::ScreenResolution);
	QPrintPreviewDialog dlg(&printer, this, Qt::WindowMaximizeButtonHint);
	connect(&dlg, SIGNAL(paintRequested(QPrinter*)), this, SLOT(on_preview_paintRequested(QPrinter*)));
	dlg.exec();
#endif
}

void TaskPublishWidget::on_btnPrint_clicked()
{
	QPrinter printer(QPrinter::ScreenResolution);
	QPrintDialog dlg(&printer, this);
	if (dlg.exec() == QDialog::Accepted)
	{
		on_preview_paintRequested(&printer);
	}
}

void TaskPublishWidget::on_preview_paintRequested(QPrinter* printer)
{
	printer->setFullPage(false);
#if QT_VERSION >= 0x040400 // Necessary until I upgrade to a linux version with Qt4.4 -- ellis, 2008-06-22
	printer->setPageMargins(25, 20, 25, 20, QPrinter::Millimeter);
#endif
	int nPageWidth = printer->pageRect().width();

	QPainter painter(printer);
	QFontMetrics fm = painter.fontMetrics();
	QRect rcCanvas = painter.viewport();
	
	int yChartTop = 0;
	
	//
	// Print the title
	//

	if (!m_edtTitle->text().isEmpty())
	{
		QFont font("Sans Serif", 16);
		font.setBold(true);
		painter.setFont(font);

		int nHeight = painter.fontMetrics().lineSpacing();
		QRect rc(0, yChartTop, nPageWidth, nHeight);
		painter.drawText(rc, Qt::AlignHCenter, m_edtTitle->text());
		
		yChartTop += nHeight * 2;
	}

	{
		QFont font("Sans Serif", 12);
		font.setBold(false);
		painter.setFont(font);

		int nHeight = painter.fontMetrics().lineSpacing();
		QRect rc(0, yChartTop, nPageWidth, nHeight);
		QString sFilename = QFileInfo(m_scope->file()->filename()).fileName();
		QString sText = tr("Filename: %0").arg(sFilename);
		painter.drawText(rc, Qt::AlignHCenter, sText);
		
		yChartTop += nHeight * 2;
	}
	
	//
	// Decide how big to print chart
	//

	//int nPrecent = Globals->publisherSettings()->nPublisherPercentSize;
	//int nWidth = qMin(nPageWidth, printer->pageRect().height()) * nPrecent / 100;

	QSize size(nPageWidth, printer->pageRect().height());
	ChartPixmapParams params;
	setupChartPixmapParams(params, size);

	int xPixmap = (nPageWidth - params.size.width()) / 2;
	int yPixmap = 0;

	ChartPixmap cp;
	painter.translate(xPixmap, yChartTop + yPixmap);
	cp.draw(painter, params);
	painter.translate(-xPixmap, -(yChartTop + yPixmap));

	//
	// Print information about the waves
	//

	int yText = yChartTop + cp.borderRect().height() + 20;

	QString sText;
	sText += tr("Timebase: %0/div<br><br>").arg(timestampString(params.nSecondsPerDivision));
	
	// Add the user's file comment
	if (!m_scope->comment().isEmpty())
		sText += m_scope->comment() + "<br><br>";
	
	// Print data for each wave
	foreach (ViewWaveInfo* vwi, params.view->allVwis())
	{
		const WaveInfo* wave = vwi->wave();
		if (wave != NULL && wave->display.size() > 0)
		{
			sText += tr("<b>%0</b>: %1 mV/div")
				.arg(wave->sName)
				.arg(vwi->voltsPerDivision());

			// Date and time of recording
			QDateTime datetime = wave->rec()->timeOfRecording();
			if (datetime.isValid())
			{
				QString sDate = datetime.date().toString(Qt::ISODate);
#if QT_VERSION >= 0x040400 // Necessary until I upgrade to a linux version with Qt4.4 -- ellis, 				QString sTime = datetime.time().toString(Qt::SystemLocaleShortDate);
				QString sTime = datetime.time().toString(Qt::SystemLocaleShortDate);
#else
				QString sTime = datetime.time().toString(Qt::SystemLocaleDate);
#endif
				sText += tr(", recorded on %0 at %1").arg(sDate).arg(sTime);
			}
			// Comments
			if (!wave->sComment.isEmpty())
				sText += tr(", comment: %0").arg(wave->sComment);

			// Newline
			sText += "<br>";
		}
	}

	QTextDocument text;
	QFont font("Sans Serif", 12);
	text.setDefaultFont(font);
	text.setTextWidth(nPageWidth);
	text.setHtml(sText);

	// Check height of wave info; Does wave info fit onto this page?
	// If not, then print wave info on a second page.
	int nTextHeight = (int) text.size().height();
	if (yText + nTextHeight > printer->pageRect().height())
	{
		printer->newPage();
		yText = 0;
	}

	painter.translate(0, yText);
	text.drawContents(&painter);
	painter.translate(0, -yText);

	painter.end();
}
