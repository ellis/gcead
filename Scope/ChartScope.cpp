#include "ChartScope.h"

#include <QtDebug>

#include <Check.h>
#include <EadEnums.h>
#include <Globals.h>
#include <IdacDriver/IdacSettings.h>
#include <PublisherSettings.h>
#include <Utils.h>
#include <ViewSettings.h>
#include <WaveInfo.h>


const double ChartScope::anSecondsPerDivision[] =
{
	0.1, 0.2, 0.3, 0.5, 0.7,
	1, 1.5, 2, 3, 5, 7,
	10, 15, 20, 30, 40, 50,
	60, 90, 2*60, 3*60, 5*60, 7*60,
	10*60, 15*60, 20*60, 30*60,
	0 // termination value
};


ChartScope::ChartScope(QObject* parent)
	: QObject(parent)
{
	m_pixmap = new ChartPixmap;

	m_params.file = NULL;
	m_params.view = NULL;
	m_params.task = EadTask_Review;
	m_params.peakMode = EadMarkerMode_Show;
	m_params.nPeakModeRecId = 0;

	m_iScrollMax = 0;
	m_iScrollValue = 0;
	m_nScrollPageStep = 1;
	m_nScrollSingleStep = 1;

	m_bRedraw = true;

	m_bRecording = false;
	m_nRecordingTime = 0;

	m_chartElements |= (ChartElement) 0xffffffff;

	setSecondsPerDivisionIndex(16); // 50s/div
}

ChartScope::~ChartScope()
{
	delete m_pixmap;
}

void ChartScope::setFile(EadFile* file)
{
	if (file != m_params.file)
	{
		if (m_params.file != NULL)
			disconnect(m_params.file);

		m_params.file = file;

		if (file != NULL)
			connect(file, SIGNAL(waveListChanged()), this, SLOT(emitParamsChanged()));

		// Zoom full if data available
		if (file != NULL && file->recs().count() > 1)
			zoomFull();
		// Otherwise
		else
			setSecondsPerDivisionMin(50);

		emitParamsChanged();
	}
}

void ChartScope::setView(ViewInfo* view)
{
	if (view != m_params.view)
	{
		if (m_params.view != NULL)
			m_params.view->disconnect(this);

		m_params.view = view;

		if (view != NULL)
			connect(view, SIGNAL(changed(ViewChangeEvents)), this, SLOT(on_view_changed(ViewChangeEvents)));

		emitParamsChanged();
	}
}

void ChartScope::setTask(EadTask task)
{
	if (task != m_params.task)
	{
		m_params.task = task;
		emitParamsChanged();
	}
}

void ChartScope::setPeakMode(EadMarkerMode peakMode)
{
	if (peakMode != m_params.peakMode)
	{
		m_params.peakMode = peakMode;
		emitParamsChanged();
	}
}

void ChartScope::setPeakModeRecId(int id)
{
	if (id != m_params.nPeakModeRecId)
	{
		m_params.nPeakModeRecId = id;
		emitParamsChanged();
	}
}

void ChartScope::setChartElement(ChartElement e, bool b)
{
	if (b != m_chartElements.testFlag(e)) {
		if (b) m_chartElements |= e;
		else m_chartElements &= ~e;
		emitParamsChanged();
	}
}

void ChartScope::setChartElementFidPeak(ChartElementFidPeak e, bool b)
{
	if (b != m_chartElementFidPeaks.testFlag(e)) {
		if (b) m_chartElementFidPeaks |= e;
		else m_chartElementFidPeaks &= ~e;
		emitParamsChanged();
	}
}

void ChartScope::setChartElementEadPeak(ChartElementEadPeak e, bool b)
{
	if (b != m_chartElementEadPeaks.testFlag(e)) {
		if (b) m_chartElementEadPeaks |= e;
		else m_chartElementEadPeaks &= ~e;
		emitParamsChanged();
	}
}

void ChartScope::setChartElementTimeMarker(ChartElementTimeMarker e, bool b)
{
	if (b != m_chartElementTimeMarkers.testFlag(e)) {
		if (b) m_chartElementTimeMarkers |= e;
		else m_chartElementTimeMarkers &= ~e;
		emitParamsChanged();
	}
}

void ChartScope::setRecordingOn(bool b)
{
	if (b != m_bRecording) {
		m_bRecording = b;

		// The scroll region is larger when recording
		updateScrollbar();
		// If we're turning recording on, make sure the recording time starts at 0
		if (b == true)
			setRecordingTime(0);
		// When turning recording off, we need to re-adjust the scroll position
		// to make sure that the user isn't scrolled past the data.
		else
			setSampleOffset(m_params.nSampleOffset);

		// HACK: This is done because sometime when we [Discard] a recording and then [Record] a new one,
		// we might get the same WaveInfo address allocated to us again, which would make ChartPixmap think
		// that it could still use the old RenderData. -- ellis, 2008-06-23
		m_pixmap->clearRenderData();
		emit recordingLabelVisibleChanged(b);
	}
}

void ChartScope::setRecordingTime(int nSeconds)
{
	m_nRecordingTime = nSeconds;

	QString sFormat;
	double nMinutes;
	/*if (Globals->idacSettings()->nRecordingDuration > 0) {
		sFormat = "REMAINING: %1";
		nMinutes = Globals->idacSettings()->nRecordingDuration - nSeconds / 60.0;
	}
	else*/
	{
		sFormat = "RECORDING: %1";
		nMinutes = nSeconds / 60.0;
	}

	QString s = sFormat.arg(nMinutes, 0, 'f', 2);
	if (s != m_sRecordingText) {
		m_sRecordingText = s;
		emit recordingLabelTextChanged(m_sRecordingText);
	}
}

QString ChartScope::timebaseString() const
{
	return timestampString(m_params.nSecondsPerDivision);
}

void ChartScope::setSampleOffset(int nSampleOffset)
{
	if (nSampleOffset > m_iScrollMax)
		nSampleOffset = m_iScrollMax;
	if (nSampleOffset < 0)
		nSampleOffset = 0;

	if (nSampleOffset != m_params.nSampleOffset)
	{
		m_params.nSampleOffset = nSampleOffset;
		emitParamsChanged();
	}
}

void ChartScope::setSecondsPerDivisionIndex(int i)
{
	if (i >= 0 && anSecondsPerDivision[i] > 0)
	{
		m_iSecondsPerDivision = i;
		m_params.nSecondsPerDivision = anSecondsPerDivision[i];
		emitParamsChanged();
		emit timebaseChanged(timebaseString());
		updateScrollbar();
	}
	// FIXME: From ChartWidget, call this: updateStatus();
}

bool ChartScope::setSecondsPerDivisionMin(double nMin)
{
	// Find next larger valid value:
	for (int i = 0; anSecondsPerDivision[i] > 0; i++)
	{
		if (anSecondsPerDivision[i] >= nMin)
		{
			setSecondsPerDivisionIndex(i);
			return true;
		}
	}

	return false;
}

void ChartScope::center(int iSample)
{
	int nSamplesToCenter = int(m_params.nSecondsPerDivision * m_pixmap->params().nCols * EAD_SAMPLES_PER_SECOND / 2);
	int n = qMax(iSample - nSamplesToCenter, 0);
	setSampleOffset(n);
}

int ChartScope::sampleCount()
{
	int nSamples = 0;

	// First look in the current view
	if (m_params.view != NULL)
	{
		foreach (ViewWaveInfo* vwi, m_params.view->vwis())
		{
			const WaveInfo* wave = vwi->wave();
			CHECK_ASSERT_NORET(wave != NULL);
			if (wave != NULL) {
				int n = wave->display.size() + wave->shift();
				if (n > nSamples)
					nSamples = n;
			}
		}
	}
	// If the current view is empty, look in the whole file
	if (nSamples == 0 && m_params.file != NULL)
	{
		foreach (RecInfo* rec, m_params.file->recs())
		{
			foreach (const WaveInfo* wave, rec->waves())
			{
				int n = wave->display.size() + wave->shift();
				if (n > nSamples)
					nSamples = n;
			}
		}
	}

	return nSamples;
}

void ChartScope::zoomOut()
{
	int iSample = m_pixmap->centerSample();

	setSecondsPerDivisionIndex(m_iSecondsPerDivision + 1);
	center(iSample);

	emitParamsChanged();
}

void ChartScope::zoomIn()
{
	if (m_iSecondsPerDivision > 0)
	{
		int iSample = m_pixmap->centerSample();

		setSecondsPerDivisionIndex(m_iSecondsPerDivision - 1);

		if (iSample < sampleCount())
			center(iSample);
	}
}

void ChartScope::zoomFull()
{
	double nSamples = sampleCount();
	double nSeconds = nSamples / EAD_SAMPLES_PER_SECOND;
	double nSecondsPerDivision = nSeconds / m_pixmap->params().nCols;
	setSecondsPerDivisionMin(nSecondsPerDivision);

	setSampleOffset(0);

	emitParamsChanged();
}

void ChartScope::scroll(int nSamplesDiff)
{
	int i = m_params.nSampleOffset + nSamplesDiff;
	setSampleOffset(i);
}

void ChartScope::scrollDivs(int nDivs) { scroll(nDivs * m_nScrollSingleStep); }
void ChartScope::scrollDivLeft() { scroll(-m_nScrollSingleStep); }
void ChartScope::scrollDivRight() { scroll(m_nScrollSingleStep); }
void ChartScope::scrollPageLeft() { scroll(-m_nScrollPageStep); }
void ChartScope::scrollPageRight() { scroll(m_nScrollPageStep); }

static int calcPrecision(double nMinutes)
{
	int nPrecision;
	if (nMinutes >= 1)
		nPrecision = 1;
	else if (nMinutes >= 0.1)
		nPrecision = 2;
	else
		nPrecision = 3;
	return nPrecision;
}

void ChartScope::setSelectionRange(int iSampleStart, int iSampleEnd)
{
	int nSamples = qAbs(iSampleEnd - iSampleStart);

	double nMinutes0 = double(iSampleStart) / (EAD_SAMPLES_PER_SECOND * 60);
	double nMinutes1 = double(iSampleEnd) / (EAD_SAMPLES_PER_SECOND * 60);
	double nMinutes = double(nSamples) / (EAD_SAMPLES_PER_SECOND * 60);

	int nPrecision = calcPrecision(nMinutes / 10);

	QString s = tr("Selecting: %0 to %1").arg(nMinutes0, 0, 'f', nPrecision).arg(nMinutes1, 0, 'f', nPrecision);;
	emit statusTextChanged(s);
}

void ChartScope::setMousePosition(int tidx, double nAmplitude)
{
	double nSeconds = double(tidx) / EAD_SAMPLES_PER_SECOND;
	QString sMinute = timestampString(nSeconds, TimestampBase_Minutes);
	QString s;
	if (nAmplitude != 0) {
		QString sVolt = QObject::tr("%0 mV");
		/*if (nAmplitude >= 10)
			sVolt = sVolt.arg(nAmplitude, 0, 'f', 0);
		else if (nAmplitude >= 1)
			sVolt = sVolt.arg(nAmplitude, 0, 'g', 3);
		else
			sVolt = sVolt.arg(nAmplitude, 0, 'g', 2);
			*/
		sVolt = sVolt.arg(nAmplitude, 0, 'f', 3);
		s = "Pos: " + sVolt + ", " + sMinute;
	}
	else {
		s = "Pos: " + sMinute;
	}

	emit statusTextChanged(s);
}

void ChartScope::setHilight(ViewWaveInfo* vwi)
{
	if (vwi != m_params.vwiHilight)
	{
		m_params.vwiHilight = vwi;
		emitParamsChanged();
	}
}

void ChartScope::redraw()
{
	emitParamsChanged();
}

const ChartPixmap* ChartScope::draw(const QSize& sz)
{
	if (m_params.size != sz)
		m_bRedraw = true;

	if (m_bRedraw)
	{
		m_bRedraw = false;

		m_params.size = sz;
		//m_params.nSampleOffset = m_nSampleOffset;

		m_params.nCols = 0;

		m_params.elements = m_chartElements;
		m_params.fidMarkerElements = m_chartElementFidPeaks;
		m_params.eadMarkerElements = m_chartElementEadPeaks;
		m_params.timeMarkerElements = m_chartElementTimeMarkers;
		if (m_params.task == EadTask_Publish)
		{
			PublisherSettings* pub = Globals->publisherSettings();
			ChartElements mask = ChartElement_Grid | ChartElement_AxisTime | ChartElement_WaveNames | ChartElement_StdDev;
			ChartElements e = pub->publisherChartElements;
			m_params.elements &= ~mask | e;
			if (pub->bPublishCols)
				m_params.nCols = pub->nPublishCols;
		}

		m_pixmap->draw(m_params);

		updateScrollbar();
	}

	return m_pixmap;
}

void ChartScope::updateScrollbar()
{
	int nPageStep = int(m_params.nSecondsPerDivision * EAD_SAMPLES_PER_SECOND * m_pixmap->params().nCols);
	int nSingleStep = nPageStep / m_pixmap->params().nCols;

	// Total number of samples in the dataset
	int nSamples = sampleCount();

	int iMax = nSamples;
	// In recording view, we can scroll so that the last sample is all the way on the left.
	// Actually, ChartScope doesn't know if which view is active, so I'm just checking
	// whether recording is on. -- ellis, 2010-03-27
	if (!m_bRecording)
		iMax -= nPageStep;
	if (iMax < 0)
		iMax = m_params.nSampleOffset;

	if (iMax != m_iScrollMax)
	{
		m_iScrollMax = iMax;
		emit scrollMaxChanged(m_iScrollMax);
	}
	if (nPageStep != m_nScrollPageStep)
	{
		m_nScrollPageStep = nPageStep;
		emit scrollPageStepChanged(m_nScrollPageStep);
	}
	if (nSingleStep != m_nScrollSingleStep)
	{
		m_nScrollSingleStep = nSingleStep;
		emit scrollSingleStepChanged(m_nScrollSingleStep);
	}
	if (m_params.nSampleOffset != m_iScrollValue)
	{
		m_iScrollValue = m_params.nSampleOffset;
		emit scrollValueChanged(m_iScrollValue);
	}
}

void ChartScope::emitParamsChanged()
{
	m_bRedraw = true;
	emit paramsChanged();
}

void ChartScope::on_view_changed(ViewChangeEvents events)
{
	if ((events & ViewChangeEvent_Render) != 0)
		m_pixmap->clearRenderData();
	emitParamsChanged();
}
