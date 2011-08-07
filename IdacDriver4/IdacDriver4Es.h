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

#include <IdacDriver/IdacDriverUsbEs.h>
#include <IdacDriver/IdacSettings.h>
#include "IdacDriver4.h"


struct usb_device;

class IdacUsb;


class IdacDriver4 : public IdacDriverUsb
{
public:
	/// Number of channels for IDAC4
	static const int IDAC_CHANNELCOUNT = 5;

public:
	IdacDriver4(IdacDriver4* driver);

// Overrides for IdacDriverUsbEs
public:
	virtual bool IdacAudio(int iChan, bool bActivate);
	virtual int IdacBoot(const QString& sFilename, int nAddress);
	virtual int IdacDataAvail();
	virtual bool IdacEag(int iChan, bool bActivate);
	virtual bool IdacEnableChannel(int iChan, bool bEnabled);
	virtual long* IdacGetAddressTable() const;
	virtual double IdacGetAnSampleBaseRate(int nChannels) const;
	virtual double IdacGetChannelBaseRate(int iChan) const;
	virtual int IdacGetChannelCount() const;
	virtual int IdacGetChannelDecimation(int iChan) const;
	virtual int IdacGetDSPFirmwareVersion() const;
	virtual int IdacGetDSPInfo(uchar* pBuffer, int nSignalCount, int nBufCount) const;
	virtual int IdacGetDigSampleBaseRate() const;
	virtual int IdacGetNrOfAnalogOutChan() const;
	virtual int IdacGetNrOfDigitalOutChan() const;
	virtual int IdacGetSampleBandwidth() const;
	virtual bool IdacHasOutput() const;
	virtual bool IdacHighPass(int iChan, int index) const;
	virtual bool IdacIsChannelEnabled(int iChan) const;
	virtual bool IdacIsOutputRunning() const;
	virtual int IdacLibVersion() const;
	virtual QString IdacLock(const QString& sUserName);
	virtual LPCDD32_SAMPLE IdacLockReadBuffer(int* pnCount);
	virtual bool IdacLowPass(int iChan, int index) const;
	virtual bool IdacNotch(int iChan, bool bActivate);
	virtual uint IdacNrOfAnChannelEnabled() const;
	virtual void IdacPowerDown();
	virtual int IdacPresent(int nAddress);
	virtual CDD32_SAMPLE IdacRead();
	virtual bool IdacScaleRange(int iChan, int index);
	virtual void IdacSetBufferEvent(int hEvent);
	virtual bool IdacSetDecimation(int iChan, int nDecimation);
	virtual bool IdacSetOffsetAnalogIn(int iChan, int nOffset);
	virtual bool IdacSmpStart();
	virtual bool IdacSmpStop();
	virtual bool IdacStartOutput();
	virtual bool IdacStopOutput();
	virtual bool IdacTuneBoard();
	virtual int IdacType() const;
	virtual void IdacUnlock();
	virtual void IdacUnlockReadBuffer();
	virtual bool IdacZeroPulse(int iChan);

private:
	IdacDriver4* const m_driver;
};

#endif
