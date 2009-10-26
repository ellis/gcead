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
	void setPeakMode(EadPeakMode peakMode);
	void setPeakModeRecId(int id);
	void setRecordingOn(bool b);

	const ChartPixmapParams& params() { return m_params; }
	const ChartPixmap* pixmap() { return m_pixmap; }
	EadFile* file() { return m_params.file; }

	QString timebaseString() const;
	int sampleOffset() const { return m_nSampleOffset; }
	double secondsPerDivision() const { return m_nSecondsPerDivision; }

	bool isRedrawRequired() const { return m_bRedraw; }

	void setSecondsPerDivisionIndex(int i);
	/// Set seconds per division using the minimum value @param nMin
	bool setSecondsPerDivisionMin(double nMin);
	/// Center the waveform display around iSample
	void center(int iSample);

	/// Set the wave show be drawn on top of all others
	void setHilight(ViewWaveInfo* vwi);

	int sampleCount();

	void redraw();
	const ChartPixmap* draw(const QSize& sz);

signals:
	void paramsChanged();
	void timebaseChanged();
	void statusTextChanged(const QString& s);
	void recordingLabelVisibleChanged(bool bVisible);
	void scrollMaxChanged(int iScrollMax);
	void scrollPageStepChanged(int nScrollPageStep);
	void scrollSingleStepChanged(int nScrollSingleStep);
	void scrollValueChanged(int iScrollValue);

public slots:
	void setSampleOffset(int nSampleOffset);
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
	//EadPeakMode m_peakMode;

	/// Scroll offset
	int m_nSampleOffset;
	/// Index into array of valid seconds per division settings
	int m_iSecondsPerDivision;
	/// Seconds per division, for convenience
	double m_nSecondsPerDivision;

	int m_iScrollMax;
	int m_nScrollPageStep;
	int m_nScrollSingleStep;
	int m_iScrollValue;
};

#endif // CHARTSCOPE_H
