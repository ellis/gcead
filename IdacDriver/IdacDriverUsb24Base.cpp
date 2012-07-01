#include "IdacDriverUsb24Base.h"

#include <Check.h>

#if defined(LIBUSBX)
#include <libusb-1.0/libusb.h>
#elif defined(LIBUSB0)
#include <usb.h>
#endif


IdacDriverUsb24Base::IdacDriverUsb24Base(UsbDevice* device, UsbHandle* handle, QObject* parent)
	: IdacDriverUsb(device, handle, parent)
{
	m_bFpgaProgrammed = false;
}

bool IdacDriverUsb24Base::checkUsbFirmwareReady()
{
	CHECK_PRECOND_RETVAL(handle() != NULL, false);

	bool b = false;
#if defined(LIBUSB0)
	if (device()->config[0].bNumInterfaces == 1)
		if (device()->config[0].interface[0].num_altsetting == 1)
			b = true;
#elif defined(LIBUSBX)
	libusb_device* dev = libusb_get_device(handle());
	CHECK_ASSERT_RETVAL(dev != NULL, false);
	libusb_config_descriptor* config = NULL;
	int res = libusb_get_config_descriptor(dev, 0, &config);
	CHECK_USBRESULT_RETVAL(res, false);
	if (config->bNumInterfaces == 1) {
		const libusb_interface* interface = &config->interface[0];
		b = (interface->num_altsetting == 1);
	}
#endif

	//interface[0].altsetting[0].bNumEndpoints != 3
	return b;
}

bool IdacDriverUsb24Base::checkDataFirmwareReady()
{
	return m_bFpgaProgrammed;
}

bool IdacDriverUsb24Base::claim(bool bUnhalt)
{
	CHECK_PRECOND_RETVAL(handle() != NULL, false);

	int res;

#if defined(LIBUSB0)
	struct usb_device* dev = device();
	int idConfiguration = dev->config[0].bConfigurationValue;
	res = usb_set_configuration(handle(), idConfiguration);
	CHECK_USBRESULT_RETVAL(res, false);

	int idInterface = dev->config[0].interface[0].altsetting[0].bInterfaceNumber;
	res = usb_claim_interface(handle(), idInterface);
	CHECK_USBRESULT_RETVAL(res, false);

	usb_interface_descriptor* setting = &dev->config[0].interface[0].altsetting[0];
	res = usb_set_altinterface(handle(), setting->bAlternateSetting);
	CHECK_USBRESULT_RETVAL(res, false);

	if (bUnhalt) {
		for (int iPipe = 0; iPipe < setting->bNumEndpoints; iPipe++)
		{
			int idPipe = setting->endpoint[iPipe].bEndpointAddress;
			res = usb_clear_halt(handle(), idPipe);
			CHECK_USBRESULT_RETVAL(res, false);
		}
	}
#elif defined(LIBUSBX)
	libusb_device* dev = libusb_get_device(handle());
	CHECK_ASSERT_RETVAL(dev != NULL, false);

	libusb_config_descriptor* config = NULL;
	res = libusb_get_config_descriptor(dev, 0, &config);
	CHECK_USBRESULT_RETVAL(res, false);

	// Select configuration for the first descriptor
	res = libusb_set_configuration(handle(), config->bConfigurationValue);
	CHECK_USBRESULT_RETVAL(res, false);

	CHECK_ASSERT_RETVAL(config->bNumInterfaces > 0, false);
	const libusb_interface* interface = &config->interface[0];
	CHECK_ASSERT_RETVAL(interface->num_altsetting > 0, false);
	const libusb_interface_descriptor* altsetting = &interface->altsetting[0];
	const int idInterface = altsetting->bInterfaceNumber;
	res = libusb_claim_interface(handle(), idInterface);
	CHECK_USBRESULT_RETVAL(res, false);

	res = libusb_set_interface_alt_setting(handle(), idInterface, altsetting->bAlternateSetting);
	CHECK_USBRESULT_RETVAL(res, false);

	if (bUnhalt) {
		for (int i = 0; i < altsetting->bNumEndpoints; i++)
		{
			const int idEndpoint = altsetting->endpoint[i].bEndpointAddress;
			res = libusb_clear_halt(handle(), idEndpoint);
			CHECK_USBRESULT_RETVAL(res, false);
		}
	}
#endif

	return true;
}
