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

#include "IdacSettings.h"
#include "IdacEnums.h"


class IdacDriver : public QObject
{
	Q_OBJECT
public:
	IdacDriver(QObject* parent = NULL);

	const QString hardwareName() { return m_sHardwareName; }
	const QList<int>& ranges() const { return m_anRanges; }
	const QStringList& lowpassStrings() const { return m_asLowpassStrings; }
	const QStringList& highpassStrings() const { return m_asHighpassStrings; }

	bool hasErrors();
	/// Get a list of existing error messages and then clear the internal list.
	/// Thread-safe.
	QStringList errorMessages();

	void setChannelSettings(int iChannel, const IdacChannelSettings& channel);

public:
	virtual bool checkUsbFirmwareReady() = 0;
	virtual bool checkDataFirmwareReady() = 0;

	virtual void initUsbFirmware() = 0;
	virtual void initDataFirmware() = 0;

	/// Load up default channel settings for the current driver
	virtual void loadDefaultChannelSettings(IdacChannelSettings* channels) = 0;

	virtual bool startSampling() = 0;
	virtual void stopSampling() = 0;
	virtual void configureChannel(int iChan) = 0;

	virtual int takeData(short* digital, short* analog1, short* analog2, int maxSize) = 0;

protected:
	void setHardwareName(const QString& s) { m_sHardwareName = s; }
	void setRanges(const QList<int>& ranges) { m_anRanges = ranges; }
	void setLowpassStrings(const QStringList& strings) { m_asLowpassStrings = strings; }
	void setHighpassStrings(const QStringList& strings) { m_asHighpassStrings = strings; }
	void addError(const QString& s);

	const IdacChannelSettings* desiredSettings();
	const IdacChannelSettings* desiredChannelSettings(int iChan);
	IdacChannelSettings* actualSettings() { return m_settingsActual; }
	IdacChannelSettings* actualChannelSettings(int iChan);

private:
	QString m_sHardwareName;
	QList<int> m_anRanges;
	QStringList m_asLowpassStrings;
	QStringList m_asHighpassStrings;

	QMutex m_errorMutex;
	QStringList m_errors;

	// NOTE: I don't think this mutex is necessary, because all members are independent and can be set atomically -- ellis, 2009-04-26
	QMutex m_settingsMutex;
	/// Settings requested by user
	IdacChannelSettings m_settingsDesired[3];
	/// Settings send to the hardware
	IdacChannelSettings m_settingsActual[3];
};

#endif
