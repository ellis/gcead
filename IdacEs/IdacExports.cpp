#include "IdacExports.h"

#include <Idac/IdacDriverManager.h>
#include <IdacDriver/IdacDriverUsbEs.h>

extern IdacDriverUsbEs* wrapper();
extern IdacDriverManager* manager();

DRIVER_EXPORT BOOL WINAPI IdacCapabilities(TIdacCapability Cap) { return wrapper()->IdacCapabilities(Cap); }
DRIVER_EXPORT DWORD WINAPI IdacDataAvail() { return wrapper()->IdacDataAvail(); }
DRIVER_EXPORT BOOL WINAPI IdacEnableChannel(DWORD Chan, BOOL bEnable) { return wrapper()->IdacEnableChannel(Chan, bEnable); }
DRIVER_EXPORT DWORD WINAPI IdacGetChannelDecimation(DWORD Chan) { return wrapper()->IdacGetChannelDecimation(Chan); }
DRIVER_EXPORT DWORD WINAPI IdacGetDSPInfo(PWORD, WORD, DWORD) { return wrapper()->IdacGetDSPInfo(); }

DRIVER_EXPORT LPDWORD WINAPI IdacGetRanges() {
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

DRIVER_EXPORT BOOL WINAPI IdacHighPass(DWORD Chan, DWORD Index) { return wrapper()->IdacHighPass(Chan, Index); }

DRIVER_EXPORT void WINAPI IdacHighPassStrings(int *NrOfStrings, const char **ppFilterStrings[]) {
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

DRIVER_EXPORT BOOL WINAPI IdacIsChannelEnabled(DWORD Chan) { return wrapper()->IdacIsChannelEnabled(Chan); }
DRIVER_EXPORT DWORD WINAPI IdacLibVersion() { return wrapper()->IdacLibVersion(); }

DRIVER_EXPORT LPCSTR WINAPI IdacLock(LPCSTR UserName) {
	static QByteArray a;
	a = wrapper()->IdacLock(UserName).toLocal8Bit();
	return a.constData();
}

DRIVER_EXPORT BOOL WINAPI IdacLowPass(DWORD Chan, DWORD Index) { return wrapper()->IdacLowPass(Chan, Index); }

DRIVER_EXPORT void WINAPI IdacLowPassStrings(int *NrOfStrings, const char **ppFilterStrings[]) {
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

DRIVER_EXPORT UINT WINAPI IdacNrOfAnChannelEnabled() { return wrapper()->IdacNrOfAnChannelEnabled(); }
DRIVER_EXPORT void WINAPI IdacPowerDown() { wrapper()->IdacPowerDown(); }
DRIVER_EXPORT LONG WINAPI IdacPresent(LONG Address) { return (manager()->state() >= IdacState_Present); }
DRIVER_EXPORT BOOL WINAPI IdacScaleRange(DWORD Chan, DWORD Index) { return wrapper()->IdacScaleRange(Chan, Index); }
DRIVER_EXPORT void WINAPI IdacSetBufferEvent(HANDLE hEvent) { wrapper()->IdacSetBufferEvent(hEvent); }
DRIVER_EXPORT BOOL WINAPI IdacSetDecimation(DWORD Chan, DWORD dwDecimation) { return wrapper()->IdacSetDecimation(Chan, dwDecimation); }
DRIVER_EXPORT BOOL WINAPI IdacSetOffsetAnalogIn(DWORD Chan, DWORD Offset) { return wrapper()->IdacSetOffsetAnalogIn(Chan, Offset); }
DRIVER_EXPORT BOOL WINAPI IdacSmpStart() { return wrapper()->IdacSmpStart(); }
DRIVER_EXPORT BOOL WINAPI IdacSmpStop() { return wrapper()->IdacSmpStop(); }
DRIVER_EXPORT void WINAPI IdacUnlock() { wrapper()->IdacUnlock(); }

DRIVER_EXPORT LPCDD32_SAMPLE WINAPI IdacLockReadBuffer(DWORD* pdwCount) {
	int nCount = 0;
	LPCDD32_SAMPLE sample = wrapper()->IdacLockReadBuffer(&nCount);
	*pdwCount = nCount;
	return sample;
}

DRIVER_EXPORT void WINAPI IdacUnlockReadBuffer() { wrapper()->IdacUnlockReadBuffer(); }
DRIVER_EXPORT DWORD WINAPI IdacType() { return wrapper()->IdacType(); }
DRIVER_EXPORT DWORD WINAPI IdacBoot(LPCSTR FileName, LONG Address) { return wrapper()->IdacBoot(FileName, Address); }
