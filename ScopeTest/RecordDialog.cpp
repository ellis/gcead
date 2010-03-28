/**
 * Copyright (C) 2010  Ellis Whitehead
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

#include "RecordDialog.h"

#include <QtDebug>
#include <QEventLoop>
#include <QPointer>
#include <QTimer>

#include <AppDefines.h>
#include <Check.h>
#include <Globals.h>
#include <Utils.h>

#include <Idac/IdacProxy.h>
#include <IdacDriver/IdacSettings.h>
#include <RecordHandler.h>


RecordDialog::RecordDialog(IdacProxy* idac, QObject* parent)
	: QObject(parent)
{
	m_idac = idac;
	m_handler = new RecordHandler(m_idac);

	m_loop = NULL;

	m_timer = new QTimer(this);
	m_timer->setInterval(200);
	m_timer->start();

	m_state = IdacState_None;
	m_nConnectAttempts = 1;
	m_nWaiting = 0;
	m_bAccepted = false;

	connect(m_timer, SIGNAL(timeout()), this, SLOT(checkWait()));

	connect(m_idac, SIGNAL(stateChanged(IdacState)), this, SLOT(setState()));
	setState();

	m_idac->startSampling(Globals->idacSettings()->channels);

}

RecordDialog::~RecordDialog()
{
	delete m_handler;
}

bool RecordDialog::exec()
{
	QEventLoop loop;
	m_loop = &loop;
	QPointer<RecordDialog> guard = this;
	loop.exec();
	if (guard.isNull())
		return false;

	if (!m_bAccepted)
		m_idac->stopSampling();
	return m_bAccepted;
}

void RecordDialog::accept()
{
	m_bAccepted = true;
	m_loop->exit();
}

void RecordDialog::reject()
{
	m_bAccepted = false;
	m_loop->exit();
}

void RecordDialog::setState()
{
	IdacState state = m_idac->state();
	qDebug() << "state:" << state;
	if (state != m_state)
	{
		m_state = state;
		m_nWaiting = 0;

		switch (m_state)
		{
		case IdacState_Searching:
		case IdacState_Initializing:
			break;

		case IdacState_None:
		case IdacState_NotPresent:
		case IdacState_Present:
		case IdacState_InitError:
			m_nConnectAttempts++;
			// Try connecting again
			if (m_nConnectAttempts <= 3)
				on_btnConnect_clicked();
			else
				reject();
			break;

		case IdacState_Ready:
			m_handler->updateRawToVoltageFactors();
			break;

		case IdacState_Sampling:
			m_handler->updateRawToVoltageFactors();
			break;
		}
	}
}

void RecordDialog::checkWait()
{
	if (m_handler->check() && m_handler->convert())
	{
		if (m_state == IdacState_Sampling)
			accept();
	}
	else
	{
		m_nWaiting++;
		if (m_nWaiting >= 25) {
			// Try connecting again
			m_nConnectAttempts++;
			if (m_nConnectAttempts <= 3)
				on_btnConnect_clicked();
			else
				reject();
		}
	}
}

void RecordDialog::on_btnConnect_clicked()
{
	qDebug() << "on_btnConnect_clicked()";
	m_idac->stopSampling();
	m_idac->startSampling(Globals->idacSettings()->channels);
}
