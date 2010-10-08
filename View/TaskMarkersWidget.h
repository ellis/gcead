#ifndef TASKMARKERWIDGET_H
#define TASKMARKERWIDGET_H

#include "TaskWidget.h"

class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;

class MainScope;


class TaskMarkersWidget : public TaskWidget
{
    Q_OBJECT
public:
	TaskMarkersWidget(MainScope* scope, QWidget *parent = 0);

private:
	void setupWidgets();

private slots:
	void on_chkAutoDetect_toggled();
	void on_cmbPeakFid_activated();
	void on_scope_peakModeChanged();
	void updateCmbPeakFid();

private:
	MainScope* m_scope;

	QGroupBox* m_grpFid;
	QCheckBox* m_chkShowFidTime;
	QCheckBox* m_chkShowFidArea;
	QCheckBox* m_chkAutoDetect;
	QComboBox* m_cmbPeakFid;

	QGroupBox* m_grpEad;
	QCheckBox* m_chkShowEadAmplitude;
	QCheckBox* m_chkShowEadTimeSpans;
	QCheckBox* m_chkShowEadTimeStamps;

	QCheckBox* m_chkTime;

	QLabel* m_lblPeakTip;

	int m_idAutoDetect;
};

#endif // TASKMARKERWIDGET_H
