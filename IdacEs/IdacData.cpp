DRIVER_EXPORT DWORD WINAPI IdacDataAvail ()
DRIVER_EXPORT BOOL WINAPI IdacDataWait (DWORD dwMilliSec)
DRIVER_EXPORT CDD32_SAMPLE WINAPI IdacRead ()
DRIVER_EXPORT CDD32_STATUS WINAPI IdacGetStatusFlags (BOOL bClear)
DRIVER_EXPORT BOOL WINAPI IdacSmpStart ()
DRIVER_EXPORT BOOL WINAPI IdacSmpStop ()
DRIVER_EXPORT LPCDD32_SAMPLE WINAPI IdacLockReadBuffer (DWORD* pdwCount)
DRIVER_EXPORT void WINAPI IdacUnlockReadBuffer ()
DRIVER_EXPORT void WINAPI IdacSetBufferEvent (HANDLE hEvent)
