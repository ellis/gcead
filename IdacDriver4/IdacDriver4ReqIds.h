/**
 * Copyright (C) 2009  Ellis Whitehead
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __IDACDRIVER4REQIDS_H
#define __IDACDRIVER4REQIDS_H

enum REQUESTID
{
	REQUESTID_ISO_IN_START = 0x20,
	REQUESTID_ISO_IN_STOP = 0x21,
	REQUESTID_GET_DEVICESTATUS = 0x22,
	REQUESTID_RESET_FPGA = 0x23,
	REQUESTID_POWER_ON = 0x24,
	REQUESTID_POWER_OFF = 0x25,
	REQUESTID_WRITE_FPGA_REG = 0x26,
	REQUESTID_WRITE_ANALOG_IN = 0x27,
	IdacSetAnOutput,
	IdacWrConfig,
	REQUESTID_READ_CONFIG_DATA = 0x2A,
	REQUESTID_PROGRAM_FPGA_START = 0x2B,
	REQUESTID_PROGRAM_FPGA_STOP = 0x2C,
	IdacStartIsoOut,
	IdacStopIsoOut,
	REQUESTID_GET_VERSION = 0x2F,
};


#endif
