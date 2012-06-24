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

#if LIBUSBX
#include <libusb-1.0/libusb.h>
#else
#include <usb.h>
extern "C" {
//#undef _GNU_SOURCE // Supress a warning
//#include <libusb/libusbi.h>
//#include <usbi.h>
//#include "../extern/libusb-compat-0.1.4/libusb/usbi.h"
}
#endif

#include <QtDebug>
#include <QCoreApplication>
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

#define ISO_CONTEXT_COUNT 8
#define ISO_PACKETS_PER_TRANSFER 8
#define ISO_PACKET_SIZE 600
#define ISO_TRANSFER_SIZE (ISO_PACKETS_PER_TRANSFER * ISO_PACKET_SIZE)


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


IdacDriver4::IdacDriver4(UsbDevice* device, UsbHandle* handle, QObject* parent)
	: IdacDriverUsb24Base(device, handle, parent),
	  m_defaultChannelSettings(3)
{
	m_bPowerOn = false;

	m_bSampling = false;

	setHardwareName("IDAC4");

	QVector<IdacChannelSettings>& channels = m_defaultChannelSettings;

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

void IdacDriver4::initUsbFirmware()
{
	   if (claim(false)) {
			   sendFirmware(g_firmwareIdacDriver4);
	   }
}

void IdacDriver4::initDataFirmware()
{
	QString sFilename = QCoreApplication::applicationDirPath() + "/idc4fpga.hex";

	if (!claim(true))
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

//#ifndef Q_WS_MAC
	sampleStart();
//#endif
	return true;
}

static char* isobuf;
static IdacDriver4Channel IdacChannelState(true); // Digital inputs are inverted
static CDD32_STATUS cdStatus;

#ifdef LIBUSB0
static void* isourb[ISO_CONTEXT_COUNT];
#else
static libusb_transfer* iso_transfer[ISO_CONTEXT_COUNT];
static bool g_abIsoTransferDone[ISO_CONTEXT_COUNT];


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

	if (transfer->status != LIBUSB_TRANSFER_COMPLETED) {
		qDebug() << "wait_for_iso_transfer()" << "transfer->status" << transfer->status;
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

	if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
		for (int iPacket = 0; iPacket < transfer->num_iso_packets; iPacket++) {
			libusb_iso_packet_descriptor* packet = &transfer->iso_packet_desc[iPacket];
			if (packet->status != LIBUSB_TRANSFER_COMPLETED) {
				qDebug() << "wait_for_iso_transfer()" << "packet" << iPacket << "of" << transfer->num_iso_packets << "status" << packet->status;
				r = LIBUSB_ERROR_OTHER;
			}
		}
	}

	return r;
}
#endif

void IdacDriver4::sampleInit()
{
	const int nBufSize = ISO_TRANSFER_SIZE * ISO_CONTEXT_COUNT;
#if defined(LIBUSB0)
	const int pipeId = device()->config[0].interface[0].altsetting[0].endpoint[1].bEndpointAddress;
#elif defined(LIBUSBX)
	libusb_device* dev = libusb_get_device(handle());
	CHECK_ASSERT_RET(dev != NULL);
	libusb_config_descriptor* config = NULL;
	const int res0 = libusb_get_config_descriptor(dev, 0, &config);
	CHECK_USBRESULT_RET(res0);
	CHECK_ASSERT_RET(config->bNumInterfaces > 0);
	const libusb_interface* interface = &config->interface[0];
	CHECK_ASSERT_RET(interface->num_altsetting > 0);
	const libusb_interface_descriptor* altsetting = &interface->altsetting[0];
	CHECK_ASSERT_RET(altsetting->bNumEndpoints > 1);
	const int pipeId = altsetting->endpoint[1].bEndpointAddress;
#endif

	isobuf = new char[nBufSize];
	// Initialize to -1's for debugging purposes
	for (int i = 0; i < nBufSize; i++)
		isobuf[i] = -1;

	// Setup
	for (int i = 0; i < ISO_CONTEXT_COUNT; i++)
	{
#ifdef LIBUSB0
		isourb[i] = NULL;
		int ret = usb_isochronous_setup_async(handle(), &isourb[i], pipeId, ISO_PACKET_SIZE);
		CHECK_USBRESULT_NORET(ret);
#else
		libusb_transfer* transfer = libusb_alloc_transfer(ISO_PACKETS_PER_TRANSFER);
		iso_transfer[i] = transfer;
		void* user_data = (void*) (intptr_t) i;
		libusb_fill_iso_transfer(
			transfer,                    // transfer
			handle(),                        // dev_handle
			pipeId,    // endpoint
			(unsigned char*) isobuf + ISO_TRANSFER_SIZE * i,                // buffer
			ISO_TRANSFER_SIZE,        // length
			ISO_PACKETS_PER_TRANSFER,           // num_iso_packets
			iso_transfer_cb,                        // callback
			user_data,                        // user_data
			5000);                       // timeout
		libusb_set_iso_packet_lengths(transfer, ISO_PACKET_SIZE);
#endif
	}
}

void IdacDriver4::sampleStart()
{
	if (isobuf == NULL)
		sampleInit();
	// FIXME: for debug only
	for (int i = 0; i < ISO_TRANSFER_SIZE * ISO_CONTEXT_COUNT; i++)
		isobuf[i] = -1;
	// ENDFIX

	// Reset error flags
	cdStatus = NULL_CDD32_STATUS;
	// Reset channel state machine
	IdacChannelState.Reset(MAX_SYNC_WORD_PER_SECOND);

	startSamplingThread();
}

/// @returns libusb error code
static int iso_submit(int iTransfer)
{
	int ret = 0;
#ifdef WIN32
	ret = usb_submit_async(isourb[iTransfer], isobuf + ISO_TRANSFER_SIZE * iTransfer, ISO_TRANSFER_SIZE);
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
		for (int iPacket = 0; iPacket < transfer->num_iso_packets; iPacket++) {
			//libusb_iso_packet_descriptor* packet = &transfer->iso_packet_desc[iPacket];
			nBytesReceived += transfer->iso_packet_desc[iPacket].actual_length;
		}
		ret = nBytesReceived;
	}
#endif
	return ret;
}

void IdacDriver4::sampleLoop()
{
	int ret;

	int iReapAttempt = 0;
	int iReapSuccess = 0;

	// Initial submits
	for (int i = 0; i < ISO_CONTEXT_COUNT; i++)
	{
		ret = iso_submit(i);
		CHECK_USBRESULT_NORET(ret);
		if (ret < 0)
			printf("isochronous submit returned %d\n", ret);
	}

	setIsoXferEnabled(true);
	m_maskDataRecived = 0;

	// Now cycle between reaps and submits
	bool bSamplingPrev = m_bSampling;
	while (bSamplingPrev)
	{
		bool bSamplingNow = m_bSampling;
		for (int i = 0; i < ISO_CONTEXT_COUNT; i++)
		{
			ret = iso_reap(i);
			//CHECK_USBRESULT_NORET(ret);
			iReapAttempt++;

			if (ret >= 0) {
				iReapSuccess++;
				processSampledData(i, ret);
			}
			else
			{
				if (ret != -5) {
					CHECK_USBRESULT_NORET(ret);
				}
				//qDebug() << "Reap " << iReapSuccess << "/" << iReapAttempt << ", Buffer " << i << ": ISOCHRONOUS READ ret = " << ret;
			}

			if (bSamplingNow)
			{
				ret = iso_submit(i);
				CHECK_USBRESULT_NORET(ret);
			}
		}
		bSamplingPrev = bSamplingNow;
	}

	setIsoXferEnabled(false);
}

bool IdacDriver4::processSampledData(int iTransfer, int nBytesReceived) {
	CHECK_ASSERT_NORET((nBytesReceived % ISO_PACKET_SIZE) == 0);

	bool bOverflow = false;
	int nPackets = (nBytesReceived > 0) ? (nBytesReceived / ISO_PACKET_SIZE) : 0;

	for (int iPacket = 0; iPacket < nPackets; iPacket++)
	{
		quint16* pBuffer = (quint16*) (isobuf + ISO_TRANSFER_SIZE * iTransfer + ISO_PACKET_SIZE * iPacket);
		int nBytes = *pBuffer++;
		int nWords = nBytes / 2;

		if (nBytes > ISO_PACKET_SIZE - 2) {
			logUsbError(__FILE__, __LINE__, QString("Invalid data size: %0").arg(nBytes));
			cerr << iTransfer << " " << iPacket << " " << nPackets << " " << nBytes << endl;
			for (int j = 0; j < 299; j++) {
				cerr << pBuffer[j] << " ";
			}
			cerr.flush();
			continue;
		}

		for (int iWord = 0; iWord < nWords; iWord++)
		{
			// Read little-endian word
			quint16 wData = *pBuffer++;

			CDD32_SAMPLE cds;
			bool bParsed = IdacChannelState.ParseSample(wData, cds, cdStatus, actualSettings());

			//printf("i:%d\t%d\t%d\t%d\t%x\n", iPacket, iWord, cds.uChannel, (int) bParsed, wData);
			if (bParsed)
			{
				short maskDataReceived = 0;
				if (cds.uChannel == 0)
				{
					m_nDig = cds.nSample;
					maskDataReceived |= 1;
				}
				else if (cds.uChannel == 1)
				{
					m_nAn1 = cds.nSample;
					maskDataReceived |= 2;
				}
				else if (cds.uChannel == 2)
				{
					m_nAn2 = cds.nSample;
					maskDataReceived |= 4;
				}

				if ((m_maskDataRecived & maskDataReceived) != 0) {
					cerr << "Received duplicate data, mask " << maskDataReceived << endl;
				}

				m_maskDataRecived |= maskDataReceived;
				if (m_maskDataRecived == 7) {
					bool b = addSample(m_nDig, m_nAn1, m_nAn2);
					if (!b)
					{
						bOverflow = true;
						logUsbError(__FILE__, __LINE__, "BUFFER OVERRUN");
						break;
					}
					m_maskDataRecived = 0;
				}
			}
		}
	}

	return bOverflow;
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
