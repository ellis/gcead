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

#ifndef __IDACUSB_H
#define __IDACUSB_H

#include <QtGlobal> // for NULL

class IdacUsbDriver;


class IdacUsb
{
public:
	struct usb_device* m_dev;
	struct usb_dev_handle* m_handle;
	bool m_bClaimed;
	IdacUsbDriver* m_driver;

public:
	IdacUsb();

	struct usb_device* dev() { return m_dev; }
	struct usb_dev_handle* handle() { return m_handle; }
	IdacUsbDriver* driver() { return m_driver; }

	bool findIdac();
	bool claim();

private:
	bool findIdac(struct usb_device* dev);
	bool boot();
};

#endif
