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

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <QObject>
#include <QStringList>

#include "EadEnums.h"


class QSettings;

class IdacSettings;
class PublisherSettings;
class ViewSettings;


class GlobalVars : public QObject
{
	Q_OBJECT
public:
	GlobalVars();
	~GlobalVars();

public:
	QStringList recentFiles;

public:
	/// Last directory that the user open or saved a file in
	const QString& lastDir() const { return m_sLastDir; }

	ViewSettings* viewSettings() { return m_viewSettings; }
	PublisherSettings* publisherSettings() { return m_publisherSettings; }
	IdacSettings* idacSettings() { return m_idacSettings; }

	void readSettings();
	void readIdacChannelSettings(const QString& sIdacName);
	void writeSettings();
	void writeIdacChannelSettings(const QString& sIdacName);

public slots:
	void setLastDir(const QString& sLastDir);

signals:
	void peakModeChanged(EadMarkerMode peakMode);

private:
	QString m_sLastDir;
	ViewSettings* m_viewSettings;
	PublisherSettings* m_publisherSettings;
	IdacSettings* m_idacSettings;
};

extern GlobalVars* Globals;

#endif
