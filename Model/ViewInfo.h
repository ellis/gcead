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

#include "EadEnums.h"
#include "WaveInfo.h"


class EadFile;
class ViewInfo;


/// Encapsulates extra view info about a wave on a given view.
/// The extra information includes a pointer to the view and
/// to the wave's position on the view.
class ViewWaveInfo : public QObject
{
	Q_OBJECT
public:
	WaveEditorFlags editorFlags;
	/// HACK: By settings this to true, ChartWidget will adjust nDivisionOffset if this wave is shifted completely off the graph
	bool bAssureVisibility;

public:
	ViewWaveInfo()
		: editorFlags(0), bAssureVisibility(false), m_view(NULL), m_wave(NULL), m_pos(NULL)
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

    void setName(const QString& sName);
    void setComment(const QString& sComment);

	void choosePeak(const WavePeakChosenInfo& peak);
	void choosePeakAtDidx(int didx);
	void unchoosePeakAtIndex(int i);

	void setMarkerType(int iMarker, MarkerType markerType);
	void setMarkerPoint(int iMarker, int iDidx, int didx);

private:
	void boundDidx(int& didx);
	void emitChanged(ViewChangeEvents e);

private:
	ViewInfo* m_view;
	QPointer<WaveInfo> m_wave;
	WavePos* m_pos;
};


/// Data about a particular view (Averages, EADs, FIDs, All, Recording)
class ViewInfo : public QObject
{
	Q_OBJECT
public:
	ViewWaveInfo vwiUser;
	WavePos posExtra;

public:
	ViewInfo(EadView viewType, EadFile* file);
	~ViewInfo();

	EadView viewType() const { return m_viewType; }

	EadFile* file() const;

	/// Get the list of standard waves in this view
	const QList<ViewWaveInfo*>& vwis() { return m_vwis; }
	/// Get the list of additional user-selected waves in this view
	const QList<ViewWaveInfo*>& vwiExtras() { return m_vwiExtras; }
	/// Generate a list that combines both vwis() and vwiExtras()
	QList<ViewWaveInfo*> allVwis();

	/// Get pointer to ViewWaveInfo for a specific wave in this ViewInfo
	ViewWaveInfo* getExtraWave(WaveInfo* wave);

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
	void setEditorFlags(ViewWaveInfo* vwi);

private:
	EadView m_viewType;
	QPointer<EadFile> m_file;
	QList<ViewWaveInfo*> m_vwis;
	QList<ViewWaveInfo*> m_vwiExtras;
	/// WavePos objects for our "extra" waves
	QList<WavePos*> m_posExtras;
};

#endif
