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

#include "ViewInfo.h"

#include <QtDebug>

#include <Check.h>
#include <Globals.h>
#include <Utils.h>

#include "EadFile.h"
#include "RecInfo.h"


ViewWaveInfo::ViewWaveInfo(ViewInfo* view, WaveInfo* wave)
	: m_view(view)
{
	init(wave);
}

ViewWaveInfo::ViewWaveInfo(ViewInfo* view, WaveInfo* wave, WavePos* pos)
	: m_view(view)
{
	init(wave, pos);
}

void ViewWaveInfo::init(WaveInfo* wave, WavePos* pos)
{
	bAssureVisibility = false;

	if (wave != NULL)
	{
		m_wave = wave;
		if (pos == NULL)
			pos = &m_wave->pos;
		m_pos = pos;
	}
	else
	{
		if (m_wave != NULL)
			m_wave->disconnect(this);
		m_wave = NULL;
		m_pos = NULL;
	}
}

void ViewWaveInfo::setDivisionOffset(double n)
{
	CHECK_PRECOND_RET(m_wave != NULL);
	CHECK_PRECOND_RET(m_pos != NULL);
	
	m_pos->nDivisionOffset = n;

	emitChanged(ViewChangeEvent_Paint);
}

void ViewWaveInfo::setShift(int nShift)
{
	CHECK_PRECOND_RET(m_wave != NULL);
	// Can only shift recorded waves
	CHECK_PRECOND_RET(m_wave->recId() > 0);

	//m_wave->rec()->setShift(nShift);
	m_wave->setShift(nShift);

	emitChanged(ViewChangeEvent_CalcAve);
}

void ViewWaveInfo::setVisible(bool bVisible)
{
	CHECK_PRECOND_RET(m_wave != NULL);
	CHECK_PRECOND_RET(m_pos != NULL);

	m_pos->bVisible = bVisible;

	// This operation requires a recalculation of the averaged waves
	if (m_wave->recId() > 0)
		emitChanged(ViewChangeEvent_CalcAve);
	else
		emitChanged(ViewChangeEvent_Paint);
}

void ViewWaveInfo::setVoltsPerDivision(double n)
{
	CHECK_PRECOND_RET(m_wave != NULL);
	CHECK_PRECOND_RET(m_pos != NULL);
	
	m_pos->nVoltsPerDivision = n;

	emitChanged(ViewChangeEvent_Paint);
}

void ViewWaveInfo::changeVoltsPerDivision(int nIndexInc)
{
	double nVoltsPerDivision = ::changeVoltsPerDivision(voltsPerDivision(), nIndexInc);
	setVoltsPerDivision(nVoltsPerDivision);
}

void ViewWaveInfo::changeShift(int nIndexInc)
{
	int nShift = shift() + nIndexInc * EAD_SAMPLES_PER_SECOND;
	setShift(nShift);
}

void ViewWaveInfo::invert()
{
	CHECK_PRECOND_RET(m_wave != NULL);
	// Can only invert recorded waves
	CHECK_PRECOND_RET(m_wave->recId() > 0);

	// Invert the raw data
	// NOTE: this would invert the actual raw data
	//for (int i = 0; i < m_wave->raw.size(); i++)
	//	m_wave->raw[i] *= -1;

	// Invert the display data
	m_wave->nRawToVoltageFactorNum *= -1;
	m_wave->nRawToVoltageFactor *= -1;
	m_wave->calcDisplayData();

	// If this is an FID, we'll have to recalculate the peaks
	if (m_wave->type == WaveType_FID)
		m_wave->findFidPeaks();
	
	emitChanged(ViewChangeEvent_CalcAve);
}

void ViewWaveInfo::setComment(const QString& sComment)
{
	m_wave->sComment = sComment;
	emitChanged(ViewChangeEvent_Paint);
}

void ViewWaveInfo::choosePeak(const WavePeakChosenInfo& peak)
{
	CHECK_PRECOND_RET(m_wave != NULL);

	m_wave->peaksChosen << peak;
	m_wave->calcPeakArea(m_wave->peaksChosen.size() - 1);
	m_wave->calcAreaPercents();

	emitChanged(ViewChangeEvent_Paint);
}

void ViewWaveInfo::choosePeakAtDidx(int didx)
{
	CHECK_PRECOND_RET(m_wave != NULL);
	m_wave->choosePeakAtDidx(didx);
	emitChanged(ViewChangeEvent_Paint);
}

void ViewWaveInfo::unchoosePeakAtIndex(int i)
{
	CHECK_PRECOND_RET(m_wave != NULL);
	m_wave->unchoosePeakAtIndex(i);
	emitChanged(ViewChangeEvent_Paint);
}

void ViewWaveInfo::emitChanged(ViewChangeEvents e)
{
	CHECK_PRECOND_RET(m_view != NULL);
	m_view->emitChanged(e);
}

/*void ViewWaveInfo::on_wave_destroyed(QObject* obj)
{
	if (obj == m_wave)
		m_wave = NULL;
}*/




ViewInfo::ViewInfo(EadView viewType, EadFile* file)
: vwiUser(this, NULL)
{
	m_viewType = viewType;
	m_file = file;
}

ViewInfo::~ViewInfo()
{
	clearWaves();

	// Delete objects that were allocated in addExtraWave()
	qDeleteAll(m_posExtras);
	qDeleteAll(m_vwiExtras);
}

QList<ViewWaveInfo*> ViewInfo::allVwis()
{
	QList<ViewWaveInfo*> all;
	all << m_vwis << m_vwiExtras;
	if (vwiUser.wave() != NULL)
		all << &vwiUser;
	return all;
}

ViewWaveInfo* ViewInfo::addWave(WaveInfo* wave)
{
	//CHECK_PRECOND_RET(!m_vwis.contains(wave));

	ViewWaveInfo* vwi = new ViewWaveInfo(this, wave);
	setEditorFlags(vwi);
	m_vwis << vwi;
	emitChanged(ViewChangeEvent_Paint);
	return vwi;
}

ViewWaveInfo* ViewInfo::addExtraWave(WaveInfo* wave)
{
	// Only insert this if it's not already here
	foreach (ViewWaveInfo* vwi, m_vwiExtras)
	{
		if (vwi->wave() == wave)
			return NULL;
	}

	// Create a new WavePos object
	WavePos* pos = new WavePos;
	*pos = wave->pos;
	m_posExtras << pos;

	ViewWaveInfo* vwi = new ViewWaveInfo(this, wave, pos);
	setEditorFlags(vwi);
	m_vwiExtras << vwi;
	emitChanged(ViewChangeEvent_Paint);

	return vwi;
}

void ViewInfo::clearWaves()
{
	//qDebug() << "ViewInfo::clearWaves()";
	qDeleteAll(m_vwis);
	m_vwis.clear();
}

void ViewInfo::setUserWave(WaveInfo* wave)
{
	vwiUser.init(wave, &posExtra);
	setEditorFlags(&vwiUser);
	// Can't change visibility of the user-selected wave
	vwiUser.editorFlags &= ~WaveEditorFlag_Visible;
	emitChanged(ViewChangeEvent_Paint);
}

void ViewInfo::setEditorFlags(ViewWaveInfo* vwi)
{
	// If this is an averaged waveform
	if (vwi->wave()->recId() == 0)
	{
		// If this is the "Averages" view:
		if (m_viewType == EadView_Averages)
			vwi->editorFlags = WaveEditorFlag_Sensitivity;
		else
			vwi->editorFlags = WaveEditorFlag_Sensitivity | WaveEditorFlag_Visible;
	}
	else
		vwi->editorFlags = WaveEditorFlag_Comment | WaveEditorFlag_Invert | WaveEditorFlag_Sensitivity | WaveEditorFlag_Timeshift | WaveEditorFlag_Visible;

	//qDebug() << "setEditorFlags:" << vwi->wave()->sName << vwi->editorFlags;
}

void ViewInfo::emitChanged(ViewChangeEvents e)
{
	if (e == ViewChangeEvent_CalcAve)
	{
		if (m_file != NULL)
			m_file->updateAveWaves();
		e = ViewChangeEvent_Render;
	}
	if (m_file != NULL)
		m_file->setDirty();
	emit changed(e);
}
