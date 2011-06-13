<<<<<<< HEAD
DRIVER_EXPORT LONG WINAPI IdacPresent (LONG Address)
DRIVER_EXPORT DWORD WINAPI IdacBoot (LPCSTR FileName, LONG Address)
DRIVER_EXPORT BOOL WINAPI IdacHasBooted()
DRIVER_EXPORT DWORD WINAPI IdacGetDSPFirmwareVersion ()
=======
#include "IdacExports.h"

#include <Idac/IdacProxy.h>

extern IdacProxy* g_proxy;

/************************************************/
/* IDAC presence test                                                   */
/* Returns 1 when USB device driver is found    */
/************************************************/
DRIVER_EXPORT LONG WINAPI IdacPresent (LONG Address) {
	Q_UNUSED(Address)
	return (g_proxy->isAvailable() ? 1 : 0);
}

/********************************************************************************/
/*      If the IDAC is detected and the download file was found, it is downloaded       */
/*      and 1 is returned.                                                                                                                */
/*                                                                                                                                        */
/*      On errors returns:                                                                                                                */
/*               0      IDAC not found at any of the valid addresses.                                           */
/*               2      IDAC program not found on disk.                                                                         */
/*               3      Problem during download                                                                                         */
/********************************************************************************/
DRIVER_EXPORT DWORD WINAPI IdacBoot (LPCSTR FileName, LONG Address) {
	Q_UNUSED(FileName)
	Q_UNUSED(Address)
	return (g_proxy->isAvailable() ? 1 : 0);
}

/* Allow the application to check wether the Idac has been booted
 */
DRIVER_EXPORT BOOL WINAPI IdacHasBooted() {
	return (g_proxy->isAvailable() ? 1 : 0);
}

/********************************************/
/* Return the firmware version from the DSP	*/
/********************************************/
DRIVER_EXPORT DWORD WINAPI IdacGetDSPFirmwareVersion () {
	return 0;
}
>>>>>>> 4ca987722c3399d2e8a78775d2ea1dbb386ca116
