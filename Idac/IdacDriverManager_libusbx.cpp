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

#include "IdacDriverManager.h"

#if defined(Q_WS_X11) || defined(Q_WS_MAC)
#include <libusb-1.0/libusb.h>
#else
#include <usb.h>
#endif

#include <QtDebug>

#include <Check.h>

#include <IdacDriver/IdacDriver.h>
#include <IdacDriver/Sleeper.h>
#include <IdacDriver2/IdacDriver2.h>
#include <IdacDriver4/IdacDriver4.h>
#include <IdacDriverES/IdacDriverES.h>


void IdacDriverManager::initLibusb()
{
    libusb_init(NULL);
#ifndef QT_NO_DEBUG
    libusb_set_debug(NULL, 3);
#endif
}

void IdacDriverManager::exitLibusb()
{
	if (m_driver != NULL) {
		delete m_driver;
		m_driver = NULL;
	}

	if (m_device != NULL) {
		libusb_close(m_device);
	}

	libusb_exit(NULL);
}

void IdacDriverManager::findDevice()
{
	UsbDevice* handle = NULL;

	libusb_device** devs;
	int r;
	ssize_t cnt = libusb_get_device_list(NULL, &devs);
	if (cnt >= 0) {
		for (int i = 0; i < cnt; i++) {
			libusb_device* dev = devs[i];
			struct libusb_device_descriptor desc;
			int r = libusb_get_device_descriptor(dev, &desc);
			if (r >= 0) {
				// IDAC 4 = 088D:0006
				// IDAC 2 = 088D:0008
				if (desc.idVendor == 0x088D) {
					if (desc.idProduct == 0x0008 || desc.idProduct == 0x0006) {
						r = libusb_open(dev, &handle);
						if (r >= 0) {
							m_device = handle;
							if (desc.idProduct == 0x0008)
								m_driver = new IdacDriver4(m_device);
							else
								m_driver = new IdacDriver2(m_device);
							break;
						}
					}
				}
			}
		}
	}

	libusb_free_device_list(devs, 1);

	return handle;
}