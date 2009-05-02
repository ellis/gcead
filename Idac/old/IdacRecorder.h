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

#ifndef __IDACRECORDER_H
#define __IDACRECORDER_H

#include <QMutex>
#include <QObject>
#include <QStringList>

#include "IdacSettings.h"


class IdacRecorder : public QObject
{
	Q_OBJECT
public:
	IdacRecorder(QObject* parent = NULL);
	~IdacRecorder();

	IdacState state() const { return m_state; }
	long address() const { return m_nAddress; }
	const QStringList& errors() const  { return m_asErrors; }
	int errorFlags() const { return m_errors; }

	void clear();

	/// Called by the IdacThread
	bool grabDataFromHardware();
	/// Called by the UI thread
	QList<short> takeChannelData(int iChannel);

// Control Commands
public slots:
	void requestState(int state);
	/// Use this to send filter settings if they've changed while receiving.
	void sendChannelSettings(int iChannel);

signals:
	/// Emitted whenever a control command has finished
	void commandFinished();

private:
	void init();
	/// This should only be called at the end of a command, because m_bReady will now automatically be set to true.
	void commandFinished(IdacState state);

	void connect();
	bool disconnect();
	void record();
	void stop();

	void transferSettings();

private:
	QStringList m_asErrors;

	// State variables
	IdacState m_state;
	bool m_bConnected;
	bool m_bActive;
	bool m_bReportingError;
	long m_nAddress;

	// Recording variables
	int m_errors;
	QList<short> m_channelData[3];
	QMutex m_dataMutex;
};


#endif
