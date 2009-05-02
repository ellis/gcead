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
	REQUESTID_ISO_IN_START = 0x29, // ?
	REQUESTID_ISO_IN_STOP = 0x2A, // ?
	REQUESTID_GET_DEVICESTATUS,
	REQUESTID_RESET_FPGA,
	REQUESTID_POWER_ON = 0x26, // ?
	REQUESTID_POWER_OFF = 0x27, // ?
	REQUESTID_WRITE_FPGA_REG,
	REQUESTID_WRITE_ANALOG_IN,
	IdacSetAnOutput,
	REQUESTID_WRITE_CHANNEL_SETTINGS = 0x28,
	REQUESTID_READ_CONFIG_DATA = 0x2C, // ??
	REQUESTID_PROGRAM_FPGA_START = 0x24,
	REQUESTID_PROGRAM_FPGA_STOP = 0x25,
	IdacStartIsoOut,
	IdacStopIsoOut,
	REQUESTID_GET_VERSION = 0x22, // ?? or perhaps REQUESTID_GET_DEVICESTATUS?
};


#endif
