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

#ifndef __IDACDRIVERUSB_H
#define __IDACDRIVERUSB_H

#include <Idac/IdacDriverManager.h> // For UsbHandle
#include "IdacDriverWithThread.h"


// These should be used in IdacDriverUsb and derived classes to check USB errors
#define CHECK_USBRESULT_RET(x) { if ((x) < 0) { logUsbError(__FILE__, __LINE__, x); return; } }
#define CHECK_USBRESULT_RETVAL(x, ret) { if ((x) < 0) { logUsbError(__FILE__, __LINE__, x); return (ret); } }
#define CHECK_USBRESULT_NORET(x) { if ((x) < 0) { logUsbError(__FILE__, __LINE__, x); } }


#define MAX_INTEL_HEX_RECORD_LENGTH 64
struct INTEL_HEX_RECORD
{
	quint32 length;
	quint32 address;
	quint32 type;
	quint8 data[MAX_INTEL_HEX_RECORD_LENGTH];
};


class IdacDriverUsb : public IdacDriverWithThread
{
public:
	IdacDriverUsb(UsbHandle* device, QObject* parent = NULL);
	~IdacDriverUsb();

	UsbHandle* handle() { return m_handle; }

protected:
	void logUsbError(const char* file, int line, int result);
	void logUsbError(const char* file, int line, const QString& s);
	bool sendOutgoingMessage(int requestId, int timeout = 5000);
	bool sendOutgoingMessage(int requestId, quint8* buffer, int size, int timeout = 5000);
	bool sendIncomingMessage(int requestId, quint8* buffer, int size, int timeout = 5000);
	bool sendFirmware(INTEL_HEX_RECORD firmware[]);
	bool sendHexFile(QString sFilename);
	bool sendHexData(const QByteArray& hex);
	bool sendBinFile(QString sFilename);
	bool sendBinData(const QByteArray& hex);

private:
	int myusb_control_transfer(
		quint8  	bmRequestType,
		quint8  	bRequest,
		quint16  	wValue,
		quint16  	wIndex,
		unsigned char *  	data,
		quint16  	wLength,
		unsigned int  	timeout
	);

	int myusb_bulk_write(
		unsigned char endpoint,
		unsigned char * data,
		int length,
		//int * transferred,
		unsigned int timeout
	);

private:
	UsbHandle* m_handle;
};

#endif
