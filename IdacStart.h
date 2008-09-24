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

#ifndef __IDACSTART_H
#define __IDACSTART_H

#include <QString>


#define IDACSTART_NOIDACBOARD	0
#define IDACSTART_NOERR			1
#define IDACSTART_TIMEOUT		2
#define IDACSTART_NOBINFILE		3

#define IDACBASE_ADDRESS	0x300


class IdacStart
{
public:
	enum BootResult
	{
		Success,
		Failure,
		Retry,
	};

// Constructor/destructor
public:
	IdacStart();
	~IdacStart();

// Attributes
public:
	int Address() { return m_Address; }
	int Version() { return m_Version; }
	QString Filename() { return m_BinFilename; }

// Operations
public:
	/*
	/// @return 0 = No IDAC board found, 1 = Booted, >1 = No bin file found
	int Boot(int NrChannels);
	*/

	/// boots all three bin files and returns true if succesfull
	BootResult Boot();

private:
	BootResult boot_2000_4(const QString& sType);
	BootResult boot_ISA_USB();
	/// @return 0 = No IDAC board found, 1 = Booted, >1 = No bin file found
	int BootAtAddress(int Address, int NrChannels);
	QString GetBinFileName(int NrOfSignals);

// Members
private:
	int m_Address;
	int m_Version;
	QString m_BinFilename;

	/// Message for user in case of failure or retry
	QString m_sMessage;
};

//extern CIdacStart IdacStart;


#endif
