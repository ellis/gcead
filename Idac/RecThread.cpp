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

#include "RecThread.h"

//#include "EHandler.h"
//#include "reginfo.h"

ThreadInfo :: ThreadInfo()
{
	// this function is called only once
	int Cnt; 

	// data
	m_NmbrEnabled = -1;
	m_DigitalEnable = false;
	for (Cnt = 0; Cnt < MAXSIGNALS; Cnt++)
	{
		m_SigData[Cnt] = NULL;	
		m_WindowArea[Cnt] = CRect(0, 0, 0, 0);	
	}
	m_DigData = NULL;
	m_Synchronize = false;

	// state
	m_StateTable = TableRecording;	
	m_RecState = RECSTATE_PRETRIG;

	// data
	m_TriggerUp = false;
	m_TotRecSamples = 0;
	m_ArmDePressed = false;
	m_ArmPressed = false;

}

void ThreadInfo :: Reset()
{
	// flags
	m_THCPressed = false;
	m_ClearPressed = false;
	m_SavePressed = false;
	m_ScaleChange = false;
	m_Init = false;		
	m_Boot = false;
	m_Close = false;

//	CMultiRecDialogBar *pBar = MAINFRAME->MultiRecDialogBar();

	bool bAutoTrigEnabled	= HEvent.DoEvent(EVENT_REPORT_AUTOTRIG_STATUS,0,0);

	for (int Cnt = 0; Cnt < MAXSIGNALS; Cnt++)
	{
//		if(pBar->m_AutoTrigBtn.GetSafeHwnd())
			m_UseATrigger[Cnt] =  bAutoTrigEnabled ? RegInfo.m_SigAutoTrigger[Cnt] : false;
//		else
//			m_UseATrigger[Cnt] = false;
	}

	m_Trigger = false;
	m_TriggerLevel = false;
	m_MaxRecTime = false;
	m_HistClear = false;
	m_HistZoomIn = false;
	m_HistZoomOut = false;

	// data
	m_GetStartTime = false;
	m_RecState = RECSTATE_PRETRIG;		
	m_DigSamplesInv = 0;
}

