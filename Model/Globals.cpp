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
#include <IdacDriver/IdacSettings.h>

#include "AppDefines.h"
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
	m_idacSettings->nGcDelay_ms = 0;
}

GlobalVars::~GlobalVars()
{
	delete m_publisherSettings;
	delete m_idacSettings;
}

void GlobalVars::readSettings()
{
	QSettings settings("Syntech", APPSETTINGSKEY);

	settings.beginGroup("Files");
	m_sLastDir = settings.value("LastDir", QDir::homePath()).toString();
	recentFiles = settings.value("RecentFiles").toStringList();
	settings.endGroup();

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
}

void GlobalVars::readIdacChannelSettings(const QString& sIdacName)
{
	CHECK_PARAM_RET(!sIdacName.isEmpty());

	QSettings settings("Syntech", APPSETTINGSKEY);

	settings.beginGroup("Hardware-" + sIdacName);
	m_idacSettings->bRecordOnTrigger = settings.value("RecordOnTrigger", false).toBool();
	m_idacSettings->nRecordingDuration = settings.value("RecordingDuration", 0).toInt();
	m_idacSettings->nGcDelay_ms = settings.value("GcDelay", 0).toInt();

	m_idacSettings->channels[0].mEnabled = settings.value("DIG_Enabled", 0x03).toInt();
	m_idacSettings->channels[0].mInvert = settings.value("DIG_Invert", 0x00).toInt();

	m_idacSettings->channels[1].mInvert = settings.value("EAD_Invert", 0).toInt();
	m_idacSettings->channels[1].iRange = settings.value("EAD_Range", 3).toInt();
	m_idacSettings->channels[1].iLowpass = settings.value("EAD_Lowpass", 10).toInt();
	m_idacSettings->channels[1].iHighpass = settings.value("EAD_Highpass", 1).toInt();
	m_idacSettings->channels[1].nOffset = settings.value("EAD_Offset", 0).toInt();
	m_idacSettings->channels[1].nExternalAmplification = settings.value("EAD_ExternalAmplification", 10).toInt();

	m_idacSettings->channels[2].mInvert = settings.value("FID_Invert", 0).toInt();
	m_idacSettings->channels[2].iRange = settings.value("FID_Range", 4).toInt();
	m_idacSettings->channels[2].iLowpass = settings.value("FID_Lowpass", 10).toInt();
	m_idacSettings->channels[2].iHighpass = settings.value("FID_Highpass", 1).toInt();
	m_idacSettings->channels[2].nOffset = settings.value("FID_Offset", 0).toInt();
	m_idacSettings->channels[2].nExternalAmplification = settings.value("FID_ExternalAmplification", 1).toInt();
	settings.endGroup();
}

void GlobalVars::writeSettings()
{
	QSettings settings("Syntech", APPSETTINGSKEY);

	settings.beginGroup("Files");
	settings.setValue("LastDir", m_sLastDir);
	settings.setValue("RecentFiles", recentFiles);
	settings.endGroup();

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
}

void GlobalVars::writeIdacChannelSettings(const QString& sIdacName)
{
	CHECK_PARAM_RET(!sIdacName.isEmpty());

	QSettings settings("Syntech", APPSETTINGSKEY);

	settings.beginGroup("IDAC-" + sIdacName);
	settings.setValue("RecordOnTrigger", m_idacSettings->bRecordOnTrigger);
	settings.setValue("RecordingDuration", m_idacSettings->nRecordingDuration);
	settings.setValue("GcDelay", m_idacSettings->nGcDelay_ms);
	
	settings.setValue("DIG_Enabled", m_idacSettings->channels[1].mEnabled);
	settings.setValue("DIG_Invert", m_idacSettings->channels[1].mInvert);

	settings.setValue("EAD_Invert", m_idacSettings->channels[1].mInvert);
	settings.setValue("EAD_Range", m_idacSettings->channels[1].iRange);
	settings.setValue("EAD_Lowpass", m_idacSettings->channels[1].iLowpass);
	settings.setValue("EAD_Highpass", m_idacSettings->channels[1].iHighpass);
	settings.setValue("EAD_Offset", m_idacSettings->channels[1].nOffset);
	settings.setValue("EAD_ExternalAmplification", m_idacSettings->channels[1].nExternalAmplification);
	
	settings.setValue("FID_Invert", m_idacSettings->channels[2].mInvert);
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
