/**
 * Copyright (C) 2008, 2009  Ellis Whitehead
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

#ifndef __IDACCONTROLTHREAD_H
#define __IDACCONTROLTHREAD_H

#include <QList>
#include <QPointer>
#include <QThread>

#include "IdacDriverManager.h"


class IdacControlThread : public QThread
{
public:
	IdacControlThread(QObject* parent = NULL);
	~IdacControlThread();

	IdacDriverManager* manager() { return m_manager; }

// Overrides for QThread
protected:
	void run();

private:
	QPointer<IdacDriverManager> m_manager;
};

#endif
