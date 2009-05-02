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

#ifndef __IDAC_LINUX_H
#define __IDAC_LINUX_H

#include "Idac.h"


class Idac_linux : public Idac
{
	Q_OBJECT
public:
	/// @param bSimulate set to true to simulate for testing purposes
	Idac_linux(bool bSimulate, QObject* parent = NULL);

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
	void setState(IdacState state);

private:
	bool m_bSimulate;
	IdacState m_state;
};

#endif
