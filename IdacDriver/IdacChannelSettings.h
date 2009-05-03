/**
 * Copyright (C) 2008,2009  Ellis Whitehead
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

#ifndef __IDACCHANNELSETTINGS_H
#define __IDACCHANNELSETTINGS_H


class IdacChannelSettings
{
public:
	IdacChannelSettings()
	{
		mEnabled = 0;
		mInvert = 0;
		nDecimation = -1;
		iRange = -1;
		iLowcut = -1;
		iHighcut = -1;
		nOffset = 0;
		nExternalAmplification = 1;
	}

	/// Mask for enabling this channel.
	/// For analog channels, only the first bit is relevant.
	/// For the digital channel, each bit represents a digital signal.
	uchar mEnabled;
	/// Make for inverting the signal upon recording.
	/// For analog channels, only the first bit is relevant.
	/// For the digital channel, each bit represents a digital signal.
	uchar mInvert;
	/// The "decimation" factor
	int nDecimation;
	/// Hardware-specific index for the range value
	int iRange;
	/// Hardware-specific index for lowcut/highpass filter
	int iLowcut;
	/// Hardware-specific index for highcut/lowpass filter
	int iHighcut;
	/// Hardware-specific raw value offset
	short nOffset;
	/// External amplification before being digitized
	int nExternalAmplification;

	// FIXME: figure out what "IdacZeroPulse" is! -- ellis, 2008-02-23
};

#endif
