#ifndef __IDACDRIVERESDEFINES_H
#define __IDACDRIVERESDEFINES_H

extern "C" {
#include "IdacControl/Sample.h"
#include "IdacControl/Ipc.h"
}

#include <DBT.H>
#include <WINUSER.H>

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

#endif
