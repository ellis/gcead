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

#ifndef __IDAC_H
#define __IDAC_H

#include <QObject>
#include <QStringList>

#include "IdacSettings.h"


/// This object interfaces between the GUI thread and the hardware
class Idac : public QObject
{
	Q_OBJECT
public:
	Idac(QObject* parent = NULL);

	IdacErrors errors() const { return m_errors; }

	const QList<int>& ranges() const { return m_anRanges; }
	const QStringList& lowpassStrings() const { return m_asLowpassStrings; }
	const QStringList& highpassStrings() const { return m_asHighpassStrings; }

public slots:
	virtual void connect() = 0;
	virtual void disconnect() = 0;
	virtual void stopReceiving() = 0;
	virtual void resendChannelSettings(int iChannel) = 0;

public:
	virtual IdacState state() const = 0;
	virtual void startReceiving() = 0;
	// REFACTOR: consider changing this to get the data for ALL channels at once. -- ellis, 2008-06-11
	virtual QList<short> takeChannelData(int iChannel) = 0;

signals:
	void stateChanged(IdacState state);

protected:
	void setErrors(IdacErrors errors);
	void setRanges(const QList<int>& ranges);
	void setLowpassStrings(const QStringList& strings);
	void setHighpassStrings(const QStringList& strings);

private:
	IdacState m_state;
	IdacErrors m_errors;
	QList<int> m_anRanges;
	QStringList m_asLowpassStrings;
	QStringList m_asHighpassStrings;
};

#endif
