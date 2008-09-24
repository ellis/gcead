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

#include "IdacThread.h"

#include <QtDebug>
#include <QMutex>
#include <QTimer>

#include <Check.h>
#include <IdacControl/Idacpc.h>

// FIXME: for debug only
//#include "IdacRecorder.h"


extern QMutex g_exitMutex;


IdacThread::IdacThread(QObject* parent)
	: QThread(parent)
{
	m_recorder = NULL;
	m_timer = NULL;
}

IdacThread::~IdacThread()
{
}

void IdacThread::run()
{
	g_exitMutex.lock();

	m_recorder = new IdacRecorder();
	emit ready();

	QTimer timer;
	connect(&timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
	timer.start(60);
	exec();

	m_recorder->requestState(IdacState_Inactive);
	delete m_recorder;
	m_recorder = NULL;

	g_exitMutex.unlock();
}

void IdacThread::on_timer_timeout()
{
	//qDebug() << "t";
	// TODO: only enable the timer when we're actually recording -- ellis, 2008-02-23
	if (m_recorder->state() == IdacState_Receiving)
		m_recorder->grabDataFromHardware();
}
