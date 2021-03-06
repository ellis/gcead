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

#ifndef __UTILS_H
#define __UTILS_H

#include <QPainterPath>
#include <QString>


enum TimestampBase
{
	TimestampBase_Milli,
	TimestampBase_Seconds,
	TimestampBase_Minutes,
};


extern QString timestampString(double nSeconds, TimestampBase base);
extern QString timestampString(double nSeconds, TimestampBase* pbase = NULL);

/// Increase or decrease the given nVoltsPerDivision in the direction given by nDirection.
/// @param nVoltsPerDivision volts/div in mV units
/// @param nDirection 1 = increase, -1 = decrease
extern double changeVoltsPerDivision(double nVoltsPerDivision, int nDirection);

extern QPainterPath roundedRect(const QRect& rc, int nRadius);

#endif
