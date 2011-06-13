#ifndef __IDACDRIVERUSBES_H
#define __IDACDRIVERUSBES_H

#include "IdacDriverUsb.h"
#include "Sample.h"


class IdacDriverUsbEs : public IdacDriverUsb
{
    Q_OBJECT
public:
	explicit IdacDriverUsbEs(struct usb_device* device, QObject *parent = 0);

// Functions used in AutoSpike, in alphabetical order
public:
	const IdacCaps* IdacCapabilities() const { return caps(); }
	const QList<int>& IdacGetRanges() const { return ranges(); }
	const QStringList& IdacHighPassStrings() const { return lowcutStrings(); }
	const QStringList& IdacLowPassStrings() const { return highcutStrings(); }

// Functions used in AutoSpike, in alphabetical order
public:
	virtual bool IdacAudio(int iChan, bool bActivate) = 0;
	virtual int IdacBoot(const QString& sFilename, int nAddress) = 0;
	virtual int IdacDataAvail() = 0;
	virtual bool IdacEag(int iChan, bool bActivate) = 0;
	virtual bool IdacEnableChannel(int iChan, bool bEnabled) = 0;
	virtual long* IdacGetAddressTable() const = 0;
	virtual double IdacGetAnSampleBaseRate(int nChannels) const = 0;
	virtual double IdacGetChannelBaseRate(int iChan) const = 0;
	virtual int IdacGetChannelCount() const = 0;
	virtual int IdacGetChannelDecimation(int iChan) const = 0;
	virtual int IdacGetDSPFirmwareVersion() const = 0;
	virtual int IdacGetDSPInfo(uchar* pBuffer, int nSignalCount, int nBufCount) const = 0;
	virtual int IdacGetDigSampleBaseRate() const = 0;
	virtual int IdacGetNrOfAnalogOutChan() const = 0;
	virtual int IdacGetNrOfDigitalOutChan() const = 0;
	virtual int IdacGetSampleBandwidth() const = 0;
	virtual bool IdacHasOutput() const = 0;
	virtual bool IdacHighPass(int iChan, int index) const = 0;
	virtual bool IdacIsChannelEnabled(int iChan) const = 0;
	virtual bool IdacIsOutputRunning() const = 0;
	virtual int IdacLibVersion() const = 0;
	virtual QString IdacLock(const QString& sUserName) = 0;
	virtual LPCDD32_SAMPLE IdacLockReadBuffer(int* pnCount) = 0;
	virtual bool IdacLowPass(int iChan, int index) const = 0;
	virtual bool IdacNotch(int iChan, bool bActivate) = 0;
	virtual uint IdacNrOfAnChannelEnabled() const = 0;
	virtual void IdacPowerDown() = 0;
	virtual int IdacPresent(int nAddress) = 0;
	virtual CDD32_SAMPLE IdacRead() = 0;
	virtual bool IdacScaleRange(int iChan, int index) = 0;
	virtual void IdacSetBufferEvent(int hEvent) = 0;
	virtual bool IdacSetDecimation(int iChan, int nDecimation) = 0;
	virtual bool IdacSetOffsetAnalogIn(int iChan, int nOffset) = 0;
	virtual bool IdacSmpStart() = 0;
	virtual bool IdacSmpStop() = 0;
	virtual bool IdacStartOutput() = 0;
	virtual bool IdacStopOutput() = 0;
	virtual bool IdacTuneBoard() = 0;
	virtual int IdacType() const = 0;
	virtual void IdacUnlock() = 0;
	virtual void IdacUnlockReadBuffer() = 0;
	virtual bool IdacZeroPulse(int iChan) = 0;
};

#endif
