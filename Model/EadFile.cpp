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

#include "EadFile.h"

#include <math.h>

#include <QtDebug>
#include <QDataStream>
#include <QDomDocument>
#include <QFile>
#include <QStringList>
#include <QTextStream>

#include "Check.h"


/// Channel 1 of fake EAD data
extern short g_anChannel1[3999];
/// Channel 2 of fake FID data
extern short g_anChannel2[3999];


EadFile::EadFile()
{
	m_newRec = NULL;

	createAveWaves();
	createViewInfo();

	m_bDirty = false;
}

EadFile::~EadFile()
{
	qDeleteAll(m_views);
	m_views.clear();
	qDeleteAll(m_recs);
	m_recs.clear();
	delete m_newRec;
}

ViewInfo* EadFile::viewInfo(EadView view)
{
	CHECK_ASSERT_RETVAL(m_views.size() > (int) view, NULL);
	return m_views[view];
}

void EadFile::setComment(const QString& s)
{
	if (s != m_sComment)
	{
		m_sComment = s;
		m_bDirty = true;
		emit dirtyChanged();
	}
}

void EadFile::clear()
{
	// Clear all but EadView_Averages
	for (int i = EadView_EADs; i <= EadView_Recording; i++)
		m_views[i]->clearWaves();

	// Clear all but averages
	while (m_recs.size() > 1) {
		delete m_recs[1];
		m_recs.removeAt(1);
	}

	m_sFilename.clear();
	m_sComment.clear();
	m_bDirty = false;
	if (m_newRec)
	{
		delete m_newRec;
		m_newRec = NULL;
	}
}

//
// SAVE functions
//

bool EadFile::saveAs(const QString& sFilename)
{
	QDomDocument doc("ead");
	QDomElement root = doc.createElement("ead");
	root.setAttribute("comment", m_sComment);
	doc.appendChild(root);

	QDomElement tag = doc.createElement("waves");
	root.appendChild(tag);

	foreach (RecInfo* rec, m_recs)
		createRecNode(doc, tag, rec);
	foreach (ViewInfo* view, m_views)
		createViewNode(doc, tag, view);

	QString xml = doc.toString();

	QFile file(sFilename);
	if (!file.open(QIODevice::WriteOnly))
		return false;

	QDataStream str(&file);
	qint32 nVersion = 1;
	str.writeRawData("EAD", 4);
	str << nVersion;
	str.setVersion(QDataStream::Qt_4_3);
	str << xml;
	saveData(str);

	qDebug() << "XML:" << endl << xml;

	m_sFilename = sFilename;
	m_bDirty = false;
	return true;
}

LoadSaveResult EadFile::load(const QString& sFilename)
{
	QFile file(sFilename);
	if (!file.open(QIODevice::ReadOnly))
		return LoadSaveResult_CouldNotOpen;

	QDataStream str(&file);

	blockSignals(true);

	clear();

	// Check that this is really and EAD file
	char sFormatId[4];
	str.readRawData(sFormatId, 4);

	LoadSaveResult result;
	// If this is the old EAD format:
	if (QString(sFormatId).startsWith("BcV"))
		result = loadOld(str);
	// If this is the current EAD format:
	else if (QString("EAD") == sFormatId)
		result = loadCurrent(str);
	else
		result = LoadSaveResult_WrongFormat;

	updateDisplay();
	updateViewInfo();
	updateAveWaves();

	// Perform FID peak detection && calculation of verified peak areas
	for (int i = 0; i < m_recs.count(); i++)
	{
		RecInfo* rec = m_recs[i];
		rec->fid()->findFidPeaks();
		rec->fid()->calcPeakAreas();
		rec->fid()->calcPeakAmplitudes();
	}

	if (result == LoadSaveResult_Ok)
		m_sFilename = sFilename;

	m_bDirty = false;

	blockSignals(false);

	emit waveListChanged();

	return result;
}

static int getInt(char* data_) {
	uchar* data = (uchar*) data_;
	int n = 0;
	n |= data[0];
	n |= data[1] << 8;
	n |= data[2] << 16;
	n |= data[3] << 24;
	return n;
}

LoadSaveResult EadFile::loadOld(QDataStream& str)
{
	char data[4];

	bool bFoundStart = false;
	bool bDone = false;
	const QString sSeek = "BcDataSet";
	QString s;
	while (!bFoundStart && !bDone) {
		char c;
		int n = str.readRawData(&c, 1);
		if (n <= 0)
			return LoadSaveResult_DataCorrupt;
		s = s.right(sSeek.length() - 1) + c;
		// If we've found the string we're scanning for:
		if (s == sSeek) {
			for (int i = 0; i < 7; i++) {
				str.readRawData(&c, 1);
			}
			bDone = true;
		}
	}

	//createAveWaves();

	RecInfo* rec = new RecInfo(1);
	WaveInfo* wave = rec->ead();

	str.readRawData(data, 4);
	int nSamples = getInt(data);
	wave->nRawToVoltageFactorDen *= 2048;
	wave->nRawToVoltageFactor = double(wave->nRawToVoltageFactorNum) / wave->nRawToVoltageFactorDen;
	wave->raw.resize(nSamples * 10);
	for (int i = 0; i < nSamples; i++) {
		str.readRawData(data, 4);
		int n = -getInt(data);
		for (int j = 0; j < 10; j++)
			wave->raw[i * 10 + j] = (short) n;
	}

	// Skip 11 bytes
	wave = rec->fid();
	for (int i = 0; i < 11; i++) {
		char c;
		str.readRawData(&c, 1);
	}
	str.readRawData(data, 4);
	nSamples = getInt(data);
	wave->nRawToVoltageFactorDen *= 2048;
	wave->nRawToVoltageFactor = double(wave->nRawToVoltageFactorNum) / wave->nRawToVoltageFactorDen;
	wave->raw.resize(nSamples * 10);
	for (int i = 0; i < nSamples; i++) {
		str.readRawData(data, 4);
		int n = -getInt(data);
		for (int j = 0; j < 10; j++)
			wave->raw[i * 10 + j] = (short) n;
	}

	m_recs << rec;

	return LoadSaveResult_ImportedOldEad;
}

LoadSaveResult EadFile::loadCurrent(QDataStream& str)
{
	// Check the file format version
	qint32 nVersion;
	str >> nVersion;
	if (nVersion < 1)
		return LoadSaveResult_VersionTooLow;
	else if (nVersion > 1)
		return LoadSaveResult_VersionTooHigh;

	str.setVersion(QDataStream::Qt_4_3);

	// Read in the XML
	QString xml;
	str >> xml;
	QDomDocument doc("ead");
	if (!doc.setContent(xml))
		return LoadSaveResult_DataCorrupt;

	m_sComment = doc.documentElement().attribute("comment");

	// Reconstruct our recordings from the XML
	qDeleteAll(m_recs);
	m_recs.clear();
	QDomNodeList recs = doc.elementsByTagName("rec");
	for (int i = 0; i < recs.size(); i++)
	{
		QDomElement elem = recs.at(i).toElement();
		loadRecNode(elem);
	}
	// At least we should have our two averaged waves
	CHECK_ASSERT_RETVAL(m_recs.size() >= 1, LoadSaveResult_DataCorrupt);

	// Load "extra" and "user" data for the views
	createViewInfo();
	QDomNodeList views = doc.elementsByTagName("view");
	for (int i = 0; i < views.size() && i < m_views.size(); i++)
	{
		QDomElement elem = views.at(i).toElement();
		ViewInfo* view = m_views[i];
		loadViewNode(elem, view);
	}

	// Load data for non-averaged waves
	for (int i = 1; i < m_recs.count(); i++)
	{
		RecInfo* rec = m_recs[i];
		foreach (WaveInfo* wave, rec->waves())
		{
			str >> wave->raw;
		}
	}

	return LoadSaveResult_Ok;
}

void EadFile::createRecNode(QDomDocument& doc, QDomElement& parent, RecInfo* rec)
{
	QDomElement elem = doc.createElement("rec");
	parent.appendChild(elem);
	
	elem.setAttribute("id", rec->id());
	elem.setAttribute("time", rec->timeOfRecording().toTime_t());

	foreach (WaveInfo* wave, rec->waves())
		createWaveNode(doc, elem, wave);
}

void EadFile::loadRecNode(QDomElement& elem)
{
	int id = elem.attribute("id").toInt();
	uint nSeconds = elem.attribute("time").toUInt();
	
	RecInfo* rec = new RecInfo(id);
	rec->setTimeOfRecording(QDateTime::fromTime_t(nSeconds));

	QDomNodeList waves = elem.elementsByTagName("wave");
	for (int i = 0; i < waves.size(); i++)
	{
		QDomElement elem = waves.at(i).toElement();
		WaveInfo* wave = rec->waves()[i];
		loadWaveNode(elem, wave);
	}

	m_recs << rec;
}

/// Convert WaveType to an XML string
static QString getWaveTypeNodeName(WaveType type)
{
	QString sType;
	switch (type)
	{
	case WaveType_EAD: sType = "EAD"; break;
	case WaveType_FID: sType = "FID"; break;
	case WaveType_Digital: sType = "DIG"; break;
	}
	return sType;
}

/// Convert the XML string to its WaveType
static WaveType getNodeNameWaveType(const QString& sWaveType)
{
	if (sWaveType == "EAD")
		return WaveType_EAD;
	else if (sWaveType == "FID")
		return WaveType_FID;
	else if (sWaveType == "DIG")
		return WaveType_Digital;
	return WaveType_EAD;
}

/*
<ead>
	<waves>
		<wave type="EAD" name="EAD 1" inverted="false" xOffset="0" sensitivity="0.05">
			<mode id="0" yOffset="5"/>
			<mode id="1" yOffset="5"/>
			<mode id="2" yOffset="5"/>
			<mode id="3" yOffset="5"/>
		</wave>
	</wave>
</ead>
*/
void EadFile::createWaveNode(QDomDocument& doc, QDomElement& parent, WaveInfo* wave)
{
	QDomElement elem = doc.createElement("wave");
	parent.appendChild(elem);
	
	elem.setAttribute("type", getWaveTypeNodeName(wave->type));
	elem.setAttribute("name", wave->sName);
	elem.setAttribute("factor_num", wave->nRawToVoltageFactorNum);
	elem.setAttribute("factor_den", wave->nRawToVoltageFactorDen);
	elem.setAttribute("shift", wave->shift());
	elem.setAttribute("visible", (wave->pos.bVisible) ? "t" : "f");
	elem.setAttribute("volts", wave->pos.nVoltsPerDivision);
	elem.setAttribute("yOffset", wave->pos.nDivisionOffset);

	if (!wave->sComment.isEmpty())
	{
		QDomElement comment = doc.createElement("comment");
		QDomText text = doc.createTextNode(wave->sComment);
		comment.appendChild(text);
		elem.appendChild(comment);
	}

	foreach (const WavePeakChosenInfo& peak, wave->peaksChosen)
		createPeakNode(doc, elem, &peak);
}

void EadFile::loadWaveNode(QDomElement& elem, WaveInfo* wave)
{
	wave->sName = elem.attribute("name");
	wave->nRawToVoltageFactorDen = elem.attribute("factor_den", "1").toInt();
	wave->nRawToVoltageFactorNum = elem.attribute("factor_num", "1").toInt();
	wave->nRawToVoltageFactorDen = elem.attribute("factor_den", "1").toInt();
	wave->pos.bVisible = (elem.attribute("visible", "t") == "t");
	wave->pos.nVoltsPerDivision = elem.attribute("volts", "1").toDouble();
	wave->pos.nDivisionOffset = elem.attribute("yOffset", "5").toDouble();
	int nShift = elem.attribute("shift").toInt();
	wave->setShift(nShift);

	if (wave->nRawToVoltageFactorDen >= 1)
		wave->nRawToVoltageFactor = double(wave->nRawToVoltageFactorNum) / wave->nRawToVoltageFactorDen;
	else
		wave->nRawToVoltageFactor = 1;

	QDomNodeList comments = elem.elementsByTagName("comment");
	if (comments.size() == 1)
	{
		QDomElement comment = comments.at(0).toElement();
		wave->sComment = comment.text();
	}

	QDomNodeList peaks = elem.elementsByTagName("peak");
	for (int i = 0; i < peaks.size(); i++)
	{
		QDomElement elem = peaks.at(i).toElement();
		loadPeakNode(elem, wave);
	}
}

void EadFile::createPeakNode(QDomDocument& doc, QDomElement& parent, const WavePeakChosenInfo* peak)
{
	QDomElement elem = doc.createElement("peak");
	parent.appendChild(elem);
	
	elem.setAttribute("left", peak->didxLeft);
	elem.setAttribute("middle", peak->didxMiddle);
	elem.setAttribute("right", peak->didxRight);
}

void EadFile::loadPeakNode(QDomElement& elem, WaveInfo* wave)
{
	WavePeakChosenInfo peak;

	peak.didxLeft = elem.attribute("left").toInt();
	peak.didxMiddle = elem.attribute("middle").toInt();
	peak.didxRight = elem.attribute("right").toInt();

	wave->peaksChosen << peak;
}

void EadFile::createViewNode(QDomDocument& doc, QDomElement& parent, ViewInfo* view)
{
	QDomElement elem = doc.createElement("view");
	parent.appendChild(elem);
	
	QDomElement elemExtras = doc.createElement("extras");
	elem.appendChild(elemExtras);
	foreach (ViewWaveInfo* vwi, view->vwiExtras())
		createViewWaveNode(doc, elemExtras, vwi);

	QDomElement elemUsers = doc.createElement("users");
	elem.appendChild(elemUsers);
	createViewWaveNode(doc, elemUsers, &view->vwiUser);
}

void EadFile::loadViewNode(QDomElement& elem, ViewInfo* view)
{
	QDomNodeList list;
	
	list = elem.elementsByTagName("extras");
	if (list.size() == 1)
	{
		list = list.at(0).toElement().elementsByTagName("vwi");
		for (int i = 0; i < list.size(); i++)
		{
			QDomElement elem = list.at(i).toElement();
			loadViewWaveNode(elem, view, true);
		}
	}

	list = elem.elementsByTagName("users");
	if (list.size() == 1)
	{
		list = list.at(0).toElement().elementsByTagName("vwi");
		for (int i = 0; i < list.size(); i++)
		{
			QDomElement elem = list.at(i).toElement();
			loadViewWaveNode(elem, view, false);
		}
	}
}

void EadFile::createViewWaveNode(QDomDocument& doc, QDomElement& parent, ViewWaveInfo* vwi)
{
	const WaveInfo* wave = vwi->wave();
	if (wave == NULL)
		return;

	QDomElement elem = doc.createElement("vwi");
	parent.appendChild(elem);

	elem.setAttribute("recID", wave->recId());
	elem.setAttribute("type", getWaveTypeNodeName(wave->type));
	elem.setAttribute("visible", (vwi->isVisible()) ? "t" : "f");
	elem.setAttribute("volts", vwi->voltsPerDivision());
	elem.setAttribute("yOffset", vwi->divisionOffset());
}

void EadFile::loadViewWaveNode(QDomElement& elem, ViewInfo* view, bool bExtra)
{
	int recId = elem.attribute("recID").toInt();
	QString sWaveType = elem.attribute("type");
	WaveType type = getNodeNameWaveType(sWaveType);

	if (recId < 0 || recId >= m_recs.size())
		return;

	RecInfo* rec = m_recs[recId];
	WaveInfo* wave = rec->wave(type);

	ViewWaveInfo* vwi = NULL;
	if (bExtra)
		vwi = view->addExtraWave(wave);
	else
	{
		view->setUserWave(wave);
		vwi = &view->vwiUser;
	}
	if (vwi == NULL)
		return;

	bool bVisible = (elem.attribute("visible") == "t");
	double nVoltsPerDivision = elem.attribute("volts", "1").toDouble();
	double nDivisionOffset = elem.attribute("yOffset", "5").toDouble();

	vwi->setVisible(bVisible);
	vwi->setVoltsPerDivision(nVoltsPerDivision);
	vwi->setDivisionOffset(nDivisionOffset);
}

void EadFile::saveData(QDataStream& str)
{
	for (int i = 1; i < m_recs.size(); i++)
	{
		RecInfo* rec = m_recs[i];
		foreach (WaveInfo* wave, rec->waves())
			str << wave->raw;
	}
}

bool EadFile::exportData(const QString& sFilename /*, EadFile::ExportFormat format*/)
{
	QFile file(sFilename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	QTextStream str(&file);

	// Build list of waves with data
	QList<WaveInfo*> waves;
	foreach (RecInfo* rec, m_recs)
	{
		foreach (WaveInfo* wave, rec->waves())
		{
			if (!wave->display.isEmpty())
				waves << wave;
		}
	}

	if (waves.isEmpty())
		return false;

	// Header row
	QStringList cols;
	foreach (WaveInfo* wave, waves)
		cols << QString("\"%0\"").arg(wave->sName);
	str << cols.join(",") << endl;

	// Data rows
	int tidx = 0;
	while (true)
	{
		cols.clear();
		bool bFound = false;
		foreach (WaveInfo* wave, waves)
		{
			double n = 0;
			//int didxEnd = wave->display.size() + wave->rec()->shift();
			//int didx = tidx - wave->rec()->shift();
			int didxEnd = wave->display.size() + wave->shift();
			int didx = tidx - wave->shift();
			if (didx < didxEnd)
			{
				bFound = true;
				if (didx >= 0)
					n = wave->display[didx];
			}
			cols << QString::number(n);
		}
		if (!bFound)
			break;

		str << cols.join(",") << endl;
		tidx++;
	}

	return true;
}

bool EadFile::exportRetentionData(const QString& sFilename /*, EadFile::ExportFormat format*/)
{
	QFile file(sFilename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	QTextStream str(&file);

	str << "\"Signal\",\"Minutes\",\"Percent\",\"Absolute\",\"Amplitude\"" << endl;

	// Find waves with peaks:
	foreach (RecInfo* rec, m_recs)
	{
		WaveInfo* wave = rec->fid();
		if (wave->peaksChosen.count() > 0)
		{
			// For each peak:
			foreach (const WavePeakChosenInfo& peak, wave->peaksChosen)
			{
				str
					<< "\"" << wave->sName << "\","
					<< (double(peak.didxMiddle) / (EAD_SAMPLES_PER_SECOND * 60)) << ','
					<< peak.nPercent << ','
					<< peak.nArea << ','
					<< peak.nAmplitude << endl;
			}
		}
	}

	return true;
}

void EadFile::createNewRecording()
{
	// FIXME: for debug only
	if (m_newRec != NULL)
		qDebug() << "Woops";
	CHECK_PRECOND_RET(m_newRec == NULL);

	m_newRec = new RecInfo(m_recs.size());
	
	ViewInfo* view = viewInfo(EadView_Recording);
	// Update the "Recording" view & save pointers for convenience
	view->clearWaves();
	view->addWave(m_newRec->ead());
	view->addWave(m_newRec->fid());
	view->addWave(m_newRec->digital());

	// Set time of recording to now
	m_newRec->setTimeOfRecording(QDateTime::currentDateTime());

	emit waveListChanged();
}

void EadFile::discardNewRecording()
{
	//qDebug() << "EadFile::discardNewRecording()";

	delete m_newRec;
	m_newRec = 0;

	ViewInfo* view = m_views[EadView_Recording];
	view->clearWaves();

	emit waveListChanged();
}

void EadFile::saveNewRecording()
{
	//qDebug() << "EadFile::saveNewRecording()";

	RecInfo* rec = m_newRec;

	m_recs << m_newRec;
	m_newRec = NULL;

	updateDisplay(rec);
	updateViewInfo();
	updateAveWaves();

	rec->fid()->findFidPeaks();

	emit waveListChanged();

	m_bDirty = true;
	emit dirtyChanged();
}

void EadFile::createAveWaves()
{
	CHECK_PRECOND_RET(m_recs.size() == 0);

	RecInfo* rec = new RecInfo(0);

	WaveInfo* wave = rec->ead();
	wave->sName = tr("EAD AVE");
	wave->pos.nDivisionOffset = 4;
	
	wave = rec->fid();
	wave->sName = tr("FID AVE");

	m_recs << rec;
}

void EadFile::createViewInfo()
{
	// Allocate views
	qDeleteAll(m_views);
	m_views.clear();

	for (int i = 0; i < EadViewCount; i++)
		m_views << new ViewInfo((EadView) i, this);

	RecInfo* recAve = m_recs[0];
	
	// Setup Averages view
	m_views[EadView_Averages]->addWave(recAve->ead());
	m_views[EadView_Averages]->addWave(recAve->fid());
	
	// Place the appropriate averages in all other views as "extra" waves
	m_views[EadView_EADs]->addExtraWave(recAve->ead());
	m_views[EadView_FIDs]->addExtraWave(recAve->fid());
	m_views[EadView_All]->addExtraWave(recAve->ead());
	m_views[EadView_All]->addExtraWave(recAve->fid());
	ViewWaveInfo* vwi;
	vwi = m_views[EadView_Recording]->addExtraWave(recAve->ead());
	vwi->setVisible(false);
	vwi = m_views[EadView_Recording]->addExtraWave(recAve->fid());
	vwi->setVisible(false);
}

void EadFile::updateViewInfo()
{
	// Don't clear EadView_Averages or EadView_Recording
	for (int i = EadView_EADs; i <= EadView_All; i++)
		m_views[i]->clearWaves();
	
	// Add default waves to the other views
	for (int i = 1; i < m_recs.count(); i++)
	{
		RecInfo* rec = m_recs[i];
		
		if (rec->ead()->raw.size() > 0)
		{
			m_views[EadView_EADs]->addWave(rec->ead());
			m_views[EadView_All]->addExtraWave(rec->ead());
		}

		if (rec->fid()->raw.size() > 0)
		{
			m_views[EadView_FIDs]->addWave(rec->fid());
			m_views[EadView_All]->addExtraWave(rec->fid());
		}

		if (rec->digital()->raw.size() > 0)
			m_views[EadView_All]->addExtraWave(rec->digital());
	}

	// Get list of all waves
	QList<WaveInfo*> waves;
	foreach (RecInfo* rec, m_recs)
	{
		foreach (WaveInfo* wave, rec->waves())
				waves << wave;
	}

	// Set user wave to NULL if the wave has been removed
	for (int iView = 0; iView < EadViewCount; iView++)
	{
		WaveInfo* waveExtra = m_views[iView]->vwiUser.waveInfo();
		if (waveExtra != NULL && !waves.contains(waveExtra))
			m_views[iView]->setUserWave(NULL);
	}
}

void EadFile::updateAveWaves()
{
	updateAveWave(WaveType_EAD);
	updateAveWave(WaveType_FID);

	m_recs[0]->fid()->findFidPeaks();
}

void EadFile::updateAveWave(WaveType type)
{
	WaveInfo* ave = m_recs[0]->wave(type);

	// Get list of relevant waves to build the averaged waveform
	QList<WaveInfo*> waves;
	foreach (RecInfo* rec, m_recs)
	{
		// Skip the averaged waveforms
		if (rec->id() == 0)
			continue;

		foreach (WaveInfo* wave, rec->waves())
		{
			if (wave->type == type && wave->pos.bVisible)
				waves << wave;
		}
	}
	if (waves.size() == 0)
		return;

	// Find the max number of samples we'll need for the average
	int nSamples = 0;
	foreach (WaveInfo* wave, waves)
	{
		int n = wave->shift() + wave->display.size();
		if (n > nSamples)
			nSamples = n;
	}

	QVector<quint8> anWavesPerSample(nSamples);
	ave->display.resize(nSamples);
	ave->display.fill(0);

	// Sum up all channels into the average wave
	foreach (WaveInfo* wave, waves)
	{
		// Start at a sample index greater than 0 if the wave is shifted to the left
		int i0 = 0;
		if (wave->shift() < 0)
			i0 = -wave->shift();

		for (int i = i0; i < wave->display.size(); i++)
		{
			int iShifted = i + wave->shift();
			double n = wave->display[i];
			ave->display[iShifted] += n;
			anWavesPerSample[iShifted]++;
		}
	}
	
	// Divide through to get the average
	for (int i = 0; i < ave->display.size(); i++)
		ave->display[i] /= anWavesPerSample[i];

	// Start calculating standard deviation
	ave->std.resize(nSamples);
	ave->std.fill(0);
	foreach (WaveInfo* wave, waves)
	{
		// Start at a sample index greater than 0 if the wave is shifted to the left
		int i0 = 0;
		if (wave->shift() < 0)
			i0 = -wave->shift();

		for (int i = i0; i < wave->display.size(); i++)
		{
			int iShifted = i + wave->shift();
			double n = wave->display[i];
			double m = ave->display[iShifted];
			double d = n - m;
			double squared = d * d;
			ave->std[iShifted] += squared;
			anWavesPerSample[iShifted]++;
		}
	}

	// Divide through and take the square root to get the standard deviation
	for (int i = 0; i < ave->display.size(); i++)
		ave->std[i] = sqrt(ave->std[i] / anWavesPerSample[i]);
}

void EadFile::updateDisplay()
{
	// Add default waves to the other views
	for (int i = 1; i < m_recs.count(); i++)
	{
		RecInfo* rec = m_recs[i];
		updateDisplay(rec);
	}
}

void EadFile::updateDisplay(RecInfo* rec)
{
	CHECK_PARAM_RET(rec != NULL);

	rec->ead()->calcDisplayData();
	rec->fid()->calcDisplayData();
	rec->digital()->calcDisplayData();
}

void EadFile::createFakeData()
{
	blockSignals(true);

	m_sFilename.clear();

	if (m_recs.size() > 1)
	{
		qDeleteAll(m_recs);
		m_recs.clear();
		createAveWaves();
	}

	RecInfo* rec = new RecInfo(1);
	rec->setTimeOfRecording(QDateTime::currentDateTime());
	m_recs << rec;

	WaveInfo* wave;

	wave = rec->ead();
	wave->pos.nDivisionOffset = 2;
	wave->nRawToVoltageFactorNum = 1;
	wave->nRawToVoltageFactorDen = 10000;
	wave->nRawToVoltageFactor = 1.0/10000;
	createFakeData2(wave, g_anChannel1, 3999, 0);

	wave = rec->fid();
	wave->pos.nVoltsPerDivision = 0.5;
	wave->nRawToVoltageFactorNum = 1;
	wave->nRawToVoltageFactorDen = 1500;
	wave->nRawToVoltageFactor = 1.0/1500;
	createFakeData2(wave, g_anChannel2, 3999, 0);

	rec = new RecInfo(2);
	rec->setTimeOfRecording(QDateTime::currentDateTime());
	m_recs << rec;
	createFakeData3(5, QList<float>() << .8 << 1.3);

	createViewInfo();

	updateDisplay();
	updateViewInfo();
	updateAveWaves();

	// Perform FID peak detection
	for (int i = 1; i < m_recs.count(); i++)
	{
		RecInfo* rec = m_recs[i];
		rec->fid()->findFidPeaks();
	}

	m_bDirty = false;
	blockSignals(false);

	emit waveListChanged();
}

void EadFile::createFakeData2(WaveInfo* wave, short* data, int nSize, short yOffset)
{
	//double nPrev = 0;
	for (int i = 0; i < nSize - 1; i++)
	{
		double n0 = data[i];
		double n1 = data[i+1];
		for (int j = 0; j < 10; j++)
		{
			double n = ((n0 * (10 - j)) + (n1 * j)) / 10;
			n += yOffset; // 5000 is an extra offset so that I can check that high-pass filtering works
			wave->raw.append(int(n + 0.5));
			
			// FIXME: for debug only
			//if (n == nPrev)
			//	nPrev = 0;
			//nPrev = n;
		}
	}
	wave->raw.append(data[nSize-1] + yOffset);
}

void EadFile::createFakeData3(int nShift, const QList<float>& factors)
{
	WaveInfo* wave;

	wave = m_recs[2]->ead();
	wave->pos.nDivisionOffset = 6;
	wave->nRawToVoltageFactorNum = m_recs[1]->ead()->nRawToVoltageFactorNum;
	wave->nRawToVoltageFactorDen = m_recs[1]->ead()->nRawToVoltageFactorDen;
	wave->nRawToVoltageFactor = m_recs[1]->ead()->nRawToVoltageFactor;
	wave->raw = createFakeData4(m_recs[1]->ead()->raw, nShift, factors);

	wave = m_recs[2]->fid();
	wave->nRawToVoltageFactorNum = m_recs[1]->fid()->nRawToVoltageFactorNum;
	wave->nRawToVoltageFactorDen = m_recs[1]->fid()->nRawToVoltageFactorDen;
	wave->nRawToVoltageFactor = m_recs[1]->fid()->nRawToVoltageFactor;
	wave->raw = createFakeData4(m_recs[1]->fid()->raw, nShift, factors);
}

QVector<short> EadFile::createFakeData4(const QVector<short>& orig, int nShift, const QList<float>& factors)
{
	QVector<short> data(orig.size() - nShift);

	int iData = 0;
	for (int i = 0; i < -nShift; i++)
		data[iData++] = (short) (orig[0] * factors[0]);

	int iOrig = 0;
	if (nShift > 0)
		iOrig = nShift;

	for (int iFactor1 = 1; iFactor1 < factors.size(); iFactor1++)
	{
		double nFactor0 = factors[iFactor1 - 1];
		double nFactor1 = factors[iFactor1];

		int iOrig1 = orig.size() / (factors.size() - 1);
		int n = iOrig1 - iOrig;

		for (int i = 0; i < n; i++)
		{
			double nFactor = nFactor0 + (nFactor1 - nFactor0) * (i / double(n-1));
			double n = orig[iOrig++] * nFactor;
			data[iData++] = short(n + 0.5);
		}
	}

	return data;
}

void EadFile::setDirty()
{
	m_bDirty = true;
	emit dirtyChanged();
}
