#ifndef __IDACDRIVERUSBES_H
#define __IDACDRIVERUSBES_H

#include <QStringList>

#include "IdacDriverUsb.h"
#include "Sample.h"
#include <Idac/IdacDriverManager.h>
#include <Idac/IdacProxy.h>


/// Interface to the IDAC functions required for Engineering Spirit's IDAC32.dll
class IdacDriverUsbEs
{
public:
	static const int IDAC_LIB_VERSION = 7;

public:
	explicit IdacDriverUsbEs(IdacDriverUsb* driver);

// Functions used in AutoSpike, in alphabetical order
public:
	const IdacCaps* IdacCapabilities() const { return m_manager->caps(); }
	/// Get number of samples available
	int IdacDataAvail() const { return m_driver->IdacDataAvail(); }
	/// Turn channel on/off
	bool IdacEnableChannel(int iChan, bool bEnabled);
	const QList<int>& IdacGetRanges() const;
	/// Get internal DSP data (IDAC2, IDAC3)
	int IdacGetDSPInfo(uchar* pBuffer, int nSignalCount, int nBufCount) const;
	const QStringList& IdacHighPassStrings() const;
	int IdacLibVersion() const { return IDAC_LIB_VERSION; }
	QString IdacLock(const QString& sUserName);
	const QStringList& IdacLowPassStrings();
	int IdacNrOfAnChannelEnabled() const;
	/// Returns 1 when USB device driver is found
	bool IdacPresent(int nAddress);
	/// Set the range for the given channel
	bool IdacScaleRange(int iChan, int index);
	void IdacSetBufferEvent(int hEvent);
	bool IdacSetDecimation(int iChan, int nDecimation);
	bool IdacSetOffsetAnalogIn(int iChan, int nOffset);

// Functions used in AutoSpike, in alphabetical order
public:
	virtual bool IdacAudio(int iChan, bool bActivate);
	virtual int IdacBoot(const QString& sFilename, int nAddress);
	virtual bool IdacEag(int iChan, bool bActivate);
	/// Returns the available addresses
	virtual long* IdacGetAddressTable() const;
	virtual double IdacGetAnSampleBaseRate(int nChannels) const;
	virtual double IdacGetChannelBaseRate(int iChan) const;
	virtual int IdacGetChannelCount() const;
	virtual int IdacGetChannelDecimation(int iChan) const;
	virtual int IdacGetDSPFirmwareVersion() const;
	virtual int IdacGetDigSampleBaseRate() const;
	virtual int IdacGetNrOfAnalogOutChan() const;
	virtual int IdacGetNrOfDigitalOutChan() const;
	virtual int IdacGetSampleBandwidth() const;
	virtual bool IdacHasOutput() const;
	virtual bool IdacHighPass(int iChan, int index) const;
	virtual bool IdacIsChannelEnabled(int iChan) const;
	virtual bool IdacIsOutputRunning() const;
	virtual CDD32_SAMPLE* IdacLockReadBuffer(int* pnCount);
	virtual bool IdacLowPass(int iChan, int index) const;
	virtual bool IdacNotch(int iChan, bool bActivate);
	virtual void IdacPowerDown();
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
	IdacDriverManager* m_manager;
	IdacProxy* m_proxy;
	IdacDriverUsb* m_driver;
};

#endif
