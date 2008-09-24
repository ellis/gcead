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

// IdacTestDlg.h : header file
//

#if !defined(AFX_IDACTESTDLG_H__36F8F846_A8A6_11D3_8E2F_00409503D9B8__INCLUDED_)
#define AFX_IDACTESTDLG_H__36F8F846_A8A6_11D3_8E2F_00409503D9B8__INCLUDED_

#include <QWidget>

#include "SampleOutput.h"


class QComboBox;
class QLabel;
class QPushButton;

class IdacRecorder;
class IdacThread;


/////////////////////////////////////////////////////////////////////////////
// CIdacTestDlg dialog

class CIdacTestDlg : public QWidget
{
	Q_OBJECT

// Construction
public:
	CIdacTestDlg(QWidget* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CIdacTestDlg)
	//enum { IDD = IDD_IDACTEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIdacTestDlg)
	protected:
	//virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//HICON			m_hIcon;

	CSampleOutput*	m_cSamples[3];

	uint			m_uTimer;

public:

	// Generated message map functions
	//{{AFX_MSG(CIdacTestDlg)
	/*
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual bool OnInitDialog();
	virtual void OnOK();
	afx_msg void OnConnect();
	afx_msg void OnStart();
	afx_msg void OnChannel0();
	afx_msg void OnChannel1();
	afx_msg void OnChannel2();
	afx_msg void OnChDecimationChan0();
	afx_msg void OnChDecimationChan1();
	afx_msg void OnChDecimationChan2();
	afx_msg void OnScaleChan1();
	afx_msg void OnScaleChan2();
	afx_msg void OnZero1();
	afx_msg void OnZero2();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	*/

private slots:
	void on_thread_ready();
	
	void updateStatus();

	void OnTimer();
	void on_btnConnect_clicked();
	void on_btnStart_clicked();
	/*
	void OnChannel0();
	void OnChannel1();
	void OnChannel2();
	void OnChDecimationChan0();
	void OnChDecimationChan1();
	void OnChDecimationChan2();
	*/
	void on_cmbScale1_activated(int index);
	void on_cmbScale2_activated(int index);
	void OnZero1();
	void OnZero2();
	void OnClear();

private:
	void setupWidgets();


protected:
	void EnableChannelControls(bool bEnable);
	void showErrors();

private:
	QPushButton* m_btnStart;
	QPushButton* m_btnConnect;
	QPushButton* m_btnClear;
	QPushButton* m_btnQuit;

	QComboBox* m_cmbDec0;
	QComboBox* m_cmbDec1;
	QComboBox* m_cmbDec2;
	QComboBox* m_cmbScale1;
	QComboBox* m_cmbScale2;

	QLabel* m_lblType;
	QLabel* m_lblAddress;

	QTimer* m_timer;
	IdacThread* m_thread;
	IdacRecorder* m_recorder;

	bool m_bReportingError;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDACTESTDLG_H__36F8F846_A8A6_11D3_8E2F_00409503D9B8__INCLUDED_)
