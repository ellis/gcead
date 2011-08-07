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

#ifndef __IDACDRIVERMANAGER_H
#define __IDACDRIVERMANAGER_H

#include <QObject>

#include <IdacDriver/IdacEnums.h>


struct usb_device;

class IdacCaps;
class IdacChannelSettings;
class IdacDriver;


class IdacDriverManager : public QObject
{
	Q_OBJECT
public:
	IdacDriverManager(QObject* parent = NULL);
	~IdacDriverManager();

	struct usb_device* device() { return m_device; }

	IdacState state() const { return m_state; }
	IdacCommand currentCommand() const { return m_cmd; }

	const IdacCaps* caps() const;
	QString hardwareName();
	QList<int> ranges();
	QStringList highcutStrings();
	QStringList lowcutStrings();
	QStringList errorMessages();

	/// Load up default channel settings for the current driver
	void loadDefaultChannelSettings(IdacChannelSettings* channels);
	void setChannelSettings(int iChannel, const IdacChannelSettings& channel);
	int takeData(short* digital, short* analog1, short* analog2, int maxSize);

public slots:
	void command(int _cmd);

signals:
	void stateChanged(int _state);
	void commandFinished(int _cmd);

private:
	static struct usb_device* findIdac();
	static struct usb_device* findIdac(struct usb_device* dev);

	void setState(IdacState state);
	void findDevice();
	void createDriver();
	void setup();

private:
	IdacState m_state;
	IdacCommand m_cmd;

	struct usb_device* m_device;
	IdacDriver* m_driver;
};

#endif
