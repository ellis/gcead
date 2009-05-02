/**
 * Copyright (C) 2008  Ellis Whitehead
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

#ifndef __IDACENUMS_H
#define __IDACENUMS_H


enum IdacState
{
	IdacState_None,
	IdacState_Searching,
	IdacState_NotPresent,
	IdacState_Present,
	IdacState_Initializing,
	IdacState_InitError,
	IdacState_Ready,
	IdacState_Sampling,
};
Q_ENUMS(IdacState);


enum IdacError
{
	IdacError_SyncFail = 0x01,
	IdacError_HwOverflow = 0x02,
	IdacError_SwOverflow = 0x04,
	IdacError_CommErr = 0x08,
	IdacError_Timeout = 0x10,
};
Q_DECLARE_FLAGS(IdacErrors, IdacError);
Q_DECLARE_OPERATORS_FOR_FLAGS(IdacErrors);


enum IdacCommand
{
	IdacCommand_None,
	IdacCommand_Connect,
	IdacCommand_Disconnect,
	IdacCommand_SamplingOn,
	IdacCommand_SamplingOff,
	IdacCommand_ConfigCh0,
	IdacCommand_ConfigCh1,
	IdacCommand_ConfigCh2,
};

#endif
