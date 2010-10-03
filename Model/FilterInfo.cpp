/**
 * Copyright (C) 2010  Ellis Whitehead
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

#include "FilterInfo.h"

#include <Check.h>

#include "EadFile.h"


FilterInfo::FilterInfo(EadFile* file)
	: QObject(file), m_file(file)
{
	CHECK_PARAM_NORET(file != NULL);
	m_waveType = WaveType_EAD;
}

void FilterInfo::setWaveType(WaveType waveType)
{
	if (waveType != m_waveType) {
		m_waveType = waveType;

		QList<WaveInfo*> old = m_waves;
		m_waves.clear();
		m_file->updateDisplay(old);

		emit waveTypeChanged();
		emit wavesChanged();
	}
}

void FilterInfo::addWave(WaveInfo* wave)
{
	CHECK_PARAM_RET(wave != NULL);
	CHECK_PRECOND_RET(!m_waves.contains(wave));
	m_waves << wave;
	m_file->updateDisplay(wave);
	emit wavesChanged();
}

void FilterInfo::removeWave(WaveInfo* wave)
{
	CHECK_PARAM_RET(wave != NULL);
	CHECK_PRECOND_RET(m_waves.contains(wave));
	m_waves.removeOne(wave);
	m_file->updateDisplay(wave);
	emit wavesChanged();
}



/*********
 * Filter1Info
 **********/

Filter1Info::Filter1Info(EadFile* file)
	: FilterInfo(file)
{
	m_nWidth = 0;
}

FilterType Filter1Info::type() const { return FilterType_1; }
QString Filter1Info::name() const { return "Smoothing"; }

void Filter1Info::setWidth(double nWidth)
{
	if (nWidth != m_nWidth) {
		m_nWidth = nWidth;
		emit widthChanged();
	}
}
