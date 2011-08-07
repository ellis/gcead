#include "IdacDriverUsbEs.h"

#include <Idac/IdacFactory.h>

#include "IdacSettings.h"


IdacDriverUsbEs::IdacDriverUsbEs(IdacDriverUsb* driver) :
	m_driver(driver),
	m_manager(IdacFactory::getDriverManager(true)),
	m_proxy(IdacFactory::getProxy())
{
}

bool IdacDriverUsbEs::IdacEnableChannel(int iChan, bool bEnabled) {
	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	chan.mEnabled = bEnabled;
	m_proxy->resendChannelSettings(iChan, chan);
}

int IdacDriverUsbEs::IdacGetDSPInfo(uchar*, int, int) const {
	if (m_manager->state() == IdacState_Sampling)
		return 0;
	return 1;
}

QString IdacDriverUsbEs::IdacLock(const QString& /*sUserName*/) {
	return QString(); // NOTE: locking is not implemented -- ellis, 2011-08-07
}

int IdacDriverUsbEs::IdacNrOfAnChannelEnabled() const {
	int n = 0;
	IdacSettings* settings = Globals->idacSettings();
	for (int iChan = 1; iChan < 3; iChan++) {
		IdacChannelSettings& chan = settings->channels[iChan];
		if (chan.mEnabled > 0)
			n++;
	}
	return n;
}

bool IdacDriverUsbEs::IdacPresent(int) {
	return (m_manager->state() >= IdacState_Present);
}

bool IdacDriverUsbEs::IdacScaleRange(int iChan, int index) {
	CHECK_PARAM_RETVAL(iChan >= 1 && iChan < IDAC_CHANNELCOUNT, false);
	CHECK_PARAM_RETVAL(index >= 0 && index < m_driver->ranges().size(), false);

	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	chan.iRange = index;
	m_proxy->resendChannelSettings(iChan, chan);
	return true;
}

void IdacDriverUsbEs::IdacSetBufferEvent(int hEvent) { return; } // FIXME: implement -- ellis, 2010-06-13

bool IdacDriverUsbEs::IdacSetDecimation(int iChan, int nDecimation) {
	CHECK_PARAM_RETVAL(iChan >= 1 && iChan < IDAC_CHANNELCOUNT, false);
	CHECK_PARAM_RETVAL(index >= 0 && index < m_driver->ranges().size(), false);

	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	chan.iRange = index;
	m_proxy->resendChannelSettings(iChan, chan);
	return true;
	return false;
} // FIXME: implement -- ellis, 2010-06-13

bool IdacDriverUsbEs::IdacSetOffsetAnalogIn(int iChan, int nOffset) { return false; } // FIXME: implement -- ellis, 2010-06-13
