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

#ifndef __IDAC_WINDOWS_H
#define __IDAC_WINDOWS_H

#include "Idac.h"


class QLabel;
class QStatusBar;

class IdacThread;
class IdacRecorder;


class Idac_windows : public Idac
{
	Q_OBJECT
public:
	Idac_windows(QStatusBar* statusBar, QObject* parent = NULL);
	~Idac_windows();

// Idac overrides
public:
	IdacState state() const;
	void connect();
	void disconnect();
	void startReceiving();
	void stopReceiving();
	void resendChannelSettings(int iChannel);
	QList<short> takeChannelData(int iChannel);

private:
	void queueCommand(IdacState state);
	void handleQueue();
	void setStatus(IdacState state);
	void setStatus(const QString& s);

private slots:
	void on_thread_ready();
	void on_recorder_commandFinished();

private:
	QStatusBar* m_statusBar;
	QLabel* m_lblStatus;
	IdacThread* m_thread;
	IdacRecorder* m_recorder;
	IdacState m_state;

	// Command queue
	/// The state we're currently trying to activate.
	/// This is only set *while* a command is being executed.
	IdacState m_stateRequested;
	/// The next state requested.
	/// We will try to reach this state once the current command has finished.
	IdacState m_stateQueued;
	QList<int> m_resendChannels;
};

#endif
