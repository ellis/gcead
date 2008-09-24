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

#ifndef SAMPLE_H
#define SAMPLE_H

#ifdef Q_OS_WIN
#include <windows.h> // For BYTE, WORD, etc
#else
#include "windows_types.h"
#endif

// The data that gets copied back will have a simple definition:
// dwData: channel number
// cbData: size in bytes of the sent data (multiple of 6)
// lpData: a pointer to structs of data that go with every sample

#pragma pack(1)

// Sample struct layout (32 bits):
// 16 bits		- signed analog word or unsigned digital value
//				  Analog data with less than 16 bits will be shifted to 16-bit
// 8 bits		- channel number
// 8 bits		- status (error) flags


typedef struct CDD32_STATUS
{
	union
	{
		BYTE	uStatus;
		struct
		{
		 
			BYTE	bSwOverflow		: 1;
			BYTE	bHwOverflow		: 1;
			BYTE	bCommErr		: 1;
			BYTE	bTimeout		: 1;
			BYTE	bSyncFail		: 1;
			// "BYTE" forces bitfield into 8 bits total instead of 16/32
		};
	};
} FAR* LPCDD32_STATUS;

const	CDD32_STATUS	NULL_CDD32_STATUS = { 0 };


typedef struct CDD32_SAMPLE
{
	union
	{
		DWORD				dwData;					// All data combined in 1 DWORD

		struct
		{
			union
			{
				SHORT		nSample;				// Value as signed
				WORD		wDigitalSample;			// Value as unsigned
			};

			BYTE			uChannel;				// Channel number

			CDD32_STATUS	flags;					// Status flags
		};
	};
		
} FAR* LPCDD32_SAMPLE;

const	CDD32_SAMPLE	NULL_CDD32_SAMPLE = { 0L };

#pragma pack()

#endif // SAMPLE_H
