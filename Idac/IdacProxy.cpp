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

#include "IdacProxy.h"

#include <QtDebug>

#include <Check.h>

#include <IdacDriver/IdacChannelSettings.h>

#include "IdacDriverManager.h"


IdacProxy::IdacProxy(IdacDriverManager* manager, QObject* parent)
		: QObject(parent)
{
	m_manager = manager;

	m_state = IdacState_None;
	m_errors = 0;

	m_cmdRequested = IdacCommand_None;
	m_cmdQueued = IdacCommand_None;

	QObject::connect(m_manager, SIGNAL(stateChanged(int)), this, SLOT(setState(int)));
	QObject::connect(m_manager, SIGNAL(commandFinished(int)), this, SLOT(commandFinished(int)));
	QObject::connect(this, SIGNAL(requestCommand(int)), m_manager, SLOT(command(int)));
}

void IdacProxy::setState(int _state)
{
	IdacState state = (IdacState) _state;
	qDebug() << "IdacProxy::setState:" << state;

	if (state != m_state)
	{
		m_sHardwareName = m_manager->hardwareName();

		// REFACTOR: consider changing this so that the assignment only occurs once. -- ellis, 2009-04-20
		bool bAvailable = false;
		if (state == IdacState_Ready && !m_bAvailable)
		{
			bAvailable = true;
			m_anRanges = m_manager->ranges();
			m_asLowpassStrings = m_manager->lowpassStrings();
			m_asHighpassStrings = m_manager->highpassStrings();
		}

		m_state = state;
		emit stateChanged(m_state);

		QString s = statusText();
		emit statusTextChanged(s);

		if (bAvailable != m_bAvailable)
		{
			m_bAvailable = bAvailable;
			emit isAvailableChanged(m_bAvailable);
		}
	}
}

void IdacProxy::commandFinished(int _cmd)
{
	CHECK_ASSERT_NORET(m_cmdRequested == _cmd);

	m_cmdRequested = IdacCommand_None;
	if (m_cmdQueued != IdacCommand_None)
		handleQueue();
}

QString IdacProxy::statusText() const
{
	QString s;
	switch (m_state)
	{
	case IdacState_None:
		s = tr("No Hardware");
		break;
	case IdacState_Searching:
		s = tr("IDAC: Searching");
		break;
	case IdacState_NotPresent:
		s = tr("IDAC: Not Present");
		break;
	case IdacState_Present:
		s = tr("%0: Present").arg(m_sHardwareName);
		break;
	case IdacState_Initializing:
		s = tr("%0: Initializing").arg(m_sHardwareName);
		break;
	case IdacState_InitError:
		s = tr("%0: Error").arg(m_sHardwareName);
		break;
	case IdacState_Ready:
		s = tr("%0: Ready").arg(m_sHardwareName);
		break;
	case IdacState_Sampling:
		s = tr("%0: Receiving").arg(m_sHardwareName);
		break;
	}

	return s;
}

void IdacProxy::setup()
{
	queueCommand(IdacCommand_Connect);
}

void IdacProxy::setdown()
{
	queueCommand(IdacCommand_Disconnect);
}

void IdacProxy::loadDefaultChannelSettings(IdacChannelSettings* channels)
{
	m_manager->loadDefaultChannelSettings(channels);
}

void IdacProxy::startSampling(const IdacChannelSettings* channels)
{
	for (int iChannel = 0; iChannel < 3; iChannel++)
		m_manager->setChannelSettings(iChannel, channels[iChannel]);
	queueCommand(IdacCommand_SamplingOn);
}

void IdacProxy::stopSampling()
{
	queueCommand(IdacCommand_SamplingOff);
}

void IdacProxy::resendChannelSettings(int iChannel, const IdacChannelSettings& channel)
{
	CHECK_PARAM_RET(iChannel >= 0 && iChannel <= 2);

	m_manager->setChannelSettings(iChannel, channel);
	queueCommand((IdacCommand) (IdacCommand_ConfigCh0 + iChannel));
}

int IdacProxy::takeData(short* digital, short* analog1, short* analog2, int maxSize)
{
	return m_manager->takeData(digital, analog1, analog2, maxSize);
}

void IdacProxy::queueCommand(IdacCommand cmd)
{
	m_cmdQueued = cmd;
	// If we're not currently waiting for a command to finish, then send this command now.
	if (m_cmdRequested == IdacCommand_None)
		handleQueue();
}

void IdacProxy::handleQueue()
{
	CHECK_PRECOND_RET(m_cmdRequested == IdacCommand_None);
	CHECK_PRECOND_RET(m_cmdQueued != IdacCommand_None);

	bool bValid = true;

	switch (m_state)
	{
	// These are states before we have a driver object
	case IdacState_None:
	case IdacState_Searching:
	case IdacState_NotPresent:
	case IdacState_Present:
	case IdacState_Initializing:
	case IdacState_InitError:
		switch (m_cmdQueued)
		{
		case IdacCommand_Connect:
		case IdacCommand_SamplingOn:
		case IdacCommand_ConfigCh0:
		case IdacCommand_ConfigCh1:
		case IdacCommand_ConfigCh2:
			m_cmdRequested = IdacCommand_Connect;
			break;
		case IdacCommand_Disconnect:
			m_cmdRequested = m_cmdQueued;
			break;
		default:
			bValid = false;
			break;
		}
		break;

	case IdacState_Ready:
		switch (m_cmdQueued)
		{
		case IdacCommand_Disconnect:
		case IdacCommand_SamplingOn:
		case IdacCommand_ConfigCh0:
		case IdacCommand_ConfigCh1:
		case IdacCommand_ConfigCh2:
			m_cmdRequested = m_cmdQueued;
			break;
		default:
			bValid = false;
			break;
		}
		break;

	case IdacState_Sampling:
		switch (m_cmdQueued)
		{
		case IdacCommand_Disconnect:
		case IdacCommand_SamplingOff:
		case IdacCommand_ConfigCh0:
		case IdacCommand_ConfigCh1:
		case IdacCommand_ConfigCh2:
			m_cmdRequested = m_cmdQueued;
			break;
		default:
			bValid = false;
			break;
		}
		break;
	}

	if (bValid)
	{
		if (m_cmdRequested == IdacCommand_None)
		{
			m_cmdQueued = IdacCommand_None;
		}
		else
		{
			emit requestCommand(m_cmdRequested);
			//QMetaObject::invokeMethod(m_manager, "command", Q_ARG(int, (int) m_cmdRequested));
			// Clear the queued state if we have now requested it.
			if (m_cmdRequested == m_cmdQueued)
				m_cmdQueued = IdacCommand_None;
		}
	}
	else
	{
		qDebug() << "IdacProxy::handleQueue: state:" << m_state << "queued:" << m_cmdQueued;
		m_cmdQueued = IdacCommand_None;
		CHECK_FAILURE_RET();
	}
}
