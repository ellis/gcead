#ifndef TASKMARKERWIDGET_H
#define TASKMARKERWIDGET_H

#include "TaskWidget.h"

class QCheckBox;
class QComboBox;
class QGroupBox;

class MainScope;


class TaskMarkersWidget : public TaskWidget
{
    Q_OBJECT
public:
	TaskMarkersWidget(MainScope* scope, QWidget *parent = 0);

signals:

public slots:

private:
	void setupWidgets();

private slots:
	void on_chkAutoDetect_toggled();
	void on_cmbPeakFid_activated();
	void on_actions_markersShow_toggled();
	void on_scope_peakModeChanged();
	void updateCmbPeakFid();

private:
	MainScope* m_scope;

	QCheckBox* m_chkShowMarkers;
	QGroupBox* m_grpFid;
	QCheckBox* m_chkShowTime;
	QCheckBox* m_chkShowArea;
	QCheckBox* m_chkAutoDetect;
	QComboBox* m_cmbPeakFid;
	QGroupBox* m_grpEad;
	QCheckBox* m_chkShowAmplitude;

	int m_idAutoDetect;
};

#endif // TASKMARKERWIDGET_H
