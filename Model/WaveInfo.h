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

#ifndef __WAVEINFO_H
#define __WAVEINFO_H

#include <QObject>
#include <QRect>
#include <QString>
#include <QVector>

#include "EadEnums.h"


class FilterInfo;
class RecInfo;


const int WaveTypeCount = WaveType_Digital + 1;


class WavePoint
{
public:
	int i;
	double n;

	WavePoint()
		: i(-1)
	{
	}

	WavePoint(int i, double n)
		: i(i), n(n)
	{
	}
};

class WavePeakInfo
{
public:
	bool bEnabled;
	WavePoint left;
	WavePoint middle;
	WavePoint right;
};

class WavePeakChosenInfo
{
public:
	int didxLeft;
	int didxMiddle;
	int didxRight;
	double nArea;
	double nPercent;
	double nAmplitude;

	WavePeakChosenInfo()
	:	didxLeft(0),
		didxMiddle(0),
		didxRight(0),
		nArea(0),
		nPercent(0),
		nAmplitude(0)
	{
	}
};


class WavePos
{
public:
	/// Is this waveform visible?
	bool bVisible;
	/// Millivolts per division
	double nVoltsPerDivision;
	/// Standard offset in divisions from top (value is generally between 0 and 10)
	double nDivisionOffset;

	WavePos()
		: bVisible(true), nVoltsPerDivision(1), nDivisionOffset(5)
	{
	}
};


/// Graph data that's independent of the view mode.
class WaveInfo : public QObject
{
public:
	/// Raw data
	QVector<short> raw;
	/// Numerator
	int nRawToVoltageFactorNum;
	/// Denominator
	int nRawToVoltageFactorDen;
	/// Factor to multiply raw data by to get volts
	/// This value needs to be explicitly set as:
	/// nRawToVoltageFactor = double(nRawToVoltageFactorNum) / nRawToVoltageFactorDen;
	double nRawToVoltageFactor;
	/// Display data
	QVector<double> display;
	/// Standard deviation for averaged waves
	QVector<double> std;
	/// List of possible peaks
	QList<WavePeakInfo> peaks0;
	/// List of chosen peaks
	QList<WavePeakChosenInfo> peaksChosen;
	WaveType type;
	QString sName;
	QString sComment;
	/// Offset and sensitivity
	WavePos pos;

public:
	WaveInfo(RecInfo* rec);

	void copyFrom(const WaveInfo* other);

	RecInfo* rec() { return m_rec; }
	const RecInfo* rec() const { return m_rec; }
	
	int recId() const;
	/// Number of samples to shift the display this wave to the right.
	/// Negative values shift the dataset to the left.
	int shift() const;
	void setShift(int nShift);

	/// Convert the raw data to display data
	void calcDisplayData(const QList<FilterInfo*> filters);

	void findFidPeaks();
	bool findFidPeak(int didxLeft, int didxRight, WavePeakInfo* peak) const;

	//int indexOfMax(int didxLeft, int didxRight) const;
	int findNextMin(int didxLeft, int didxRight) const;

	int indexOfChosenPeakAtDidx(int didx) const;
	void choosePeakAtDidx(int didx);
	void unchoosePeakAtIndex(int i);
	//void unchoosePeakAtDidx(int didx);

	/// Calculate the peak amplitude
	void calcPeakAmplitude(int iPeak);
	/// Call this if you've changed the indexes of a peak in order to recalculate the area under the curve.
	void calcPeakArea(int iPeak);
	/// This calculates the amplitudes of all peaks and implicitly calls calcPeakAmplitude().
	/// This method should be called when a WaveInfo object is loaded from storage.
	void calcPeakAmplitudes();
	/// This calculates the areas of all peaks and implicitly calls calcAreaPercents().
	/// This method should be called when a WaveInfo object is loaded from storage.
	void calcPeakAreas();
	/// Calculate the area percents
	void calcAreaPercents();

private:
	void findFidPeaks(const QList<WavePoint>& peaksAll, QList<WavePeakInfo>& peaks) const;

private:
	RecInfo* m_rec;
	int m_nShift;
};

#endif
