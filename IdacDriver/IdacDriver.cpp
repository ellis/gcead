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

#include "IdacDriver.h"

#include <iostream>
using namespace std;

#include <QtDebug>

#include <Check.h>


IdacDriver::IdacDriver(QObject* parent)
	: QObject(parent)
{
}

void IdacDriver::init()
{
	loadCaps(&m_caps);
	loadDefaultChannelSettings(m_settingsActual);
	loadDefaultChannelSettings(m_settingsDesired);
}

void IdacDriver::addError(const QString& s)
{
	//checkLog("IDAC ERROR: " + s);
	//cerr << "addError: " << qPrintable(s) << endl;
	m_errorMutex.lock();
	m_errors << s;
	m_errorMutex.unlock();
}

bool IdacDriver::hasErrors()
{
	int n;
	m_errorMutex.lock();
	n = m_errors.size();
	m_errorMutex.unlock();
	return (n > 0);
}

QStringList IdacDriver::errorMessages()
{
	QStringList list;
	m_errorMutex.lock();
	list = m_errors;
	m_errors.clear();
	m_errorMutex.unlock();
	return list;
}

void IdacDriver::setChannelSettings(int iChan, const IdacChannelSettings& channel)
{
	CHECK_PARAM_RET(iChan >= 0 && iChan < 3);

	QMutexLocker locker(&m_settingsMutex);
	m_settingsDesired[iChan] = channel;
}

const IdacChannelSettings* IdacDriver::desiredSettings()
{
	//QMutexLocker locker(&m_settingsMutex);
	return m_settingsDesired;
}

const IdacChannelSettings* IdacDriver::desiredChannelSettings(int iChan)
{
	CHECK_PARAM_RETVAL(iChan >= 0 && iChan < 3, NULL);
	return &m_settingsDesired[iChan];
}

IdacChannelSettings* IdacDriver::actualChannelSettings(int iChan)
{
	CHECK_PARAM_RETVAL(iChan >= 0 && iChan < 3, NULL);
	return &m_settingsActual[iChan];
}
