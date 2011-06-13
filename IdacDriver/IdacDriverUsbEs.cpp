#include "IdacDriverUsbEs.h"

IdacDriverUsbEs::IdacDriverUsbEs(struct usb_device* device, QObject *parent) :
	IdacDriverUsb(device, parent)
{
}
