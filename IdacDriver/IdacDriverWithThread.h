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

#ifndef __IDACDRIVERWITHTHREAD_H
#define __IDACDRIVERWITHTHREAD_H

#include "IdacDriver.h"

#include <QMutex>


class IdacDriverSamplingThread;


class IdacDriverWithThread : public IdacDriver
{
public:
	IdacDriverWithThread(QObject* parent = NULL);
	~IdacDriverWithThread();

// IdacDriver overrides
public:
	virtual void stopSampling();
	virtual int takeData(short* digital, short* analog1, short* analog2, int maxSize);

protected:
	friend class IdacDriverSamplingThread;
	/// WARNING: ONLY TO BE CALLED FROM IdacDriverSamplingThread
	virtual void sampleLoop() = 0;

protected:
	void startSamplingThread();
	void stopSamplingThread();
	void msleep(unsigned long msecs);
	/// @returns true if there was no overflow, false if there was overflow
	bool addSample(short digital, short analog1, short analog2);

protected:
	bool m_bSampling;

private:
	int m_iSampleRead;
	int m_iSampleWrite;
	int m_nSamplesInBuffer;
	short* m_samplesDigital;
	short* m_samplesAnalog1;
	short* m_samplesAnalog2;
	IdacDriverSamplingThread* m_recordThread;
	QMutex m_sampleMutex;
};

#endif
