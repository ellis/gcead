#include "IdacDriver2Es.h"

#include <Check.h>

#include <IdacDriver/IdacSettings.h>
#include <IdacEs/IdacExports.h>


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
static long AddressList[] = { 1, -1 };
const int IDAC_HIGHPASSCOUNT = 9;
//const int HighPassTable[IDAC_HIGHPASSCOUNT] = {0, 8, 9, 10, 11, 12, 13, 14, 15};


bool IdacDriver2Es::IdacAudio(int /*iChan*/, bool /*bActivate*/) {
	return false;
}

int IdacDriver2Es::IdacBoot(const QString& /*sFilename*/, int /*nAddress*/) {
	m_driver->initDataFirmware();
	if (!m_driver->checkDataFirmwareReady())
		return 3;
	return 1; // success
}

bool IdacDriver2Es::IdacEag(int, bool) {
	return false;
}

long* IdacDriver2Es::IdacGetAddressTable() const {
	return AddressList;
}

double IdacDriver2Es::IdacGetAnSampleBaseRate(int nChannels) const {
	if (nChannels == 0)
		return 0.0;
	return (double) IDAC_AN_SAMPLEBASE;
}

double IdacDriver2Es::IdacGetChannelBaseRate(int iChan) const {
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

int IdacDriver2Es::IdacGetChannelCount() const {
	return IDAC_CHANNELCOUNT;
}

int IdacDriver2Es::IdacGetChannelDecimation(int iChan) const {
	CHECK_PARAM_RETVAL(iChan >= 0 && iChan < IDAC_CHANNELCOUNT, 0);
	const IdacChannelSettings* chan = desiredChannelSettings(iChan);
	return chan->nDecimation;
}

int IdacDriver2Es::IdacGetDSPFirmwareVersion() const {
	return 0;
}

int IdacDriver2Es::IdacGetDigSampleBaseRate() const {
	return IDAC_DIG_SAMPLEBASE;
}

int IdacDriver2Es::IdacGetNrOfAnalogOutChan() const {
	return ANALOGOUT_CHANNELCOUNT;
}

int IdacDriver2Es::IdacGetNrOfDigitalOutChan() const {
	return IDAC_DIGITAL_CHANNELCOUNT;
}

int IdacDriver2Es::IdacGetSampleBandwidth() const {
	return IDAC_SMP_BANDWIDTH;
}

bool IdacDriver2Es::IdacHasOutput() const {
	return false;
}

bool IdacDriver2Es::IdacHighPass(int iChan, int index) const {
	CHECK_PARAM_RETVAL(iChan >= 1 && iChan < IDAC_CHANNELCOUNT, false);
	CHECK_PARAM_RETVAL(index >= 0 && index < IDAC_HIGHPASSCOUNT, false);

	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	chan.iLowcut = index;
	m_proxy->resendChannelSettings(iChan, chan);
	return true;
}

bool IdacDriver2Es::IdacIsChannelEnabled(int iChan) const {
	CHECK_PARAM_RETVAL(iChan >= 0 && iChan < IDAC_CHANNELCOUNT, false);
	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	return chan->mEnabled;
}

bool IdacDriver2Es::IdacIsOutputRunning() const {
	return false;
}

CDD32_SAMPLE* IdacDriver2Es::IdacLockReadBuffer(int* pnCount) {
	const int N = 1000;
	static CDD32_SAMPLE samples[N * 3];
	static short digital[N];
	static short analog1[N];
	static short analog2[N];

	const int n = m_driver->takeData(digital, analog1, analog2, N);
	if (n > 0) {
		int iSample = 0;
		for (int i = 0; i < n; i++) {
			CDD32_SAMPLE* pSample = samples[iSample];
			pSample->flags = 0;
			pSample->uChannel = 0;
			pSample->dwData = digital[i];
			pSample->flags = 0;
			pSample->uChannel = 1;
			pSample->dwData = analog1[i];
			pSample->flags = 0;
			pSample->uChannel = 2;
			pSample->dwData = analog2[i];
			iSample += 3;
		}
		*pnCount = iSample;
		return &sample;
	}

	*pnCount = 0;
	return NULL;
}

bool IdacDriver2Es::IdacLowPass(int, int) const {
	return false;
}

bool IdacDriver2Es::IdacNotch(int, bool) {
	return false;
}

bool IdacDriver2Es::IdacStartOutput() {
	return false;
}

bool IdacDriver2Es::IdacStopOutput() {
	return false;
}

bool IdacDriver2Es::IdacTuneBoard() { return false; } // FIXME: implement -- ellis, 2010-06-13

int IdacDriver2Es::IdacType() const {
	return IDAC_TYPE_2_USB;
}

void IdacDriver2Es::IdacUnlockReadBuffer() {
	// Do nothing
}

bool IdacDriver2Es::IdacZeroPulse(int iChan) {
	return m_driver->IdacZeroPulse(iChan);
}
