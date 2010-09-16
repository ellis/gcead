#include "TaskMarkersWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include <MainScope.h>


TaskMarkersWidget::TaskMarkersWidget(MainScope* scope, QWidget *parent) :
    TaskWidget(parent)
{
	m_scope = scope;

	setupWidgets();
}

void TaskMarkersWidget::setupWidgets()
{
	m_chkShowMarkers = new QCheckBox(tr("Show markers"));
	m_grpFid = new QGroupBox(tr("FID"));
	m_chkShowTime = new QCheckBox(tr("Show time"));
	m_chkShowArea = new QCheckBox(tr("Show area"));
	m_chkAutoDetect = new QCheckBox(tr("Auto-detect peaks"));
	m_grpEad = new QGroupBox(tr("EAD"));
	m_chkShowAmplitude = new QCheckBox(tr("Show amplitude"));

	QVBoxLayout* vbox;

	vbox = new QVBoxLayout();
	vbox->addWidget(m_chkShowTime);
	vbox->addWidget(m_chkShowArea);
	vbox->addWidget(m_chkAutoDetect);
	m_grpFid->setLayout(vbox);

	vbox = new QVBoxLayout();
	vbox->addWidget(m_chkShowAmplitude);
	m_grpEad->setLayout(vbox);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_chkShowMarkers);
	layout->addWidget(m_grpFid);
	layout->addWidget(m_grpEad);
	layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
	setLayout(layout);

	Actions* actions = m_scope->actions();

	m_chkShowMarkers->setChecked(!actions->markersHide->isChecked());
	m_chkShowTime->setChecked(actions->markersShowTime->isChecked());
	m_chkShowArea->setChecked(actions->markersShowFidArea->isChecked());
	m_chkShowAmplitude->setChecked(actions->markersShowEadAmplitude->isChecked());
	m_chkAutoDetect->setChecked(true);

	connect(m_chkShowMarkers, SIGNAL(toggled(bool)), this, SLOT(on_chkShowMarkers_toggled()));
	//m_chkShowTime->addAction(m_scope->actions()->markersShowTime);
	connect(m_chkShowTime, SIGNAL(toggled(bool)), actions->markersShowTime, SLOT(setChecked(bool)));
	connect(m_chkShowArea, SIGNAL(toggled(bool)), actions->markersShowFidArea, SLOT(setChecked(bool)));
	connect(m_chkShowAmplitude, SIGNAL(toggled(bool)), actions->markersShowEadAmplitude, SLOT(setChecked(bool)));
	connect(actions->markersShowTime, SIGNAL(toggled(bool)), m_chkShowTime, SLOT(setChecked(bool)));
	connect(actions->markersShowFidArea, SIGNAL(toggled(bool)), m_chkShowArea, SLOT(setChecked(bool)));
	connect(actions->markersShowEadAmplitude, SIGNAL(toggled(bool)), m_chkShowAmplitude, SLOT(setChecked(bool)));
}

void TaskMarkersWidget::on_chkShowMarkers_toggled()
{
	m_scope->actions()->markersHide->setChecked(!m_chkShowMarkers->isChecked());
}

void TaskMarkersWidget::on_actions_markersHide_toggled()
{
	bool b = !m_scope->actions()->markersHide->isChecked();
	m_chkShowMarkers->setCheckable(b);
	m_grpFid->setEnabled(b);
	m_grpEad->setEnabled(b);
}
