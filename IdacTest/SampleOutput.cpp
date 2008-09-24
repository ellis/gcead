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

// SampleOutput.cpp : implementation file
//

//#include "IdacTest.h"
#include "SampleOutput.h"

#include <QPainter>


CSampleOutput::CSampleOutput(bool bDigital /*= false*/) : m_bDigital(bDigital)
{
	Clear();

	m_bDirty = false;
}

CSampleOutput::~CSampleOutput()
{
}


void CSampleOutput::AddValue(short nValue)
{
	for (int i = 0; i < 9; i++)
	{
		m_nOutValues[i] = m_nOutValues[i+1];
	}

	m_nOutValues[9] = nValue;

	m_bDirty = true;
}

void CSampleOutput::UpdateScreen()
{
	if (m_bDirty)
	{
		m_bDirty = false;
		update();
	}
}

/*
BEGIN_MESSAGE_MAP(CSampleOutput, QWidget)
	//{{AFX_MSG_MAP(CSampleOutput)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/

void CSampleOutput::paintEvent(QPaintEvent* e)
{
	QPainter p(this);

	//QFont* pPrevFont = dc.SelectObject(&m_cClientFont);

	for (int y = 0; y < 10; y++)
	{
		QString OutBuffer;

		if (m_bDigital)
		{
			unsigned wDig = (unsigned) m_nOutValues[y];
			for (int nBit = 0; nBit < 8; nBit++)
			{
				OutBuffer += (wDig & 0x1) ? char('0' + nBit) : '.';
				OutBuffer += ' ';
				wDig = wDig >> 1;
			}
		}
		else
		{
			OutBuffer += QString::number(m_nOutValues[y]);
		}

		p.drawText(5, 5 + 18*y, OutBuffer);
	}
}


void CSampleOutput::Clear()
{
	for (int i=0; i < 10; i++)
		m_nOutValues[i] = 0;

	m_bDirty = true;
}
