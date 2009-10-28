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

#ifndef __RENDERDATA_H
#define __RENDERDATA_H

#include <QVector>


class ViewWaveInfo;


/// Holds the min and max y values for a given x-coordinate
struct MinMax
{
	double yBot, yTop;

	MinMax()
		: yBot(0), yTop(0)
	{
	}

	MinMax(short yBot, short yTop)
		: yBot(yBot), yTop(yTop)
	{
	}
};


/// Class to calculate rendering points for wave for fast rendering later.
class RenderData
{
public:
	/// First timeline index sample to be rendered
	int tidxStart;
	double nSamplesPerPixel;

	/// The number of rendered pixels
	int nPixelsRequested;
	/// The number of rendered pixels
	int nPixels;
	/// First data index sample where rendering should be done if possible
	int didxFirstRequested;
	/// Last data index sample where rendering should be done if possible
	int didxLastRequested;
	/// First data index sample actually rendered (always >= 0)
	int didxFirst;
	/// Last data index sample actually rendered (always <= sample count - 1)
	int didxLast;
	/// Number of pixels offset from left edge of tidxStart, where rendering of didxFirst actually starts
	int xOffset;

	QVector<MinMax> pixels;

	RenderData();

	void setup(ViewWaveInfo* wave, int tidxStart, int nPixels, double nSamplesPerPixel);
	void render(ViewWaveInfo* wave, int tidxStart, int nPixels, double nSamplesPerPixel);
	/// Render the standard deviation area around the average
	void renderStd(ViewWaveInfo* wave, int tidxStart, int nPixels, double nSamplesPerPixel);
};

#endif
