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

#if !defined(AFX_SAMPLEOUTPUT_H__36F8F84F_A8A6_11D3_8E2F_00409503D9B8__INCLUDED_)
#define AFX_SAMPLEOUTPUT_H__36F8F84F_A8A6_11D3_8E2F_00409503D9B8__INCLUDED_

#include <QWidget>


class CSampleOutput : public QWidget
{
protected:
	const bool m_bDigital;

// Construction
public:
	CSampleOutput(bool bDigital = false);

// Attributes
public:
	short		m_nOutValues[10];

	bool		m_bDirty;

// Operations
public:
	void		AddValue(short nValue);
	void		UpdateScreen();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleOutput)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Clear();
	virtual		~CSampleOutput();

	// Generated message map functions
protected:
	/*
	//{{AFX_MSG(CSampleOutput)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg bool OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	*/
	void paintEvent(QPaintEvent* e);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPLEOUTPUT_H__36F8F84F_A8A6_11D3_8E2F_00409503D9B8__INCLUDED_)
