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

#ifndef __TESTRECORDING_H
#define __TESTRECORDING_H

#include "TestBase.h"


class QTimer;


class TestRecording : public TestBase
{
	Q_OBJECT
public:
    TestRecording(int id);

// QObject overrides
protected:
	void timerEvent(QTimerEvent* e);

private:
	void run();

private slots:
	void record();
	void save();
	void on_scope_isRecordingChanged(bool bRecording);

private:
	//QTimer* m_timerRecord;
	int m_nRecordings;
};

#endif
