  /* Interface definition for IDAC32.DLL, for IDAC-ISA, IDAC-USB, DAQ, IDAC2000 and IDAC4 */
#ifndef IDACPC_H
#define IDACPC_H

//Obsolete as a .def file is used
//#ifdef IDAC_DRIVER
//#define DRIVER_EXPORT __declspec(dllexport)
//#else
//#define DRIVER_EXPORT __declspec(dllimport)
// #endif
#define DRIVER_EXPORT

// Common data definitions
<<<<<<< HEAD
#ifndef SAMPLE_H
#include "sample.h"
#endif

#ifndef IPC_H
#include "ipc.h"
#endif
=======
#include <IdacDriverES/IdacControl/Sample.h>
#include <IdacDriverES/IdacControl/Ipc.h>
>>>>>>> 4ca987722c3399d2e8a78775d2ea1dbb386ca116

/* C/C++ declarations */

#ifdef  __cplusplus
extern "C" 
{
#endif

#include <DBT.H>
#include <WINUSER.H>
<<<<<<< HEAD
=======
#include <windows.h>
>>>>>>> 4ca987722c3399d2e8a78775d2ea1dbb386ca116

	/* Version */
#define IDAC_LIB_VERSION	7L


/* Defines for IDAC hardware types	*/
#define IDAC_TYPE_NONE		0
#define IDAC_TYPE_ISA_V2	1
#define IDAC_TYPE_USB_V3	2
#define IDAC_TYPE_SERIAL	3
#define IDAC_TYPE_2000		4
#define IDAC_TYPE_4			5
#define IDAC_TYPE_2_USB		6

// Used for the test apllication IdacBox and IdacAutoTest (Production testing)
// Not used for AutoSpike.
DRIVER_EXPORT	BOOL	WINAPI	IdacZero				(DWORD Chan, BOOL bActivate);	// Disconnect the input and make it zero
DRIVER_EXPORT	BOOL	WINAPI	IdacSetAdcZero			(DWORD Chan, BYTE Offset);
DRIVER_EXPORT	BOOL	WINAPI	IdacSetInputZero		(DWORD Chan, DWORD Offset);
DRIVER_EXPORT	BOOL	WINAPI	IdacNetfrequency		(DWORD Chan, BOOL bActivate);
DRIVER_EXPORT	BOOL	WINAPI	IdacTuneNotch			(DWORD Chan, BYTE Offset);
DRIVER_EXPORT	BOOL	WINAPI	ResetFpgaTestDll		();
DRIVER_EXPORT	BOOL	WINAPI	GetTransferData			();
DRIVER_EXPORT	BOOL	WINAPI	IdacConfigRead			();
DRIVER_EXPORT	BOOL	WINAPI	IdacConfigWrite			();

/************************************************************************************/
/* Compiling note: to ensure that the exported functions match this header file,	*/
/* browse the definition of each function to see that you will not be exporting		*/
/* a different version of the same function name. If you cannot browse to the		*/
/* definition, there will be a conflict in function parameters.						*/
/************************************************************************************/

/************************************************************************************/
/* Version (IDACDLL.CPP)															*/
/************************************************************************************/

/* Return library version. */
DRIVER_EXPORT	DWORD			WINAPI	IdacLibVersion				();

/************************************************************************************/
/* System control functions (IDACDLL.CPP)											*/
/************************************************************************************/

/* Type of IDAC card (hardware type). */
DRIVER_EXPORT	DWORD			WINAPI	IdacType					();


/* IDAC Capabilitity information */
enum TIdacCapability
{	IDAC_CAN_SCALE,
	IDAC_HAS_LOWPASS,
	IDAC_HAS_HIGHPASS,
	IDAC_HAS_NOTCH,
	IDAC_HAS_AUDIOOUT,
	IDAC_HAS_DIGITALOUT,
	IDAC_HAS_ANALOGOUT,
	IDAC_HAS_ZEROPULSE,
	IDAC_HAS_INPUTOFFSET,
	IDAC_CAN_TUNE,
	IDAC_HAS_EAG
};

DRIVER_EXPORT	BOOL			WINAPI	IdacCapabilities			(TIdacCapability Cap);


/* Lock with a program name, (starts reading iso buffer from driver)*/
DRIVER_EXPORT	LPCSTR			WINAPI	IdacLock					(LPCSTR UserName);

/* Unlock ( stops isotransfer ) */
DRIVER_EXPORT	void			WINAPI	IdacUnlock					();

/* Bring the board electronics in power-down mode */
DRIVER_EXPORT	void			WINAPI	IdacPowerDown				();

/************************************************************************************/
/* Board connection funtions (IDACBRD.CPP)											*/
/************************************************************************************/

/* IDAC presence test (before boot)
 * Parameters: Address = requested address to test, or -1 to test all addresses
 * Returns: numeric address (meaning depends on board type)
 */
DRIVER_EXPORT	LONG			WINAPI	IdacPresent					(LONG Address);

/* Boot the DSP with the program in 'FileName'.
 * Parameters: Address = requested address to use, or -1 to detect automatically.
 * Returns: 0 - IDAC not found
 *			1 - IDAC found and successfully booted
 *			2 - IDAC found, but file not found
 *			3 - IDAC found, but error during download
 *	If the board has powerdown functionality, it is powered-up before booting.
 */
DRIVER_EXPORT	DWORD			WINAPI	IdacBoot					(LPCSTR FileName, LONG Address);

/* Allow the application to check wether the Idac has been booted
 */
DRIVER_EXPORT	BOOL			WINAPI	IdacHasBooted				();

/* Return the firmware version, if the IDAC has a DSP (IDAC2, IDAC3) */
// not used in idac 2000!!!!!!!!!!!
DRIVER_EXPORT	DWORD			WINAPI	IdacGetDSPFirmwareVersion	();

/************************************************************************************/
/* Debugging functions (IDACDBG.CPP)							(IDAC3 only)		*/
/************************************************************************************/

/* Program the FPGA with 'FileName'. */
DRIVER_EXPORT	BOOL			WINAPI	IdacProgramFPGA				(LPCSTR FileName);

/* Program the firmware with 'FileName'. */
DRIVER_EXPORT	BOOL			WINAPI	IdacProgramFirmware			(LPCSTR FileName);

/* Program the DSP with 'FileName'. */
DRIVER_EXPORT	BOOL			WINAPI	IdacProgramDSP				(LPCSTR FileName);

/* Get a descriptor string for the USB config records. */
DRIVER_EXPORT	BOOL			WINAPI	IdacGetUsbConfigDescriptor	(LPSTR pBuffer, DWORD dwBufSize);

/* Get a descriptor string for the USB device record. */
DRIVER_EXPORT	BOOL			WINAPI	IdacGetUsbDeviceDescriptor	(LPSTR pBuffer, DWORD dwBufSize);

/************************************************************************************/
/* On-board scaling, filtering and utilities (IDACCTRL.CPP)							*/
/************************************************************************************/
/* Switch channel on/off. */
DRIVER_EXPORT	BOOL			WINAPI	IdacEnableChannel			(DWORD Chan, BOOL bEnable);

/* Get channel on/off status. */
DRIVER_EXPORT	BOOL			WINAPI	IdacIsChannelEnabled		(DWORD Chan);

/* Get the number of active channels */
DRIVER_EXPORT	UINT			WINAPI	IdacNrOfAnChannelEnabled	();


/* Send a zero-pulse to the channel, or the entire board if it cannot differentiate. Analog channels only.
	Chan == -1 -> Zero to ALL (active) channels.
 */
DRIVER_EXPORT	BOOL			WINAPI	IdacZeroPulse				(DWORD Chan);

/* Set a amplification (IDAC3 and for analog channels only):
 * Index	scale
 *	0		1	
 *	1		4	
 *	2		16
 *  3		64
 *	4		256
 *	5       1024	
 */
/* Set the scaling index. This will scale according to the values of IdacGetRanges(). Analog channels only. */
DRIVER_EXPORT	BOOL			WINAPI	IdacScaleRange				(DWORD Chan, DWORD Index);

/* Set the decimation values of the given channel. */
DRIVER_EXPORT	BOOL			WINAPI	IdacSetDecimation			(DWORD Chan, DWORD dwDecimation);

// replaces BOOL WINAPI IdacOffset(DWORD Chan, LONG Offset);
DRIVER_EXPORT	BOOL			WINAPI	IdacSetOffsetAnalogIn		(DWORD Chan, DWORD Offset);

// Idac 2000, empty for others
DRIVER_EXPORT	BOOL			WINAPI	IdacTuneBoard				();


// Set a lowpass filter	(if IDAC_HAS_LOWPASS)
DRIVER_EXPORT	void			WINAPI	IdacLowPassStrings			(int *NrOfStrings, const char **ppFilterStrings[]);
DRIVER_EXPORT	BOOL			WINAPI	IdacLowPass					(DWORD Chan, DWORD Index);


// Set a highpass filter (if IDAC_HAS_HIGHPASS):
DRIVER_EXPORT	void			WINAPI	IdacHighPassStrings			(int *NrOfStrings, const char **ppFilterStrings[]);
DRIVER_EXPORT	BOOL			WINAPI	IdacHighPass				(DWORD Chan, DWORD Index);

/* Switch 50/60Hz notchfilter on/off (if IDAC_HAS_NOTCH) */
DRIVER_EXPORT	BOOL			WINAPI	IdacNotch					(DWORD Chan, BOOL bActivate);

/* Switch eag filter (5e order lowpass 12hz) on/off (if IDAC_HAS_NOTCH) */
DRIVER_EXPORT	BOOL			WINAPI	IdacEag						(DWORD Chan, BOOL bActivate);

/* Pass the signal to the audio output (if IDAC_HAS_AUDIOOUT) */
DRIVER_EXPORT	BOOL			WINAPI	IdacAudio					(DWORD Chan, BOOL bActivate);

// ----- XV 12/11/2002 : New functions and struct
typedef struct _CHANOUTDATA 
{
	DWORD nTime;
	WORD  nData;
} TChanOutData;

// Set the buffer to generate analog/digital out signal.
DRIVER_EXPORT	BOOL	WINAPI	IdacSetAnalogOut(DWORD Chan, TChanOutData *sBuffer, int nBufLen);

//	IdacSetDigitalOut
//	Sets the buffer in the USB client driver to generate digital output signal.
//  **********************************************************************
//	NOTE:
//		If IdacSetDigitalOut is called while the output is still running,
//		only the LAST value of sBuffer will be used.
//  **********************************************************************
//	@param	sBuffer, TChanOutData*. Buffer for all digital output channels.
//	@param	nBufLen, int. Length of the buffer.
//	@return BOOL. Returns nonzero (TRUE) when no errors occured.
DRIVER_EXPORT	BOOL	WINAPI	IdacSetDigitalOut(TChanOutData *sBuffer, int nBufLen);

// Start / stop output signals.
DRIVER_EXPORT	BOOL	WINAPI	IdacStartOutput();
DRIVER_EXPORT	BOOL	WINAPI	IdacStopOutput();
DRIVER_EXPORT	BOOL	WINAPI	IdacFlushOutput();	// Removes all output data
DRIVER_EXPORT	BOOL	WINAPI	IdacHasOutput();	// Indicates that some output data has been provided

// Number of analog/digital channels.
DRIVER_EXPORT	DWORD	WINAPI	IdacGetNrOfAnalogOutChan();
DRIVER_EXPORT	DWORD	WINAPI	IdacGetNrOfDigitalOutChan();

// Range of the analog out channels in mV (cx = LOW, cy = HIGH)
DRIVER_EXPORT	SIZE	WINAPI	IdacGetAnalogOutRange();

// Returns TRUE when output is running
DRIVER_EXPORT	BOOL	WINAPI	IdacIsOutputRunning();

// ----- XV END

/************************************************************************************/
/* Information functions (IDACINFO.CPP)												*/
/************************************************************************************/

/* Table of valid numeric addresses, -1 terminated. */
DRIVER_EXPORT	LPLONG			WINAPI	IdacGetAddressTable			();

/* Total number of channels, including digital. */
DRIVER_EXPORT	DWORD			WINAPI	IdacGetChannelCount			();



/* Number of channels in 1 decimation cluster (generally 4).
 * A decimation cluster is generally a group of analog channels that are multiplexed
 * by one AD-Convertor.
 */
DRIVER_EXPORT	DWORD			WINAPI	IdacGetClusterSize			();

/* Number of decimation clusters (often 1). */
DRIVER_EXPORT	DWORD			WINAPI	IdacGetClusterCount			();

/* Max possible Total Sample rate (sum of all active channels)  */
DRIVER_EXPORT	DWORD			WINAPI	IdacGetSampleBandwidth		();

/* Sample rate base freq. for a given number analog channels in 1 cluster. (BaseRate/ChannelCount) */
DRIVER_EXPORT	DOUBLE			WINAPI	IdacGetAnSampleBaseRate		(DWORD ChannelCount);

/* Sample rate base freq. for the digital channel. */
DRIVER_EXPORT	DWORD			WINAPI	IdacGetDigSampleBaseRate	();

/* Sample rate base freq for a selected channel. */
DRIVER_EXPORT	DOUBLE			WINAPI	IdacGetChannelBaseRate		(DWORD Chan);

/* Current Decimation factor for a selected channel, possibly depending on the number of active channels */
DRIVER_EXPORT	DWORD			WINAPI	IdacGetChannelDecimation	(DWORD Chan);


/* Get available ranges for scalings in uV on analog input, discounting any 
 * external amplifications (-1 terminated). The values are half-ranges,
 * 1000000 represents +/- 1 volt.
 * Example: { 1000000, 250000, -1 }
 */
DRIVER_EXPORT	LPDWORD			WINAPI	IdacGetRanges				();

/* Get internal DSP data (IDAC2, IDAC3) */
// not used in idac 2000!!!!!!!!!!!!!!!!!!!
DRIVER_EXPORT	DWORD			WINAPI	IdacGetDSPInfo				(PWORD pBuffer, WORD wSignalCount, DWORD dwBufCount);

/************************************************************************************/
/* Data acquisition functions (IDACDATA.CPP)										*/
/* See also directions for use in IDACDATA.CPP.										*/
/************************************************************************************/

/* Get number of samples available. */
DRIVER_EXPORT	DWORD			WINAPI	IdacDataAvail				();

/* Wait specified number of milliseconds for new samples to come available. */
DRIVER_EXPORT	BOOL			WINAPI	IdacDataWait				(DWORD dwMilliSec);

/* Read 1 sample. */
DRIVER_EXPORT	CDD32_SAMPLE	WINAPI	IdacRead					();

/* Get the sampling status. */
DRIVER_EXPORT	CDD32_STATUS	WINAPI	IdacGetStatusFlags			(BOOL bClear);

/* Start the sampling process.
 * You must have selected at least 1 analog channel.
 * Buffers will be cleared.
 */
DRIVER_EXPORT	BOOL			WINAPI	IdacSmpStart				();

/* Stop the sampling process. Return TRUE if Idac was running */
DRIVER_EXPORT	BOOL			WINAPI	IdacSmpStop					();

/* Get a buffer with available samples.
 * Returns a pointer to the buffer, or NULL if no samples are available.
 * pdwCount will receive number of samples.
 * The read buffer will be locked to keep the buffer valid,
 * Data must be dispatched quickly and unlocked after use.
 * Unlocking is not necessary if the function returned NULL.
 */
DRIVER_EXPORT	LPCDD32_SAMPLE	WINAPI	IdacLockReadBuffer			(DWORD* pdwCount);

/* Unlock the buffer obtained by IdacLockReadBuffer. */
DRIVER_EXPORT	void			WINAPI	IdacUnlockReadBuffer		();

/* Specify a handle to an event that will be set after every ~ 100 ms during sampling,
 * or when a data buffer is full, whichever happens first.
 * Set to NULL to stop using the event.
 */
DRIVER_EXPORT	void			WINAPI	IdacSetBufferEvent			(HANDLE hEvent);

/**
 *	IdacOSA_Initialize
 *	Initializes the analog ouput to streaming mode.
 *	@return	FALSE on error.
 */
DRIVER_EXPORT BOOL	WINAPI IdacOSA_Initialize		();

/**
 *	IdacOSA_EnableChannel
 *	En-/disable a channel in the output stream.
 *	@param	nChanId, BYTE. One based id of the channel.
 *	@param	bEnable, BOOL. En-/disable the channel.
 *	@return FALSE on error.
 */
DRIVER_EXPORT BOOL	WINAPI IdacOSA_EnableChannel	(BYTE nChanId, BOOL bEnable);

/**
 *	IdacOSA_AddBuffer
 *	Adds a buffer of data to the output stream.
 *	The layout of the buffer is (c = channel, A/B = chanid, 1..n=valueid):
 *		{cA1,cB1,cA2,cB2,cA3...}
 *	@param	nLen, DWORD. Length of the buffer (in USHORT).
 *	@param	pBuffer, USHORT*. Buffer of stream data 
 *	@return FALSE on error.
 */
DRIVER_EXPORT BOOL	WINAPI IdacOSA_AddBuffer		(DWORD nLen, USHORT* pBuffer);

/**
 *	IdacOSA_BlockedWaitForBuffer
 *	Stays blocked waiting until a buffer becomes available.
 *	@param	dwInQueue, DWORD&. A reference to a DWORD to report the number of 
							   buffers currently in queue.
 *	@return FALSE on error.
 */
DRIVER_EXPORT BOOL	WINAPI IdacOSA_BlockedWaitForBuffer(DWORD& dwInQueue);

/**
 *	IdacOSA_Finish
 *	Sets the output mode back to pattern.
 *	@return FALSE on error.
 */
DRIVER_EXPORT BOOL	WINAPI IdacOSA_Finish			();


/**
 *	IdacTypeName
 *	Returns the name of the current idac type. 
 *	First call may be a probe to determine the length of the name
 *	so a large enough array can be allocated.
 *	@param	pName, char*. Contains the idac type name on return.
 *	@param	nLength, int&. IN: The length of pName. OUT: Nr of chars copied
 *	@return	The actual length of the type name.
 */
DRIVER_EXPORT int WINAPI IdacTypeName(char* pName, int& nLength);

/**
 *	IdacAIOffset
 *	Idac Analog In Offset
 *	Sets the analog-in offset to the specified voltage.
 *	@param	nChan, DWORD. The channel id to set the offset of.
 *	@param	dOffset, DOUBLE. The offset to use in volts.
 *	@return	TRUE on success.
 */
DRIVER_EXPORT BOOL WINAPI IdacAISetOffset(DWORD nChan, DOUBLE dOffset);

/**
 *	IdacAIOffsetRange
 *	Get Idac Analog In Offset Range
 *	Gets the analog-in offset range..
 *	@param	dRangeMin, DOUBLE&. Is set to range minimum (uV).
 *	@param	dRangeMax, DOUBLE&. Is set to range maximum (uV).
 */
DRIVER_EXPORT void WINAPI IdacAIOffsetRange(DOUBLE &dRangeMin, DOUBLE &dRangeMax);


//	IdacGetNrOfDigitalChan
//	Get the number of digital channels
//	@return UINT the number of digital channels that the IDAC has.
DRIVER_EXPORT	UINT WINAPI	IdacGetNrOfDigitalChan();

<<<<<<< HEAD
/************************************************************************/
/*	Author: X. Vink                                                                      
/*  Date  : 17/10/2003                                                                    
/************************************************************************/
=======
//	Author: X. Vink
//  Date  : 17/10/2003
// ************************************************************************
>>>>>>> 4ca987722c3399d2e8a78775d2ea1dbb386ca116
// *	Register a window to receive Device Notifications
// *	Device notifications are always from the idac who's driver handle
// *	is specified in dbch.
// *
// *	hWnd HWND. Handle to the window that will receive the notifications
// *	dbch DEV_BROADCAST_HANDLE&. 
// *					all members zero except for 
// *						dbch_hdevnotify, which should contain an 
// *							earlier received handle to a interface notification.
// *						dbch_handle, which may contain an earlier
// *							received driver handle or zero.
// *					On return:
// *					dbch_handle will contain a handle to the devicedriver
// *						which should be	released on a remove message 
// *						or program end.
// *	return:	NULL on failure or a notification handle on success.
// */
//DRIVER_EXPORT   HDEVNOTIFY	WINAPI IdacRegDevNotify(HWND hWnd, DEV_BROADCAST_HANDLE &dbch);
///* Unregister a previously registered notification
// *	
// */
//DRIVER_EXPORT	void	WINAPI IdacUnregNotify(HDEVNOTIFY hNotify);
///* Register a window for interface notifications
// * Interface notifications can be sent by any idac connected	
// */
//DRIVER_EXPORT	HDEVNOTIFY	WINAPI IdacRegIntNotify(HWND hWnd);

#ifdef  __cplusplus
}
#endif

#endif	/* IDACPC_H */

