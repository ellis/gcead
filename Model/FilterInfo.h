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

#ifndef FILTERINFO_H
#define FILTERINFO_H

#include <QList>
#include <QVariantMap>
#include <QObject>
#include <QVector>

#include "EadEnums.h"


class EadFile;
class WaveInfo;


class FilterInfo : public QObject
{
    Q_OBJECT

	Q_PROPERTY(FilterType type READ type CONSTANT)
	Q_PROPERTY(QString name READ name CONSTANT)
	Q_PROPERTY(WaveType waveType READ waveType WRITE setWaveType NOTIFY waveTypeChanged)

public:
	FilterInfo(EadFile *file);

	EadFile* file() const { return m_file; }

	virtual FilterType type() const = 0;
	virtual QString name() const = 0;
	virtual void filter(QVector<double>& data) = 0;

	WaveType waveType() const { return m_waveType; }
	const QList<WaveInfo*>& waves() const { return m_waves; }

signals:
	void waveTypeChanged();
	void wavesChanged();

public slots:
	void setWaveType(WaveType waveType);
	void addWave(WaveInfo* wave);
	void removeWave(WaveInfo* wave);

private:
	EadFile* const m_file;
	WaveType m_waveType;
	QList<WaveInfo*> m_waves;
};


/*
class Filter1Info : public FilterInfo
{
	Q_OBJECT

	Q_PROPERTY(double width READ width WRITE setWidth NOTIFY widthChanged)

public:
	Filter1Info(EadFile* file);

// FilterInfo overrides
public:
	FilterType type() const;
	QString name() const;
	void filter(QVector<double>& data);

public:
	double width() const { return m_nWidth; }
	void setWidth(double nWidth);

signals:
	void widthChanged();

private:
	double m_nWidth;
};
*/


class FilterTesterInfo : public FilterInfo
{
	Q_OBJECT

public:
	FilterTesterInfo(EadFile* file, WaveType waveType);

// FilterInfo overrides
public:
	FilterType type() const;
	QString name() const;
	void filter(QVector<double>& data);

public:
	//WaveType waveType() const { return m_waveType; }
	int filterCount() const;

	int filterId() const { return m_filterId; }
	/// Set which filter to use.
	/// @param filterId 0 means no filter.  filterId must be <= filterCount()
	void setFilterId(int filterId);
	QVariantMap& properties(int filterId) { return m_properties[filterId]; }

private:
	//const WaveType m_waveType;
	int m_filterId;
	QList<QVariantMap> m_properties;
};

#endif // FILTERINFO_H
