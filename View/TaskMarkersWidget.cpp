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
	QGroupBox* fid = new QGroupBox(tr("FID"));
	m_chkShowTime = new QCheckBox(tr("Show time"));
	m_chkShowArea = new QCheckBox(tr("Show area"));
	m_chkAutoDetect = new QCheckBox(tr("Auto-detect peaks"));
	QGroupBox* ead = new QGroupBox(tr("EAD"));
	m_chkShowAmplitude = new QCheckBox(tr("Show amplitude"));

	QVBoxLayout* vbox;

	vbox = new QVBoxLayout();
	vbox->addWidget(m_chkShowTime);
	vbox->addWidget(m_chkShowArea);
	vbox->addWidget(m_chkAutoDetect);
	fid->setLayout(vbox);

	vbox = new QVBoxLayout();
	vbox->addWidget(m_chkShowAmplitude);
	ead->setLayout(vbox);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_chkShowMarkers);
	layout->addWidget(fid);
	layout->addWidget(ead);
	layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
	setLayout(layout);

	connect(m_chkShowMarkers, SIGNAL(toggled(bool)), this, SLOT(on_chkShowMarkers_toggled()));
	//m_chkShowTime->addAction(m_scope->actions()->markersShowTime);
}

void TaskMarkersWidget::on_chkShowMarkers_toggled()
{
	m_scope->actions()->markersHide->setChecked(!m_chkShowMarkers->isChecked());
}
