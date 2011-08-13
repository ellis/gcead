#ifndef __IDACDRIVERUSBES_H
#define __IDACDRIVERUSBES_H

#include <QPointer>
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
	explicit IdacDriverUsbEs(IdacDriverUsb* driver, const QVector<IdacChannelSettings>& channelSettings);

// Functions used in AutoSpike, in alphabetical order
public:
	const IdacCaps* IdacCapabilities() const { return m_proxy->caps(); }
	/// Get number of samples available
	int IdacDataAvail() const { return m_driver->IdacDataAvail(); }
	/// Turn channel on/off
	bool IdacEnableChannel(int iChan, bool bEnabled);
	virtual int IdacGetChannelDecimation(int iChan) const;
	/// Get internal DSP data (IDAC2, IDAC3)
	int IdacGetDSPInfo(uchar* pBuffer, int nSignalCount, int nBufCount) const;
	const QList<int>& IdacGetRanges() const { return m_proxy->ranges(); }
	const QStringList& IdacHighPassStrings() const { return m_proxy->lowcutStrings(); }
	bool IdacIsChannelEnabled(int iChan) const;
	/// Set the high-pass index
	bool IdacHighPass(int iChan, int index);
	int IdacLibVersion() const { return IDAC_LIB_VERSION; }
	QString IdacLock(const QString& sUserName);
	const QStringList& IdacLowPassStrings() { return m_proxy->highcutStrings(); }
	int IdacNrOfAnChannelEnabled() const;
	void IdacPowerDown();
	/// Returns 1 when USB device driver is found
	bool IdacPresent(int nAddress);
	/// Set the range for the given channel
	bool IdacScaleRange(int iChan, int index);
	void IdacSetBufferEvent(int hEvent);
	bool IdacSetDecimation(int iChan, int nDecimation);
	bool IdacSetOffsetAnalogIn(int iChan, int nOffset);
	bool IdacSmpStart();
	bool IdacSmpStop();
	void IdacUnlock();

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
	virtual int IdacGetDSPFirmwareVersion() const;
	virtual int IdacGetDigSampleBaseRate() const;
	virtual int IdacGetNrOfAnalogOutChan() const;
	virtual int IdacGetNrOfDigitalOutChan() const;
	virtual int IdacGetSampleBandwidth() const;
	virtual bool IdacHasOutput() const;
	virtual bool IdacIsOutputRunning() const;
	virtual CDD32_SAMPLE* IdacLockReadBuffer(int* pnCount);
	virtual bool IdacLowPass(int iChan, int index) const;
	virtual bool IdacNotch(int iChan, bool bActivate);
	virtual bool IdacStartOutput();
	virtual bool IdacStopOutput();
	virtual bool IdacTuneBoard();
	virtual int IdacType() const;
	virtual void IdacUnlockReadBuffer();
	virtual bool IdacZeroPulse(int iChan);

protected:
	bool isValidChannel(int iChan) const { return iChan >= 0 && iChan <= m_channelSettings.size(); }
	int channelCount() const { return m_channelSettings.size(); }
	IdacChannelSettings* channelSettings(int iChan);
	const IdacChannelSettings* channelSettings(int iChan) const;

protected:
	// FIXME: this should be replaced with a list of allowable sampling frequencies in IdacDriver -- ellis, 2011-08-13
	virtual int decimationCount() const = 0;
	virtual bool isValidAudioChannel(int iChan) const = 0;
	virtual bool isValidDigitalChannel(int iChan) const = 0;

private:
	QPointer<IdacDriverUsb> m_driver;
	QVector<IdacChannelSettings> m_channelSettings;
	QPointer<IdacDriverManager> m_manager;
	QPointer<IdacProxy> m_proxy;
};

#endif
