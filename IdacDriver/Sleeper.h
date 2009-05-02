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

#ifndef __SLEEPER_H
#define __SLEEPER_H

#include <QThread>


class Sleeper : public QThread
{
public:
	//static void sleep(unsigned long secs) {
	//	QThread::sleep(secs);
	//}
	static void msleep(unsigned long msecs) {
		QThread::msleep(msecs);
	}
	//static void usleep(unsigned long usecs) {
	//	QThread::usleep(usecs);
	//}
};

#endif // SLEEPER_H
