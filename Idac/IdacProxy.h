/**
 * Copyright (C) 2009  Ellis Whitehead
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

#ifndef __IDACPROXY_H
#define __IDACPROXY_H

#include <QObject>
#include <QStringList>

#include <IdacDriver/IdacEnums.h>


class IdacChannelSettings;
class IdacDriver;
class IdacDriverManager;


/// This object interfaces between the GUI thread and the hardware
class IdacProxy : public QObject
{
	Q_OBJECT
public:
	IdacProxy(IdacDriverManager* manager, QObject* parent = NULL);

	IdacState state() const { return m_state; }
	/// True if the IDAC was properly initialized
	bool isAvailable() const { return m_bAvailable; }
	IdacErrors errors() const { return m_errors; }

	const QString& hardwareName() const { return m_sHardwareName; }
	const QList<int>& ranges() const { return m_anRanges; }
	const QStringList& lowpassStrings() const { return m_asLowpassStrings; }
	const QStringList& highpassStrings() const { return m_asHighpassStrings; }
	QString statusText() const;

	/// Load up default channel settings for the current driver
	void loadDefaultChannelSettings(IdacChannelSettings* channels);

	void startSampling(const IdacChannelSettings* channels);
	int takeData(short* digital, short* analog1, short* analog2, int maxSize);

public slots:
	void setup();
	void setdown();
	void stopSampling();
	void resendChannelSettings(int iChannel, const IdacChannelSettings& channel);

signals:
	/// This is only for internal use with IdacDriverManager
	void requestCommand(int _cmd);
	void stateChanged(IdacState state);
	void statusTextChanged(const QString& sStatus);
	void statusErrorChanged(const QString& sError);
	void isAvailableChanged(bool b);

private slots:
	void setState(int _state);
	void commandFinished(int _cmd);

private:
	void queueCommand(IdacCommand cmd);
	void handleQueue();
	void updateStatusError();

private:
	IdacState m_state;
	IdacErrors m_errors;
	IdacDriverManager* m_manager;

	IdacCommand m_cmdRequested;
	IdacCommand m_cmdQueued;

	bool m_bAvailable;
	QString m_sHardwareName;
	QList<int> m_anRanges;
	QStringList m_asLowpassStrings;
	QStringList m_asHighpassStrings;

	friend class IdacFactory;
};

#endif
