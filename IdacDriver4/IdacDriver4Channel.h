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

// IdacDriver4Channel: class for state machine
//
// IdacDriver4Channel tracks the data words from the IDAC card.
//
// This class is specific to the IDAC DSP boards.

// Input: data words as retrieved from the IDAC board
// Output: SampleRecords created according to process state
// Reset: reinitialize all members


#ifndef __IDACDRIVER4CHANNEL_H
#define __IDACDRIVER4CHANNEL_H

#include <QVector>

#include "Sample.h"


class IdacChannelSettings;


class IdacDriver4Channel
{
private:
	IdacDriver4Channel();							// Do not use

protected:
	// Constants
	const bool			m_bInvertDigital;		// Whether digital bits are straight or inverted

	// Data members
	bool				m_bSynchronized;		// Sync flag
	bool				m_bDigital;				// Digital mode

	quint16				m_wSyncTracker;			// Counter for syncword
	quint16				m_wSyncCount;			// Counter startvalue for syncword

	quint16*				m_pDecCounter;			// Decimations array

public:
	// Construction
	IdacDriver4Channel							(bool bInvert);
	~IdacDriver4Channel							();

	// Initialization
	void				Reset				(quint16 wSyncCount);

	// Parsing function
	bool				ParseSample			(quint16 wRead, CDD32_SAMPLE& sr, CDD32_STATUS& csStat, const QVector<IdacChannelSettings>& channels);

protected:
	// Private utilities
	void				Synchronize			(const QVector<IdacChannelSettings>& channels);
	quint8				GetNextAnChannel	(const QVector<IdacChannelSettings>& channels);
};

#endif
