#include "IdacDriver4Es.h"

bool IdacDriver4Es::IdacAudio(int iChan, bool bActivate) { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacBoot(const QString& sFilename, int nAddress) { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacDataAvail() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacEag(int iChan, bool bActivate) { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacEnableChannel(int iChan, bool bEnabled) { return false; } // FIXME: implement -- ellis, 2010-06-13
long* IdacDriver4Es::IdacGetAddressTable() const { return false; } // FIXME: implement -- ellis, 2010-06-13
double IdacDriver4Es::IdacGetAnSampleBaseRate(int nChannels) const { return false; } // FIXME: implement -- ellis, 2010-06-13
double IdacDriver4Es::IdacGetChannelBaseRate(int iChan) const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetChannelCount() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetChannelDecimation(int iChan) const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetDSPFirmwareVersion() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetDSPInfo(uchar* pBuffer, int nSignalCount, int nBufCount) const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetDigSampleBaseRate() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetNrOfAnalogOutChan() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetNrOfDigitalOutChan() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacGetSampleBandwidth() const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacHasOutput() const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacHighPass(int iChan, int index) const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacIsChannelEnabled(int iChan) const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacIsOutputRunning() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacLibVersion() const { return false; } // FIXME: implement -- ellis, 2010-06-13
QString IdacDriver4Es::IdacLock(const QString& sUserName) { return false; } // FIXME: implement -- ellis, 2010-06-13
LPCDD32_SAMPLE IdacDriver4Es::IdacLockReadBuffer(int* pnCount) { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacLowPass(int iChan, int index) const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacNotch(int iChan, bool bActivate) { return false; } // FIXME: implement -- ellis, 2010-06-13
uint IdacDriver4Es::IdacNrOfAnChannelEnabled() const { return false; } // FIXME: implement -- ellis, 2010-06-13
void IdacDriver4Es::IdacPowerDown() { return; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacPresent(int nAddress) { return false; } // FIXME: implement -- ellis, 2010-06-13
CDD32_SAMPLE IdacDriver4Es::IdacRead() { return NULL_CDD32_SAMPLE; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacScaleRange(int iChan, int index) { return false; } // FIXME: implement -- ellis, 2010-06-13
void IdacDriver4Es::IdacSetBufferEvent(int hEvent) { return; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacSetDecimation(int iChan, int nDecimation) { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacSetOffsetAnalogIn(int iChan, int nOffset) { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacSmpStart() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacSmpStop() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacStartOutput() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacStopOutput() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacTuneBoard() { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver4Es::IdacType() const { return -1; } // FIXME: implement -- ellis, 2010-06-13
void IdacDriver4Es::IdacUnlock() { return; } // FIXME: implement -- ellis, 2010-06-13
void IdacDriver4Es::IdacUnlockReadBuffer() { return; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver4Es::IdacZeroPulse(int iChan) { return false; } // FIXME: implement -- ellis, 2010-06-13
