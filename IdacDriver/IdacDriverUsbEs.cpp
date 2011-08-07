#include "IdacDriverUsbEs.h"

#include <Check.h>
#include <Globals.h>

#include <Idac/IdacFactory.h>

#include "IdacSettings.h"


IdacDriverUsbEs::IdacDriverUsbEs(IdacDriverUsb* driver)
{
	m_driver = driver;
	m_manager = IdacFactory::getDriverManager(true);
	m_proxy = IdacFactory::getProxy();
	Globals = new GlobalVars();
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

void IdacDriverUsbEs::IdacPowerDown() {
	m_proxy->setdown();
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
	CHECK_PARAM_RETVAL(iChan >= 0 && iChan < IDAC_CHANNELCOUNT, false);
	CHECK_PARAM_RETVAL(index >= 0 && index < m_driver->ranges().size(), false);

	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	chan.nDecimation = nDecimation;
	m_proxy->resendChannelSettings(iChan, chan);
	return true;
}

bool IdacDriverUsbEs::IdacSetOffsetAnalogIn(int iChan, int nOffset) {
	CHECK_PARAM_RETVAL(iChan >= 1 && iChan < IDAC_CHANNELCOUNT, false);
	CHECK_PARAM_RETVAL(index >= 0 && index < m_driver->ranges().size(), false);

	IdacSettings* settings = Globals->idacSettings();
	IdacChannelSettings& chan = settings->channels[iChan];
	chan.nOffset = nOffset;
	m_proxy->resendChannelSettings(iChan, chan);
	return true;
}

bool IdacDriverUsbEs::IdacSmpStart() {
	IdacState state = m_manager->state();
	CHECK_PRECOND_RETVAL(state != IdacState_Sampling, false);
	CHECK_PRECOND_RETVAL(state >= IdacState_Ready, false);

	IdacSettings* settings = Globals->idacSettings();
	m_proxy->startSampling(settings->channels);
	return true;
}

bool IdacDriverUsbEs::IdacSmpStop() {
	IdacState state = m_manager->state();
	CHECK_PRECOND_RETVAL(state == IdacState_Sampling, false);
	m_proxy->stopSampling();
	return true;
}

void IdacDriverUsbEs::IdacUnlock() {
	IdacFactory::exitIdacThreads();
	delete Globals;
	delete m_manager;
	delete m_proxy;
	delete m_driver;
	m_manager = NULL;
	m_proxy = NULL;
	m_driver = NULL;
	// FIXME: after this point, calls to any other member methods will crash -- ellis, 2011-08-07
}
