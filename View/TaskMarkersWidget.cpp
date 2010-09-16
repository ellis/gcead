#include "TaskMarkersWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include <Check.h>
#include <MainScope.h>


TaskMarkersWidget::TaskMarkersWidget(MainScope* scope, QWidget *parent) :
    TaskWidget(parent)
{
	m_scope = scope;

	m_idAutoDetect = -1;

	setupWidgets();
}

void TaskMarkersWidget::setupWidgets()
{
	m_chkShowMarkers = new QCheckBox(tr("Show markers"));
	m_grpFid = new QGroupBox(tr("FID"));
	m_chkShowTime = new QCheckBox(tr("Show time"));
	m_chkShowArea = new QCheckBox(tr("Show area"));
	m_chkAutoDetect = new QCheckBox(tr("Auto-detect peaks"));
	m_cmbPeakFid = new QComboBox();
	m_grpEad = new QGroupBox(tr("EAD"));
	m_chkShowAmplitude = new QCheckBox(tr("Show amplitude"));

	QVBoxLayout* vbox;

	vbox = new QVBoxLayout();
	vbox->addWidget(m_chkShowTime);
	vbox->addWidget(m_chkShowArea);
	vbox->addWidget(m_chkAutoDetect);
	vbox->addWidget(m_cmbPeakFid);//, 0, Qt::AlignHCenter);
	//m_cmbPeakFid->setStyleSheet("padding-left: 20px");
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

	m_cmbPeakFid->setToolTip(tr("Select an FID wave for peak editing"));

	connect(m_chkShowMarkers, SIGNAL(toggled(bool)), this, SLOT(on_chkShowMarkers_toggled()));
	//m_chkShowTime->addAction(m_scope->actions()->markersShowTime);
	connect(m_chkShowTime, SIGNAL(toggled(bool)), actions->markersShowTime, SLOT(setChecked(bool)));
	connect(m_chkShowArea, SIGNAL(toggled(bool)), actions->markersShowFidArea, SLOT(setChecked(bool)));
	connect(m_chkAutoDetect, SIGNAL(toggled(bool)), this, SLOT(on_chkAutoDetect_toggled()));
	connect(m_cmbPeakFid, SIGNAL(activated(int)), this, SLOT(on_cmbPeakFid_activated()));
	connect(m_chkShowAmplitude, SIGNAL(toggled(bool)), actions->markersShowEadAmplitude, SLOT(setChecked(bool)));
	connect(actions->markersShowTime, SIGNAL(toggled(bool)), m_chkShowTime, SLOT(setChecked(bool)));
	connect(actions->markersShowFidArea, SIGNAL(toggled(bool)), m_chkShowArea, SLOT(setChecked(bool)));
	connect(actions->markersShowEadAmplitude, SIGNAL(toggled(bool)), m_chkShowAmplitude, SLOT(setChecked(bool)));

	connect(m_scope, SIGNAL(fileChanged(EadFile*)), this, SLOT(updateCmbPeakFid()));
	connect(m_scope, SIGNAL(taskTypeChanged(EadTask)), this, SLOT(updateCmbPeakFid()));
	connect(m_scope, SIGNAL(viewTypeChanged(EadView)), this, SLOT(updateCmbPeakFid()));
	connect(m_scope, SIGNAL(waveListChanged()), this, SLOT(updateCmbPeakFid()));
	connect(m_scope, SIGNAL(peakModeChanged(EadMarkerMode)), this, SLOT(on_scope_peakModeChanged()));
}

void TaskMarkersWidget::on_chkShowMarkers_toggled()
{
	m_scope->actions()->markersHide->setChecked(!m_chkShowMarkers->isChecked());
}

void TaskMarkersWidget::on_chkAutoDetect_toggled()
{
	bool b = m_chkAutoDetect->isChecked();
	m_cmbPeakFid->setEnabled(b);
	if (b) {
		updateCmbPeakFid();
		on_cmbPeakFid_activated();
	}
	else
		m_scope->setPeakModeRecId(-1);
}

void TaskMarkersWidget::on_cmbPeakFid_activated()
{
	int iItem = m_cmbPeakFid->currentIndex();
	m_idAutoDetect = m_cmbPeakFid->itemData(iItem).toInt();
	if (m_idAutoDetect >= 0)
		m_scope->setPeakModeRecId(m_idAutoDetect);
}

void TaskMarkersWidget::on_actions_markersHide_toggled()
{
	bool b = !m_scope->actions()->markersHide->isChecked();
	m_chkShowMarkers->setCheckable(b);
	m_grpFid->setEnabled(b);
	m_grpEad->setEnabled(b);
}

void TaskMarkersWidget::on_scope_peakModeChanged()
{
	updateCmbPeakFid();
}

// REFACTOR: Should be moved to MainScope
void TaskMarkersWidget::updateCmbPeakFid()
{
	m_cmbPeakFid->clear();
	int iCmbPeakFid = -1;

	int id;
	if (m_scope->peakModeRecId() >= 0)
		id = m_scope->peakModeRecId();
	else
		id = m_idAutoDetect;

	if (m_scope->file() != NULL)
	{
		ViewInfo* view = m_scope->file()->viewInfo(m_scope->viewType());

		QList<ViewWaveInfo*> vwis;
		vwis << view->vwis() << view->vwiExtras();
		if (view->vwiUser.wave() != NULL)
			vwis << &view->vwiUser;
		foreach (ViewWaveInfo* vwi, vwis)
		{
			CHECK_ASSERT_RET(vwi->wave() != NULL);
			const WaveInfo* wave = vwi->wave();

			// Add all visible FIDs to m_cmbPeakFid
			if (wave->type == WaveType_FID)
			{
				m_cmbPeakFid->addItem(wave->sName, wave->recId());
				if (wave->recId() == id)
					iCmbPeakFid = m_cmbPeakFid->count() - 1;
			}
		}
	}

	// If the selected peak FID isn't on the current view, add a blank item to the combobox
	if (iCmbPeakFid < 0)
	{
		iCmbPeakFid = 0;
		if (id >= 0)
			m_cmbPeakFid->insertItem(0, "", -1);
	}
	m_cmbPeakFid->setCurrentIndex(iCmbPeakFid);
}
