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

#include "EadFile.h"

#include <Check.h>
#include <Filters/Filters.h>


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

/*
Filter1Info::Filter1Info(EadFile* file)
	: FilterInfo(file)
{
	m_nWidth = 0;
}

FilterType Filter1Info::type() const { return FilterType_1; }
QString Filter1Info::name() const { return "Smoothing"; }

void Filter1Info::filter(QVector<double>& signal)
{
	if (signal.size() == 0)
		return;

	double* x = signal.data();
	int len = signal.size();
	double *xy;

	Filters* whitef = new Filters;
	whitef->calcWhiteningFilterYW(x);

	whitef->calcWienerFilter();
	xy = new double[whitef->get_wienerFiltlen() + len - 1];
	whitef->convolve_wiener(x, len, xy);
	//  w.convolve(x, len, whitef->wienerFilt, 32768, xy);
	for(int i = 0; i < len; ++i)
	{
		x[i] = xy[i + 16384];  //FFTLEN/2 this is only temporary
	}
	delete xy;
}

void Filter1Info::setWidth(double nWidth)
{
	if (nWidth != m_nWidth) {
		m_nWidth = nWidth;
		emit widthChanged();
	}
}
*/



/*********
 * FilterTesterInfo
 **********/

FilterTesterInfo::FilterTesterInfo(EadFile* file, WaveType waveType)
	: FilterInfo(file)
	//m_waveType(waveType)
{
	setWaveType(waveType);
	m_filterId = 0;
	QVariantMap p0, p1, p2;
	p1.insert("param1", 1.0);
	p2.insert("alpha", 2.0);
	m_properties << p0 << p1 << p2;
}

FilterType FilterTesterInfo::type() const { return FilterType_1; }
QString FilterTesterInfo::name() const { return "Tester"; }

int FilterTesterInfo::filterCount() const
{
	return 2;
}

void FilterTesterInfo::setFilterId(int filterId)
{
	m_filterId = filterId;
}

void FilterTesterInfo::filter(QVector<double>& signal)
{
	if (m_filterId == 0 || signal.size() == 0)
		return;

	double* x = signal.data();
	int len = signal.size();

	Filters* whitef = new Filters;
	whitef->calcWhiteningFilterYW(x);

	if (m_filterId == 1) {
		whitef->calcNWMFFilter();
		double* xy = new double[whitef->get_NWMFlen()+len-1];
		whitef->convolve_NWMF(x,len,xy);
		// w.convolve(x, len, whitef->NWMFFilt, whitef->get_NWMFlen(), xy);
		for(int i = 0; i < len; ++i)
		{
			x[i] = xy[i+31768];  //FFTLEN - placement of peak @ 10 sec....  this is only temporary
		}
		delete[] xy;
	}
	else if (m_filterId == 2) {
		whitef->calcWienerFilter();
		double* xy = new double[whitef->get_wienerFiltlen() + len - 1];
		whitef->convolve_wiener(x, len, xy);
		//  w.convolve(x, len, whitef->wienerFilt, 32768, xy);
		for(int i = 0; i < len; ++i)
		{
			x[i] = xy[i + 16384];  //FFTLEN/2 this is only temporary
		}
		delete[] xy;
	}
}
