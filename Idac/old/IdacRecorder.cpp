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

#include "IdacRecorder.h"

#include <QtDebug>

#include <Check.h>
#include <Globals.h>
#include <IdacControl/Idacpc.h>
#include <IdacStart.h>


IdacRecorder::IdacRecorder(QObject* parent)
	: QObject(parent)
{
	init();
}

IdacRecorder::~IdacRecorder()
{
	if (m_bConnected)
		disconnect();
}

void IdacRecorder::init()
{
	m_state = IdacState_Inactive;

	m_asErrors.clear();

	m_bConnected = false;
	m_bActive = false;
	m_bReportingError = false;
	m_nAddress = 0;

	m_errors = 0;
}

void IdacRecorder::requestState(int state_)
{
	IdacState state = (IdacState) state_;

	switch (state)
	{
	case IdacState_NotAvailable:
		break;
	case IdacState_Inactive:
		disconnect();
		break;
	case IdacState_Connected:
		if (m_state == IdacState_Receiving)
			stop();
		else
			connect();
		break;
	case IdacState_Receiving:
		record();
		break;
	}
}

void IdacRecorder::commandFinished(IdacState state)
{
	m_state = state;
	emit commandFinished();
}

void IdacRecorder::connect()
{
	init();

	// Connect to the IDAC DLL
	long lVersion = IdacLibVersion();
	if (lVersion != IDAC_LIB_VERSION)
	{
		m_asErrors << tr("Incorrect DLL version!");
		commandFinished(IdacState_NotAvailable);
		return;
	}

	LPCSTR pResult = IdacLock("IdacTest");
	if (pResult)
	{
		m_asErrors << tr("DLL in use by: %0").arg(pResult);
		commandFinished(IdacState_Inactive);
		return;
	}

	IdacStart start;
	IdacStart::BootResult result = start.Boot();
	IdacState state = IdacState_Inactive;
	switch (result)
	{
	case IdacStart::Success:
		m_nAddress = start.Address();
		state = IdacState_Connected;
		break;

	case IdacStart::Failure:
		IdacUnlock();
		state = IdacState_NotAvailable;
		break;

	case IdacStart::Retry:
		IdacUnlock();
		state = IdacState_Inactive;
		break;
	}

	commandFinished(state);
}

bool IdacRecorder::disconnect()
{
	IdacState state = m_state;
	switch (m_state)
	{
	case IdacState_NotAvailable:
	case IdacState_Inactive:
		break;
	case IdacState_Connected:
	case IdacState_Receiving:
		stop();
		state = IdacState_Inactive;
		break;
	}

	IdacUnlock();
	m_nAddress = 0;
	commandFinished(state);

	return true;
}

void IdacRecorder::transferSettings()
{
	for (int i = 0; i < 3; i++)
	{
		const IdacChannelSettings& chan = Globals->idacSettings()->channels[i];

		IdacEnableChannel(i, chan.bEnabled);
		IdacSetDecimation(i, chan.nDecimation);
		if (i != 0)
		{
			IdacScaleRange(i, chan.iRange);
			sendChannelSettings(i);
		}
	}

	/*
	for (int i = 0; i < 3; i++)
	{
		qDebug() << "transferSettings:" << i << IdacGetChannelBaseRate(i) << IdacGetChannelDecimation(i);
	}
	*/
}

void IdacRecorder::record()
{
	transferSettings();

	//double nBaseRate = IdacGetChannelBaseRate(1);

	IdacState state = m_state;
	bool bReceiving = IdacSmpStart();
	if (bReceiving)
		state = IdacState_Receiving;
	else
		m_asErrors << tr("Start failed");

	commandFinished(state);
}

// Stop recording
void IdacRecorder::stop()
{
	IdacState state = m_state;

	if (m_state == IdacState_Receiving)
	{
		bool bStopped = IdacSmpStop();

		if (bStopped)
			state = IdacState_Connected;
		else
			m_asErrors << tr("Stop failed");

		clear();
	}

	commandFinished(state);
}

void IdacRecorder::sendChannelSettings(int iChannel)
{
	const IdacChannelSettings& chan = Globals->idacSettings()->channels[iChannel];
	IdacLowPass(iChannel, chan.iLowpass);
	IdacHighPass(iChannel, chan.iHighpass);
	IdacSetOffsetAnalogIn(iChannel, chan.nOffset);
	commandFinished(m_state);
}

bool IdacRecorder::grabDataFromHardware()
{
	CHECK_PRECOND_RETVAL(m_state == IdacState_Receiving, false);

	CDD32_STATUS cdErr = IdacGetStatusFlags(true);

	// FIXME: need to address the timeout error -- we keep timing out!
	//if (cdErr.bSyncFail || cdErr.bHwOverflow || cdErr.bSwOverflow || cdErr.bCommErr || cdErr.bTimeout)
	if (cdErr.bSyncFail || cdErr.bHwOverflow || cdErr.bSwOverflow || cdErr.bCommErr)
	{
		if (cdErr.bSyncFail) m_errors |= IdacError_SyncFail;
		if (cdErr.bHwOverflow) m_errors |= IdacError_HwOverflow;
		if (cdErr.bSwOverflow) m_errors |= IdacError_SwOverflow;
		if (cdErr.bCommErr) m_errors |= IdacError_CommErr;
		if (cdErr.bTimeout) m_errors |= IdacError_Timeout;
	}

	// 2. Read data and send to display
	DWORD n = 0;
	LPCDD32_SAMPLE lpcs = NULL;

	// FIXME: for debug only
	//int nChannel0 = 0;
	// ENDFIX

	//qDebug() << "\tg1";
	QMutexLocker dataLocker(&m_dataMutex);
	//qDebug() << "\tg2";
	while (lpcs = IdacLockReadBuffer(&n))
	{
		while (n > 0)
		{
			int iChannel = (int) lpcs->uChannel;
			if (iChannel >= 0 && iChannel < 3)
			{
				m_channelData[iChannel].append(lpcs->nSample);
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

	return true;
}

void IdacRecorder::clear()
{
	m_errors = 0;
	for (int i = 0; i < 3; i++)
		m_channelData[i].clear();
}

QList<short> IdacRecorder::takeChannelData(int iChannel)
{
	QList<short> data;
	
	CHECK_PARAM_RETVAL(iChannel >= 0 && iChannel < 3, data);
	
	//qDebug() << "\t\tr1";
	QMutexLocker dataLocker(&m_dataMutex);
	//qDebug() << "\t\tr2";
	data = m_channelData[iChannel];
	// REFACTOR: consider using QVector instead of QList -- ellis, 2008-06-11
	m_channelData[iChannel].clear();
	
	return data;
}
