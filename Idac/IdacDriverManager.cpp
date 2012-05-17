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

#include <QtDebug>

#include <Check.h>

#include <IdacDriver/IdacDriver.h>
#include <IdacDriver/Sleeper.h>
#include <IdacDriver2/IdacDriver2.h>
#include <IdacDriver4/IdacDriver4.h>
#include <IdacDriverES/IdacDriverES.h>


IdacDriverManager::IdacDriverManager(QObject* parent)
	: QObject(parent)
{
	m_state = IdacState_None;
	m_cmd = IdacCommand_None;

	m_handle = NULL;
	m_driver = NULL;

	initLibusb();
}

IdacDriverManager::~IdacDriverManager()
{
	exitLibusb();
	delete m_driver;
}

void IdacDriverManager::setState(IdacState state)
{
	qDebug() << "IdacDriverManager::setState:" << state;
	if (state != m_state)
	{
		m_state = state;
		emit stateChanged(m_state);
	}
}

void IdacDriverManager::command(int _cmd)
{
	CHECK_PRECOND_RET(m_cmd == IdacCommand_None);

	m_cmd = (IdacCommand) _cmd;
	bool bValid = false;
	switch (m_cmd)
	{
	case IdacCommand_None:
		break;
	case IdacCommand_Connect:
		switch (m_state)
		{
		case IdacState_None:
		case IdacState_NotPresent:
		case IdacState_InitError:
			bValid = true;
			setup();
			break;
		default:
			break;
		}
		break;
	case IdacCommand_Disconnect:
		break;
	case IdacCommand_SamplingOn:
		if (m_state == IdacState_Ready)
		{
			bValid = true;
			if (m_driver->startSampling())
				setState(IdacState_Sampling);
		}
		break;
	case IdacCommand_SamplingOff:
		if (m_state == IdacState_Sampling)
		{
			bValid = true;
			m_driver->stopSampling();
			setState(IdacState_Ready);
		}
		break;
	case IdacCommand_ConfigCh0:
	case IdacCommand_ConfigCh1:
	case IdacCommand_ConfigCh2:
		if (m_state == IdacState_Ready || m_state == IdacState_Sampling)
		{
			bValid = true;
			int iChan = _cmd - IdacCommand_ConfigCh0;
			m_driver->configureChannel(iChan);
		}
		break;
	}

	m_cmd = IdacCommand_None;
	emit commandFinished(_cmd);

	CHECK_ASSERT_RET(bValid);
}

const IdacCaps* IdacDriverManager::caps() const
{
	if (m_driver == NULL)
		return NULL;
	return m_driver->caps();
}

QString IdacDriverManager::hardwareName()
{
	if (m_driver == NULL)
		return QString();
	return m_driver->hardwareName();
}

QList<int> IdacDriverManager::ranges()
{
	if (m_driver == NULL)
		return QList<int>();
	return m_driver->ranges();
}

QStringList IdacDriverManager::highcutStrings()
{
	if (m_driver == NULL)
		return QStringList();
	return m_driver->highcutStrings();
}

QStringList IdacDriverManager::lowcutStrings()
{
	if (m_driver == NULL)
		return QStringList();
	return m_driver->lowcutStrings();
}

QStringList IdacDriverManager::errorMessages()
{
	if (m_driver == NULL)
		return QStringList();
	return m_driver->errorMessages();
}

const QVector<IdacChannelSettings>& IdacDriverManager::defaultChannelSettings()
{
	static QVector<IdacChannelSettings> none(0);
	if (m_driver == NULL)
		return none;
	return m_driver->defaultChannelSettings();
}

void IdacDriverManager::setChannelSettings(int iChannel, const IdacChannelSettings& channel)
{
	if (m_driver == NULL)
		return;
	m_driver->setChannelSettings(iChannel, channel);
}

int IdacDriverManager::takeData(short* digital, short* analog1, short* analog2, int maxSize)
{
	if (m_driver == NULL)
		return 0;

	return m_driver->takeData(digital, analog1, analog2, maxSize);
}

void IdacDriverManager::setup()
{
	setState(IdacState_Searching);
	findDevice();
#ifdef WIN32
	if (m_driver == NULL) {
		if (IdacDriverES::driverIsPresent()) {
			m_driver = new IdacDriverES();
			m_driver->init();
		}
	}
#endif
	if (m_driver == NULL)
	{
		setState(IdacState_NotPresent);
		return;
	}

	m_driver->init();

	setState(IdacState_Initializing);
	if (!m_driver->checkUsbFirmwareReady())
	{
		m_driver->initUsbFirmware();
		delete m_driver;
		m_driver = NULL;
		m_handle = NULL;

		// After USB initialization, try to find the device again
		bool bReady = false;
		for (int iTry = 0; iTry < 3; iTry++)
		{
			setState(IdacState_Searching);
			Sleeper::msleep(5000);
			findDevice();
			if (m_driver != NULL && m_driver->checkUsbFirmwareReady())
			{
				bReady = true;
				break;
			}
		}

		if (!bReady)
		{
			setState(IdacState_InitError);
			return;
		}
	}

	if (!m_driver->checkDataFirmwareReady())
	{
		m_driver->initDataFirmware();
		if (!m_driver->checkDataFirmwareReady())
		{
			setState(IdacState_InitError);
			return;
		}
	}

	setState(IdacState_Ready);
}

/*
void print_endpoint(struct usb_endpoint_descriptor *endpoint)
{
	printf("     ENDPOINT:\n");
	printf("      bEndpointAddress: %02xh\n", endpoint->bEndpointAddress);
	printf("      bmAttributes:     %02xh\n", endpoint->bmAttributes);
	printf("      wMaxPacketSize:   %d\n", endpoint->wMaxPacketSize);
	printf("      bInterval:        %d\n", endpoint->bInterval);
	printf("      bRefresh:         %d\n", endpoint->bRefresh);
	printf("      bSynchAddress:    %d\n", endpoint->bSynchAddress);
}

void print_altsetting(struct usb_interface_descriptor *interface)
{
	int i;

	printf("   ALTSETTING:\n");
	printf("    bInterfaceNumber:   %d\n", interface->bInterfaceNumber);
	printf("    bAlternateSetting:  %d\n", interface->bAlternateSetting);
	printf("    bNumEndpoints:      %d\n", interface->bNumEndpoints);
	printf("    bInterfaceClass:    %d\n", interface->bInterfaceClass);
	printf("    bInterfaceSubClass: %d\n", interface->bInterfaceSubClass);
	printf("    bInterfaceProtocol: %d\n", interface->bInterfaceProtocol);
	printf("    iInterface:         %d\n", interface->iInterface);

	for (i = 0; i < interface->bNumEndpoints; i++)
		print_endpoint(&interface->endpoint[i]);
}

void print_interface(struct usb_interface *interface)
{
	int i;

	printf("  INTERFACE:\n");
	for (i = 0; i < interface->num_altsetting; i++)
		print_altsetting(&interface->altsetting[i]);
}

void print_configuration(struct usb_config_descriptor *config)
{
	int i;

	printf(" CONFIGURATION:\n");
	printf("  wTotalLength:         %d\n", config->wTotalLength);
	printf("  bNumInterfaces:       %d\n", config->bNumInterfaces);
	printf("  bConfigurationValue:  %d\n", config->bConfigurationValue);
	printf("  iConfiguration:       %d\n", config->iConfiguration);
	printf("  bmAttributes:         %02xh\n", config->bmAttributes);
	printf("  MaxPower:             %d\n", config->MaxPower);

	for (i = 0; i < config->bNumInterfaces; i++)
		print_interface(&config->interface[i]);
}

int print_device(UsbHandle *dev, int level)
{
	usb_dev_handle *udev;
	char description[256];
	char string[256];
	int ret, i;

	udev = usb_open(dev);
	if (udev) {
		if (dev->descriptor.iManufacturer) {
			ret = usb_get_string_simple(udev, dev->descriptor.iManufacturer, string, sizeof(string));
			if (ret > 0)
				snprintf(description, sizeof(description), "%s - ", string);
			else
				snprintf(description, sizeof(description), "%04X - ",
						 dev->descriptor.idVendor);
		} else
			snprintf(description, sizeof(description), "%04X - ",
					 dev->descriptor.idVendor);

		if (dev->descriptor.iProduct) {
			ret = usb_get_string_simple(udev, dev->descriptor.iProduct, string, sizeof(string));
			if (ret > 0)
				snprintf(description + strlen(description), sizeof(description) -
						 strlen(description), "%s", string);
			else
				snprintf(description + strlen(description), sizeof(description) -
						 strlen(description), "%04X", dev->descriptor.idProduct);
		} else
			snprintf(description + strlen(description), sizeof(description) -
					 strlen(description), "%04X", dev->descriptor.idProduct);

	} else
		snprintf(description, sizeof(description), "%04X - %04X",
				 dev->descriptor.idVendor, dev->descriptor.idProduct);

	printf("%.*sDev #%d: %s\n", level * 2, "                    ", dev->devnum,
		   description);

	if (udev && verbose) {
		if (dev->descriptor.iSerialNumber) {
			ret = usb_get_string_simple(udev, dev->descriptor.iSerialNumber, string, sizeof(string));
			if (ret > 0)
				printf("%.*s  - Serial Number: %s\n", level * 2,
					   "                    ", string);
		}
	}

	if (udev)
		usb_close(udev);

	if (verbose) {
		if (!dev->config) {
			printf("  Couldn't retrieve descriptors\n");
			return 0;
		}

		for (i = 0; i < dev->descriptor.bNumConfigurations; i++)
			print_configuration(&dev->config[i]);
	} else {
		for (i = 0; i < dev->num_children; i++)
			print_device(dev->children[i], level + 1);
	}

	return 0;
}
*/
