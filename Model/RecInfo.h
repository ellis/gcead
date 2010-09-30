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

#ifndef __RECORDINGINFO_H
#define __RECORDINGINFO_H

#include <QDateTime>
#include <QList>
#include <QObject>

#include "WaveInfo.h"


class EadFile;


class RecInfo : public QObject
{
public:
	// REFACTOR: I should probably remove the EadFile* member; I think it was a mistake to add it; it's not used anywhere right now -- ellis, 2010-09-29
	RecInfo(EadFile* file, int id);
	~RecInfo();

	/// EadFile object which this record belongs to
	EadFile* file() const { return m_file; }
	/// Unique ID for this recording
	int id() const { return m_id; }
	/// Date/time of the start of recording
	const QDateTime& timeOfRecording() const { return m_time; }
	/// Set the data/time of the start of recording
	void setTimeOfRecording(const QDateTime& time) { m_time = time; }
	/// Number of samples to shift the display of this dataset to the right.
	/// Negative values shift the dataset to the left.
	//int shift() const { return m_nShift; }
	//void setShift(int nShift);

	WaveInfo* ead() { return m_waves[WaveType_EAD]; }
	WaveInfo* fid() { return m_waves[WaveType_FID]; }
	WaveInfo* digital() { return m_waves[WaveType_Digital]; }

	WaveInfo* wave(WaveType type) { return m_waves[type]; }
	const QList<WaveInfo*>& waves() const { return m_waves; }

private:
	WaveInfo* createWave(WaveType type);

private:
	EadFile* const m_file;
	int m_id;
	QList<WaveInfo*> m_waves;
	/// Date/time of recording
	QDateTime m_time;
	int m_nShift;
};

#endif
