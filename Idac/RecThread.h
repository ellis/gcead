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

#pragma once

#ifndef __AFXEXT_H__
//#include <afxext.h>
#endif

#include <windows.h> // for HANDLE

#include "RecordState.h"
#include "DeBounce.h"
#include "Callback.h"	//For method
#include "../IdacStart.h"	//For MAXSIGNALS
//#include "appdata.h"	//For NR_SIGNALS
#include "WaveData.h"
//#include "DigData.h"
//#include "EHandler.h"
#include "IdacDefines.h"


class QThread;

class CDigitalData;

//static uint RecordingThread(LPVOID pParam);

typedef struct 
{
	RecordState	m_CurState;		// the current state 
	method		m_CondFunc;		// pointer to condition function
	method		m_ActionFunc;	// pointer to action function
	RecordState	m_NextState;	// the next state
}
StateTableEntry;

extern StateTableEntry	TableRecording[]; //

class ThreadInfo
{
// Members
public:
	// thread
	QThread*			m_RecThread;		// thread
	//DWORD				m_RecThreadID;		// thread ID

	// flags
	bool				m_THCPressed;		// THC button is pressed, clear after reading
	bool				m_ClearPressed;		// The CLEAR button is pressed.
	bool				m_ArmPressed;		//the ARM button is pressed, clear after reading
	bool				m_ArmDePressed;		//indicate is the ARM Button is DePressed or not
	bool				m_SavePressed;		// Save button is pressed, clear after reading
	bool				m_ScaleChange;		// amplitude is changed, clear after reading

	bool				m_Init;				// Initialize the thread, clear after reading	
	bool				m_Boot;				// boot the idac for the thread
	bool				m_Close;			// close the thread, clear after reading

	bool				m_UseATrigger[MAXSIGNALS];	// use auto trigger for three signals
	CDeBounce			m_TriggerDebounce;	// Trigger input level debouncer
	bool				m_Trigger;			// trigger is detected, clear after reading
	bool				m_TriggerUp;		// last trigger value
	bool				m_TriggerLevel;		// trigger level is reached, clear after reading
	bool				m_MaxRecTime;		// maximum recording time reached clear after reading

	bool				m_HistClear;		// clear the histogram, clear after reading
	bool				m_HistZoomIn;		// zoom in histogram, clear after reading
	bool				m_HistZoomOut;		// zoom out histogram, clear after reading

	// state
	RecordState			m_RecState;			// current record state
	StateTableEntry*	m_StateTable;		// table containing the states

	// MS: data event
	HANDLE				m_hDataReady;		// data event for Idac

	// data
	uint				m_NmbrEnabled;		// number currently enabled signals
	CWaveData*			m_SigData[MAXSIGNALS];		// the three signals
	CDigitalData*		m_DigData;			// the digital data
	bool				m_DigitalEnable;	// digital enabling
	CRect				m_WindowArea[MAXSIGNALS];	// signal window area
	bool				m_Synchronize;		// synchronize during sampling
	bool				m_GetStartTime;		// get start time for clock
	int					m_CurSignal;		// current signal when sampling
	_int64				m_TotRecSamples;	// total recorded samples
	DWORD				m_TimeTick;			// used for drawing the time each second

	short				m_DigPolarity;		// Complement mask for the polarity setting
	int					m_DigSamplesInv;	// digital samples to invert when the the trigger button is pessed

// Constructor/Destructor
public:
	ThreadInfo();

// Operations
public:
	void Reset();
};