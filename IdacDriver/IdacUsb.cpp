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

#include "IdacUsb.h"

#include <usb.h>

#include "Check.h"
#include "IdacDriver4.h"


IdacUsb::IdacUsb()
{
	m_dev = NULL;
	m_handle = NULL;
	m_bClaimed = false;
	m_driver = NULL;
}

bool IdacUsb::findIdac()
{
	if (m_handle != NULL)
	{
		usb_close(m_handle);
		m_dev = NULL;
		m_handle = NULL;
	}

	bool bFound = false;
	for (struct usb_bus* bus = usb_busses; bus && !bFound; bus = bus->next) {
		if (bus->root_dev)
			bFound = findIdac(bus->root_dev);
		else {
			struct usb_device *dev;

			for (dev = bus->devices; dev && !bFound; dev = dev->next) {
				bFound = findIdac(dev);
			}
		}
	}
	return bFound;
}

bool IdacUsb::findIdac(struct usb_device* dev)
{
	// IDAC 2
	if (dev->descriptor.idVendor == 0x088D) {
		if (dev->descriptor.idProduct == 0x0008 || dev->descriptor.idProduct == 0x0006) {
			usb_dev_handle* handle = usb_open(dev);
			if (handle != NULL) {
				m_dev = dev;
				m_handle = handle;
				return true;
			}
		}
	}

	for (int i = 0; i < dev->num_children; i++)
	{
		if (findIdac(dev->children[i]))
			return true;
	}

	return false;
}

#include <iostream>
#include <QString>

bool IdacUsb::claim()
{
	CHECK_PRECOND_RETVAL(!m_bClaimed, false);

	int res;

	/*
	quint8 data[18];
	res = usb_control_msg(m_handle, 0x80, 0x06, 0x0100, 0x0000, (char*) &data, 18, 5000);
	if (res >= 0)
	{
		for (int i = 0; i < 18; i += 2)
			std::cerr << qPrintable(QString("%0 ").arg((int)data[0], 16));
		std::cerr << std::endl;
	}
	*/

	int idConfiguration = m_dev->config[0].bConfigurationValue;
	res = usb_set_configuration(m_handle, idConfiguration);
	if (res < 0)
		return false;

	int idInterface = m_dev->config[0].interface[0].altsetting[0].bInterfaceNumber;
	res = usb_claim_interface(m_handle, idInterface);
	if (res < 0)
		return false;

	usb_interface_descriptor* setting = &m_dev->config[0].interface[0].altsetting[0];
	res = usb_set_altinterface(m_handle, setting->bAlternateSetting);
	if (res < 0)
		return false;

	for (int iPipe = 0; iPipe < setting->bNumEndpoints; iPipe++)
	{
		int idPipe = setting->endpoint[iPipe].bEndpointAddress;
		res = usb_clear_halt(m_handle, idPipe);
	}

	m_bClaimed = true;

	m_driver = new IdacUsbDriver4(this);
	return m_driver->boot();
}
