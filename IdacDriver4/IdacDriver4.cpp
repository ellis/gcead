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

#include "IdacDriver4.h"

#include <iostream>

#include <usb.h>

#ifdef Q_WS_X11
extern "C" {
#include <libusb/libusb.h>
//#undef _GNU_SOURCE // Supress a warning
//#include <libusb/libusbi.h>
#include <usbi.h>
}
#endif

#include <QtDebug>
#include <QFile>
#include <QMutex>
#include <QThread>

#include <Check.h>

#include <IdacDriver/IdacDriverSamplingThread.h>
#include <IdacDriver/Sleeper.h>

#include "IdacDriver4Constants.h"
#include "IdacDriver4Channel.h"
#include "IdacDriver4ReqIds.h"

using namespace std;


// IDAC4 addresses
#define		ADDRESS_ANALOG_OFF_MSB(chan)		(chan-1) * 4
#define		ADDRESS_ANALOG_OFF_LSB(chan)		(chan-1) * 4 + 1
#define		ADDRESS_ANALOG_DEC_MSB(chan)		(chan-1) * 4 + 2
#define		ADDRESS_ANALOG_DEC_LSB(chan)		(chan-1) * 4 + 3
#define		ADDRESS_ANALOG_IN(chan)			(chan-1) + 128
#define		ADDRESS_DIGITAL_DEC_MSB				0x40
#define		ADDRESS_DIGITAL_DEC_LSB				0x41
#define		ADDRESS_NETFREQ_CLOCK				0xA2

#define BOXSTRINGLENGTH 32

static bool IdacBoxString[IdacDriver4::IDAC_CHANNELCOUNT][BOXSTRINGLENGTH];

// Bit positions for box string (IDACINT.H)
IdacDriver4::BitPosition IdacDriver4::bpIdacBox[BI_COUNT] =
{
	// BI_ADC_ZERO
	{ 0, 8, true },
	// BI_NOTCH_ADJUST
	{ 8, 8, true },
	// BI_AUDIO_SWITCH
	{ 16, 1, false },
	// BI_LOWPASS_ADJUST
	{ 17, 4, true },
	// BI_SCALE_ADJUST
	{ 21, 3, true },
	// BI_HIGHPASS_ADJUST
	{ 24, 5, true },
	// BI_SETZERO_SWITCH
	{ 29, 1, false },
	// BI_EAG_SWITCH
	{ 30, 1, false },
	// BI_NOTCH_SWITCH
	{ 31, 1, false }
};

extern INTEL_HEX_RECORD g_firmwareIdacDriver4[];

#define		MAX_INPUT_VOLTAGE_ADC_V1	1000000		// full scale is 1V   ( 2Vpp)
#define		MAX_INPUT_VOLTAGE_ADC_V2	5500000		// full scale is 5.5V (11Vpp)

int dwRangesList_V1[IDAC_SCALERANGECOUNT+1] = {	MAX_INPUT_VOLTAGE_ADC_V1 / 1,
													MAX_INPUT_VOLTAGE_ADC_V1 / 4,
													MAX_INPUT_VOLTAGE_ADC_V1 / 16,
													MAX_INPUT_VOLTAGE_ADC_V1 / 64,
													MAX_INPUT_VOLTAGE_ADC_V1 / 256,
													MAX_INPUT_VOLTAGE_ADC_V1 / 1024,
													-1 };

int dwRangesList_V2[IDAC_SCALERANGECOUNT+1] = {	MAX_INPUT_VOLTAGE_ADC_V2 / 1,
													MAX_INPUT_VOLTAGE_ADC_V2 / 4,
													MAX_INPUT_VOLTAGE_ADC_V2 / 16,
													MAX_INPUT_VOLTAGE_ADC_V2 / 64,
													MAX_INPUT_VOLTAGE_ADC_V2 / 256,
													MAX_INPUT_VOLTAGE_ADC_V2 / 1024,
													-1 };


IdacDriver4::IdacDriver4(struct usb_device* device, QObject* parent)
	: IdacDriverUsb(device, parent)
{
	m_bPowerOn = false;
	m_bFpgaProgrammed = false;

	m_bSampling = false;

	setHardwareName("IDAC4");

	loadDefaultChannelSettings(actualSettings());
}

IdacDriver4::~IdacDriver4()
{
	if (handle() != NULL)
	{
		power(false);
	}
}

void IdacDriver4::loadCaps(IdacCaps* caps)
{
	caps->bHighcut = true;
	caps->bRangePerChannel = true;
}

void IdacDriver4::loadDefaultChannelSettings(IdacChannelSettings* channels)
{
	channels[0].mEnabled = 0x03;
	channels[0].mInvert = 0x00;
	channels[0].nDecimation = 960; // 100 samples per second

	channels[1].mEnabled = 1;
	channels[1].mInvert = 0;
	channels[1].nDecimation = 960; // 100 samples per second
	channels[1].iRange = 3;
	channels[1].iHighcut = 10; // 3kHz on IDAC4
	channels[1].iLowcut = 1; // 0.1 Hz on IDAC4
	channels[1].nExternalAmplification = 10;

	channels[2].mEnabled = 1;
	channels[2].mInvert = 0;
	channels[2].nDecimation = 960; // 100 samples per second
	channels[2].iRange = 4;
	channels[2].iHighcut = 10; // 3kHz on IDAC4
	channels[2].iLowcut = 1; // 0.1 Hz on IDAC4
	channels[2].nExternalAmplification = 1;
}

bool IdacDriver4::checkUsbFirmwareReady()
{
	CHECK_PRECOND_RETVAL(device() != NULL, false);

	bool b = false;
	if (device()->config[0].bNumInterfaces == 1)
		if (device()->config[0].interface[0].num_altsetting == 1)
			b = true;

	//interface[0].altsetting[0].bNumEndpoints != 3
	return b;
}

bool IdacDriver4::checkDataFirmwareReady()
{
	return m_bFpgaProgrammed;
}

void IdacDriver4::initUsbFirmware()
{
	CHECK_PRECOND_RET(handle() != NULL);

	int res;

	struct usb_device* dev = device();
	int idConfiguration = dev->config[0].bConfigurationValue;
	res = usb_set_configuration(handle(), idConfiguration);
	CHECK_USBRESULT_RET(res);

	int idInterface = dev->config[0].interface[0].altsetting[0].bInterfaceNumber;
	res = usb_claim_interface(handle(), idInterface);
	CHECK_USBRESULT_RET(res);

	usb_interface_descriptor* setting = &dev->config[0].interface[0].altsetting[0];
	res = usb_set_altinterface(handle(), setting->bAlternateSetting);
	CHECK_USBRESULT_RET(res);

	sendFirmware(g_firmwareIdacDriver4);
}

void IdacDriver4::initDataFirmware()
{
	QString sFilename = "idc4fpga.hex";

	if (!claim())
		return;

	//Power up
	power(true);
	Sleeper::msleep(1000);

	// Program FPGA
	for (int nRetries = 6; nRetries > 0; nRetries--)
	{
		// Program FPGA
		sendOutgoingMessage(REQUESTID_PROGRAM_FPGA_START);
		bool bResult = sendHexFile(sFilename);
		sendOutgoingMessage(REQUESTID_PROGRAM_FPGA_STOP);

		if (!bResult)
			break;				// Problem sending FPGA file

		fetchDeviceStatus();
		if (m_bFpgaProgrammed)
			break;
	}

	// if, after 6 retries, the FPGA still isn't booted
	if (!m_bFpgaProgrammed)
	{
		power(false);
		return;
	}

	// Reset FPGA
	resetFpga();

	/*
	// resets analog and digital outputs
	IdacStopOutput();
	IdacFlushOutput();
	IdacStartOutput();
	Sleep(200);
	IdacStopOutput();
	// enable channels and set decimation value
	int iChan;

	for (iChan = 1; iChan < IDAC_CHANNELCOUNT; iChan++)
	{
		if (bIdacChannelEnabled[iChan])
			UpdateFPGAReg(iChan, FR_DECIMATE_ADJUST, wDecimations [iChan]);
	}
	*/

	// the switching of the netfrequency is necessary to discharge the netfrequency line
	enableNetFrequency(true);		// enable netfrequency

	fetchConfig();
	initStringsAndRanges();

	for (int iChan = 1; iChan < 3; iChan++)
	{
		// Adc-Zero
		IdacSetAdcZero(iChan, m_config.adcZeroAdjust[iChan-1]);
		// Input-Zero
		setChannelOffsetAnalogIn(iChan, 0);
		// Notch frequency
		IdacTuneNotch(iChan, m_config.notchAdjust[iChan-1]);
	}

	Sleeper::msleep(100);
	enableNetFrequency(false);	// disable netfrequency
}

void IdacDriver4::initStringsAndRanges()
{
	setHighcutStrings(QStringList() <<
		" 10Hz" <<
		" 20Hz" <<
		" 30Hz" <<
		" 50Hz" <<
		"100Hz" <<
		"200Hz" <<
		"300Hz" <<
		"500Hz" <<
		" 1kHz" <<
		" 2kHz" <<
		" 3kHz" <<
		" 5kHz" <<
		"10kHz" <<
		"20kHz" <<
		"30kHz" <<
		"50kHz"
	 );

	setLowcutStrings(QStringList() <<
		"   DC" <<
		"0.1Hz" <<
		"0.2Hz" <<
		"0.3Hz" <<
		"0.5Hz" <<
		"  1Hz" <<
		"  2Hz" <<
		"  3Hz" <<
		"  5Hz" <<
		" 10Hz" <<
		" 20Hz" <<
		" 30Hz" <<
		" 50Hz" <<
		"100Hz" <<
		"200Hz" <<
		"300Hz" <<
		"500Hz"
	);

	QList<int> list;
	int* ranges;
	if (m_nVersion == 2)
		ranges = dwRangesList_V2;
	else
		ranges = dwRangesList_V1;
	while (*ranges != -1)
		list << *ranges++;
	setRanges(list);
}

bool IdacDriver4::claim()
{
	int res;

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

	for (int iPipe = 0; iPipe < setting->bNumEndpoints; iPipe++)
	{
		int idPipe = setting->endpoint[iPipe].bEndpointAddress;
		res = usb_clear_halt(handle(), idPipe);
		CHECK_USBRESULT_RETVAL(res, false);
	}

	return true;
}

void IdacDriver4::configureChannel(int iChan)
{
	const IdacChannelSettings* chan = desiredChannelSettings(iChan);
	CHECK_ASSERT_RET(chan != NULL);

	setChannelLowPass(iChan, chan->iHighcut);
	setChannelHighPass(iChan, chan->iLowcut);
	setChannelOffsetAnalogIn(iChan, chan->nOffset);
}

bool IdacDriver4::startSampling()
{
	for (int iChan = 0; iChan < 3; iChan++)
	{
		const IdacChannelSettings* chan = desiredChannelSettings(iChan);
		CHECK_ASSERT_RETVAL(chan != NULL, false);

		setChannelEnabled(iChan, chan->mEnabled);
		if (iChan != 0)
		{
			setChannelRange(iChan, chan->iRange);
			setChannelLowPass(iChan, chan->iHighcut);
			setChannelHighPass(iChan, chan->iLowcut);
			setChannelOffsetAnalogIn(iChan, chan->nOffset);
		}
	}

#ifndef Q_WS_MAC
	sampleStart();
#endif
	return true;
}

#define ISOCHRONOUS_CONTEXT_COUNT 8

static IdacDriverSamplingThread* g_recordThread;
static char* isobuf;
static IdacDriver4Channel IdacChannelState(true); // Digital inputs are inverted
static CDD32_STATUS cdStatus;
static int g_nSamplesInBuffer;
const int g_nSampleMax = 300;
static short g_samplesDigital[g_nSampleMax];
static short g_samplesAnalog1[g_nSampleMax];
static short g_samplesAnalog2[g_nSampleMax];
static int g_iSampleRead;
static int g_iSampleWrite;
static QMutex g_sampleMutex;

#ifdef WIN32
static void* isourb[ISOCHRONOUS_CONTEXT_COUNT];
#else
static libusb_transfer* iso_transfer[ISOCHRONOUS_CONTEXT_COUNT];
static bool g_abIsoTransferDone[ISOCHRONOUS_CONTEXT_COUNT];


static void iso_transfer_cb(struct libusb_transfer *transfer)
{
        int iTransfer = (int) (size_t) transfer->user_data;
	g_abIsoTransferDone[iTransfer] = true;
	//qDebug() << "iso_transfer_cb" << iTransfer;
}

/// @returns libusb error code
static int wait_for_iso_transfer(libusb_transfer* transfer)
{
	int r = 0;
        int iTransfer = (int) (size_t) transfer->user_data;

	while (!g_abIsoTransferDone[iTransfer])
	{
		r = libusb_handle_events(NULL);
		if (r < 0) {
			if (r == LIBUSB_ERROR_INTERRUPTED)
				continue;
			libusb_cancel_transfer(transfer);
			while (!g_abIsoTransferDone[iTransfer])
				if (libusb_handle_events(NULL) < 0)
					break;
			return r;
		}
	}

	switch (transfer->status) {
	case LIBUSB_TRANSFER_COMPLETED:
		r = 0;
		break;
	case LIBUSB_TRANSFER_TIMED_OUT:
		r = LIBUSB_ERROR_TIMEOUT;
		break;
	case LIBUSB_TRANSFER_STALL:
		r = LIBUSB_ERROR_PIPE;
		break;
	case LIBUSB_TRANSFER_OVERFLOW:
		r = LIBUSB_ERROR_OVERFLOW;
		break;
	case LIBUSB_TRANSFER_NO_DEVICE:
		r = LIBUSB_ERROR_NO_DEVICE;
		break;
	default:
		//usbi_warn(HANDLE_CTX(dev_handle),
		//	"unrecognised status code %d", transfer->status);
		r = LIBUSB_ERROR_OTHER;
	}

	return r;
}
#endif

void IdacDriver4::sampleInit()
{
	int nBufSize = 4800 * ISOCHRONOUS_CONTEXT_COUNT;
	int pipeId = device()->config[0].interface[0].altsetting[0].endpoint[1].bEndpointAddress;

	isobuf = new char[nBufSize];
	for (int i = 0; i < nBufSize; i++)
		isobuf[i] = -1;

	// Setup
	for (int i = 0; i < ISOCHRONOUS_CONTEXT_COUNT; i++)
	{
#ifdef WIN32
		isourb[i] = NULL;
		int ret = usb_isochronous_setup_async(handle(), &isourb[i], pipeId, 600);
		CHECK_USBRESULT_NORET(ret);
		if (ret < 0)
			printf("isochronous setup returned %d\n", ret);
#else
		libusb_transfer* transfer = libusb_alloc_transfer(ISOCHRONOUS_CONTEXT_COUNT);
		iso_transfer[i] = transfer;
		libusb_fill_iso_transfer(
			transfer,                    // transfer
			handle()->handle,                        // dev_handle
			pipeId,    // endpoint
			(unsigned char*) isobuf + 4800 * i,                // buffer
			4800,        // length
			8,                            // num_iso_packets
			iso_transfer_cb,                        // callback
			(void*) i,                        // user_data
			5000);                       // timeout
		libusb_set_iso_packet_lengths(transfer, 600);
#endif
	}
}

void IdacDriver4::sampleStart()
{
	CHECK_PRECOND_RET(g_recordThread == NULL);

	if (isobuf == NULL)
		sampleInit();

	// Reset error flags
	cdStatus = NULL_CDD32_STATUS;
	// Reset channel state machine
	IdacChannelState.Reset(MAX_SYNC_WORD_PER_SECOND);

	m_bSampling = true;
	g_iSampleRead = 0;
	g_iSampleWrite = 0;
	g_nSamplesInBuffer = 0;
	g_recordThread = new IdacDriverSamplingThread(this);
	g_recordThread->start(QThread::TimeCriticalPriority);

#ifndef WIN32
	//g_isoQueue.clear();
#endif
}

/// @returns libusb error code
static int iso_submit(int iTransfer)
{
	int ret = 0;
#ifdef WIN32
	ret = usb_submit_async(isourb[iTransfer], isobuf + 4800 * iTransfer, 4800);
#else
	g_abIsoTransferDone[iTransfer] = false;
	ret = libusb_submit_transfer(iso_transfer[iTransfer]);
#endif
	return ret;
}

/// @returns if no error, # of bytes recevied; otherwise libusb error code
static int iso_reap(int iTransfer)
{
	int ret = 0;
#ifdef WIN32
	ret = usb_reap_async(isourb[iTransfer], 5000);
#else
	libusb_transfer* transfer = iso_transfer[iTransfer];
	ret = wait_for_iso_transfer(transfer);
	if (ret >= 0) {
		int nBytesReceived = 0;
		for (int iPacket = 0; iPacket < transfer->num_iso_packets; iPacket++)
			nBytesReceived += transfer->iso_packet_desc[iPacket].actual_length;
		ret = nBytesReceived;
	}
#endif
	return ret;
}

void IdacDriver4::sampleLoop()
{
	int ret;

	int iReap = 0;
	bool bRepeat = false;
	do {
		bool abSubmitted[ISOCHRONOUS_CONTEXT_COUNT];

		// Initial submits
		for (int i = 0; i < ISOCHRONOUS_CONTEXT_COUNT; i++)
		{
			ret = iso_submit(i);
			CHECK_USBRESULT_NORET(ret);
			if (ret < 0)
				printf("isochronous submit returned %d\n", ret);
			abSubmitted[i] = true;
		}

		setIsoXferEnabled(true);

		// Now cycle between reaps and submits
		bool bError = false;
		bool bOverflow = false;

		do
		{
			for (int i = 0; i < ISOCHRONOUS_CONTEXT_COUNT; i++)
			{
				CHECK_ASSERT_NORET(abSubmitted[i] == true);
				ret = iso_reap(i);
				CHECK_USBRESULT_NORET(ret);
				iReap++;
				abSubmitted[i] = false;

				if (ret < 0)
				{
					cout << "Reap " << (iReap - 1) << ", Buffer " << i << ": ISOCHRONOUS READ ret = " << ret << " " << usb_strerror() << endl;
					bError = true;
					break;
				}

				bOverflow = processSampledData(i, ret);
				if (bOverflow)
					break;

				ret = iso_submit(i);
				CHECK_USBRESULT_NORET(ret);
				if (ret < 0) {
					printf("isochronous submit returned %d\n", ret);
					bError = true;
					break;
				}
				abSubmitted[i] = true;
			}

			for (int i = 0; i < ISOCHRONOUS_CONTEXT_COUNT; i++)
			{
				if (abSubmitted[i])
					iso_reap(i);
			}
		} while (m_bSampling && !bError && !bOverflow);

		setIsoXferEnabled(false);
		bRepeat = m_bSampling;
	} while (bRepeat);

	// TODO: Consider restarting if bOverflow == true && m_bSampling == true -- ellis, 2010-01-24
}

bool IdacDriver4::processSampledData(int iTransfer, int nBytesReceived) {
	CHECK_PARAM_RETVAL(iTransfer >= 0 && iTransfer < ISOCHRONOUS_CONTEXT_COUNT, false);
	if (nBytesReceived < 0 || nBytesReceived > 4800) {
		qDebug() << "ERROR: nBytesReceived =" << nBytesReceived;
	}
	CHECK_PARAM_RETVAL(nBytesReceived >= 0 && nBytesReceived <= 4800, false);

	bool bOverflow = false;

	int nPackets = (nBytesReceived > 0) ? (nBytesReceived / 600) : 0;

	// FIXME: this variable is for debug only -- ellis, 2010-01-24
	quint16* pEnd = (quint16*) (isobuf + 4800 * ISOCHRONOUS_CONTEXT_COUNT);
	for (int iPacket = 0; iPacket < nPackets; iPacket++)
	{
		quint16* pBuffer = (quint16*) (isobuf + 4800 * iTransfer + 600 * iPacket);
		int nBytes = *pBuffer++;
		if (nBytes < 0 || nBytes > 600) {
			qDebug() << "ERROR: nBytes =" << nBytes;
		}
		CHECK_ASSERT_RETVAL(nBytes >= 0, bOverflow);
		CHECK_ASSERT_RETVAL(nBytes <= 600, bOverflow);
		int nWords = nBytes / 2;
		for (int iWord = 0; iWord < nWords; iWord++)
		{
			// FIXME: for debug only -- ellis, 2010-01-24
			if (pBuffer == pEnd) {
				qDebug() << "processSampledData: iTransfer:" << iTransfer << "iPacket:" << iPacket << "nWords:" << nWords << "iWord:" << iWord;
			}
			// Read little-endian word
			quint16 wData = *pBuffer++;

			CDD32_SAMPLE cds;
			bool bParsed = IdacChannelState.ParseSample(wData, cds, cdStatus, actualSettings());

			//printf("i:%d\t%d\t%d\t%d\t%x\n", iPacket, iWord, cds.uChannel, (int) bParsed, wData);
			if (bParsed)
			{
				// A sample was produced
				if (g_nSamplesInBuffer < g_nSampleMax - 1)
				{
					if (cds.uChannel == 0)
						g_samplesDigital[g_iSampleWrite] = cds.nSample;
					else if (cds.uChannel == 1)
						g_samplesAnalog1[g_iSampleWrite] = cds.nSample;
					else if (cds.uChannel == 2)
						g_samplesAnalog2[g_iSampleWrite] = cds.nSample;

					if (cds.uChannel == 2)
					{
						g_iSampleWrite++;
						if (g_iSampleWrite == g_nSampleMax)
							g_iSampleWrite = 0;
						g_sampleMutex.lock();
						g_nSamplesInBuffer++;
						g_sampleMutex.unlock();
					}
				}
				else
				{
					bOverflow = true;
					m_bSampling = false;
					printf("OVERFLOW\n");
					break;
				}
				/*
				if (cds.uChannel == 1)// && !bPrinted)
				{
					//printf("D:%d\n", (int) cds.nSample);
					bPrinted = true;
				}
				*/
			}
		}
	}

	return bOverflow;
}

void IdacDriver4::stopSampling()
{
	CHECK_PRECOND_RET(g_recordThread != NULL);

	m_bSampling = false;
	if (g_recordThread->wait(5000))
	{
		delete g_recordThread;
		g_recordThread = NULL;
	}
}

int IdacDriver4::takeData(short* digital, short* analog1, short* analog2, int maxSize)
{
	int size = 0;
	while (size < maxSize && g_nSamplesInBuffer > 0)
	{
		size++;
		*digital++ = g_samplesDigital[g_iSampleRead];
		*analog1++ = g_samplesAnalog1[g_iSampleRead];
		*analog2++ = g_samplesAnalog2[g_iSampleRead];
		g_iSampleRead++;
		if (g_iSampleRead == g_nSampleMax)
			g_iSampleRead = 0;

		g_sampleMutex.lock();
		g_nSamplesInBuffer--;
		g_sampleMutex.unlock();
	}
	return size;
}

bool IdacDriver4::power(bool bOn)
{
	return sendOutgoingMessage(bOn ? REQUESTID_POWER_ON : REQUESTID_POWER_OFF);
}

bool IdacDriver4::resetFpga()
{
	return sendOutgoingMessage(REQUESTID_RESET_FPGA);
}

bool IdacDriver4::enableNetFrequency(bool bOn)
{
	quint8 buffer[2];
	buffer[0] = (bOn) ? 1 : 0;
	buffer[1] = ADDRESS_NETFREQ_CLOCK;
	return sendOutgoingMessage(REQUESTID_WRITE_FPGA_REG, buffer, sizeof(buffer));
}

bool IdacDriver4::fetchDeviceStatus()
{
	quint8 buffer[3];
	bool b = sendIncomingMessage(REQUESTID_GET_DEVICESTATUS, buffer, sizeof(buffer));

	// FIXME: I don't know the actual order of data in the device status structure.
	m_bPowerOn = buffer[0];
	m_bFpgaProgrammed = buffer[1];

	return b;
}

bool IdacDriver4::fetchConfig()
{
	bool b;
	b = sendIncomingMessage(REQUESTID_GET_VERSION, &m_nVersion, 1);
	if (!b)
		return false;

	quint8 buffer[sizeof(m_config) + 10]; // +10 for sizeof NTSTATUS // = 28
	int size = sizeof(buffer); // +10 for sizeof NTSTATUS // = 28
	b = sendIncomingMessage(REQUESTID_READ_CONFIG_DATA, buffer, size);
	if (!b)
		return false;

	memcpy(&m_config, buffer, sizeof(m_config));

	return true;
}

bool IdacDriver4::IdacSetAdcZero(int iChan, int iOffset)
{
	CHECK_PARAM_RETVAL(iChan >= 1 && iChan < IDAC_CHANNELCOUNT, false);
	CHECK_PARAM_RETVAL(IS_UINT8(iOffset), false);

	bool b = UpdateAnalogIn(iChan, BI_ADC_ZERO_ADJUST, iOffset);
	if (!b)
		return false;

	// Store in the config data
	m_config.adcZeroAdjust[iChan - 1] = iOffset;

	return true;
}

bool IdacDriver4::IdacTuneNotch(int iChan, quint8 Offset)
{
	if ((iChan >= IDAC_CHANNELCOUNT) || (iChan < 1))
		return false;

	UpdateAnalogIn(iChan, BI_NOTCH_ADJUST, Offset);

	m_config.notchAdjust[iChan-1] = Offset;		// Store in the config data

	return true;
}

bool IdacDriver4::setChannelLowPass(int iChan, int index)
{
	CHECK_PARAM_RETVAL(iChan >= 1 && iChan < IDAC_CHANNELCOUNT, false);
	CHECK_PARAM_RETVAL(index >= 0 && index < IDAC_LOWPASSCOUNT, false);
	actualChannelSettings(iChan)->iHighcut = index;
	return UpdateAnalogIn(iChan, BI_LOWPASS_ADJUST, (quint32) index);
}

static const quint32 HighPassTable[IDAC_HIGHPASSCOUNT] = {0, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

bool IdacDriver4::setChannelHighPass(int iChan, int index)
{
	CHECK_PARAM_RETVAL(iChan >= 1 && iChan < IDAC_CHANNELCOUNT, false);
	CHECK_PARAM_RETVAL(index >= 0 && index < IDAC_HIGHPASSCOUNT, false);
	actualChannelSettings(iChan)->iLowcut = index;
	quint32 n = HighPassTable[index];
	return UpdateAnalogIn(iChan, BI_HIGHPASS_ADJUST, n);
}

/************************************/
/* Offset1 function					*/
/* Offset range (-32000..+32000)	*/
/************************************/
// new in idac2000!!!!!!!!!!!!
bool IdacDriver4::setChannelOffsetAnalogIn (int iChan, quint32 Offset)
{
	CHECK_PARAM_RETVAL(iChan >= 1 && iChan < IDAC_CHANNELCOUNT, false);

	long CorOffset = m_config.inputZeroAdjust[iChan - 1] + (long)Offset;

	// Clip the level within the short range

	bool bRc = true;
	if (CorOffset > SHRT_MAX)
	{
		CorOffset = SHRT_MAX;
		bRc = false;
	}
	if (CorOffset < SHRT_MIN)
	{
		CorOffset = SHRT_MIN;
		bRc = false;
	}

	quint8 buffer[4];
	buffer[0] = CorOffset >> 8;
	buffer[1] = (quint8) ADDRESS_ANALOG_OFF_MSB(iChan);
	buffer[2] = CorOffset & 0xFF;
	buffer[3] = (quint8) ADDRESS_ANALOG_OFF_LSB(iChan);

	bool b = sendOutgoingMessage(REQUESTID_WRITE_FPGA_REG, buffer, sizeof(buffer));
	if (b)
		actualChannelSettings(iChan)->nOffset = CorOffset;
	bRc &= b;

	return bRc;
}

// Updates analog input stage of specific channel
bool IdacDriver4::UpdateAnalogIn(int iChan, IdacDriver4::BOXINDEX Bi, quint32 nValue)
{
	// edit bittrain string
	SetBoxBits(iChan, Bi, nValue);

	// send bittrain
	return OutputBoxBits(iChan);
}

void IdacDriver4::SetBoxBits(int iChan, IdacDriver4::BOXINDEX Bi, quint32 nValue)
{
	// Lookup entry in box-bit table
	BitPosition* pBP = &(bpIdacBox[Bi]);

	if (pBP->bReverse)
	{
		// Lsb last
		for (qint32 nPosition = pBP->Position + pBP->Length - 1; nPosition >= pBP->Position; nPosition--)
		{
			IdacBoxString[iChan][nPosition] = nValue & 1;
			nValue = nValue >> 1;
		}
	}
	else
	{
		// Lsb first
		qint32 nEndPosition = pBP->Position + pBP->Length;
		for (qint32 nPosition = pBP->Position; nPosition < nEndPosition; nPosition++)
		{
			IdacBoxString[iChan][nPosition] = nValue & 1;
			nValue = nValue >> 1;
		}
	}
}

// Set box string to defaults (IDACINT.H)
void IdacDriver4::ResetBoxString ()
{
	memset(IdacBoxString, 1, sizeof(IdacBoxString));

	for (int iChan = 1; iChan < IDAC_CHANNELCOUNT; iChan++)
	{
		SetBoxBits(iChan, BI_ADC_ZERO_ADJUST,		127);	// 127=>5kOhm: midrange
		SetBoxBits(iChan, BI_NOTCH_ADJUST,			127);	// 127=>5kOhm: midrange
		SetBoxBits(iChan, BI_AUDIO_SWITCH,			0);		// Audio off
		SetBoxBits(iChan, BI_LOWPASS_ADJUST,			15);	// low pass at 50kHz
		SetBoxBits(iChan, BI_SCALE_ADJUST,			0);		// 1 time amplified
		SetBoxBits(iChan, BI_HIGHPASS_ADJUST,		0);		// high pass at DC
		SetBoxBits(iChan, BI_SETZERO_SWITCH,			0);		// 1=>pulled down to 0V; 0=>analog in
		SetBoxBits(iChan, BI_EAG_SWITCH,				0);		// eag disabled
		SetBoxBits(iChan, BI_NOTCH_SWITCH,			0);		// Notchfilter disabled

	}

	for (int iChan = 1; iChan < IDAC_CHANNELCOUNT; iChan++)
		OutputBoxBits(iChan);
}

// Send box string to IDAC (IDACINT.H)
bool IdacDriver4::OutputBoxBits (int iChan)
{
#define BUFFERLENGTH_ANALOG_IN	5

	quint8 DataBuffer[BUFFERLENGTH_ANALOG_IN] = {0,0,0,0,0};

	DataBuffer[0] = (quint8)ADDRESS_ANALOG_IN(iChan);

	for (quint32 uBit = 0; uBit < BOXSTRINGLENGTH; uBit++)
	{
		quint32 ByteIdx = uBit / 8;
		quint32 BitIdx = 7 - (uBit % 8);

		if (IdacBoxString [iChan] [uBit])
		{
			DataBuffer[ByteIdx + 1] |= 1<<BitIdx;
		}
	}

	return sendOutgoingMessage(REQUESTID_WRITE_ANALOG_IN, DataBuffer, BUFFERLENGTH_ANALOG_IN);
}

bool IdacDriver4::setChannelEnabled(int iChan, bool bEnabled)
{
	// Disabled channels will set 0 as their decimation value
	int nDecimation = 0;
	if (bEnabled)
		nDecimation = actualChannelSettings(iChan)->nDecimation;

	bool b = setChannelDecimation(iChan, nDecimation, bEnabled);
	if (b)
		actualChannelSettings(iChan)->mEnabled = bEnabled;
	return b;
}

// Turn sample generation on/off
bool IdacDriver4::setSamplingEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		// Clear data buffers
		/* FIXME:
		ChannelBuffer.LockWrite();
		ChannelBuffer.LockRead();
		ChannelBuffer.Reset();
		ChannelBuffer.UnlockRead();
		ChannelBuffer.UnlockWrite();
		*/

		// Reset error flags
		// FIXME: cdStatus = NULL_CDD32_STATUS;

		// Reset channel state machine
		IdacChannelState.Reset(MAX_SYNC_WORD_PER_SECOND);

		// Activate USB data transfer
		setIsoXferEnabled(true);

		// Activate thread
		// FIXME: SetEvent(hSamplingEvent);

		// Announce activation
		m_bSampling = true;
		// FIXME: bIdacRecentActivation = true;
	}
	else
	{
		// Remove activations
		m_bSampling = false;
		// FIXME: bIdacRecentActivation = false;

		// Halt thread
		// FIXME: ResetEvent(hSamplingEvent);

		// Stop USB data transfer
		setIsoXferEnabled(false);

		// Allow for isochrone latency (restart may be unsafe)
		Sleeper::msleep(IDAC_BUFFER_DELAY);
	}

	return true;
}

// Sets the decimation factor for the given analog channel
bool IdacDriver4::setChannelDecimation(int iChan, int nDecimation, bool bRecordSetting)
{
	CHECK_PARAM_RETVAL(iChan >= 0 && iChan <= 2, false);
	CHECK_PARAM_RETVAL(nDecimation >= 0 && nDecimation < IDAC_DECIMATIONCOUNT, false);

	bool bBusySampling = m_bSampling;

	if (bBusySampling)
		setSamplingEnabled(false);

	CHECK_PARAM_RETVAL(iChan >= 0 && iChan <= 2, false);

	CHECK_PARAM_RETVAL(nDecimation >= 0 && nDecimation < IDAC_DECIMATIONCOUNT, false);

	quint8 buffer[4];
	buffer[0] = nDecimation >> 8;
	buffer[2] = nDecimation & 0xff;
	// Digital channel
	if (iChan == 0)
	{
		buffer[1] = ADDRESS_DIGITAL_DEC_MSB;
		buffer[3] = ADDRESS_DIGITAL_DEC_LSB;
	}
	// Analog channel
	else
	{
		buffer[1] = ADDRESS_ANALOG_DEC_MSB(iChan);
		buffer[3] = ADDRESS_ANALOG_DEC_LSB(iChan);
	}

	bool b = sendOutgoingMessage(REQUESTID_WRITE_FPGA_REG, buffer, sizeof(buffer));
	if (b && bRecordSetting)
		actualChannelSettings(iChan)->nDecimation = nDecimation;

	if (bBusySampling)
		setSamplingEnabled(true);

	return true;
}

// This function will set the range for the given channel
void IdacDriver4::setChannelRange(int iChan, int Index)
{
	if ((iChan >= IDAC_CHANNELCOUNT) || (iChan < 1) || (Index >= IDAC_SCALERANGECOUNT))
		return;

	if (UpdateAnalogIn(iChan, BI_SCALE_ADJUST, Index))
		actualChannelSettings(iChan)->iRange = Index;
}

// Activate / deactivate isochrone transfer (SUPPINT.H)
void IdacDriver4::setIsoXferEnabled(bool bEnabled)
{
	int requestId = (bEnabled) ? REQUESTID_ISO_IN_START : REQUESTID_ISO_IN_STOP;
	sendOutgoingMessage(requestId);
}
