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

/********************************************************************************/
/* Header file for shared use between IDACDLL and thunking agent (IDAC32TH.DLL)	*/
/********************************************************************************/

// Note: this file will ONLY use generic data types that are identical
//		 between WIN16 and WIN32

#ifndef IPC_H
#define IPC_H


// Interdependent values for IPC
const DWORD	IPC_TIMER_INTERVAL	= 100;		// 100 ms
const WORD	IPC_MAX_BUFFERSIZE	= 0xFF00;	// nearly 64k
const DWORD IPC_BUFFERTIME		= 2000;		// store data up to 2 secs (2000 ms)

const WORD	IPC_MAX_BUFFERCOUNT = IPC_MAX_BUFFERSIZE / sizeof(CDD32_SAMPLE);


// Message IDs for WM_COPYDATA from IDACDLL.DLL to IDAC32TH.DLL
// ------------------------------------------------------------

// Initialization functions
const DWORD CD_INITIALIZE			= 0x00;

// Board functions
const DWORD	CD_IDACPRESENT			= 0x10;
const DWORD	CD_BOOT					= 0x11;

// Filtering, scaling and utilities
const DWORD CD_ENABLECHANNEL		= 0x20;
const DWORD	CD_ZEROPULSE			= 0x21;
const DWORD	CD_OFFSET				= 0x22;
const DWORD	CD_LOWPASS				= 0x23;
const DWORD	CD_HIGHPASS				= 0x24;
const DWORD	CD_SCALERANGE			= 0x25;
const DWORD	CD_NOTCH				= 0x26;
const DWORD	CD_AUDIO				= 0x27;
const DWORD	CD_SETDECIMATION		= 0x28;
const DWORD CD_GETCHANNELBASERATE	= 0x29;

// Data acquisition
const DWORD	CD_SMPSTART				= 0x30;
const DWORD	CD_SMPSTOP				= 0x31;
const DWORD CD_GETSTATUSFLAGS		= 0x32;


// Struct definitions for passing data from IDACDLL.DLL to IDAC32TH.DLL
// --------------------------------------------------------------------

#pragma pack(1)		// Must be byte-aligned

// These structs will fill the lpData member of the packet

typedef struct CDD_INITIALIZE
{
	WORD	bInitialize;
} FAR* LPCDD_INITIALIZE;

typedef struct CDD_IDACPRESENT
{
	SHORT	nAddress;
} FAR* LPCDD_IDACPRESENT;

typedef struct CDD_BOOT
{
	SHORT	nAddress;
	char	szFileName[128];
} FAR* LPCDD_BOOT;

typedef struct CDD_ENABLECHANNEL
{
	WORD	wChan;
	WORD	bEnable;
} FAR* LPCDD_ENABLECHANNEL;

typedef struct CDD_ZEROPULSE
{
	WORD	wChan;
} FAR* LPCDD_ZEROPULSE;

typedef struct CDD_OFFSET
{
	WORD	wChan;
	SHORT	nOffset;
} FAR* LPCDD_OFFSET;

typedef struct CDD_LOWPASS
{
	WORD	wChan;
	WORD	wIndex;
} FAR* LPCDD_LOWPASS;

typedef struct CDD_HIGHPASS
{
	WORD	wChan;
	WORD	wIndex;
} FAR* LPCDD_HIGHPASS;

typedef struct CDD_SCALERANGE
{
	WORD	wChan;
	WORD	wIndex;
} FAR* LPCDD_SCALERANGE;

typedef struct CDD_NOTCH
{
	WORD	wChan;
	WORD	bActivate;
} FAR* LPCDD_NOTCH;

typedef struct CDD_AUDIO
{
	WORD	wChan;
	WORD	bActivate;
} FAR* LPCDD_AUDIO;

typedef struct CDD_SETDECIMATION
{
	WORD	wChan;
	DWORD	dwDecimation;
} FAR* LPCDD_SETDECIMATION;

typedef struct CDD_GETCHANNELBASERATE
{
	WORD	wChan;
} FAR* LPCDD_GETCHANNELBASERATE;

typedef struct CDD_GETSTATUSFLAGS
{
	WORD	bClear;
} FAR* LPCDD_GETSTATUSFLAGS;

#pragma pack()


#endif //IPC_H
