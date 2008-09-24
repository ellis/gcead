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

// REFACTOR: Rename file and class to PublishSettings (without the "er" after Publish)
// REFACTOR: Remove "Publish" prefix from all the variables
#ifndef __PUBLISHSETTINGS_H
#define __PUBLISHSETTINGS_H

#include <QColor>

#include "EadEnums.h"


class PublisherSettings
{
public:
	PublisherSettings();

	//
	// Publish settings
	//

	/// True if nPublishCols should be used
	bool bPublishCols;
	/// Number of colums in the graph
	int nPublishCols;
	/// True if nPublishTimebase should be used
	bool bPublishTimebase;
	/// User-specified number of minutes per division
	int nPublishTimebase;
	/// True if nPublishSize should be used
	bool bPublishHeight;
	/// Fixed height of chart (0 for no fixed height)
	int nPublishSize;

	/// Should the publish chart be in color or b/w?
	bool bPublishInColor;
	/// Color of the border for publishing
	QColor clrPublishBorder;

	/// 0 = File, 1 = Print
	int iPublishMode;

	ChartElements publisherChartElements;
};

#endif
