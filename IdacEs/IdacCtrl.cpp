#include "IdacExports.h"

#include <Idac/IdacProxy.h>

extern IdacProxy* g_proxy;

/************************************/
/* Enable channel function			*/
/*									*/
/* Turn channel on/off				*/
/************************************/
DRIVER_EXPORT BOOL WINAPI IdacEnableChannel (DWORD Chan, BOOL bEnable) {
	return g_proxy->
}

DRIVER_EXPORT	BOOL WINAPI	IdacIsChannelEnabled (DWORD Chan)
DRIVER_EXPORT	UINT	WINAPI	IdacNrOfAnChannelEnabled	()
DRIVER_EXPORT BOOL WINAPI IdacZeroPulse (DWORD Chan)
DRIVER_EXPORT BOOL WINAPI IdacSetOffsetAnalogIn (DWORD Chan, DWORD Offset)
DRIVER_EXPORT BOOL WINAPI IdacAISetOffset(DWORD nChan, DOUBLE dOffset)
DRIVER_EXPORT void WINAPI IdacAIOffsetRange(DOUBLE &dRangeMin, DOUBLE &dRangeMax)
DRIVER_EXPORT	void WINAPI	IdacLowPassStrings (int *NrOfStrings, const char **ppFilterStrings[])
DRIVER_EXPORT BOOL WINAPI IdacLowPass (DWORD Chan, DWORD Index)
DRIVER_EXPORT	void WINAPI	IdacHighPassStrings (int *NrOfStrings, const char **ppFilterStrings[])
DRIVER_EXPORT BOOL WINAPI IdacHighPass (DWORD Chan, DWORD Index)
DRIVER_EXPORT BOOL WINAPI IdacScaleRange (DWORD Chan, DWORD Index)
DRIVER_EXPORT BOOL WINAPI IdacNotch (DWORD Chan, BOOL bActivate)
DRIVER_EXPORT BOOL WINAPI IdacEag (DWORD Chan, BOOL bActivate)
DRIVER_EXPORT BOOL WINAPI IdacAudio (DWORD Chan, BOOL bActivate)
DRIVER_EXPORT BOOL WINAPI IdacSetDecimation (DWORD Chan, DWORD dwDecimation)
DRIVER_EXPORT BOOL WINAPI IdacTuneBoard()
