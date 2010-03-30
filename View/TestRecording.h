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

#include <QWidget>


class QLabel;
class QPushButton;
class QTimer;

class MainScope;


class TestRecording : public QWidget
{
	Q_OBJECT
public:
	TestRecording(MainScope* scope);

// QObject overrides
protected:
	void timerEvent(QTimerEvent* e);

private slots:
	void run();
	void record();
	void save();
	void on_scope_isRecordingChanged(bool bRecording);

private:
	MainScope* scope;

	QPushButton* m_btnRecord;
	QLabel* m_lblRecordings;

	//QTimer* m_timerRecord;
	int m_nRecordings;
	bool m_bRunning;
};

#endif
