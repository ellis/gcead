#include "IdacDriverUsbEs.h"

#include <Check.h>

#include <Idac/IdacFactory.h>

#include "IdacSettings.h"


IdacDriverUsbEs::IdacDriverUsbEs(IdacDriverUsb* driver, const QVector<IdacChannelSettings>& channelSettings)
{
	m_driver = driver;
	m_channelSettings = channelSettings;
	m_manager = IdacFactory::getDriverManager(true);
	m_proxy = IdacFactory::getProxy();
}

IdacChannelSettings* IdacDriverUsbEs::channelSettings(int iChan) {
	CHECK_PARAM_RETVAL(isValidChannel(iChan), NULL);
	return &m_channelSettings[iChan];
}

const IdacChannelSettings* IdacDriverUsbEs::channelSettings(int iChan) const {
	CHECK_PARAM_RETVAL(isValidChannel(iChan), NULL);
	return &m_channelSettings[iChan];
}

bool IdacDriverUsbEs::IdacEnableChannel(int iChan, bool bEnabled) {
	CHECK_PARAM_RETVAL(isValidChannel(iChan), false);
	IdacChannelSettings* chan = channelSettings(iChan);
	chan->mEnabled = bEnabled;
	m_proxy->resendChannelSettings(iChan, *chan);
	return true;
}

int IdacDriverUsbEs::IdacGetChannelDecimation(int iChan) const {
	CHECK_PARAM_RETVAL(isValidChannel(iChan), 0);
	return channelSettings(iChan)->nDecimation;
}

int IdacDriverUsbEs::IdacGetDSPInfo() const {
	if (m_manager->state() == IdacState_Sampling)
		return 0;
	return 1;
}

bool IdacDriverUsbEs::IdacHighPass(int iChan, int index) {
	CHECK_PARAM_RETVAL(isValidAudioChannel(iChan), false);
	CHECK_PARAM_RETVAL(index >= 0 && index < IdacHighPassStrings().size(), false);

	IdacChannelSettings& chan = *channelSettings(iChan);
	chan.iLowcut = index;
	m_proxy->resendChannelSettings(iChan, chan);
	return true;
}

bool IdacDriverUsbEs::IdacIsChannelEnabled(int iChan) const {
	CHECK_PARAM_RETVAL(isValidChannel(iChan), false);
	return channelSettings(iChan)->mEnabled;
}

QString IdacDriverUsbEs::IdacLock(const QString& /*sUserName*/) {
	return QString(); // NOTE: locking is not implemented -- ellis, 2011-08-07
}

bool IdacDriverUsbEs::IdacLowPass(int iChan, int index) {
	CHECK_PARAM_RETVAL(isValidAudioChannel(iChan), false);
	CHECK_PARAM_RETVAL(index >= 0 && index < IdacLowPassStrings().size(), false);

	IdacChannelSettings& chan = *channelSettings(iChan);
	chan.iHighcut = index;
	m_proxy->resendChannelSettings(iChan, chan);
	return true;
}

int IdacDriverUsbEs::IdacNrOfAnChannelEnabled() const {
	int n = 0;
	for (int iChan = 0; iChan < channelCount(); iChan++) {
		if (isValidAudioChannel(iChan) && channelSettings(iChan)->mEnabled > 0)
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
	CHECK_PARAM_RETVAL(isValidAudioChannel(iChan), NULL);
	CHECK_PARAM_RETVAL(index >= 0 && index < m_driver->ranges().size(), false);

	IdacChannelSettings& chan = *channelSettings(iChan);
	chan.iRange = index;
	m_proxy->resendChannelSettings(iChan, chan);
	return true;
}

void IdacDriverUsbEs::IdacSetBufferEvent(void* /*hEvent*/) { return; } // FIXME: implement -- ellis, 2010-06-13

bool IdacDriverUsbEs::IdacSetDecimation(int iChan, int nDecimation) {
	CHECK_PARAM_RETVAL(isValidChannel(iChan), false);
	CHECK_PARAM_RETVAL(nDecimation >= 0 && nDecimation < decimationCount(), false);

	IdacChannelSettings& chan = *channelSettings(iChan);
	chan.nDecimation = nDecimation;
	m_proxy->resendChannelSettings(iChan, chan);
	return true;
}

bool IdacDriverUsbEs::IdacSetOffsetAnalogIn(int iChan, int nOffset) {
	CHECK_PARAM_RETVAL(isValidAudioChannel(iChan), false);
	CHECK_PARAM_RETVAL(nOffset >= SHRT_MIN && nOffset <= SHRT_MAX, false);

	IdacChannelSettings& chan = *channelSettings(iChan);
	chan.nOffset = nOffset;
	m_proxy->resendChannelSettings(iChan, chan);
	return true;
}

bool IdacDriverUsbEs::IdacSmpStart() {
	IdacState state = m_manager->state();
	CHECK_PRECOND_RETVAL(state != IdacState_Sampling, false);
	CHECK_PRECOND_RETVAL(state >= IdacState_Ready, false);

	m_proxy->startSampling(m_channelSettings);
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
	delete m_manager;
	delete m_proxy;
	delete m_driver;
	m_manager = NULL;
	m_proxy = NULL;
	m_driver = NULL;
	// FIXME: after this point, calls to any other member methods will crash -- ellis, 2011-08-07
}
