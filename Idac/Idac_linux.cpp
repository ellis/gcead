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

#include "Idac_linux.h"

#include <IdacControl/Idacpc.h>


extern "C" DWORD IdacType()
{
	return IDAC_TYPE_NONE;
}


Idac_linux::Idac_linux(bool bSimulate, QObject* parent)
	: Idac(parent)
{
	m_bSimulate = bSimulate;
	m_state = IdacState_NotAvailable;
}

IdacState Idac_linux::state() const
{
	return m_state;
}

void Idac_linux::setState(IdacState state)
{
	if (state != m_state)
	{
		m_state = state;
		emit stateChanged(state);
	}
}

void Idac_linux::connect()
{
	setState(IdacState_Connected);
}

void Idac_linux::disconnect()
{
	setState(IdacState_Inactive);
}

void Idac_linux::startReceiving()
{
	setState(IdacState_Receiving);
}

void Idac_linux::stopReceiving()
{
	setState(IdacState_Connected);
}

void Idac_linux::resendChannelSettings(int iChannel)
{
	Q_UNUSED(iChannel);
}

QList<short> Idac_linux::takeChannelData(int iChannel)
{
	Q_UNUSED(iChannel);
	
	if (m_bSimulate)
	{
		QList<short> list;
		for (int i = 0; i < 10; i++)
			list << 0;
		return list;
	}
	else
		return QList<short>();
}
