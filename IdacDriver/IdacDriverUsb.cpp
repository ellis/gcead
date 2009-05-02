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

#include <usb.h>

#include <QtDebug>
#include <QFile>

#include <Check.h>

#include "Sleeper.h"


IdacDriverUsb::IdacDriverUsb(struct usb_device* device, QObject* parent)
	: IdacDriver(parent)
{
	m_device = device;

	if (m_device != NULL)
	{
		m_handle = usb_open(m_device);
		if (m_handle == NULL)
			logUsbError();
	}
	else
		m_handle = NULL;
}

IdacDriverUsb::~IdacDriverUsb()
{
	if (m_handle != NULL)
	{
		// The pointers needs to be checked incase the USB device has
		// been unplugged and the pointers have been NULLed by libusb.
		struct usb_config_descriptor* config = &m_device->config[0];
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
}

void IdacDriverUsb::logUsbError()
{
	const char* s = usb_strerror();
	if (s != NULL && *s != 0)
		addError(tr("USB ERROR: %0").arg(s));
}

bool IdacDriverUsb::sendOutgoingMessage(int requestId, int timeout)
{
	int n = usb_control_msg(
			m_handle,
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
	int n = usb_control_msg(
			m_handle,
			//usb_sndctrlpipe(m_handle, 0), // requesttype
			0x40, // requesttype (vendor message, host -> device)
			requestId, // request
			0,
			0,
			(char*) buffer,
			size,
			timeout);
	CHECK_USBRESULT_RETVAL(n, false);

	return true;
}

bool IdacDriverUsb::sendIncomingMessage(int requestId, quint8* buffer, int size, int timeout)
{
	int n = usb_control_msg(
			m_handle,
			0xC0, // requesttype (vendor message, device -> host)
			requestId, // request
			0,
			0,
			(char*) buffer,
			size,
			timeout);
	CHECK_USBRESULT_RETVAL(n, false);

	return true;
}

bool IdacDriverUsb::sendFirmware(INTEL_HEX_RECORD firmware[])
{
	char buffer[1];
	int res;
	bool bOk = true;

	// USB Reset
	buffer[0] = 1;
	res = usb_control_msg(m_handle, 0x40, 0xA0, 0xE600, 0, (char*) &buffer, 1, 5000);
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

			res = usb_control_msg(m_handle, 0x40, 0xA0, d.address, 0, (char*) d.data, d.length, 5000);
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
	res = usb_control_msg(m_handle, 0x40, 0xA0, 0xE600, 0, (char*) &buffer, 1, 5000);
	CHECK_USBRESULT_NORET(res);
	if (res < 0)
	{
		bOk = false;
	}

	m_handle = NULL;

	Sleeper::msleep(5000);

	return bOk;
}

/*
struct Idac4InitLine
{
	int requesttype;
	int request;
	int value;
	int index;
	int size;
	const char *code;
};

void IdacDriverUsb::sendMessages(Idac4InitLine* lines, int count)
{
	for (int i = 0; i < count; i++) {
		Idac4InitLine& d = lines[i];

		QString sCode = d.code;
		int iBuffer = 0;
		while (!sCode.isEmpty()) {
			QString sByte = sCode.left(2);
			buffer[iBuffer++] = (quint8) sByte.toInt(NULL, 16);
			sCode = sCode.mid(2).trimmed();
		}

		CHECK_ASSERT_RET(d.size <= sizeof(buffer));
		res = usb_control_msg(m_handle, d.requesttype, d.request, d.value, d.index, (char*) &buffer, d.size, 5000);
		if (res < 0)
		{
			// TODO: add some error message/handling here
			qDebug() << "Failed to init: res =" << res;
			break;
		}
	}
}
*/

bool IdacDriverUsb::sendHexFile(QString sFilename)
{
	QFile file(sFilename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		addError(QString("File not found: %0").arg(sFilename));
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

		int res = usb_bulk_write(
				m_handle,
				1, // end point, TODO: don't hardcode the endpoint? -- ellis, 2009-04-26
				(char*) buffer, // bytes
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
		addError(QString("File not found: %0").arg(sFilename));
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

		int res = usb_bulk_write(
				m_handle,
				1, // end point, TODO: don't hardcode the endpoint? -- ellis, 2009-04-26
				(char*) bin.data() + iData, // bytes
				nData, // size
				5000); // timeout
		CHECK_USBRESULT_NORET(res);
		b = (res >= 0);

		iData += nData;
	}

	return b;
}
