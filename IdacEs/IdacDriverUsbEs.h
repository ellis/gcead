#ifndef __IDACDRIVERUSBES_H
#define __IDACDRIVERUSBES_H

#include <QStringList>

#include <IdacDriver/IdacDriverUsb.h>
#include <IdacDriver/Sample.h>
#include <Idac/IdacDriverManager.h>


/// Interface to the IDAC functions required for Engineering Spirit's IDAC32.dll
class IdacDriverUsbEs
{
public:
	static const int IDAC_LIB_VERSION = 7;

public:
	explicit IdacDriverUsbEs();

// Functions used in AutoSpike, in alphabetical order
public:
	const IdacCaps* IdacCapabilities() const { return m_manager->caps(); }
	const QList<int>& IdacGetRanges() const;
	const QStringList& IdacHighPassStrings() const;
	int IdacLibVersion() const { return IDAC_LIB_VERSION; }
	const QStringList& IdacLowPassStrings();

// Functions used in AutoSpike, in alphabetical order
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
	IdacDriverManager* m_manager;
};

#endif
