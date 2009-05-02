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

#ifndef __IDACSETTINGS_H
#define __IDACSETTINGS_H


enum IdacState
{
	IdacState_NotAvailable,
	IdacState_Inactive,
	IdacState_Connected,
	IdacState_Receiving,
};


enum IdacError
{
	IdacError_SyncFail = 0x01,
	IdacError_HwOverflow = 0x02,
	IdacError_SwOverflow = 0x04,
	IdacError_CommErr = 0x08,
	IdacError_Timeout = 0x10,
};
Q_DECLARE_FLAGS(IdacErrors, IdacError);
Q_DECLARE_OPERATORS_FOR_FLAGS(IdacErrors);


class IdacChannelSettings
{
public:
	IdacChannelSettings()
	{
		bEnabled = false;
		bInvert = false;
		nDecimation = 960;
		iRange = 4;
		iLowpass = 10; // 3kHz on IDAC4
		iHighpass = 1; // 0.1 Hz on IDAC4
		nOffset = 0;
		nExternalAmplification = 10;
	}

	bool bEnabled;
	/// Invert the signal upon recording
	bool bInvert;
	/// The "decimation" factor
	int nDecimation;
	/// Hardware-specific index for the range value
	int iRange;
	/// Hardware-specific index for high-pass filter
	int iLowpass;
	/// Hardware-specific index for high-pass filter
	int iHighpass;
	/// Hardware-specific raw value offset
	short nOffset;
	/// External amplification before being digitized
	int nExternalAmplification;

	// FIXME: figure out what "IdacZeroPulse" is! -- ellis, 2008-02-23
};


class IdacSettings
{
public:
	/// Whether to record when trigger signal comes
	bool bRecordOnTrigger;
	/// Number of minutes to record for (0 = unlimited)
	int nRecordingDuration;
	/// Bitmask of the digital channels that are enabled
	/// A bit value of 1 means that the channel is inverted
	uchar nDigitalEnabledMask;
	/// Bitmask of the digital channels that are inverted
	/// A bit value of 1 means that the channel is inverted
	uchar nDigitalInversionMask;
	/// Preset delay for the FID signal in milliseconds (ms)
	int nGcDelay_ms;
	/// Settings for the individual channels
	IdacChannelSettings channels[3];
};


#endif
