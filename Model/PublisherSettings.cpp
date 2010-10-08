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

#include "PublisherSettings.h"


PublisherSettings::PublisherSettings()
{
	publisherChartElements = ChartElement_AxisTime | ChartElement_Grid | ChartElement_WaveNames | ChartElement_WaveComments | ChartElement_StdDev;

	bPublishCols = false;
	nPublishCols = 10;
	bPublishTimebase = false;
	nPublishTimebase = 1;
	bPublishHeight = false;
	nPublishSize = 500;

	bPublishInColor = true;
	clrPublishBorder = QColor(128, 128, 200); // 180, 180, 255
}
