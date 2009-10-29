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

#include "RenderData.h"

#include <math.h>

#include <Check.h>

#include "ViewInfo.h"


RenderData::RenderData()
{
	tidxStart = 0;
	nPixels = 0;
	nSamplesPerPixel = 0;
}

void RenderData::setup(ViewWaveInfo* vwi, int tidxStart, int nPixels_, double nSamplesPerPixel)
{
	CHECK_PARAM_RET(nSamplesPerPixel > 0);

	this->tidxStart = tidxStart;
	this->nPixelsRequested = nPixels_;
	this->nSamplesPerPixel = nSamplesPerPixel;
	
	int didx1 = tidxStart - vwi->shift();
	int iPixelFirstRequested = int(didx1 / nSamplesPerPixel);
	didxFirstRequested = (int) ceil(iPixelFirstRequested * nSamplesPerPixel);
	didxFirst = qMax(didxFirstRequested, 0);

	int iPixelEndRequested = iPixelFirstRequested + nPixelsRequested;
	didxLastRequested = (int) ceil(iPixelEndRequested * nSamplesPerPixel) - 1;
	didxLast = qMin(didxLastRequested, vwi->wave()->display.size() - 1);

	int iPixelFirst = int(didxFirst / nSamplesPerPixel);
	int iPixelLast = int(didxLast / nSamplesPerPixel);

	xOffset = iPixelFirst - iPixelFirstRequested;
	nPixels = iPixelLast - iPixelFirst + 1;

	if (nPixels < 0)
		nPixels = 0;
}

void RenderData::render(ViewWaveInfo* vwi, int tidxStart_, int nPixels_, double nSamplesPerPixel_)
{
	CHECK_PARAM_RET(vwi != NULL);
	CHECK_PARAM_RET(vwi->wave() != NULL && vwi->isVisible());

	setup(vwi, tidxStart_, nPixels_, nSamplesPerPixel_);

	pixels.resize(nPixels);
	if (nPixels == 0)
		return;

	const WaveInfo* wave = vwi->wave();
	const double* data = wave->display.constData() + didxFirst;
	MinMax* pixdata = pixels.data();

	if (nSamplesPerPixel > 1)
	{
		int iSample = didxFirst;
		for (int iPixel = 0; iPixel < nPixels; iPixel++)
		{
			int iSampleLast = int((iPixel + 1) * nSamplesPerPixel + 0.5) + didxFirst - 1;
			if (iSampleLast > didxLast)
				iSampleLast = didxLast;

			double n = *data;
			double nMin = n;
			double nMax = n;
			while (iSample < iSampleLast)
			{
				iSample++;
				n = *++data;
				if (n < nMin)
					nMin = n;
				else if (n > nMax)
					nMax = n;
			}

			pixdata->yBot = nMin;
			pixdata->yTop = nMax;
			pixdata++;

			iSample++;
			data++;
		}
	}
	else
	{
		for (int iPixel = 0; iPixel < nPixels; iPixel++)
		{
			int iSample = int((iPixel + 1) * nSamplesPerPixel + 0.5) + didxFirst - 1;
			if (iSample > didxLast)
				iSample = didxLast;

			double n = data[iSample - didxFirst]; // Need to subtract didxFirst here because we offset data by didxFirst above
			pixdata->yBot = n;
			pixdata->yTop = n;
			pixdata++;
		}
	}
}

void RenderData::renderStd(ViewWaveInfo* vwi, int tidxStart_, int nPixels_, double nSamplesPerPixel_)
{
	CHECK_PARAM_RET(vwi != NULL);
	CHECK_PARAM_RET(vwi->wave() != NULL && vwi->isVisible());

	setup(vwi, tidxStart_, nPixels_, nSamplesPerPixel_);

	pixels.resize(nPixels);
	if (nPixels == 0)
		return;

	const WaveInfo* wave = vwi->wave();
	const double* data = wave->std.constData() + didxFirst;
	const double* avedata = wave->display.constData() + didxFirst;
	MinMax* pixdata = pixels.data();

	if (nSamplesPerPixel > 1)
	{
		int iSample = didxFirst;
		for (int iPixel = 0; iPixel < nPixels; iPixel++)
		{
			int iSampleLast = int((iPixel + 1) * nSamplesPerPixel + 0.5) + didxFirst - 1;
			if (iSampleLast > didxLast)
				iSampleLast = didxLast;

			double n = *avedata;
			double less = n - *data;
			double more = n + *data;
			double nMin = less;
			double nMax = more;
			while (iSample < iSampleLast)
			{
				iSample++;
				n = *++avedata;
				double nStd = *++data;
				less = n - nStd;
				more = n + nStd;
				if (less < nMin)
					nMin = less;
				else if (more > nMax)
					nMax = more;
			}

			pixdata->yBot = nMin;
			pixdata->yTop = nMax;
			pixdata++;

			iSample++;
			data++;
			avedata++;
		}
	}
	else
	{
		for (int iPixel = 0; iPixel < nPixels; iPixel++)
		{
			int iSample = int((iPixel + 1) * nSamplesPerPixel + 0.5) + didxFirst - 1;
			if (iSample > didxLast)
				iSample = didxLast;

			double n = avedata[iSample - didxFirst]; // Need to subtract didxFirst here because we offset data by didxFirst above
			double nStd = data[iSample - didxFirst];
			double less = n - nStd;
			pixdata->yBot = n - nStd;
			pixdata->yTop = n + nStd;
			pixdata++;
		}
	}
}
