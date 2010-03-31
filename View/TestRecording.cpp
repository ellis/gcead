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

#include <QtDebug>
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTimerEvent>
#include <QVBoxLayout>

#include <AppDefines.h>
#include <Globals.h>

#include <Scope/MainScope.h>
//#include <Scope/MainScopeUi.h>

#include <IdacDriver/IdacSettings.h>


TestRecording::TestRecording(MainScope* scope)
{
	this->scope = scope;
	m_nRecordings = 0;
	m_bRunning = false;

	QVBoxLayout* layout = new QVBoxLayout();

	m_btnRecord = new QPushButton("Start");
	m_lblRecordings = new QLabel("0");
	layout->addWidget(m_btnRecord);
	layout->addWidget(m_lblRecordings);

	setLayout(layout);

	connect(scope, SIGNAL(isRecordingChanged(bool)), this, SLOT(on_scope_isRecordingChanged(bool)));

	connect(m_btnRecord, SIGNAL(clicked()), this, SLOT(run()));
	//connect(m_timerRecord, SIGNAL(timeout()), this, SLOT(record()));

	//startTimer(0);
}

void TestRecording::timerEvent(QTimerEvent* e)
{
	killTimer(e->timerId());
	run();
}

void TestRecording::run()
{
	if (!m_bRunning)
	{
		m_bRunning = true;
		scope->save("TestRecording.ead");

		Globals->idacSettings()->nRecordingDuration = 10;
		record();
	}
	else
		m_bRunning = false;
}

void TestRecording::record()
{
	m_nRecordings++;
	m_lblRecordings->setText(QString::number(m_nRecordings));
	if (m_nRecordings < 15 && m_bRunning) {
		qDebug() << "RECORD #" << m_nRecordings;
		scope->actions()->recordRecord->trigger();
		//QTimer::singleShot(10000, this, SLOT(save()));
	}
	//else
	//	QApplication::exit();
}

void TestRecording::save()
{
	scope->actions()->recordSave->trigger();
	QTimer::singleShot(1000, this, SLOT(record()));
}

void TestRecording::on_scope_isRecordingChanged(bool bRecording)
{
	if (!bRecording) {
		QTimer::singleShot(1000, this, SLOT(record()));
	}
}
