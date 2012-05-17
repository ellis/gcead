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

// Using libusbx on linux/mac
#ifdef LIBUSBX
struct libusb_device_handle;
typedef libusb_device_handle UsbHandle;
// Using libusb0 on windows
#else
struct usb_device;
typedef usb_device UsbHandle;
#endif

class IdacCaps;
class IdacChannelSettings;
class IdacDriver;


class IdacDriverManager : public QObject
{
	Q_OBJECT
public:
	IdacDriverManager(QObject* parent = NULL);
	~IdacDriverManager();

	UsbHandle* handle() { return m_handle; }
	IdacDriver* driver() { return m_driver; }

	IdacState state() const { return m_state; }
	IdacCommand currentCommand() const { return m_cmd; }

	const IdacCaps* caps() const;
	QString hardwareName();
	QList<int> ranges();
	QStringList highcutStrings();
	QStringList lowcutStrings();
	QStringList errorMessages();

	/// Load up default channel settings for the current driver
	const QVector<IdacChannelSettings>& defaultChannelSettings();
	void setChannelSettings(int iChannel, const IdacChannelSettings& channel);
	int takeData(short* digital, short* analog1, short* analog2, int maxSize);

public slots:
	void command(int _cmd);

signals:
	void stateChanged(int _state);
	void commandFinished(int _cmd);

private:
	void setState(IdacState state);
	void setup();
	void loadDriver();

// Specialize these for the libusb library being used
private:
	void initLibusb();
	void exitLibusb();
	void createLibusbDriver();
	//void createDriver();

private:
	IdacState m_state;
	IdacCommand m_cmd;

	UsbHandle* m_handle;
	IdacDriver* m_driver;
};

#endif
