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

#include "Idac_windows.h"

#include <QtDebug>
#include <QLabel>
#include <QStatusBar>

#include <Check.h>
#include <IdacControl/Idacpc.h>

#include "IdacThread.h"
#include "IdacRecorder.h"


Idac_windows::Idac_windows(QStatusBar* statusBar, QObject* parent)
	: Idac(parent)
{
	m_recorder = NULL;
	m_state = IdacState_Inactive;
	
	m_stateRequested = IdacState_NotAvailable;
	m_stateQueued = IdacState_NotAvailable;

	// Start the IDAC thread
	m_thread = new IdacThread();
	QObject::connect(m_thread, SIGNAL(ready()), this, SLOT(on_thread_ready()));
	m_thread->start();

	m_statusBar = statusBar;
	m_lblStatus = new QLabel();
	m_statusBar->addPermanentWidget(m_lblStatus);
}

Idac_windows::~Idac_windows()
{
	m_thread->exit(0);
}

IdacState Idac_windows::state() const
{
	return m_state;
}

void Idac_windows::connect()
{
	queueCommand(IdacState_Connected);
}

void Idac_windows::disconnect()
{
	queueCommand(IdacState_Inactive);
}

void Idac_windows::startReceiving()
{
	queueCommand(IdacState_Receiving);
}

void Idac_windows::stopReceiving()
{
	queueCommand(IdacState_Connected);
}

void Idac_windows::resendChannelSettings(int iChannel)
{
	// TODO: queue this command too
	QMetaObject::invokeMethod(m_recorder, "sendChannelSettings", Q_ARG(int, iChannel));
}

QList<short> Idac_windows::takeChannelData(int iChannel)
{
	return m_recorder->takeChannelData(iChannel);
}

void Idac_windows::queueCommand(IdacState state)
{
	if (m_recorder == NULL)
		return;

	m_stateQueued = state;
	// If we're not currently waiting for a command to finish, then send this command now.
	if (m_stateRequested == IdacState_NotAvailable)
		handleQueue();
}

void Idac_windows::handleQueue()
{
	CHECK_PRECOND_RET(m_stateRequested == IdacState_NotAvailable);
	CHECK_PRECOND_RET(m_stateQueued != IdacState_NotAvailable);

	QString sInvoke;

	switch (m_state)
	{
	case IdacState_NotAvailable:
		m_stateQueued = IdacState_NotAvailable;
		break;

	case IdacState_Inactive:
		if (m_stateQueued == IdacState_Connected || m_stateQueued == IdacState_Receiving)
		{
			m_stateRequested = IdacState_Connected;
			setStatus(tr("Connecting"));
		}
		break;

	case IdacState_Connected:
		if (m_stateQueued == IdacState_Inactive || m_stateQueued == IdacState_Receiving)
			m_stateRequested = m_stateQueued;
		break;

	case IdacState_Receiving:
		if (m_stateQueued == IdacState_Inactive || m_stateQueued == IdacState_Connected)
			m_stateRequested = m_stateQueued;
		break;
	}

	if (m_stateRequested == IdacState_NotAvailable)
	{
		m_stateQueued = IdacState_NotAvailable;
	}
	else
	{
		QMetaObject::invokeMethod(m_recorder, "requestState", Q_ARG(int, (int) m_stateRequested));
		// Clear the queued state if we have now requested it.
		if (m_stateRequested == m_stateQueued)
			m_stateQueued = IdacState_NotAvailable;
	}
}

void Idac_windows::setStatus(IdacState state)
{
	QString s;
	switch (state)
	{
	case IdacState_NotAvailable:
		s = tr("Not Available");
		break;
	case IdacState_Inactive:
		s = tr("Inactive");
		break;
	case IdacState_Connected:
		s = tr("Connected");
		break;
	case IdacState_Receiving:
		s = tr("Receiving");
		break;
	}

	setStatus(s);
}

void Idac_windows::setStatus(const QString& s)
{
	m_lblStatus->setText(tr("IDAC: %0").arg(s));
}

void Idac_windows::on_thread_ready()
{
	CHECK_PRECOND_RET(m_recorder == NULL);

	m_recorder = m_thread->recorder();
	QObject::connect(m_recorder, SIGNAL(commandFinished()), this, SLOT(on_recorder_commandFinished()));

	// Get range information from the hardware
	LPDWORD pRanges = IdacGetRanges();
	QList<int> ranges;
	for (LPDWORD pnRange = pRanges; *pnRange != -1; pnRange++)
	{
		ranges << *pnRange;
	}
	setRanges(ranges);
	qDebug() << ranges;

	// Get low-pass filter information from the hardware
	int nStrings;
	const char** asStrings;
	IdacLowPassStrings(&nStrings, &asStrings);
	QStringList list;
	for (int i = 0; i < nStrings; i++)
		list << asStrings[i];
	setLowpassStrings(list);

	// Get high-pass filter information from the hardware
	IdacHighPassStrings(&nStrings, &asStrings);
	list.clear();
	for (int i = 0; i < nStrings; i++)
		list << asStrings[i];
	setHighpassStrings(list);

	// Go ahead and try to connect
	connect();
}

void Idac_windows::on_recorder_commandFinished()
{
	IdacState state = m_recorder->state();

	bool bStateChanged = (state != m_state);
	
	m_stateRequested = IdacState_NotAvailable;
	m_state = state;
	setStatus(m_state);

	if (m_stateQueued != IdacState_NotAvailable)
		handleQueue();

	if (bStateChanged)
		emit stateChanged(m_state);
}
