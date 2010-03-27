#ifndef CHARTSCOPE_H
#define CHARTSCOPE_H

#include <QObject>

#include <ChartPixmap.h>


class ViewWaveInfo;


class ChartScope : public QObject
{
	Q_OBJECT
public:
	static const double anSecondsPerDivision[];

public:
	ChartScope(QObject* parent = NULL);
	~ChartScope();

	void setFile(EadFile* file);
	void setView(ViewInfo* view);
	void setTask(EadTask task);
	void setPeakMode(EadMarkerMode peakMode);
	void setPeakModeRecId(int id);
	void setRecordingOn(bool b);
	void setRecordingTime(int nSeconds);

	const ChartPixmapParams& params() { return m_params; }
	const ChartPixmap* pixmap() { return m_pixmap; }
	EadFile* file() { return m_params.file; }

	QString timebaseString() const;
	int sampleOffset() const { return m_params.nSampleOffset; }
	double secondsPerDivision() const { return m_params.nSecondsPerDivision; }

	bool isRedrawRequired() const { return m_bRedraw; }
	bool isRecording() const { return m_bRecording; }
	/// Get the recording time in seconds
	int recordingTime() const { return m_nRecordingTime; }

	/// Used during recording to set the sample offset without checking for scroll limits
	void forceSampleOffset(int nSampleOffset);
	void setSecondsPerDivisionIndex(int i);
	/// Set seconds per division using the minimum value @param nMin
	bool setSecondsPerDivisionMin(double nMin);
	/// Center the waveform display around iSample
	void center(int iSample);

	/// Set the wave show be drawn on top of all others
	void setHilight(ViewWaveInfo* vwi);

	int sampleCount();

	const ChartPixmap* draw(const QSize& sz);

signals:
	void paramsChanged();
	void timebaseChanged(const QString& s);
	void statusTextChanged(const QString& s);
	void recordingLabelVisibleChanged(bool bVisible);
	void recordingLabelTextChanged(const QString& sText);
	void scrollMaxChanged(int iScrollMax);
	void scrollPageStepChanged(int nScrollPageStep);
	void scrollSingleStepChanged(int nScrollSingleStep);
	void scrollValueChanged(int iScrollValue);

public slots:
	void setSampleOffset(int nSampleOffset);
	void redraw();
	void zoomOut();
	void zoomIn();
	void zoomFull();
	void scroll(int nSamplesDiff);
	void scrollDivs(int nDivs);
	void scrollDivLeft();
	void scrollDivRight();
	void scrollPageLeft();
	void scrollPageRight();
	void setSelectionRange(int iSampleStart, int iSampleEnd);
	void setMousePosition(int iSample);

private:
	void updateScrollbar();

private slots:
	void emitParamsChanged();
	void on_view_changed(ViewChangeEvents events);

private:
	ChartPixmapParams m_params;
	ChartPixmap* m_pixmap;

	bool m_bRedraw;
	//QPointer<EadFile> m_file;
	//QPointer<ViewInfo> m_view;
	//EadTask m_task;
	//EadMarkerMode m_peakMode;

	/// Scroll offset
	//int m_nSampleOffset;
	/// Index into array of valid seconds per division settings
	int m_iSecondsPerDivision;
	/// Seconds per division, for convenience
	//double m_nSecondsPerDivision;

	int m_iScrollMax;
	int m_nScrollPageStep;
	int m_nScrollSingleStep;
	int m_iScrollValue;

	bool m_bRecording;
	int m_nRecordingTime;
	QString m_sRecordingText;
};

#endif // CHARTSCOPE_H
