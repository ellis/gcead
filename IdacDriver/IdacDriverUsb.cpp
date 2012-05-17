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

#include "IdacDriverUsb.h"

#ifdef LIBUSBX
#include <libusb-1.0/libusb.h>
#else
#include <usb.h>
#endif

#include <QtDebug>
#include <QFile>

#include <Check.h>

#include "Sleeper.h"


IdacDriverUsb::IdacDriverUsb(UsbHandle* handle, QObject* parent)
	: IdacDriverWithThread(parent)
{
	m_handle = handle;

#ifdef LIBUSB0
	if (m_handle != NULL)
	{
		m_handle = usb_open(m_handle);
		if (m_handle == NULL)
			logUsbError(__FILE__, __LINE__, "Unable to open USB device");
	}
	else
		m_handle = NULL;
#endif
}

IdacDriverUsb::~IdacDriverUsb()
{
#ifdef LIBUSB0
	if (m_handle != NULL)
	{
		// The pointers needs to be checked incase the USB device has
		// been unplugged and the pointers have been NULLed by libusb.
		struct usb_config_descriptor* config = &m_handle->config[0];
		if (config != NULL)
		{
			struct usb_interface* interface = &config->interface[0];
			if (interface != NULL)
			{
				struct usb_interface_descriptor* altsetting = &interface->altsetting[0];
				if (altsetting != NULL)
				{
					int idInterface = altsetting->bInterfaceNumber;
					usb_release_interface(m_handle, idInterface);
				}
			}
		}
		usb_close(m_handle);
		m_handle = NULL;
	}
#endif
}

void IdacDriverUsb::logUsbError(const char* file, int line, int result)
{
#ifdef LIBUSBX
	const char* s = libusb_error_name(result);
#else
	const char* s = usb_strerror();
#endif
	if (s != NULL && *s != 0) {
		logUsbError(file, line, QString("USB ERROR %0: %1").arg(result).arg(s));
	}
}

void IdacDriverUsb::logUsbError(const char* file, int line, const QString& s)
{
	QString s2 = s.trimmed();
	addError(s2);
	checkLog(file, line, "USB ERROR", s2);
}

bool IdacDriverUsb::sendOutgoingMessage(int requestId, int timeout)
{
#ifdef LIBUSBX
	int n = libusb_control_transfer(
			m_handle,
#else // LIBUSB0
	int n = usb_control_msg(
			m_handle,
#endif
			0x40, // requesttype (vendor message, host -> device)
			requestId, // request
			0,
			0,
			NULL,
			0,
			timeout);
	CHECK_USBRESULT_RETVAL(n, false);

	return true;
}

bool IdacDriverUsb::sendOutgoingMessage(int requestId, quint8* buffer, int size, int timeout)
{
#ifdef LIBUSBX
	int n = libusb_control_transfer(
			m_handle,
#else // LIBUSB0
	int n = usb_control_msg(
			m_handle,
#endif
			//usb_sndctrlpipe(m_handle, 0), // requesttype
			0x40, // requesttype (vendor message, host -> device)
			requestId, // request
			0,
			0,
#ifdef LIBUSBX
			buffer,
#else
			(char*) buffer,
#endif
			size,
			timeout);
	CHECK_USBRESULT_RETVAL(n, false);

	return true;
}

bool IdacDriverUsb::sendIncomingMessage(int requestId, quint8* buffer, int size, int timeout)
{
#ifdef LIBUSBX
	int n = libusb_control_transfer(
			m_handle,
#else // LIBUSB0
	int n = usb_control_msg(
			m_handle,
#endif
			0xC0, // requesttype (vendor message, device -> host)
			requestId, // request
			0,
			0,
#ifdef LIBUSBX
			buffer,
#else
			(char*) buffer,
#endif
			size,
			timeout);
	CHECK_USBRESULT_RETVAL(n, false);

	return true;
}

bool IdacDriverUsb::sendFirmware(INTEL_HEX_RECORD firmware[])
{
	unsigned char buffer[1];
	int res;
	bool bOk = true;

	// USB Reset
	buffer[0] = 1;
	res = myusb_control_transfer(0x40, 0xA0, 0xE600, 0, (unsigned char*) &buffer, 1, 5000);
	CHECK_USBRESULT_NORET(res);
	if (res < 0)
	{
		bOk = false;
	}

	if (res >= 0)
	{
		int i = 0;
		while (firmware[i].type == 0)
		{
			INTEL_HEX_RECORD& d = firmware[i];

			res = myusb_control_transfer(0x40, 0xA0, d.address, 0, d.data, d.length, 5000);
			CHECK_USBRESULT_NORET(res);
			if (res < 0)
			{
				bOk = false;
				// TODO: add some error message/handling here
				qDebug() << "Failed to init: res =" << res;
				break;
			}
			i++;
		}
	}

	// USB Reset
	buffer[0] = 0;
	res = myusb_control_transfer(0x40, 0xA0, 0xE600, 0, (unsigned char*) &buffer, 1, 5000);
	CHECK_USBRESULT_NORET(res);
	if (res < 0)
	{
		bOk = false;
	}

	m_handle = NULL;
#ifdef LIBUSB0
	m_handle = NULL;
#endif

	Sleeper::msleep(5000);

	return bOk;
}

bool IdacDriverUsb::sendHexFile(QString sFilename)
{
	QFile file(sFilename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		logUsbError(__FILE__, __LINE__, QString("File not found: %0").arg(sFilename));
		return false;
	}

	QByteArray hex = file.readAll();
	file.close();

	return sendHexData(hex);
}

bool IdacDriverUsb::sendHexData(const QByteArray& hex)
{
	bool b = true;
	quint8* buffer = new quint8[5000];

	int iHex = 0;
	while (iHex + 1 < hex.count() && b) {
		int nHex = hex.count() - iHex;
		if (nHex > 10000)
			nHex = 10000;

		int nData = nHex / 2;
		QString sHex;
		for (int iData = 0; iData < nData; iData++)
		{
			sHex = hex[iHex++];
			sHex += hex[iHex++];
			buffer[iData] = (quint8) sHex.toInt(NULL, 16);
		}

		int res = myusb_bulk_write(
				1, // end point, TODO: don't hardcode the endpoint? -- ellis, 2009-04-26
				buffer, // bytes
				nData, // size
				5000); // timeout
		CHECK_USBRESULT_NORET(res);
		b = (res >= 0);
	}

	delete[] buffer;
	return b;
}

bool IdacDriverUsb::sendBinFile(QString sFilename)
{
	QFile file(sFilename);
	if (!file.open(QIODevice::ReadOnly))
	{
		logUsbError(__FILE__, __LINE__, QString("File not found: %0").arg(sFilename));
		return false;
	}

	QByteArray bin = file.readAll();
	file.close();

	return sendBinData(bin);
}

bool IdacDriverUsb::sendBinData(const QByteArray& bin)
{
	bool b = true;

	int iData = 0;
	while (iData < bin.count() && b) {
		int nData = bin.count() - iData;
		if (nData > 5000)
			nData = 5000;

		int res = myusb_bulk_write(
				1, // end point, TODO: don't hardcode the endpoint? -- ellis, 2009-04-26
				(unsigned char*) bin.data() + iData, // bytes
				nData, // size
				5000); // timeout
		CHECK_USBRESULT_NORET(res);
		b = (res >= 0);

		iData += nData;
	}

	return b;
}

int IdacDriverUsb::myusb_control_transfer(
	uint8_t  	bmRequestType,
	uint8_t  	bRequest,
	uint16_t  	wValue,
	uint16_t  	wIndex,
	unsigned char *  	data,
	uint16_t  	wLength,
	unsigned int  	timeout
) {
	return
#ifdef LIBUSBX
	libusb_control_transfer(m_handle,
#else
	usb_control_msg(m_handle,
#endif
		bmRequestType, bRequest, wValue, wIndex,
#ifdef LIBUSBX
		data,
#else
		(char*) data,
#endif
		wLength, timeout);
}

int IdacDriverUsb::myusb_bulk_write(
	unsigned char endpoint,
	unsigned char * data,
	int length,
	//int * transferred,
	unsigned int timeout
) {
#ifdef LIBUSBX
	int transferred = 0;
	return libusb_bulk_transfer(m_handle,
#else
	return usb_bulk_write(m_handle,
#endif
		endpoint,
#ifdef LIBUSBX
		data,
#else
		(char*) data,
#endif
		length,
#ifdef LIBUSBX
		&transferred,
#endif
		timeout);
}
