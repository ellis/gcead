/**
 * Copyright (C) 2010  Ellis Whitehead
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

#ifndef __IdacDriverES_H
#define __IdacDriverES_H

#include <QtGlobal> // for quint8 and related types

#include <IdacDriver/IdacDriverWithThread.h>


class IdacDriverES : public IdacDriverWithThread
{
	Q_OBJECT
public:
	IdacDriverES(QObject* parent = NULL);
	~IdacDriverES();

// IdacDriver overrides
public:
	/// Load up the capabilities of the current driver
	virtual void loadCaps(IdacCaps* caps);
	/// Load up default channel settings for the current driver
	virtual void loadDefaultChannelSettings(IdacChannelSettings* channels);

	virtual bool checkUsbFirmwareReady();
	virtual bool checkDataFirmwareReady();

	virtual void initUsbFirmware();
	virtual void initDataFirmware();

	virtual bool startSampling();
	virtual void configureChannel(int iChan);

// IdacDriverWithThread overrides
protected:
	virtual void sampleLoop();

private:
	bool boot();
	bool boot_2000_4(const QString& sType);
	bool boot_2_USB();
	bool boot_ISA_USB();
	int bootAtAddress(int addr, int nChannels);
	QString getBinFileName(int nSignals);
	void grabDataFromDll();

private:
	bool m_bFirmwareSent;
	bool m_bIdac2;
	bool m_bIdac4;
};

#endif
