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

// Implementation file for the Channelstate class
//
// M Smitshoek, 17-1-2000

#include "IdacDriver4Channel.h"

#include <string.h>

#include <IdacDriver/IdacChannelSettings.h>


// Special constants for IDAC
// --------------------------

const qint16	SMP_SYNCWORD	= 0x7FFF;

/****************************************/
/* ChannelState class					*/
/****************************************/

const int nIdacChannelCount = 5;

// Construction
IdacDriver4Channel::IdacDriver4Channel(bool bInvertDigital) :
	m_bInvertDigital(bInvertDigital)
{
	m_pDecCounter = new quint16[nIdacChannelCount];

	Reset(0);
}

IdacDriver4Channel::~IdacDriver4Channel()
{
	delete[] m_pDecCounter;
}

// Initialization
void IdacDriver4Channel::Reset(quint16 wSyncCount)
{
	m_bSynchronized	= FALSE;	// TRUE will generally work but there will be sync errors at higher speeds
	m_bDigital		= FALSE;

	m_wSyncTracker	= 0;
	m_wSyncCount	= wSyncCount;

	memset(m_pDecCounter, 0, nIdacChannelCount * sizeof(quint16));
}


// Parse a sample

// Input:
// wRead		- Data received from IDAC
// sr			- Sample record that is to receive the sample
// csStat		- Will receive error flags if applicable
//
// Returns:
// TRUE			- sr contains an analog or digital sample
// FALSE		- only a temporary status was set, or an error occured

bool IdacDriver4Channel::ParseSample(quint16 wRead, CDD32_SAMPLE& sr, CDD32_STATUS& csStat, const QVector<IdacChannelSettings>& channels)
{
	qint16 nRead = (qint16) wRead;

	if (nRead != SMP_SYNCWORD)
	{
		// if not synchronised: dont process samples
		if (!m_bSynchronized) return FALSE;

		quint8 Chan = GetNextAnChannel(channels);

		// digital channel
		if (Chan == 0)
		{
			sr.wDigitalSample	= (m_bInvertDigital ? ~wRead : wRead) & 0xFF;
			sr.uChannel			= 0;
			sr.flags			= csStat;

			return TRUE;
		}
		// analog channels
		else
		{
			sr.nSample		= nRead;
			sr.uChannel		= Chan;
			sr.flags		= csStat;

			return TRUE;
		}
	}
	else
	{
		// Escape codes

		switch (nRead)
		{
		case SMP_SYNCWORD:
			Synchronize(channels);
			break;

		default:
			// This should never happen, this signals a communication fault
			csStat.bCommErr = TRUE;
			break;
		}
	}

	return FALSE;	
}

// Respond to sync word
void IdacDriver4Channel::Synchronize(const QVector<IdacChannelSettings>& channels)
{
	// Detect first synchronization
	if (!m_bSynchronized)
	{
		m_bSynchronized = TRUE;
	}

	for (quint8 Chan = 0; Chan < nIdacChannelCount; Chan++)
	{
		// Decimations - 1 will speed up GetNextAnChannel()
		if (Chan < 3 && channels[Chan].mEnabled)
			m_pDecCounter[Chan] = 0;
		else
			m_pDecCounter[Chan] = 0xffff;
	}
}

// Return next analog channel in line, or 0 if not available
quint8 IdacDriver4Channel::GetNextAnChannel(const QVector<IdacChannelSettings>& channels)
{
	if (!m_bSynchronized) return 0;

	quint16 wLowestCounter = 0xffff;		// lowest counter found
	quint8 FirstChannel = 0;				// channel that has lowest counter

	// Find channel that has samples available now OR has lowest counter
	for (quint8 Chan = 0; Chan < nIdacChannelCount; Chan++)
	{
		if (Chan < 3 && channels[Chan].mEnabled)
		{
			if (m_pDecCounter[Chan] == 0)
			{
				// Sample is available now

				// Return channel and recharge counter, done.
				m_pDecCounter[Chan] = (quint16) channels[Chan].nDecimation;
				return Chan;
			}

			if (m_pDecCounter[Chan] < wLowestCounter)
			{
				wLowestCounter = m_pDecCounter[Chan];
				FirstChannel = Chan;
			}
		}
	}

	// Update decimation counters to next available sample
	for (quint8 Chan = 0 ; Chan < nIdacChannelCount; Chan++)
	{
		if (Chan < 3 && channels[Chan].mEnabled)
		{
			m_pDecCounter[Chan] -= wLowestCounter;
		}
	}

	// Return channel and recharge counter
	m_pDecCounter[FirstChannel] = channels[FirstChannel].nDecimation;
	return FirstChannel;
}
