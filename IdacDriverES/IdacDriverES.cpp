/**
 * Copyright (C) 2010  Ellis Whitehead
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

#include "IdacDriverES.h"

#include <QtDebug>
#include <QDir>

#include <Check.h>

#include "IdacControl/Idacpc.h"


#define IDACSTART_NOIDACBOARD	0
#define IDACSTART_NOERR			1
#define IDACSTART_TIMEOUT		2
#define IDACSTART_NOBINFILE		3
#define MAXSIGNALS_V2_V3	4


static const char* BinFilename[2][MAXSIGNALS_V2_V3] =
	{
		{ "ASPK32_1.BIN", "ASPK32_2.BIN", "ASPK32_3.BIN", "ASPK32_4.BIN" },
		{ "ASPK32U1.BIN", "ASPK32U2.BIN", "ASPK32U3.BIN", "ASPK32U4.BIN" }
	};
static const char* IDS_BOOT_RETRY1 = QT_TR_NOOP("ERROR: IDAC%0 failed to boot\nPossible error:\n* No IDAC device connected\n* Main power is not connected");
static const char* IDS_BOOT_RETRY2 = QT_TR_NOOP("Error starting the IDAC%0\r\nConfirm that the IDAC device is available and switched on.");


IdacDriverES::IdacDriverES(QObject* parent)
	: IdacDriverWithThread(parent)
{
	m_bFirmwareSent = false;
	m_bIdac2 = false;
	m_bIdac4 = false;

	switch (IdacType()) {
	case IDAC_TYPE_2_USB:
		m_bIdac2 = true;
		setHardwareName("IDAC2");
		break;

	case IDAC_TYPE_4:
		m_bIdac4 = true;
		setHardwareName("IDAC4");
		break;

	default:
		setHardwareName("Unknown Device");
		break;
	}

}

IdacDriverES::~IdacDriverES()
{
	if (m_bFirmwareSent) {
		IdacPowerDown();
		IdacUnlock();
	}
}

void IdacDriverES::loadCaps(IdacCaps* caps)
{
	CHECK_PARAM_RET(caps != NULL);

	caps->bHighcut = IdacCapabilities(IDAC_HAS_LOWPASS);
	caps->bRangePerChannel = false;

	switch (IdacType()) {
	case IDAC_TYPE_4:
		caps->bRangePerChannel = true;
		break;
	case IDAC_TYPE_2_USB:
		caps->bRangePerChannel = false;
	}
}

void IdacDriverES::loadDefaultChannelSettings(IdacChannelSettings* channels)
{
	channels[0].mEnabled = 0x03;
	channels[0].mInvert = 0x00;
	channels[0].nDecimation = -1;

	channels[1].mEnabled = 1;
	channels[1].mInvert = 0;
	channels[1].nDecimation = -1;
	channels[1].iRange = 1;
	channels[1].iHighcut = -1;
	channels[1].iLowcut = -1;
	channels[1].nExternalAmplification = 10;

	channels[2].mEnabled = 1;
	channels[2].mInvert = 0;
	channels[2].nDecimation = 960; // 100 samples per second
	channels[2].iRange = 1;
	channels[1].iHighcut = -1;
	channels[1].iLowcut = -1;
	channels[2].nExternalAmplification = 1;

	switch (IdacType()) {
	case IDAC_TYPE_4:
		channels[0].nDecimation = 960; // 100 samples per second

		channels[1].nDecimation = 960; // 100 samples per second
		channels[1].iRange = 3;
		channels[1].iHighcut = 10; // 3kHz on IDAC4
		channels[1].iLowcut = 1; // 0.1 Hz on IDAC4

		channels[2].nDecimation = 960; // 100 samples per second
		channels[2].iRange = 4;
		channels[2].iHighcut = 10; // 3kHz on IDAC4
		channels[2].iLowcut = 1; // 0.1 Hz on IDAC4
		break;

	case IDAC_TYPE_2_USB:
		channels[0].mInvert = 0x01; // Invert the trigger channel

		channels[1].iRange = 3;
		channels[1].iHighcut = -1;
		channels[1].iLowcut = 1; // 0.05 Hz on IDAC2

		channels[2].iRange = 3;
		channels[2].iHighcut = -1;
		channels[2].iLowcut = 1; // 0.05 Hz on IDAC2
		break;
	}
}

bool IdacDriverES::checkUsbFirmwareReady()
{
	return true;
}

bool IdacDriverES::checkDataFirmwareReady()
{
	return m_bFirmwareSent;
}

void IdacDriverES::initUsbFirmware()
{
}

void IdacDriverES::initDataFirmware()
{
	// Connect to the IDAC DLL
	long lVersion = IdacLibVersion();
	if (lVersion != IDAC_LIB_VERSION)
	{
		addError(tr("Incorrect DLL version!"));
		return;
	}

	LPCSTR pResult = IdacLock("IdacTest");
	if (pResult)
	{
		addError(tr("DLL in use by: %0").arg(pResult));
		return;
	}

	m_bFirmwareSent = boot();
	if (!m_bFirmwareSent) {
		IdacUnlock();
		return;
	}

	// Get range information from the hardware
	LPDWORD pRanges = IdacGetRanges();
	QList<int> ranges;
	for (LPDWORD pnRange = pRanges; int(*pnRange) != -1; pnRange++)
	{
		ranges << *pnRange;
	}
	setRanges(ranges);
	qDebug() << ranges;

	// Get low-pass filter information from the hardware
	int nStrings;
	const char** asStrings;
	IdacLowPassStrings(&nStrings, &asStrings);
	QStringList list;
	for (int i = 0; i < nStrings; i++)
		list << asStrings[i];
	setHighcutStrings(list);

	// Get high-pass filter information from the hardware
	IdacHighPassStrings(&nStrings, &asStrings);
	list.clear();
	for (int i = 0; i < nStrings; i++)
		list << asStrings[i];
	setLowcutStrings(list);
}

bool IdacDriverES::boot()
{
	//QString			strMsg;
	//int				Key;
	//int				Present;
	//LONG			Adr;

	int n = IdacType();
	switch (n)
	{
	case IDAC_TYPE_2_USB:
		return boot_2_USB();

	case IDAC_TYPE_4:
		return boot_2000_4("4");

	/*case IDAC_TYPE_2000:
		return boot_2000_4("2000");

	case IDAC_TYPE_SERIAL:
		/ * FIXME:
		m_Address = RegInfo . m_IdacAddress;
		Present = IdacPresent(RegInfo . m_IdacAddress);

		if (Present == 0)
			return false;
		break;
		* /
		addError(tr("Your IDAC type is not supported"));
		return false;

	case IDAC_TYPE_ISA_V2:
	case IDAC_TYPE_USB_V3:
		return boot_ISA_USB();
	*/

	default:
		addError(tr("Your IDAC device is not supported"));
		return false;
	}

	// FIXME: RegInfo.m_IdacAddress = Address();
}

bool IdacDriverES::boot_2000_4(const QString& sType)
{
	bool b = false;

	int Adr = IdacPresent(0);
	// Device found:
	if (Adr != 0)
	{
		int Present = IdacBoot(NULL, 1);
		// Device not booted successfully:
		if(Present == 0 || Present == 3)
			addError(tr(IDS_BOOT_RETRY1).arg(sType));
		else
			b = true;
	}
	// Device not found:
	else
	{
		addError(tr(IDS_BOOT_RETRY2).arg(sType));
	}

	return b;
}

bool IdacDriverES::boot_2_USB()
{
	CHECK_ASSERT_RETVAL(IdacType() == IDAC_TYPE_2_USB, false);
	CHECK_ASSERT_RETVAL(IdacPresent(0), false);

	bool b = false;
	DWORD Result = IdacBoot(NULL, 1);
	switch (Result)
	{
	case 0:
		addError(tr("Invalid address"));
		break;
	case 1:
		b = true;
		break;
	case 3:
		addError(tr("Booting failed"));
		break;
	default:
		addError(tr("Error"));
		break;
	}
	return b;
}

bool IdacDriverES::boot_ISA_USB()
{
	// Boot the IDAC and check for proper response of all .bin files
	for (unsigned int Signal = 1; Signal < IdacGetChannelCount(); Signal++)
	{
		bool result = false;

		// FIXME: switch(BootAtAddress(RegInfo . m_IdacAddress, Signal))
		switch(bootAtAddress(-1, Signal))
		// ENDFIX
		{
		case IDACSTART_NOIDACBOARD:
			addError(tr(IDS_BOOT_RETRY1).arg("3"));
			break;

		case IDACSTART_TIMEOUT:
			addError(tr("Response Timeout from IDAC3 (Ch %0)").arg(Signal));
			break;

		case IDACSTART_NOBINFILE:
			result = false;
			addError(tr("File not found: %0").arg(getBinFileName(Signal - 1)));
			break;

		default:
			result = true;
			break;
		}

		if (result != true)
			return result;
	}

	return true;
}

int IdacDriverES::bootAtAddress(int Address, int NrChannels)
{
	int m_Address = 0;
	int m_Version = 0;
	QString m_BinFilename;

	int				Present;
	LONG			Adr;

	Adr = IdacPresent(Address);

	Present = 0;

	if (Adr != 0)
	{
		QString sPath = getBinFileName(NrChannels - 1);
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
			m_BinFilename = getBinFileName(NrChannels - 1);

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

QString IdacDriverES::getBinFileName(int NrOfSignals)
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

void IdacDriverES::configureChannel(int iChan)
{
	const IdacChannelSettings* chan = desiredChannelSettings(iChan);
	CHECK_ASSERT_RET(chan != NULL);

	IdacLowPass(iChan, chan->iHighcut);
	IdacHighPass(iChan, chan->iLowcut);
	IdacSetOffsetAnalogIn(iChan, chan->nOffset);
}

bool IdacDriverES::startSampling()
{
	for (int iChan = 0; iChan < 3; iChan++)
	{
		const IdacChannelSettings* chan = desiredChannelSettings(iChan);
		CHECK_ASSERT_RETVAL(chan != NULL, false);

		IdacEnableChannel(iChan, chan->mEnabled);
		IdacSetDecimation(iChan, chan->nDecimation);
		if (iChan != 0)
		{
			IdacScaleRange(iChan, chan->iRange);
			IdacLowPass(iChan, chan->iHighcut);
			IdacHighPass(iChan, chan->iLowcut);
			IdacSetOffsetAnalogIn(iChan, chan->nOffset);
		}
	}

	startSamplingThread();
	return true;
}

void IdacDriverES::sampleLoop() {
	IdacSmpStart();
	while (m_bSampling) {
		grabDataFromDll();
		msleep(50);
	}
	IdacSmpStop();
}

void IdacDriverES::grabDataFromDll()
{
	CDD32_STATUS cdErr = IdacGetStatusFlags(true);

	// FIXME: need to address the timeout error -- we keep timing out!
	//if (cdErr.bSyncFail || cdErr.bHwOverflow || cdErr.bSwOverflow || cdErr.bCommErr || cdErr.bTimeout)
	if (cdErr.bSyncFail || cdErr.bHwOverflow || cdErr.bSwOverflow || cdErr.bCommErr)
	{
		if (cdErr.bSyncFail) addError(tr("SYNC failed"));
		if (cdErr.bHwOverflow) addError(tr("Hardware Overflow"));
		if (cdErr.bSwOverflow) addError(tr("Software Overflow"));
		if (cdErr.bCommErr) addError(tr("Communication error"));
		if (cdErr.bTimeout) addError(tr("Timeout"));
	}

	// 2. Read data and send to display
	DWORD n = 0;
	LPCDD32_SAMPLE lpcs = NULL;

	// FIXME: for debug only
	//int nChannel0 = 0;
	// ENDFIX

	//qDebug() << "\tg1";
	//QMutexLocker dataLocker(&m_dataMutex);
	//qDebug() << "\tg2";
	short data[3];
	while ((lpcs = IdacLockReadBuffer(&n)))
	{
		while (n > 0)
		{
			int iChannel = (int) lpcs->uChannel;
			if (iChannel >= 0 && iChannel < 3)
			{
				data[iChannel] = lpcs->nSample;
				if (iChannel == 2) {
					addSample(~data[0], data[1], data[2]);
				}
			}
			// FIXME: for debug only
			//if (iChannel == 0)
			//	nChannel0++;
			// ENDFIX

			lpcs++;
			n--;
		}

		IdacUnlockReadBuffer();
	}

	// FIXME: for debug only
	//qDebug() << "grab T:" << QTime::currentTime().msec() << nChannel0 << m_channelData[0].size();
	// ENDFIX
}
