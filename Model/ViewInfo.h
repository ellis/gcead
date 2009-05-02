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

#ifndef __VIEWINFO_H
#define __VIEWINFO_H

#include <QObject>
#include <QPointer>

#include "WaveInfo.h"

#include <EadEnums.h>


class EadFile;
class ViewInfo;


class ViewWaveInfo : public QObject
{
	Q_OBJECT
public:
	/// HACK: By settings this to true, ChartWidget will adjust nDivisionOffset if this wave isn't visible
	bool bAssureVisibility;

public:
	ViewWaveInfo()
		: bAssureVisibility(false), m_view(NULL), m_wave(NULL), m_pos(NULL)
	{
	}
	ViewWaveInfo(ViewInfo* view, WaveInfo* wave);
	ViewWaveInfo(ViewInfo* view, WaveInfo* wave, WavePos* pos);

	void init(WaveInfo* wave, WavePos* pos = NULL);

	const WaveInfo* wave() const { return m_wave; }
	WaveInfo* waveInfo() { return m_wave; }

	int shift() const { return m_wave->shift(); }
	void setShift(int nShift);

	bool isVisible() const { return m_pos->bVisible; }
	void setVisible(bool bVisible);

	double voltsPerDivision() const { return m_pos->nVoltsPerDivision; }
	void setVoltsPerDivision(double n);

	double divisionOffset() const { return m_pos->nDivisionOffset; }
	void setDivisionOffset(double n);

	void changeVoltsPerDivision(int nIndexInc);
	void changeShift(int nIndexInc);

	void invert();

	void setComment(const QString& sComment);

	void choosePeak(const WavePeakChosenInfo& peak);
	void choosePeakAtDidx(int didx);
	void unchoosePeakAtDidx(int didx);

private:
	void emitChanged(ViewChangeEvents e);

private slots:
	//void on_wave_destroyed(QObject* obj);

private:
	ViewInfo* m_view;
	QPointer<WaveInfo> m_wave;
	WavePos* m_pos;
};


class ViewInfo : public QObject
{
	Q_OBJECT
public:
	ViewInfo(EadFile* file);
	~ViewInfo();

	ViewWaveInfo vwiUser; // REFACTOR: rename to vwiUser
	WavePos posExtra; // REFACTOR: rename to posExtra

	/// Get the list of standard waves in this view
	const QList<ViewWaveInfo*>& vwis() { return m_vwis; }
	/// Get the list of additional user-selected waves in this view
	const QList<ViewWaveInfo*>& vwiExtras() { return m_vwiExtras; }
	/// Generate a list that combines both vwis() and vwiExtras()
	QList<ViewWaveInfo*> allVwis();

	/// Add a wave whose chart position is taken from the WaveInfo
	ViewWaveInfo* addWave(WaveInfo* wave);
	/// Add a wave whose chart position is given by this ViewInfo
	ViewWaveInfo* addExtraWave(WaveInfo* wave);
	
	/// Delete our list of vwis() objects (doesn't delete the WaveInfos)
	void clearWaves();

	void setUserWave(WaveInfo* wave);

	/// For use by ViewInfo only
	void emitChanged(ViewChangeEvents e);

signals:
	/// Emitted when wave info or position is changed.
	void changed(ViewChangeEvents e);

private:
	QPointer<EadFile> m_file;
	QList<ViewWaveInfo*> m_vwis;
	QList<ViewWaveInfo*> m_vwiExtras;
	//QList<ViewWaveInfo*> m_vwiUsers;
	/// WavePos objects for our "extra" waves
	QList<WavePos*> m_posExtras;
};

#endif
