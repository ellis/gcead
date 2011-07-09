#include "IdacDriver2.h"

#include <Check.h>


#define IDAC_CHANNELCOUNT       3
#define IDAC_SMP_BANDWIDTH		500
#define IDAC_AN_SAMPLEBASE		100
#define IDAC_DIG_SAMPLEBASE		100
#define ANALOGOUT_CHANNELCOUNT  0
#define IDAC_DIGITAL_CHANNELCOUNT 0

/*
#define		MAX_INPUT_VOLTAGE_ADC	5000000		// full scale is +-5V

CConfigData			ConfigData;		// Handled by IdacConfigWrite/Read
CIdacStatusStruct	IdacStatus;
CIdacControlStruct	IdacControlStruct = {0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};

DWORD dwRangesList[IDAC_SCALERANGECOUNT+1] = {  MAX_INPUT_VOLTAGE_ADC / 1,
												MAX_INPUT_VOLTAGE_ADC / 4,
												MAX_INPUT_VOLTAGE_ADC / 16,
												MAX_INPUT_VOLTAGE_ADC / 64,
												MAX_INPUT_VOLTAGE_ADC / 256,
												MAX_INPUT_VOLTAGE_ADC / 1024,
												-1 };

// Addresses (IDACINT.H)
LONG			AddressList			[]	= { 1, -1 };


// Common internal data
// --------------------

// State machine (SUPPDATA.H)
// TODO check if digital channel must be inverted
CChannelState	IdacChannelState		(FALSE);		// Do invert digital data

// Buffer (SUPPDATA.H)
CIdacFifo		ChannelBuffer;

// Buffer event flag for caller process (SUPPDATA.H)
HANDLE			hBufferEvent			= NULL;

// Sampling parameters (SUPPINT.H)
BOOL	bIdacChannelEnabled	[]	= { FALSE, FALSE, FALSE};
WORD	wDecimations		[]	= { 1, 1, 1 };	// sample rate is fixed at 100hz
WORD	wScaleAmp			[]	= { 0, 0, 0 };

*/


bool IdacDriver2::IdacAudio(int /*iChan*/, bool /*bActivate*/) {
	return false;
}

int IdacDriver2::IdacBoot(const QString& sFilename, int nAddress) { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacDataAvail() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacEag(int iChan, bool bActivate) { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacEnableChannel(int iChan, bool bEnabled) { return false; } // FIXME: implement -- ellis, 2010-06-13
long* IdacDriver2::IdacGetAddressTable() const { return false; } // FIXME: implement -- ellis, 2010-06-13

double IdacDriver2::IdacGetAnSampleBaseRate(int nChannels) const {
	if (nChannels == 0)
		return 0.0;
	return (double) IDAC_AN_SAMPLEBASE;
}

double IdacDriver2::IdacGetChannelBaseRate(int iChan) const {
	CHECK_PARAM_RETVAL(iChan >= 0 && iChan < IDAC_CHANNELCOUNT, 0);

	const IdacChannelSettings* chan = desiredChannelSettings(iChan);
	if (chan->mEnabled == 0)
		return 0;
	// Digital (chan 0) is fixed
	else if (iChan == 0)
		return IDAC_DIG_SAMPLEBASE;
	else
		return IDAC_AN_SAMPLEBASE;
}

int IdacDriver2::IdacGetChannelCount() const {
	return IDAC_CHANNELCOUNT;
}

int IdacDriver2::IdacGetChannelDecimation(int iChan) const {
	CHECK_PARAM_RETVAL(iChan >= 0 && iChan < IDAC_CHANNELCOUNT, 0);
	const IdacChannelSettings* chan = desiredChannelSettings(iChan);
	return chan->nDecimation;
}

int IdacDriver2::IdacGetDSPFirmwareVersion() const {
	return 0;
}

int IdacDriver2::IdacGetDSPInfo(uchar* pBuffer, int nSignalCount, int nBufCount) const { return false; } // FIXME: implement -- ellis, 2010-06-13

int IdacDriver2::IdacGetDigSampleBaseRate() const {
	return IDAC_DIG_SAMPLEBASE;
}

int IdacDriver2::IdacGetNrOfAnalogOutChan() const {
	return ANALOGOUT_CHANNELCOUNT;
}

int IdacDriver2::IdacGetNrOfDigitalOutChan() const {
	return IDAC_DIGITAL_CHANNELCOUNT;
}

int IdacDriver2::IdacGetSampleBandwidth() const {
	return IDAC_SMP_BANDWIDTH;
}

bool IdacDriver2::IdacHasOutput() const { return false; } // FIXME: implement -- ellis, 2010-06-13

bool IdacDriver2::IdacHighPass(int iChan, int index) const {
	CHECK_PARAM_RETVAL(iChan >= 0 && iChan < IDAC_CHANNELCOUNT, false);
	const IdacChannelSettings* chan = desiredChannelSettings(iChan);
	if (iChan == 0 || index >= IDAC_HIGHPASSCOUNT)
		return false;
	...
}// FIXME: implement -- ellis, 2010-06-13

bool IdacDriver2::IdacIsChannelEnabled(int iChan) const {
	CHECK_PARAM_RETVAL(iChan >= 0 && iChan < IDAC_CHANNELCOUNT, false);
	const IdacChannelSettings* chan = desiredChannelSettings(iChan);
	return chan->mEnabled;
}

bool IdacDriver2::IdacIsOutputRunning() const { return false; } // FIXME: implement -- ellis, 2010-06-13
QString IdacDriver2::IdacLock(const QString& sUserName) { return false; } // FIXME: implement -- ellis, 2010-06-13
LPCDD32_SAMPLE IdacDriver2::IdacLockReadBuffer(int* pnCount) { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacLowPass(int iChan, int index) const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacNotch(int iChan, bool bActivate) { return false; } // FIXME: implement -- ellis, 2010-06-13
uint IdacDriver2::IdacNrOfAnChannelEnabled() const { return false; } // FIXME: implement -- ellis, 2010-06-13
void IdacDriver2::IdacPowerDown() { return; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacPresent(int nAddress) { return false; } // FIXME: implement -- ellis, 2010-06-13
CDD32_SAMPLE IdacDriver2::IdacRead() { return NULL_CDD32_SAMPLE; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacScaleRange(int iChan, int index) { return false; } // FIXME: implement -- ellis, 2010-06-13
void IdacDriver2::IdacSetBufferEvent(int hEvent) { return; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacSetDecimation(int iChan, int nDecimation) { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacSetOffsetAnalogIn(int iChan, int nOffset) { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacSmpStart() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacSmpStop() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacStartOutput() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacStopOutput() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacTuneBoard() { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacType() const { return -1; } // FIXME: implement -- ellis, 2010-06-13
void IdacDriver2::IdacUnlock() { return; } // FIXME: implement -- ellis, 2010-06-13
void IdacDriver2::IdacUnlockReadBuffer() { return; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacZeroPulse(int iChan) { return false; } // FIXME: implement -- ellis, 2010-06-13
