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

#ifndef _EADFILE_H
#define _EADFILE_H

#include <QList>
#include <QObject>
#include <QPair>

#include "EadEnums.h"
#include "RecInfo.h"
#include "WaveInfo.h"
#include "ViewInfo.h"


class QDataStream;
class QDomDocument;
class QDomElement;
class QFile;


class EadFile : public QObject
{
	Q_OBJECT
public:
	/*
	/// File format for export
	enum ExportFormat
	{
		CSV,
		TAB,
	};
	*/
public:
	EadFile();
	~EadFile();

	/// User comment for this file
	const QString& comment() const { return m_sComment; }
	void setComment(const QString& s);

	const QString& filename() const { return m_sFilename; }
	bool isDirty() const { return m_bDirty; }
	//const QList<WaveInfo*>& waves() { return m_waves; }
	const QList<RecInfo*>& recs() const { return m_recs; }
	ViewInfo* viewInfo(EadView view);
	
	void clear();
	bool saveAs(const QString& sFilename);
	LoadSaveResult load(const QString& sFilename);
	void importWaves(const EadFile* other);
	bool exportData(const QString& sFilename /*, ExportFormat format*/);
	bool exportRetentionData(const QString& sFilename /*, ExportFormat format*/);

	RecInfo* newRec() { return m_newRec; }
	/// Allocate new recording objects and add them to the recording view.
	/// viewInfo(EadView_Recording).vwis() will then have at indexes
	/// 0, 1, and 2 the ead, fid, and digital waves.
	void createNewRecording();
	void discardNewRecording();
	/// Place m_newRec into m_recs and set m_newRec = NULL
	void saveNewRecording();

	/// Force a recalculation of the averaged waves
	void updateAveWaves();

	void updateDisplay();
	void updateDisplay(RecInfo* rec);
	void updateDisplay(WaveInfo* wave);

	void createFakeData();

	/// Called by ViewInfo when the user changes stuff
	void setDirty();

signals:
	/// Emitted when the file has been changed (used by MainWindow to know when the user should save)
	void dirtyChanged();
	/// Emitted when a wave is added or removed from the file
	void waveListChanged();

private:
	//void addRec(RecInfo* rec);

	void createRecNode(QDomDocument& doc, QDomElement& parent, RecInfo* rec);
	void createWaveNode(QDomDocument& doc, QDomElement& parent, WaveInfo* wave);
	void createPeakNode(QDomDocument& doc, QDomElement& parent, const WavePeakChosenInfo* peak);
	void createViewNode(QDomDocument& doc, QDomElement& parent, ViewInfo* view);
	void createViewWaveNode(QDomDocument& doc, QDomElement& parent, ViewWaveInfo *vwi);
	void saveData(QDataStream& str);

	LoadSaveResult loadOld(QDataStream& str);
	LoadSaveResult loadCurrent(QDataStream& str);
	void loadRecNode(QDomElement& elem);
	void loadWaveNode(QDomElement& elem, WaveInfo* wave);
	void loadPeakNode(QDomElement& elem, WaveInfo* wave);
	void loadViewNode(QDomElement& elem, ViewInfo* view);
	/// @param bExtra true if this is an "extra" wave, false if it's the "user" wave
	void loadViewWaveNode(QDomElement& elem, ViewInfo* view, bool bExtra);
	//WaveInfo* createWave(WaveType type);

	void createAveWaves();
	/// Create m_views
	void createViewInfo();
	/// Update the ViewInfo list
	void updateViewInfo();

	void updateAveWave(WaveType type);

	/// @param nSize number of doubles in 'data'
	void createFakeData2(WaveInfo* wave, short* data, int nSize, short yOffset);
	void createFakeData3(int nShift, const QList<float>& factors);
	QVector<short> createFakeData4(const QVector<short>& data, int nShift, const QList<float>& factors);

private:
	QList<RecInfo*> m_recs;
	QList<ViewInfo*> m_views;
	QString m_sFilename;
	QString m_sComment;
	/// True when the file has changed since being saved
	bool m_bDirty;
	RecInfo* m_newRec;
};

#endif
