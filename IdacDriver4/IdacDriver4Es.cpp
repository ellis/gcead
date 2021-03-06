#include "IdacDriver4Es.h"

#include <Check.h>

#include <IdacDriver/IdacSettings.h>


#define IDAC_DECIMATIONCOUNT 65535


IdacDriver4Es::IdacDriver4Es(IdacDriver4* driver, const QVector<IdacChannelSettings>& channelSettings)
	: IdacDriverUsbEs(driver, channelSettings),
	m_driver(driver)
{
}

bool IdacDriver4Es::IdacAudio(int iChan, bool bActivate) { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacBoot(const QString& sFilename, int nAddress) { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacDataAvail() { return false; } // FIXME: implement -- ellis, 2010-06-13

bool IdacDriver4Es::IdacCapabilities(int iCap) const {
	static bool CapTbl[] =
	{
        true,	// IDAC_CAN_SCALE
        true,	// IDAC_HAS_LOWPASS
        true,	// IDAC_HAS_HIGHPASS
        true,	// IDAC_HAS_NOTCH
        true,	// IDAC_HAS_AUDIOOUT
        true,	// IDAC_HAS_DIGITALOUT
        true,	// IDAC_HAS_ANALOGOUT
        true,	// IDAC_HAS_ZEROPULSE
        true,	// IDAC_HAS_INPUTOFFSET
        true,	// IDAC_CAN_TUNE
        true	// IDAC_HAS_EAG
	};

	CHECK_PARAM_RETVAL(iCap >= 0 && iCap < int(sizeof(CapTbl)/sizeof(CapTbl[0])), false);

	return CapTbl[iCap];
}

bool IdacDriver4Es::IdacEag(int iChan, bool bActivate) { return false; } // FIXME: implement -- ellis, 2010-06-13
long* IdacDriver4Es::IdacGetAddressTable() const { return false; } // FIXME: implement -- ellis, 2010-06-13
double IdacDriver4Es::IdacGetAnSampleBaseRate(int nChannels) const { return false; } // FIXME: implement -- ellis, 2010-06-13
double IdacDriver4Es::IdacGetChannelBaseRate(int iChan) const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetChannelCount() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetChannelDecimation(int iChan) const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetDSPFirmwareVersion() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetDigSampleBaseRate() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetNrOfAnalogOutChan() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetNrOfDigitalOutChan() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetSampleBandwidth() const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacHasOutput() const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacHighPass(int iChan, int index) const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacIsChannelEnabled(int iChan) const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacIsOutputRunning() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacLibVersion() const { return false; } // FIXME: implement -- ellis, 2010-06-13
LPCDD32_SAMPLE IdacDriver4Es::IdacLockReadBuffer(int* pnCount) { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacNotch(int iChan, bool bActivate) { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacStartOutput() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacStopOutput() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacTuneBoard() { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacType() const { return -1; } // FIXME: implement -- ellis, 2010-06-13
void IdacDriver4Es::IdacUnlockReadBuffer() { return; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacZeroPulse(int iChan) { return false; } // FIXME: implement -- ellis, 2010-06-13


int IdacDriver4Es::decimationCount() const {
	return IDAC_DECIMATIONCOUNT;
}

bool IdacDriver4Es::isValidAudioChannel(int iChan) const {
	return (iChan >= 1 && iChan <= 4);
}

bool IdacDriver4Es::isValidDigitalChannel(int iChan) const {
	return (iChan == 0);
}
