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

#ifndef __IDACDRIVER4_H
#define __IDACDRIVER4_H

#include <QtGlobal> // for quint8 and related types

#include <IdacDriver/IdacDriverUsb.h>
#include <IdacDriver/IdacSettings.h>


struct usb_device;

class IdacUsb;


class IdacDriver4 : public IdacDriverUsb
{
public:
	/// Number of channels for IDAC4
	static const int IDAC_CHANNELCOUNT = 5;

public:
	IdacDriver4(struct usb_device* device, QObject* parent = NULL);
	~IdacDriver4();

// Implement IdacDriver
public:
	bool checkUsbFirmwareReady();
	bool checkDataFirmwareReady();

	void initUsbFirmware();
	void initDataFirmware();

	void loadDefaultChannelSettings(IdacChannelSettings* channels);
	void configureChannel(int iChan);

	bool startSampling();
	void stopSampling();

	int takeData(short* digital, short* analog1, short* analog2, int maxSize);

public:
	bool power(bool bOn);
	bool resetFpga();
	bool enableNetFrequency(bool bOn);
	bool fetchDeviceStatus();
	bool fetchConfig();
	bool IdacSetAdcZero(int iChan, int iOffset);
	bool IdacTuneNotch(int iChan, quint8 Offset);
	bool setChannelLowPass(int iChan, int index);
	bool setChannelHighPass(int iChan, int index);
	bool setChannelOffsetAnalogIn(int iChan, quint32 Offset);
	bool setChannelEnabled(int iChan, bool bEnabled);
	// Turn sample generation on/off
	bool setSamplingEnabled(bool bEnabled);
	// Sets the decimation factor for the given analog channel
	bool setChannelDecimation(int iChan, int nDecimation, bool bRecordSetting);
	/// This function will set the range for the given channel
	void setChannelRange(int iChan, int Index);
	/// Activate / deactivate isochrone transfer (SUPPINT.H)
	void setIsoXferEnabled(bool bEnabled);

private:
	struct ConfigData
	{
		short version;
		short inputZeroAdjust[IDAC_CHANNELCOUNT-1];	// Analog channels from (zero-based index)
		quint8 adcZeroAdjust[IDAC_CHANNELCOUNT-1];	// Analog channels from (zero-based index)
		quint8 notchAdjust[IDAC_CHANNELCOUNT-1];	// Analog channels from (zero-based index)
	};

	/// Box string programming
	enum BOXINDEX
	{
		BI_ADC_ZERO_ADJUST = 0,
		BI_NOTCH_ADJUST,
		BI_AUDIO_SWITCH,
		BI_LOWPASS_ADJUST,
		BI_SCALE_ADJUST,
		BI_HIGHPASS_ADJUST,
		BI_SETZERO_SWITCH,
		BI_EAG_SWITCH,
		BI_NOTCH_SWITCH,

		BI_COUNT
	};

	struct BitPosition
	{
		qint32	Position;
		qint32	Length;
		bool	bReverse;
	};

private:
	void initStringsAndRanges();
	bool claim();
	/// Updates analog input stage of specific channel
	bool UpdateAnalogIn(int iChan, BOXINDEX Bi, quint32 nValue);
	void SetBoxBits(int iChan, BOXINDEX Bi, quint32 nValue);
	/// Set box string to defaults (IDACINT.H)
	void ResetBoxString();
	/// Send box string to IDAC (IDACINT.H)
	bool OutputBoxBits(int iChan);

	void sampleStart();
	void sampleInit();
	void sampleLoop();

private:
	static BitPosition bpIdacBox[BI_COUNT];

	bool m_bPowerOn;
	bool m_bFpgaProgrammed;
	quint8 m_nVersion;
	ConfigData m_config;

	bool m_bSampling;
};

#endif
