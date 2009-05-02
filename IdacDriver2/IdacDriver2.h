/**
 * Copyright (C) 2009  Ellis Whitehead
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

#ifndef __IDACDRIVER2_H
#define __IDACDRIVER2_H

#include <QtGlobal> // for quint8 and related types

#include <IdacDriver/IdacDriverUsb.h>
#include <IdacDriver/IdacSettings.h>


struct usb_device;

class IdacUsb;


class IdacDriver2 : public IdacDriverUsb
{
public:
	/// Number of channels for IDAC4
	static const int IDAC_CHANNELCOUNT = 5;

public:
	IdacDriver2(struct usb_device* device, QObject* parent = NULL);
	~IdacDriver2();

// Implement IdacDriver
public:
	bool checkUsbFirmwareReady();
	bool checkDataFirmwareReady();

	void initUsbFirmware();
	void initDataFirmware();

	void loadDefaultChannelSettings(IdacChannelSettings* channels);

	bool startSampling();
	void stopSampling();
	void configureChannel(int iChan);

	int takeData(short* digital, short* analog1, short* analog2, int maxSize);

public:
	bool setPowerOn(bool bOn);
	bool sendChannelSettings();
	// Temporarily turn sampling off (if we're sampling) in order to change settings
	bool setSamplingPaused(bool bPause);
	/// Activate / deactivate isochrone transfer (SUPPINT.H)
	bool setIsoXferEnabled(bool bEnabled);

private:
	struct ConfigData
	{
		short version;
		short inputZeroAdjust[IDAC_CHANNELCOUNT-1];	// Analog channels from (zero-based index)
		quint8 adcZeroAdjust[IDAC_CHANNELCOUNT-1];	// Analog channels from (zero-based index)
		quint8 notchAdjust[IDAC_CHANNELCOUNT-1];	// Analog channels from (zero-based index)
	};

private:
	void initStringsAndRanges();
	bool claim();

	void sampleStart();
	void sampleLoop();

private:
	bool m_bFpgaProgrammed;
	char m_nVersion;
	ConfigData m_config;

	bool m_bSampling;
	bool m_bSamplingPaused;
};

#endif
