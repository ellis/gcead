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

#ifndef __IDACDRIVER_H
#define __IDACDRIVER_H

#include <QObject>
#include <QMutex>
#include <QStringList>
#include <QVector>

#include "IdacCaps.h"
#include "IdacChannelSettings.h"
#include "IdacEnums.h"


class IdacDriver : public QObject
{
	Q_OBJECT
public:
	IdacDriver(QObject* parent = NULL);

	/// Should be called by IdacDriverManager after instantiating a derived IdacDriver class
	/// This will ask the derived class to tell about its capabilities and defaults.
	void init();

	const IdacCaps* caps() const { return &m_caps; }
	const QString hardwareName() { return m_sHardwareName; }
	const QList<int>& ranges() const { return m_anRanges; }
	const QStringList& highcutStrings() const { return m_asHighcutStrings; }
	const QStringList& lowcutStrings() const { return m_asLowcutStrings; }
	int channelCount() const { return m_settingsActual.size(); }

	bool hasErrors();
	/// Get a list of existing error messages and then clear the internal list.
	/// Thread-safe.
	QStringList errorMessages();

	const QVector<IdacChannelSettings>& desiredSettings();
	const IdacChannelSettings* desiredChannelSettings(int iChan);
	void setChannelSettings(int iChannel, const IdacChannelSettings& channel);

public:
	/// Load up the capabilities of the current driver
	virtual void loadCaps(IdacCaps* caps) = 0;
	/// Load up default channel settings for the current driver
	virtual const QVector<IdacChannelSettings>& defaultChannelSettings() const = 0;

	virtual bool checkUsbFirmwareReady() = 0;
	virtual bool checkDataFirmwareReady() = 0;

	virtual void initUsbFirmware() = 0;
	virtual void initDataFirmware() = 0;

	virtual bool startSampling() = 0;
	virtual void stopSampling() = 0;
	virtual void configureChannel(int iChan) = 0;

	virtual int takeData(short* digital, short* analog1, short* analog2, int maxSize) = 0;

protected:
	void setHardwareName(const QString& s) { m_sHardwareName = s; }
	void setRanges(const QList<int>& ranges) { m_anRanges = ranges; }
	void setHighcutStrings(const QStringList& strings) { m_asHighcutStrings = strings; }
	void setLowcutStrings(const QStringList& strings) { m_asLowcutStrings = strings; }
	void addError(const QString& s);

	const QVector<IdacChannelSettings>& actualSettings() { return m_settingsActual; }
	IdacChannelSettings* actualChannelSettings(int iChan);

private:
	QString m_sHardwareName;
	QList<int> m_anRanges;
	QStringList m_asHighcutStrings;
	QStringList m_asLowcutStrings;

	QMutex m_errorMutex;
	QStringList m_errors;

	IdacCaps m_caps;
	// NOTE: I don't think this mutex is necessary, because all members are independent and can be set atomically -- ellis, 2009-04-26
	QMutex m_settingsMutex;
	/// Settings requested by user
	QVector<IdacChannelSettings> m_settingsDesired;
	/// Settings send to the hardware
	QVector<IdacChannelSettings> m_settingsActual;
};

#endif
