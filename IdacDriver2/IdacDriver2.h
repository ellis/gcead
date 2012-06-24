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

#include <IdacDriver/IdacDriverUsb24Base.h>
#include <IdacDriver/IdacSettings.h>


class IdacUsb;


class IdacDriver2 : public IdacDriverUsb24Base
{
public:
	IdacDriver2(UsbDevice* device, UsbHandle* handle, QObject* parent = NULL);
	~IdacDriver2();

// Implement IdacDriver
public:
	void loadCaps(IdacCaps* caps);
	const QVector<IdacChannelSettings>& defaultChannelSettings() const { return m_defaultChannelSettings; }

	void initUsbFirmware();
	void initDataFirmware();

	bool startSampling();
	void configureChannel(int iChan);

public:
	bool setPowerOn(bool bOn);
	bool sendChannelSettings();
	// Temporarily turn sampling off (if we're sampling) in order to change settings
	bool setSamplingPaused(bool bPause);
	/// Activate / deactivate isochrone transfer (SUPPINT.H)
	bool setIntXferEnabled(bool bEnabled);

// IdacDriverWithThread overrides
protected:
	void sampleLoop();

// Methods for IdacDriver2Es
private:
	friend class IdacDriver2Es;
	bool IdacZeroPulse(int iChan);

private:
	void initStringsAndRanges();

private:
	QVector<IdacChannelSettings> m_defaultChannelSettings;
	char m_nVersion;
	//ConfigData m_config;

	bool m_bSamplingPaused;
};

#endif
