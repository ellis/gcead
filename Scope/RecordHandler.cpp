/**
 * Copyright (C) 2008,2009  Ellis Whitehead
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

#include "RecordHandler.h"

#include <QtDebug>
#include <QMessageBox>

#include <Check.h>
#include <Globals.h>

#include <Idac/IdacProxy.h>
#include <IdacDriver/IdacSettings.h>


RecordHandler::RecordHandler(IdacProxy* idac)
{
	Q_ASSERT(idac != NULL);
	m_idac = idac;
	m_bReportingError = false;

	for (int i = 0; i < 3; i++)
		m_anBuffers[i] = new short[300];
}

void RecordHandler::updateRawToVoltageFactors()
{
	int nNum, nDen;
	
	calcRawToVoltageFactors(1, nNum, nDen);
	m_anRawToVoltageFactors[1] = double(nNum) / nDen;

	calcRawToVoltageFactors(2, nNum, nDen);
	m_anRawToVoltageFactors[2] = double(nNum) / nDen;
}

void RecordHandler::calcRawToVoltageFactors(int iChan, int& nNum, int& nDen)
{
	if (m_idac->ranges().size() == 0)
	{
		nNum = 1;
		nDen = 1;
		return;
	}

	const IdacChannelSettings* chan = &Globals->idacSettings()->channels[iChan];

	nNum =
		m_idac->ranges()[chan->iRange]; // max range in uVolts
	nDen =
		32768 // Normalize
		* chan->nExternalAmplification // Account for 10x external amplification before being digitized
		//* 1000000.0 // 1000000 micro volts (1 volt) is full range
		* 1000; // We use mVolt unit, so: 1mV = 1000uV

	// FIXME: for debug  only
	//qDebug() << "nNum:" << nNum << "nDen:" << nDen;
}

bool RecordHandler::check()
{
	if (m_idac->state() != IdacState_Sampling)
		return false;

	bool bOk = true;

	// 1. Check IDAC status
	if (!m_bReportingError)
	{
		m_bReportingError = true;

		//if (cdErr.bSyncFail || cdErr.bHwOverflow	|| cdErr.bSwOverflow || cdErr.bCommErr || cdErr.bTimeout)
		//if (cdErr.bSyncFail || cdErr.bHwOverflow || cdErr.bSwOverflow || cdErr.bCommErr)
		if (m_idac->errors() != 0)
		{
			QString csMessage;

			// FIXME:
			csMessage = "Some kind of error, Dude.\n";
			/*
			if (cdErr.bSyncFail) csMessage		= "Sync error\r\n";
			if (cdErr.bHwOverflow) csMessage	+= "Hardware buffer overflow\r\n";
			if (cdErr.bSwOverflow) csMessage	+= "Software buffer overflow\r\n";
			if (cdErr.bCommErr) csMessage		+= "General communication error\r\n";
			if (cdErr.bTimeout) csMessage		+= "Communication timeout\r\n";
			*/
			//ENDFIX

			csMessage += "Press Cancel to stop.";

			// FIXME: probably need to stop the IDAC here to prevent the buffer from becoming too large 
			// while waiting for the user to respond.  Or put up a mode-less dialog box instead. -- ellis, 2008-09-04

			QMessageBox::StandardButton nResult = QMessageBox::warning(NULL, "Communication Error", csMessage, QMessageBox::Ok | QMessageBox::Cancel);

			if (nResult == QMessageBox::Cancel)
			{
				// Stop sampling
				// TODO: I need to decide what should really be done here... -- ellis, 2008-06-23
				//on_btnSave_clicked();
				bOk = false;
			}
		}

		m_bReportingError = false;
	}

	return bOk;
}

bool RecordHandler::convert()
{
	if (m_idac->state() != IdacState_Sampling)
		return false;

	uchar nDigitalEnabledMask = Globals->idacSettings()->channels[0].mEnabled;
	uchar nDigitalInversionMask = (Globals->idacSettings()->channels[0].mInvert & nDigitalEnabledMask);

	int nSamples = m_idac->takeData(m_anBuffers[0], m_anBuffers[1], m_anBuffers[2], 300);

	// 2. Read data and send to display
	bool bHaveData = false;
	for (int iChan = 0; iChan < 3; iChan++)
	{
		// Size our data arrays appropriately
		short* buffer = m_anBuffers[iChan];
		QVector<short>& raw = m_anRaw[iChan];
		QVector<double>& display = m_anDisplay[iChan];
		raw.resize(nSamples);
		display.resize(nSamples);

		//if (iChan == 0)
		//	qDebug() << "conver:" << QTime::currentTime().msec() << data.size();

		for (int i = 0; i < nSamples; i++)
		{
			short nRaw;
			double nDisplay;

			// Digital channel
			if (iChan == 0)
			{
				uchar n = (uchar) buffer[i]; // here, 0 = on
				n = ~n; // Switch it up so that 1 = on
				n ^= nDigitalInversionMask; // Invert bits, if necessary

				nRaw = n;
				nDisplay = ((n & 0x02) > 0) ? 0.5 : -0.5;
			}
			// Analog channel
			else
			{
				nRaw = buffer[i];
				if (Globals->idacSettings()->channels[iChan].mInvert)
					nRaw *= -1;
				nDisplay = nRaw * m_anRawToVoltageFactors[iChan];
			}

			raw[i] = nRaw;
			display[i] = nDisplay;
			bHaveData = true;
		}
	}

	return bHaveData;
}
