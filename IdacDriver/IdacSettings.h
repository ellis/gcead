/**
 * Copyright (C) 2008,2009  Ellis Whitehead
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

#ifndef __IDACSETTINGS_H
#define __IDACSETTINGS_H

#include <QtGlobal> // for uchar

#include "IdacChannelSettings.h"


// REFACTOR: This class has nothing to do with IdacDriver, so move it to a different appropriate module -- ellis, 2009-04-26
class IdacSettings
{
public:
	/// Whether to record when trigger signal comes
	bool bRecordOnTrigger;
	/// Number of minutes to record for (0 = unlimited)
	int nRecordingDuration;
	/// Preset delay for the FID signal in milliseconds (ms)
	int nGcDelay_ms;
	/// Settings for the individual channels
	IdacChannelSettings channels[3];
};


#endif
