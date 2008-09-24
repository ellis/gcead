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

#include "IdacStart.h"

#include <QApplication>
#include <QChar>
#include <QDir>

#include "Check.h"
#include "IdacControl/Idacpc.h"
#include "WaitCursor.h"


#define MAXSIGNALS_V2_V3	4


static char* BinFilename[2][MAXSIGNALS_V2_V3] =
	{
		{ "ASPK32_1.BIN", "ASPK32_2.BIN", "ASPK32_3.BIN", "ASPK32_4.BIN" },
		{ "ASPK32U1.BIN", "ASPK32U2.BIN", "ASPK32U3.BIN", "ASPK32U4.BIN" }
	};

const char* IDS_BOOT_RETRY1 = QT_TR_NOOP("ERROR: IDAC%0 failed to boot\nPossible error:\n* No IDAC device connected\n* Main power is not connected");
const char* IDS_BOOT_RETRY2 = QT_TR_NOOP("Error starting the IDAC%0\r\nConfirm that the IDAC device is available and switched on.");


IdacStart::IdacStart()
{
	m_Address = 0;
	m_Version = 0;
}

IdacStart::~IdacStart()
{
}

/*
int IdacStart::Boot(int NrChannels)
{
	int				Present;
	LONG			Adr;
	
	Adr = IdacPresent(-1);

	if (Adr != 0)
	{
		QByteArray acPath = GetBinFileName(NrChannels - 1).toAscii();
		LPCSTR pcPath = static_cast<LPCSTR>(acPath.constData());
		Present = IdacBoot(pcPath, Adr);
	}
	else
		Present = 0;

	int nIdacType;

	switch (Present)
	{
	case 0:
		m_Address = 0;					// No board found
		m_Version = 0;
		m_BinFilename = "";

		return IDACSTART_NOIDACBOARD;

	case 1:
		// check if version is returned
		nIdacType = IdacType();
		switch (nIdacType)
		{
		case IDAC_TYPE_NONE:
		case IDAC_TYPE_SERIAL:

			m_Address = Adr;				// OK
			m_Version = 0;
			m_BinFilename = "";
			return IDACSTART_NOERR;

		case IDAC_TYPE_ISA_V2:
		case IDAC_TYPE_USB_V3:

			m_Address = Adr;
			m_Version = IdacGetDSPFirmwareVersion();
			m_BinFilename = GetBinFileName(NrChannels - 1);

			if (m_Version == 0xA501)
			{
				return IDACSTART_NOERR;		// OK
			}
			else
			{
				return IDACSTART_TIMEOUT;	// Found, but failed to start
			}

		default:
			CHECK_FAILURE_RETVAL(IDACSTART_NOIDACBOARD);					// Board type not supported
		}

	case 2:
		m_Address = 0;					// No DSP file
		m_Version = 0;
		m_BinFilename = "";

		return IDACSTART_NOBINFILE;

	case 3:
		m_Address = 0;					// Failed during download
		m_Version = 0;
		m_BinFilename = "";

		return IDACSTART_TIMEOUT;

	default:
		CHECK_FAILURE_RETVAL(IDACSTART_NOIDACBOARD);					// Unexpected answer
	}
}
*/

IdacStart::BootResult IdacStart::Boot()
{
	QString			strMsg;
	int				Key;
	int				Present;
	LONG			Adr;

	m_sMessage.clear();

	switch (IdacType())
	{
	case IDAC_TYPE_2000:
		return boot_2000_4("2000");

	case IDAC_TYPE_4:
		return boot_2000_4("4");

	case IDAC_TYPE_NONE:
	case IDAC_TYPE_SERIAL:
		/* FIXME:
		m_Address = RegInfo . m_IdacAddress;
		Present = IdacPresent(RegInfo . m_IdacAddress);

		if (Present == 0)
			return false;
		break;
		*/
		m_sMessage = QObject::tr("Your IDAC type is not supported");
		return Failure;
	
	case IDAC_TYPE_ISA_V2:
	case IDAC_TYPE_USB_V3:
		return boot_ISA_USB();

	default:
		m_sMessage = QObject::tr("Your IDAC device is not supported");
		return Failure;
	}

	// FIXME: RegInfo.m_IdacAddress = Address();
}

IdacStart::BootResult IdacStart::boot_2000_4(const QString& sType)
{
	BootResult result = Retry;

	int Adr = IdacPresent(0);
	// Device found:
	if (Adr != 0)
	{
		//WaitCursor WaitCurs;
		int Present = IdacBoot(NULL, 1);
		// Device not booted successfully:
		if(Present == 0 || Present == 3)
			m_sMessage = QObject::tr(IDS_BOOT_RETRY1).arg(sType);
		else
			result = Success;
	}
	// Device not found:
	else
	{
		m_sMessage = QObject::tr(IDS_BOOT_RETRY2).arg(sType);
	}

	return result;
}

IdacStart::BootResult IdacStart::boot_ISA_USB()
{
	// Boot the IDAC and check for proper response of all .bin files
	for (unsigned int Signal = 1; Signal < IdacGetChannelCount(); Signal++)
	{
		BootResult result = Retry;

		// FIXME: switch(BootAtAddress(RegInfo . m_IdacAddress, Signal))
		switch(BootAtAddress(-1, Signal))
		// ENDFIX
		{
		case IDACSTART_NOIDACBOARD:
			m_sMessage = QObject::tr(IDS_BOOT_RETRY1).arg("3");
			break;

		case IDACSTART_TIMEOUT:
			m_sMessage = QObject::tr("Response Timeout from IDAC3 (Ch %0)").arg(Signal);
			break;

		case IDACSTART_NOBINFILE:
			result = Failure;
			m_sMessage = QObject::tr("File not found: %0").arg(GetBinFileName(Signal - 1));
			break;

		default:
			result = Success;
			break;
		}

		if (result != Success)
			return result;
	}

	return Success;
}

int IdacStart::BootAtAddress(int Address, int NrChannels)
{
	int				Present;
	LONG			Adr;
	
 	Adr = IdacPresent(Address);

	Present = 0;

	if (Adr != 0)
	{
		QString sPath = GetBinFileName(NrChannels - 1);
		if (!sPath.isEmpty())
		{
			QByteArray acPath = sPath.toAscii();
			LPCSTR pcPath = static_cast<LPCSTR>(acPath.constData());
			Present = IdacBoot(pcPath, Adr);
		}
	}

	switch (Present)
	{
	case 0:
		m_Address = 0;					// No board found
		m_Version = 0;
		m_BinFilename = "";

		return IDACSTART_NOIDACBOARD;

	case 1:
		// check if version is returned
		switch (IdacType())
		{
		case IDAC_TYPE_NONE:
		case IDAC_TYPE_SERIAL:

			m_Address = Adr;				// OK
			m_Version = 0;
			m_BinFilename = "";
			return IDACSTART_NOERR;

		case IDAC_TYPE_ISA_V2:
		case IDAC_TYPE_USB_V3:

			m_Address = Adr;
			m_Version = IdacGetDSPFirmwareVersion();
			m_BinFilename = GetBinFileName(NrChannels - 1);

			if (m_Version == 0xA501)
			{
				return IDACSTART_NOERR;		// OK
			}
			else
			{
				return IDACSTART_TIMEOUT;	// Found, but failed to start
			}

		default:
			CHECK_FAILURE_RETVAL(IDACSTART_NOIDACBOARD);					// Board type not supported
		}

	case 2:
		m_Address = 0;					// No DSP file
		m_Version = 0;
		m_BinFilename = "";

		return IDACSTART_NOBINFILE;

	case 3:
		m_Address = 0;					// Failed during download
		m_Version = 0;
		m_BinFilename = "";

		return IDACSTART_TIMEOUT;

	default:
		CHECK_FAILURE_RETVAL(IDACSTART_NOIDACBOARD);					// Unexpected answer
	}
}

QString IdacStart::GetBinFileName(int NrOfSignals)
{
	QString sPath;

	if (NrOfSignals < MAXSIGNALS_V2_V3)
	{
		//sPath = QApplication::applicationFilePath();
		// FIXME: sPath = QCoreApplication::applicationDirPath(); // This doesn't for some reason
		//sPath = QApplication::applicationDirPath() + "/";
		sPath = QDir::currentPath() + "/";
		const char* pName = BinFilename[IdacType() != IDAC_TYPE_ISA_V2][NrOfSignals];
		sPath += pName;
		sPath = QDir::toNativeSeparators(sPath);
	}

	return sPath;
}
