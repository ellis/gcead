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

#include <Check.h>

#include "IdacControl/Idacpc.h"


IdacDriverES::IdacDriverES(QObject* parent)
	: IdacDriverWithThread(parent)
{
	m_bIdac2 = false;
	m_bIdac4 = false;
}

IdacDriverES::~IdacDriverES()
{
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
	case IDAC_TYPE_SERIAL: // I don't know if this is actually also an IDAC2
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

	m_bIdac2 = false;
	m_bIdac4 = false;

	switch (IdacType()) {
	case IDAC_TYPE_4:
		m_bIdac4 = true;
		setHardwareName("IDAC4");

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
	case IDAC_TYPE_SERIAL: // I don't know if this is actually also an IDAC2
		m_bIdac2 = true;
		setHardwareName("IDAC2");

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
	return true;
}

void IdacDriverES::initUsbFirmware()
{
}

void IdacDriverES::initDataFirmware()
{
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
					addSample(data[0], data[1], data[2]);
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
