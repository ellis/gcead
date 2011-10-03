#include "IdacExports.h"

#include <Core/Check.h>
#include <Idac/IdacDriverManager.h>
#include <IdacDriver/IdacDriverUsbEs.h>

#define NOTE checkLog(__FILE__, __LINE__, "CALL", "")

extern IdacDriverUsbEs* wrapper();
extern IdacDriverManager* manager();


DRIVER_EXPORT BOOL WINAPI IdacCapabilities(TIdacCapability Cap) { NOTE; return wrapper()->IdacCapabilities(Cap); }
DRIVER_EXPORT DWORD WINAPI IdacDataAvail() { NOTE; return wrapper()->IdacDataAvail(); }
DRIVER_EXPORT BOOL WINAPI IdacEnableChannel(DWORD Chan, BOOL bEnable) { NOTE; return wrapper()->IdacEnableChannel(Chan, bEnable); }
DRIVER_EXPORT DWORD WINAPI IdacGetChannelDecimation(DWORD Chan) { NOTE; return wrapper()->IdacGetChannelDecimation(Chan); }
DRIVER_EXPORT DWORD WINAPI IdacGetDSPInfo(PWORD, WORD, DWORD) { NOTE; return wrapper()->IdacGetDSPInfo(); }

DRIVER_EXPORT LPDWORD WINAPI IdacGetRanges() {
	NOTE;
	static QVector<DWORD> ranges;
	const QList<int>& l = wrapper()->IdacGetRanges();
	if (l.size() + 1 > ranges.size())
		ranges.resize(l.size() + 1);
	int i = 0;
	for (; i < l.size(); i++)
		ranges[i] = l[i];
	ranges[i] = -1;
	return const_cast<LPDWORD>(ranges.constData());
}

DRIVER_EXPORT BOOL WINAPI IdacHighPass(DWORD Chan, DWORD Index) { NOTE; return wrapper()->IdacHighPass(Chan, Index); }

DRIVER_EXPORT void WINAPI IdacHighPassStrings(int *NrOfStrings, const char **ppFilterStrings[]) {
	NOTE;
	static QVector<const char*> l0;
	const QStringList& l = wrapper()->IdacHighPassStrings();
	qDeleteAll(l0);
	l0.resize(l.size());
	for (int i = 0; i < l0.size(); i++) {
		const QByteArray a = l[i].toLocal8Bit();
		char* sz = new char[a.size()];
		strcpy(sz, a.constData());
	}
	*NrOfStrings = l0.size();
	*ppFilterStrings = l0.data();
}

DRIVER_EXPORT BOOL WINAPI IdacIsChannelEnabled(DWORD Chan) { NOTE; return wrapper()->IdacIsChannelEnabled(Chan); }
DRIVER_EXPORT DWORD WINAPI IdacLibVersion() { NOTE; return wrapper()->IdacLibVersion(); }

DRIVER_EXPORT LPCSTR WINAPI IdacLock(LPCSTR UserName) {
	NOTE;
	static QByteArray a;
	a = wrapper()->IdacLock(UserName).toLocal8Bit();
	return a.constData();
}

DRIVER_EXPORT BOOL WINAPI IdacLowPass(DWORD Chan, DWORD Index) { NOTE; return wrapper()->IdacLowPass(Chan, Index); }

DRIVER_EXPORT void WINAPI IdacLowPassStrings(int *NrOfStrings, const char **ppFilterStrings[]) {
	NOTE;
	static QVector<const char*> l0;
	const QStringList& l = wrapper()->IdacLowPassStrings();
	qDeleteAll(l0);
	l0.resize(l.size());
	for (int i = 0; i < l0.size(); i++) {
		const QByteArray a = l[i].toLocal8Bit();
		char* sz = new char[a.size()];
		strcpy(sz, a.constData());
	}
	*NrOfStrings = l0.size();
	*ppFilterStrings = l0.data();
}

DRIVER_EXPORT UINT WINAPI IdacNrOfAnChannelEnabled() { NOTE; return wrapper()->IdacNrOfAnChannelEnabled(); }
DRIVER_EXPORT void WINAPI IdacPowerDown() { NOTE; wrapper()->IdacPowerDown(); }
DRIVER_EXPORT LONG WINAPI IdacPresent(LONG Address) { NOTE; return (manager()->state() >= IdacState_Present); }
DRIVER_EXPORT BOOL WINAPI IdacScaleRange(DWORD Chan, DWORD Index) { NOTE; return wrapper()->IdacScaleRange(Chan, Index); }
DRIVER_EXPORT void WINAPI IdacSetBufferEvent(HANDLE hEvent) { NOTE; wrapper()->IdacSetBufferEvent(hEvent); }
DRIVER_EXPORT BOOL WINAPI IdacSetDecimation(DWORD Chan, DWORD dwDecimation) { NOTE; return wrapper()->IdacSetDecimation(Chan, dwDecimation); }
DRIVER_EXPORT BOOL WINAPI IdacSetOffsetAnalogIn(DWORD Chan, DWORD Offset) { NOTE; return wrapper()->IdacSetOffsetAnalogIn(Chan, Offset); }
DRIVER_EXPORT BOOL WINAPI IdacSmpStart() { NOTE; return wrapper()->IdacSmpStart(); }
DRIVER_EXPORT BOOL WINAPI IdacSmpStop() { NOTE; return wrapper()->IdacSmpStop(); }
DRIVER_EXPORT void WINAPI IdacUnlock() { NOTE; wrapper()->IdacUnlock(); }

DRIVER_EXPORT LPCDD32_SAMPLE WINAPI IdacLockReadBuffer(DWORD* pdwCount) {
	NOTE;
	int nCount = 0;
	LPCDD32_SAMPLE sample = wrapper()->IdacLockReadBuffer(&nCount);
	*pdwCount = nCount;
	return sample;
}

DRIVER_EXPORT void WINAPI IdacUnlockReadBuffer() { NOTE; wrapper()->IdacUnlockReadBuffer(); }
DRIVER_EXPORT DWORD WINAPI IdacType() { NOTE; return wrapper()->IdacType(); }
DRIVER_EXPORT DWORD WINAPI IdacBoot(LPCSTR FileName, LONG Address) { NOTE; return wrapper()->IdacBoot(FileName, Address); }

/****************************************************/
#define ZERO(x) { checkLog(__FILE__, __LINE__, "CALL", "NOT IMPLEMENTED"); return x; }
static CDD32_SAMPLE g_sampleDummy;
static CDD32_STATUS g_statusDummy;
static SIZE g_sizeDummy;
DRIVER_EXPORT	BOOL	WINAPI	IdacZero				(DWORD Chan, BOOL bActivate) {  ZERO(0) }	// Disconnect the input and make it zero
DRIVER_EXPORT	BOOL	WINAPI	IdacSetAdcZero			(DWORD Chan, BYTE Offset) {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	IdacSetInputZero		(DWORD Chan, DWORD Offset) {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	IdacNetfrequency		(DWORD Chan, BOOL bActivate) {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	IdacTuneNotch			(DWORD Chan, BYTE Offset) {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	ResetFpgaTestDll		() {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	GetTransferData			() {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	IdacConfigRead			() {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	IdacConfigWrite			() {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacHasBooted				() {  ZERO(0) }
DRIVER_EXPORT	DWORD			WINAPI	IdacGetDSPFirmwareVersion	() {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacProgramFPGA				(LPCSTR FileName) {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacProgramFirmware			(LPCSTR FileName) {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacProgramDSP				(LPCSTR FileName) {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacGetUsbConfigDescriptor	(LPSTR pBuffer, DWORD dwBufSize) {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacGetUsbDeviceDescriptor	(LPSTR pBuffer, DWORD dwBufSize) {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacZeroPulse				(DWORD Chan) {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacTuneBoard				() {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacNotch					(DWORD Chan, BOOL bActivate) {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacEag						(DWORD Chan, BOOL bActivate) {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacAudio					(DWORD Chan, BOOL bActivate) {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	IdacSetAnalogOut(DWORD Chan, TChanOutData *sBuffer, int nBufLen) {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	IdacSetDigitalOut(TChanOutData *sBuffer, int nBufLen) {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	IdacStartOutput() {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	IdacStopOutput() {  ZERO(0) }
DRIVER_EXPORT	BOOL	WINAPI	IdacFlushOutput() {  ZERO(0) }	// Removes all output data
DRIVER_EXPORT	BOOL	WINAPI	IdacHasOutput() {  ZERO(0) }	// Indicates that some output data has been provided
DRIVER_EXPORT	DWORD	WINAPI	IdacGetNrOfAnalogOutChan() {  ZERO(0) }
DRIVER_EXPORT	DWORD	WINAPI	IdacGetNrOfDigitalOutChan() {  ZERO(0) }
DRIVER_EXPORT	SIZE	WINAPI	IdacGetAnalogOutRange() {  ZERO(g_sizeDummy) }
DRIVER_EXPORT	BOOL	WINAPI	IdacIsOutputRunning() {  ZERO(0) }
DRIVER_EXPORT	LPLONG			WINAPI	IdacGetAddressTable			() {  ZERO(0) }
DRIVER_EXPORT	DWORD			WINAPI	IdacGetChannelCount			() {  ZERO(0) }
DRIVER_EXPORT	DWORD			WINAPI	IdacGetClusterSize			() {  ZERO(0) }
DRIVER_EXPORT	DWORD			WINAPI	IdacGetClusterCount			() {  ZERO(0) }
DRIVER_EXPORT	DWORD			WINAPI	IdacGetSampleBandwidth		() {  ZERO(0) }
DRIVER_EXPORT	DOUBLE			WINAPI	IdacGetAnSampleBaseRate		(DWORD ChannelCount) {  ZERO(0) }
DRIVER_EXPORT	DWORD			WINAPI	IdacGetDigSampleBaseRate	() {  ZERO(0) }
DRIVER_EXPORT	DOUBLE			WINAPI	IdacGetChannelBaseRate		(DWORD Chan) {  ZERO(0) }
DRIVER_EXPORT	BOOL			WINAPI	IdacDataWait				(DWORD dwMilliSec) {  ZERO(0) }
DRIVER_EXPORT	CDD32_SAMPLE	WINAPI	IdacRead					() {  ZERO(g_sampleDummy) }
DRIVER_EXPORT	CDD32_STATUS	WINAPI	IdacGetStatusFlags			(BOOL bClear) {  ZERO(g_statusDummy) }
DRIVER_EXPORT BOOL	WINAPI IdacOSA_Initialize		() {  ZERO(0) }
DRIVER_EXPORT BOOL	WINAPI IdacOSA_EnableChannel	(BYTE nChanId, BOOL bEnable) {  ZERO(0) }
DRIVER_EXPORT BOOL	WINAPI IdacOSA_AddBuffer		(DWORD nLen, USHORT* pBuffer) {  ZERO(0) }
DRIVER_EXPORT BOOL	WINAPI IdacOSA_BlockedWaitForBuffer(DWORD& dwInQueue) {  ZERO(0) }
DRIVER_EXPORT BOOL	WINAPI IdacOSA_Finish			() {  ZERO(0) }
DRIVER_EXPORT int WINAPI IdacTypeName(char* pName, int& nLength) {  ZERO(0) }
DRIVER_EXPORT BOOL WINAPI IdacAISetOffset(DWORD nChan, DOUBLE dOffset) {  ZERO(0) }
DRIVER_EXPORT void WINAPI IdacAIOffsetRange(DOUBLE &dRangeMin, DOUBLE &dRangeMax) {  ZERO() }
DRIVER_EXPORT	UINT WINAPI	IdacGetNrOfDigitalChan() {  ZERO(0) }
