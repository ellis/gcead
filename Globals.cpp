/**
 * Copyright (C) 2008  Ellis Whitehead
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

#include "Globals.h"

#include <QDir>
#include <QSettings>

#include <Check.h>
#include <Idac/IdacSettings.h>

#include "PublisherSettings.h"
#include "ViewSettings.h"


GlobalVars* Globals;


GlobalVars::GlobalVars()
{
	m_viewSettings = new ViewSettings();
	m_publisherSettings = new PublisherSettings();
	m_idacSettings = new IdacSettings();

	m_idacSettings->bRecordOnTrigger = false;
	m_idacSettings->nRecordingDuration = 0;
	m_idacSettings->nDigitalEnabledMask = 0x03; // Enable channels 1 & 2
	m_idacSettings->nDigitalInversionMask = 0x00;
	m_idacSettings->nGcDelay_ms = 0;
	m_idacSettings->channels[0].bEnabled = true;
	m_idacSettings->channels[0].nDecimation = 960; // 100 samples per second
	m_idacSettings->channels[1].bEnabled = true;
	m_idacSettings->channels[1].nDecimation = 960; // 100 samples per second
	m_idacSettings->channels[1].iRange = 3;
	m_idacSettings->channels[1].nExternalAmplification = 10;
	m_idacSettings->channels[2].bEnabled = true;
	m_idacSettings->channels[2].nDecimation = 960; // 100 samples per second
	m_idacSettings->channels[2].iRange = 4;
	m_idacSettings->channels[2].nExternalAmplification = 1;
}

GlobalVars::~GlobalVars()
{
	delete m_publisherSettings;
	delete m_idacSettings;
}

void GlobalVars::readSettings(QSettings& settings)
{
	settings.beginGroup("Files");
	m_sLastDir = settings.value("LastDir", QDir::homePath()).toString();
	recentFiles = settings.value("RecentFiles").toStringList();

	settings.beginGroup("View");
	m_viewSettings->bShowWaveComments = settings.value("ShowWaveComments", true).toBool();
	settings.endGroup();

	settings.beginGroup("Publish");
	m_publisherSettings->publisherChartElements = (ChartElements) settings.value("ElementFlags", (int) m_publisherSettings->publisherChartElements).toInt();
	m_publisherSettings->bPublishCols = settings.value("bGraphCols", false).toBool();
	m_publisherSettings->nPublishCols = settings.value("nGraphCols", 10).toInt();
	m_publisherSettings->bPublishTimebase = settings.value("bGraphTimebase", false).toBool();
	m_publisherSettings->nPublishTimebase = settings.value("nGraphTimebase", 1).toInt();
	m_publisherSettings->bPublishHeight = settings.value("bGraphHeight", false).toBool();
	m_publisherSettings->nPublishSize = settings.value("nGraphHeight", 500).toInt();
	m_publisherSettings->bPublishInColor = settings.value("PublishInColor", true).toBool();
	m_publisherSettings->clrPublishBorder = settings.value("BorderColor", QColor(128, 128, 200).rgba()).value<QRgb>();
	m_publisherSettings->iPublishMode = settings.value("PublishMode", 0).toInt();
	settings.endGroup();

	settings.beginGroup("IDAC");
	m_idacSettings->bRecordOnTrigger = settings.value("RecordOnTrigger", false).toBool();
	m_idacSettings->nRecordingDuration = settings.value("RecordingDuration", 0).toInt();
	m_idacSettings->nDigitalEnabledMask = settings.value("DigitalEnabledMask", 0x03).toInt();
	m_idacSettings->nDigitalInversionMask = settings.value("DigitalInversionMask", 0x00).toInt();
	m_idacSettings->nGcDelay_ms = settings.value("GcDelay", 0).toInt();

	m_idacSettings->channels[1].bInvert = settings.value("EAD_Invert", false).toBool();
	m_idacSettings->channels[1].iRange = settings.value("EAD_Range", 3).toInt();
	m_idacSettings->channels[1].iLowpass = settings.value("EAD_Lowpass", 10).toInt();
	m_idacSettings->channels[1].iHighpass = settings.value("EAD_Highpass", 1).toInt();
	m_idacSettings->channels[1].nOffset = settings.value("EAD_Offset", 0).toInt();
	m_idacSettings->channels[1].nExternalAmplification = settings.value("EAD_ExternalAmplification", 10).toInt();

	m_idacSettings->channels[2].bInvert = settings.value("FID_Invert", false).toBool();
	m_idacSettings->channels[2].iRange = settings.value("FID_Range", 4).toInt();
	m_idacSettings->channels[2].iLowpass = settings.value("FID_Lowpass", 10).toInt();
	m_idacSettings->channels[2].iHighpass = settings.value("FID_Highpass", 1).toInt();
	m_idacSettings->channels[2].nOffset = settings.value("FID_Offset", 0).toInt();
	m_idacSettings->channels[2].nExternalAmplification = settings.value("FID_ExternalAmplification", 1).toInt();
	settings.endGroup();
}

void GlobalVars::writeSettings(QSettings& settings)
{
	settings.beginGroup("Files");
	settings.setValue("LastDir", m_sLastDir);
	settings.setValue("RecentFiles", recentFiles);

	settings.beginGroup("View");
	settings.setValue("ShowWaveComments", m_viewSettings->bShowWaveComments);
	settings.endGroup();

	settings.beginGroup("Publish");
	settings.setValue("ElementFlags", (int) m_publisherSettings->publisherChartElements);
	settings.setValue("bGraphCols", m_publisherSettings->bPublishCols);
	settings.setValue("nGraphCols", m_publisherSettings->nPublishCols);
	settings.setValue("bGraphTimebase", m_publisherSettings->bPublishTimebase);
	settings.setValue("nGraphTimebase", m_publisherSettings->nPublishTimebase);
	settings.setValue("bGraphHeight", m_publisherSettings->bPublishHeight);
	settings.setValue("nGraphHeight", m_publisherSettings->nPublishSize);
	settings.setValue("PublishInColor", m_publisherSettings->bPublishInColor);
	settings.setValue("BorderColor", m_publisherSettings->clrPublishBorder.rgba());
	settings.setValue("PublishMode", m_publisherSettings->iPublishMode);
	settings.endGroup();

	settings.beginGroup("IDAC");
	settings.setValue("RecordOnTrigger", m_idacSettings->bRecordOnTrigger);
	settings.setValue("RecordingDuration", m_idacSettings->nRecordingDuration);
	settings.setValue("DigitalEnabledMask", m_idacSettings->nDigitalEnabledMask);
	settings.setValue("DigitalInversionMask", m_idacSettings->nDigitalInversionMask);
	settings.setValue("GcDelay", m_idacSettings->nGcDelay_ms);
	
	settings.setValue("EAD_Invert", m_idacSettings->channels[1].bInvert);
	settings.setValue("EAD_Range", m_idacSettings->channels[1].iRange);
	settings.setValue("EAD_Lowpass", m_idacSettings->channels[1].iLowpass);
	settings.setValue("EAD_Highpass", m_idacSettings->channels[1].iHighpass);
	settings.setValue("EAD_Offset", m_idacSettings->channels[1].nOffset);
	settings.setValue("EAD_ExternalAmplification", m_idacSettings->channels[1].nExternalAmplification);
	
	settings.setValue("FID_Invert", m_idacSettings->channels[2].bInvert);
	settings.setValue("FID_Range", m_idacSettings->channels[2].iRange);
	settings.setValue("FID_Lowpass", m_idacSettings->channels[2].iLowpass);
	settings.setValue("FID_Highpass", m_idacSettings->channels[2].iHighpass);
	settings.setValue("FID_Offset", m_idacSettings->channels[2].nOffset);
	settings.setValue("FID_ExternalAmplification", m_idacSettings->channels[2].nExternalAmplification);
	settings.endGroup();
}

void GlobalVars::setLastDir(const QString& sLastDir)
{
	m_sLastDir = sLastDir;
}
