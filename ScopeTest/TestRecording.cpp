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

#include "TestRecording.h"

#include <QApplication>
#include <QTimerEvent>

#include <IdacDriver/IdacSettings.h>


TestRecording::TestRecording() : TestBase(true)
{
	m_nRecordings = 0;

	connect(scope, SIGNAL(isRecordingChanged(bool)), this, SLOT(on_scope_isRecordingChanged(bool)));

	startTimer(0);
}

void TestRecording::timerEvent(QTimerEvent* e)
{
	killTimer(e->timerId());
	run();
}

void TestRecording::run()
{
	Actions* actions = scope->actions();

	ui->s = "ScopeTest.ead";
	actions->fileSave->trigger();

	Globals->idacSettings()->nRecordingDuration = 1;
	record();
}

void TestRecording::record()
{
	m_nRecordings++;
	qDebug() << "RECORD #" << m_nRecordings;
	scope->actions()->recordRecord->trigger();
}

void TestRecording::on_scope_isRecordingChanged(bool bRecording)
{
	if (!bRecording) {
		if (m_nRecordings < 3)
			record();
		else
			QApplication::exit();
	}
}
