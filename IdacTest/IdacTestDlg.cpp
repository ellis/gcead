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

#include "IdacTestDlg.h"

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include <IdacControl/Idacpc.h>
#include <IdacStart.h>
#include <Idac/IdacRecorder.h>
#include <Idac/IdacThread.h>


CIdacTestDlg::CIdacTestDlg(QWidget* pParent)
:	QWidget(pParent)
{
	//m_recorder = new IdacRecorder(this);
	m_recorder = NULL;
	m_thread = new IdacThread(this);
	connect(m_thread, SIGNAL(ready()), this, SLOT(on_thread_ready()));
	m_thread->start();

	m_bReportingError = false;

	setupWidgets();
}

void CIdacTestDlg::setupWidgets()
{
	QHBoxLayout* l1 = new QHBoxLayout;
	QGridLayout* l2 = new QGridLayout;
	QVBoxLayout* l3 = new QVBoxLayout;

	l1->addLayout(l2);
	l1->addLayout(l3);

	setLayout(l1);

	QComboBox* cmb;

	cmb = new QComboBox;
	cmb->addItems(QStringList() << "1" << "2" << "5" << "10" << "20" << "50" << "100" << "200" << "500" << "1000" << "2000" << "5000" << "10000");
	cmb->setCurrentIndex(6);
	l2->addWidget(cmb, 1, 0);
	m_cmbDec0 = cmb;

	cmb = new QComboBox;
	cmb->addItems(QStringList() << "1" << "2" << "5" << "10" << "20" << "50" << "100" << "200" << "500" << "1000" << "2000" << "5000" << "10000");
	cmb->setCurrentIndex(6);
	l2->addWidget(cmb, 1, 2);
	m_cmbDec1 = cmb;

	cmb = new QComboBox;
	cmb->addItems(QStringList() << "1" << "2" << "5" << "10" << "20" << "50" << "100" << "200" << "500" << "1000" << "2000" << "5000" << "10000");
	cmb->setCurrentIndex(6);
	l2->addWidget(cmb, 1, 4);
	m_cmbDec2 = cmb;

	// Setup "Range" (scale) comboboxes
	cmb = new QComboBox;
	l2->addWidget(cmb, 1, 3);
	m_cmbScale1 = cmb;
	connect(m_cmbScale1, SIGNAL(activated(int)), this, SLOT(on_cmbScale1_activated(int)));

	cmb = new QComboBox;
	l2->addWidget(cmb, 1, 5);
	m_cmbScale2 = cmb;
	connect(m_cmbScale2, SIGNAL(activated(int)), this, SLOT(on_cmbScale2_activated(int)));

	// Setup "Range" items
	LPDWORD pRanges = IdacGetRanges();
	for (LPDWORD pTest = pRanges; *pTest != -1; pTest++)
	{
		QString s = QString::number(*pTest);
		m_cmbScale1->addItem(s);
		m_cmbScale2->addItem(s);
	}
	m_cmbScale1->setCurrentIndex(0);
	m_cmbScale2->setCurrentIndex(0);

	m_btnStart = new QPushButton(tr("Start"));
	m_btnStart->setEnabled(false);
	l3->addWidget(m_btnStart);
	connect(m_btnStart, SIGNAL(clicked()), this, SLOT(on_btnStart_clicked()));
	
	m_btnConnect = new QPushButton(tr("Connect"));
	l3->addWidget(m_btnConnect);
	connect(m_btnConnect, SIGNAL(clicked()), this, SLOT(on_btnConnect_clicked()));
	
	m_btnClear = new QPushButton(tr("Clear"));
	l3->addWidget(m_btnClear);
	
	m_btnQuit = new QPushButton(tr("Quit"));
	l3->addWidget(m_btnQuit);

	// Setup the Data groupbox
	QGroupBox* group = new QGroupBox(tr("Data"));
	l3->addWidget(group);
	QVBoxLayout* l4 = new QVBoxLayout();
	group->setLayout(l4);
	l4->addWidget(new QLabel(tr("Board Type:")));
	m_lblType = new QLabel("...");
	l4->addWidget(m_lblType);
	l4->addWidget(new QLabel(tr("Address:")));
	m_lblAddress = new QLabel("...");
	l4->addWidget(m_lblAddress);
	

/*
BEGIN_MESSAGE_MAP(CIdacTestDlg, CDialog)
	//{{AFX_MSG_MAP(CIdacTestDlg)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_CHECK0, OnChannel0)
	ON_BN_CLICKED(IDC_CHECK1, OnChannel1)
	ON_BN_CLICKED(IDC_CHECK2, OnChannel2)
	ON_CBN_SELCHANGE(IDC_DEC_CHAN0, OnChDecimationChan0)
	ON_CBN_SELCHANGE(IDC_DEC_CHAN1, OnChDecimationChan1)
	ON_CBN_SELCHANGE(IDC_DEC_CHAN2, OnChDecimationChan2)
	ON_CBN_SELCHANGE(IDC_SCALE_CHAN1, OnScaleChan1)
	ON_CBN_SELCHANGE(IDC_SCALE_CHAN2, OnScaleChan2)
	ON_BN_CLICKED(IDC_ZERO1, OnZero1)
	ON_BN_CLICKED(IDC_ZERO2, OnZero2)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/

	m_cSamples[0] = new CSampleOutput(true);
	l2->addWidget(m_cSamples[0], 2, 0, 1, 2);

	m_cSamples[1] = new CSampleOutput(false);
	l2->addWidget(m_cSamples[1], 2, 2, 1, 2);

	m_cSamples[2] = new CSampleOutput(false);
	l2->addWidget(m_cSamples[2], 2, 4, 1, 2);
	/*
	m_cSamples[0].Create(NULL,
					   NULL,
					   WS_BORDER|WS_CHILD|WS_VISIBLE,
					   CRect(23, 90, 188, 300),
					   this,
					   -1,
					   NULL);

	m_cSamples[1].Create(NULL,
					   NULL,
					   WS_BORDER|WS_CHILD|WS_VISIBLE,
					   CRect(210, 90, 375, 300),
					   this,
					   -1,
					   NULL);

	m_cSamples[2].Create(NULL,
					   NULL,
					   WS_BORDER|WS_CHILD|WS_VISIBLE,
					   CRect(398, 90, 562, 300),
					   this,
					   -1,
					   NULL);
	*/

	QString pszText;
	switch (IdacType())
	{
	case IDAC_TYPE_NONE:
		pszText = "None (dummy DLL)";
		break;

	case IDAC_TYPE_4:
		pszText = "USB 4";
		break;

	case IDAC_TYPE_ISA_V2:
		pszText = "ISA (v2)";
		break;

	case IDAC_TYPE_USB_V3:
		pszText = "USB (v3)";
		break;

	case IDAC_TYPE_SERIAL:
		pszText = "Serial (DAQ)";
		break;

	default:
		pszText = "** Invalid DLL response **";
		break;
	}

	m_lblType->setText(pszText);

	// Start timer to collect data from DLL
	m_timer = new QTimer(this);
	m_timer->start(200);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
}

/*
void CIdacTestDlg::OnDestroy() 
{
	if (m_uTimer) KillTimer(m_uTimer);

	if (m_bConnected) OnConnect();

	CDialog::OnDestroy();
}
*/

void CIdacTestDlg::showErrors()
{
	QMessageBox::critical(this, tr("Error"), m_thread->recorder()->errors().join("\n"));
}

void CIdacTestDlg::on_thread_ready()
{
	m_recorder = m_thread->recorder();
	connect(m_recorder, SIGNAL(statusChanged()), this, SLOT(updateStatus()));
}

void CIdacTestDlg::updateStatus()
{
	// FIXME: who errors when relevant
	//if (!m_recorder->connect())
	//	showErrors();

	if (m_recorder == NULL)
	{
		m_btnConnect->setText(tr("&Connect"));
		m_btnStart->setText(tr("&Start"));
		m_btnStart->setEnabled(false);
		m_lblAddress->setText(tr("---"));
		EnableChannelControls(true);
	}
	else if (!m_recorder->isConnected())
	{
		m_btnConnect->setText(tr("&Connect"));
		m_btnStart->setText(tr("&Start"));
		m_btnStart->setEnabled(false);
		m_lblAddress->setText(tr("---"));
	}
	else if (!m_recorder->isRecording())
	{
		m_btnConnect->setText(tr("&Disconnect"));
		m_btnStart->setText(tr("&Start"));
		m_btnStart->setEnabled(true);
		m_lblAddress->setText(QString::number(m_recorder->address(), 16));
	}
	else
	{
		m_btnConnect->setText(tr("&Disconnect"));
		m_btnStart->setText(tr("&Stop"));
		m_btnStart->setEnabled(true);
		m_lblAddress->setText(QString::number(m_recorder->address(), 16));
	}
}

void CIdacTestDlg::OnTimer()
{
	if (m_thread->recorder() == NULL)
		return;

	//m_thread->on_timer_timeout();

	// 1. Check IDAC status
	if (!m_bReportingError)
	{
		m_bReportingError = true;

		//if (cdErr.bSyncFail || cdErr.bHwOverflow	|| cdErr.bSwOverflow || cdErr.bCommErr || cdErr.bTimeout)
		//if (cdErr.bSyncFail || cdErr.bHwOverflow || cdErr.bSwOverflow || cdErr.bCommErr)
		if (m_thread->recorder()->errorFlags() != 0)
		{
			QString csMessage;

			// FIXME:
			csMessage = "Some kind of error, Dude.\n";
			/*
			if (cdErr.bSyncFail) csMessage		= "Sync error\r\n";
			if (cdErr.bHwOverflow) csMessage	+= "Hardware buffer overflow\r\n";
			if (cdErr.bSwOverflow) csMessage	+= "Software buffer overflow\r\n";
			if (cdErr.bCommErr) csMessage		+= "General communication error\r\n";
			if (cdErr.bTimeout) csMessage		+= "Communication timeout\r\n";
			*/
			//ENDFIX

			csMessage += "Press Cancel to stop.";

			QMessageBox::StandardButton nResult = QMessageBox::warning(this, "Communication Error", csMessage, QMessageBox::Ok | QMessageBox::Cancel);

			if (nResult == QMessageBox::Cancel)
			{
				// Stop sampling
				on_btnStart_clicked();
			}
		}

		m_bReportingError = false;
	}

	// 2. Read data and send to display
	for (int iChannel = 0; iChannel < 3; iChannel++)
	{
		QList<short> data = m_thread->recorder()->takeChannelData(iChannel);
		for (int i = 0; i < data.size(); i++)
			m_cSamples[iChannel]->AddValue(data[i]);
		m_cSamples[iChannel]->update();
	}
}

void CIdacTestDlg::on_btnConnect_clicked()
{
	if (!m_recorder->isConnected())
	{
		QMetaObject::invokeMethod(m_recorder, "connect");
		OnClear();
	}
	else
		QMetaObject::invokeMethod(m_recorder, "disconnect");
}

void CIdacTestDlg::on_btnStart_clicked()
{
	if (!m_recorder->isRecording())
	{
		IdacSettings settings;
		settings.channels[0].bEnabled = true;
		settings.channels[0].nDecimation = m_cmbDec0->currentText().toInt();
		settings.channels[1].bEnabled = true;
		settings.channels[1].nDecimation = m_cmbDec1->currentText().toInt();
		settings.channels[1].iRange = m_cmbScale1->currentIndex();
		settings.channels[2].bEnabled = true;
		settings.channels[2].nDecimation = m_cmbDec2->currentText().toInt();
		settings.channels[2].iRange = m_cmbScale2->currentIndex();
		m_recorder->setSettings(settings);

		QMetaObject::invokeMethod(m_recorder, "record");
	}
	else
		QMetaObject::invokeMethod(m_recorder, "stop");
}

/*
void CIdacTestDlg::OnChannel0() 
{
	//CButton* pCheck = (CButton*) GetDlgItem(IDC_CHECK0);
	bool bCheck = true; // FIXME: pCheck->GetCheck();

	IdacEnableChannel(0, bCheck);
}

void CIdacTestDlg::OnChannel1() 
{
	//CButton* pCheck = (CButton*) GetDlgItem(IDC_CHECK1);
	bool bCheck = true; // FIXME: pCheck->GetCheck();

	IdacEnableChannel(1, bCheck);
}

void CIdacTestDlg::OnChannel2() 
{
	//CButton* pCheck = (CButton*) GetDlgItem(IDC_CHECK2);
	bool bCheck = true; // FIXME: pCheck->GetCheck();

	IdacEnableChannel(2, bCheck);
}

void CIdacTestDlg::OnChDecimationChan0() 
{
	QComboBox* cmb = m_cmbDec0;
	QString s = cmb->currentText();
	int n = s.toInt();
	IdacSetDecimation(0, n);
}

void CIdacTestDlg::OnChDecimationChan1() 
{
	QComboBox* cmb = m_cmbDec1;
	QString s = cmb->currentText();
	int n = s.toInt();
	IdacSetDecimation(1, n);
}

void CIdacTestDlg::OnChDecimationChan2() 
{
	QComboBox* cmb = m_cmbDec2;
	QString s = cmb->currentText();
	int n = s.toInt();
	IdacSetDecimation(2, n);
}
*/

void CIdacTestDlg::on_cmbScale1_activated(int index)
{
	IdacScaleRange(1, index);
}

void CIdacTestDlg::on_cmbScale2_activated(int index)
{
	IdacScaleRange(2, index);
}

void CIdacTestDlg::OnZero1() 
{
	IdacZeroPulse(1);
}

void CIdacTestDlg::OnZero2() 
{
	IdacZeroPulse(2);
}

void CIdacTestDlg::EnableChannelControls(bool bEnable)
{
	// FIXME: GetDlgItem(IDC_CHECK0)->EnableWindow(bEnable);
	// FIXME: GetDlgItem(IDC_CHECK1)->EnableWindow(bEnable);
	// FIXME: GetDlgItem(IDC_CHECK2)->EnableWindow(bEnable);
	m_cmbDec0->setEnabled(bEnable);
	m_cmbDec1->setEnabled(bEnable);
	m_cmbDec2->setEnabled(bEnable);
	m_cmbScale1->setEnabled(bEnable);
	m_cmbScale2->setEnabled(bEnable);
}

void CIdacTestDlg::OnClear()
{
	m_cSamples[0]->Clear();
	m_cSamples[1]->Clear();
	m_cSamples[2]->Clear();
	m_cSamples[0]->update();
	m_cSamples[1]->update();
	m_cSamples[2]->update();
}
