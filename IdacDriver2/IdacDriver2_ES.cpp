#include "IdacDriver2.h"

bool IdacDriver2::IdacAudio(int iChan, bool bActivate) { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacBoot(const QString& sFilename, int nAddress) { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacDataAvail() { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacEag(int iChan, bool bActivate) { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacEnableChannel(int iChan, bool bEnabled) { return false; } // FIXME: implement -- ellis, 2010-06-13
long* IdacDriver2::IdacGetAddressTable() const { return false; } // FIXME: implement -- ellis, 2010-06-13
double IdacDriver2::IdacGetAnSampleBaseRate(int nChannels) const { return false; } // FIXME: implement -- ellis, 2010-06-13
double IdacDriver2::IdacGetChannelBaseRate(int iChan) const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacGetChannelCount() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacGetChannelDecimation(int iChan) const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacGetDSPFirmwareVersion() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacGetDSPInfo(uchar* pBuffer, int nSignalCount, int nBufCount) const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacGetDigSampleBaseRate() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacGetNrOfAnalogOutChan() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacGetNrOfDigitalOutChan() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacGetSampleBandwidth() const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacHasOutput() const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacHighPass(int iChan, int index) const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacIsChannelEnabled(int iChan) const { return false; } // FIXME: implement -- ellis, 2010-06-13
bool IdacDriver2::IdacIsOutputRunning() const { return false; } // FIXME: implement -- ellis, 2010-06-13
int IdacDriver2::IdacLibVersion() const { return false; } // FIXME: implement -- ellis, 2010-06-13
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
