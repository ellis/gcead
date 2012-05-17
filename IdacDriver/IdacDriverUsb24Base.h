/**
 * Copyright (C) 2012  Ellis Whitehead
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

#ifndef __IDACDRIVERUSB24BASE_H
#define __IDACDRIVERUSB24BASE_H


#include <QtGlobal> // for quint8 and related types

#include <IdacDriver/IdacDriverUsb.h>
#include <IdacDriver/IdacSettings.h>


class IdacUsb;


/// Base class for some common code for IdacDrive2 and IdacDriver4
class IdacDriverUsb24Base : public IdacDriverUsb
{
public:
	IdacDriverUsb24Base(UsbHandle* device, QObject* parent = NULL);

public:
	bool checkUsbFirmwareReady();
	bool checkDataFirmwareReady();

protected:
	bool claim(bool bUnhalt);

protected:
	bool m_bFpgaProgrammed;
};

#endif
