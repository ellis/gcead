#ifndef __IDACDRIVERUSBES_H
#define __IDACDRIVERUSBES_H

#include <QPointer>
#include <QStringList>

#include "IdacDriverUsb.h"
#include "Sample.h"
#include <Idac/IdacDriverManager.h>
#include <Idac/IdacProxy.h>


#define IDAC_LIB_VERSION 7


/// Interface to the IDAC functions required for Engineering Spirit's IDAC32.dll
class IdacDriverUsbEs
{
public:
	explicit IdacDriverUsbEs(IdacDriverUsb* driver, const QVector<IdacChannelSettings>& channelSettings);

// Functions used in AutoSpike, in alphabetical order
public:
	/// Get number of samples available
	int IdacDataAvail() const { return m_driver->IdacDataAvail(); }
	/// Turn channel on/off
	bool IdacEnableChannel(int iChan, bool bEnabled);
	virtual int IdacGetChannelDecimation(int iChan) const;
	/// Get internal DSP data (IDAC2, IDAC3)
	int IdacGetDSPInfo() const;
	const QList<int>& IdacGetRanges() const { return m_proxy->ranges(); }
	/// Set the high-pass index
	bool IdacHighPass(int iChan, int index);
	const QStringList& IdacHighPassStrings() const { return m_proxy->lowcutStrings(); }
	bool IdacIsChannelEnabled(int iChan) const;
	int IdacLibVersion() const { return IDAC_LIB_VERSION; }
	QString IdacLock(const QString& sUserName);
	bool IdacLowPass(int iChan, int index);
	const QStringList& IdacLowPassStrings() const { return m_proxy->highcutStrings(); }
	int IdacNrOfAnChannelEnabled() const;
	void IdacPowerDown();
	/// Returns 1 when USB device driver is found
	bool IdacPresent(int nAddress);
	/// Set the range for the given channel
	bool IdacScaleRange(int iChan, int index);
	void IdacSetBufferEvent(void* hEvent);
	bool IdacSetDecimation(int iChan, int nDecimation);
	bool IdacSetOffsetAnalogIn(int iChan, int nOffset);
	bool IdacSmpStart();
	bool IdacSmpStop();
	void IdacUnlock();

// Functions used in AutoSpike, in alphabetical order
public:
	virtual bool IdacAudio(int iChan, bool bActivate) = 0;
	virtual int IdacBoot(const QString& sFilename, int nAddress) = 0;
	/// Returns true if the requested feature is supported
	virtual bool IdacCapabilities(int iCap) const = 0;
	virtual bool IdacEag(int iChan, bool bActivate) = 0;
	/// Returns the available addresses
	virtual long* IdacGetAddressTable() const = 0;
	virtual double IdacGetAnSampleBaseRate(int nChannels) const = 0;
	virtual double IdacGetChannelBaseRate(int iChan) const = 0;
	virtual int IdacGetChannelCount() const = 0;
	virtual int IdacGetDSPFirmwareVersion() const = 0;
	virtual int IdacGetDigSampleBaseRate() const = 0;
	virtual int IdacGetNrOfAnalogOutChan() const = 0;
	virtual int IdacGetNrOfDigitalOutChan() const = 0;
	virtual int IdacGetSampleBandwidth() const = 0;
	virtual bool IdacHasOutput() const = 0;
	virtual bool IdacIsOutputRunning() const = 0;
	virtual CDD32_SAMPLE* IdacLockReadBuffer(int* pnCount) = 0;
	virtual bool IdacNotch(int iChan, bool bActivate) = 0;
	virtual bool IdacStartOutput() = 0;
	virtual bool IdacStopOutput() = 0;
	virtual bool IdacTuneBoard() = 0;
	virtual int IdacType() const = 0;
	virtual void IdacUnlockReadBuffer() = 0;
	virtual bool IdacZeroPulse(int iChan) = 0;

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
