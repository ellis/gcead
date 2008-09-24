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

#include "RecInfo.h"


RecInfo::RecInfo(int id)
{
	m_id = id;
	m_nShift = 0;

	for (int i = 0; i < WaveTypeCount; i++)
		m_waves << createWave((WaveType) i);
}

RecInfo::~RecInfo()
{
	qDeleteAll(m_waves);
	m_waves.clear();
}

/*void RecInfo::setShift(int nShift)
{
	m_nShift = nShift;
}*/

WaveInfo* RecInfo::createWave(WaveType type)
{
	WaveInfo* wave = new WaveInfo(this);
	
	wave->type = type;
	wave->pos.bVisible = true;

	switch (type)
	{
	case WaveType_EAD:
		wave->sName = tr("EAD %0").arg(m_id);
		wave->pos.nVoltsPerDivision = 2;
		wave->pos.nDivisionOffset = 5;
		break;
	case WaveType_FID:
		wave->sName = tr("FID %0").arg(m_id);
		wave->pos.nVoltsPerDivision = 0.5;
		wave->pos.nDivisionOffset = 9;
		break;
	case WaveType_Digital:
		wave->sName = tr("DIG %0").arg(m_id);
		wave->nRawToVoltageFactorNum = 1;
		wave->nRawToVoltageFactorDen = 2;
		wave->nRawToVoltageFactor = 0.5;
		wave->pos.nVoltsPerDivision = 1;
		wave->pos.nDivisionOffset = 1.5;
		break;
	}

	return wave;
}
