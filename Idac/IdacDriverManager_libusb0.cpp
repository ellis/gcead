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

#include <usb.h>

#include <QtDebug>

#include <Check.h>

#include <IdacDriver/IdacDriver.h>
#include <IdacDriver/Sleeper.h>
#include <IdacDriver2/IdacDriver2.h>
#include <IdacDriver4/IdacDriver4.h>
#include <IdacDriverES/IdacDriverES.h>


void IdacDriverManager::initLibusb()
{
	usb_set_debug(255);
	usb_init();
#ifndef QT_NO_DEBUG
	usb_set_debug(255);
#endif
}

void IdacDriverManager::exitLibusb()
{
	if (m_driver != NULL) {
		delete m_driver;
		m_driver = NULL;
	}

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

/// Return usb_device* of IDAC device,
/// if @ref dev or one of its children is an IDAC.
static struct usb_device* findIdac(struct usb_device* dev)
{
	// IDAC 4 = 088D:0006
	// IDAC 2 = 088D:0008
	if (dev->descriptor.idVendor == 0x088D) {
		if (dev->descriptor.idProduct == 0x0008 || dev->descriptor.idProduct == 0x0006)
			return dev;
	}

	struct usb_device* found = NULL;
	for (int i = 0; i < dev->num_children && !found; i++)
	{
		// TODO: Add an error message if child is NULL -- that happens when user doesn't have permissions for the device
		if (dev->children == NULL)
			break;
		if (dev->children[i] != NULL)
			found = findIdac(dev->children[i]);
	}

	return found;
}

/// Return usb_device* of IDAC device, if found on any of the USB busses.
static struct usb_device* findIdac()
{
	struct usb_device* found = NULL;

	usb_find_busses();
	usb_find_devices();

	for (struct usb_bus* bus = usb_busses; bus && !found; bus = bus->next)
	{
		if (bus->root_dev)
			found = findIdac(bus->root_dev);
		else
		{
			for (struct usb_device* dev = bus->devices; dev && !found; dev = dev->next)
				found = findIdac(dev);
		}
	}

	return found;
}

void IdacDriverManager::createLibusbDriver()
{
	struct usb_device* dev = findIdac();
	if (dev != NULL) {
		m_device = dev;
		m_handle = usb_open(dev);
		if (m_handle != NULL) {
			// IDAC 4
			if (dev->descriptor.idProduct == 0x0006)
				m_driver = new IdacDriver4(m_device, m_handle);
			// IDAC 2
			else if (dev->descriptor.idProduct == 0x0008)
				m_driver = new IdacDriver2(m_device, m_handle);

			m_driver->init();
		}
		else {
			qDebug() << "Unable to open USB device";
		}
	}
#ifdef WIN32
	else {
		if (IdacDriverES::driverIsPresent()) {
			m_driver = new IdacDriverES();
			m_driver->init();
		}
	}
#endif
}
