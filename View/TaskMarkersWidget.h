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
	void on_chkShowMarkers_toggled();
	void on_actions_markersHide_toggled();

private:
	MainScope* m_scope;

	QCheckBox* m_chkShowMarkers;
	QGroupBox* m_grpFid;
	QCheckBox* m_chkShowTime;
	QCheckBox* m_chkShowArea;
	QCheckBox* m_chkAutoDetect;
	QGroupBox* m_grpEad;
	QCheckBox* m_chkShowAmplitude;
};

#endif // TASKMARKERWIDGET_H
