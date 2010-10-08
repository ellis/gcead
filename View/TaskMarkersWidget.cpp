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
	m_grpFid = new QGroupBox(tr("Show FID Peak Markers"));
	m_chkShowFidTime = new QCheckBox(tr("Show time"));
	m_chkShowFidArea = new QCheckBox(tr("Show area"));
	m_chkAutoDetect = new QCheckBox(tr("Auto-detect peaks"));
	m_cmbPeakFid = new QComboBox();

	m_grpEad = new QGroupBox(tr("Show EAD Peak Markers"));
	m_chkShowEadAmplitude = new QCheckBox(tr("Show amplitude"));
	m_chkShowEadTimeSpans = new QCheckBox(tr("Show time spans"));
	m_chkShowEadTimeStamps = new QCheckBox(tr("Show time stamps"));

	m_chkTime = new QCheckBox(tr("Show time markers"));

	m_lblPeakTip = new QLabel(this);

	QVBoxLayout* vbox;

	vbox = new QVBoxLayout();
	vbox->addWidget(m_chkShowFidTime);
	vbox->addWidget(m_chkShowFidArea);
	vbox->addWidget(m_chkAutoDetect);
	vbox->addWidget(m_cmbPeakFid);//, 0, Qt::AlignHCenter);
	//m_cmbPeakFid->setStyleSheet("padding-left: 20px");
	m_grpFid->setLayout(vbox);

	vbox = new QVBoxLayout();
	vbox->addWidget(m_chkShowEadAmplitude);
	vbox->addWidget(m_chkShowEadTimeSpans);
	vbox->addWidget(m_chkShowEadTimeStamps);
	m_grpEad->setLayout(vbox);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_grpFid);
	layout->addWidget(m_grpEad);
	layout->addWidget(m_chkTime);
	layout->addWidget(m_lblPeakTip);
	layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
	setLayout(layout);

	Actions* actions = m_scope->actions();

	m_grpFid->setCheckable(true);
	m_grpFid->setChecked(actions->markersShowTimeMarkers->isChecked());
	m_chkShowFidTime->setChecked(actions->markersShowFidPeakTime->isChecked());
	m_chkShowFidArea->setChecked(actions->markersShowFidPeakArea->isChecked());
	m_chkAutoDetect->setChecked(true);
	m_cmbPeakFid->setToolTip(tr("Select an FID wave for peak editing"));

	m_grpEad->setCheckable(true);
	m_grpEad->setChecked(actions->markersShowEadPeakMarkers->isChecked());
	m_chkShowEadAmplitude->setChecked(actions->markersShowEadPeakAmplitude->isChecked());
	m_chkShowEadTimeSpans->setChecked(actions->markersShowEadPeakTimeSpans->isChecked());
	m_chkShowEadTimeStamps->setChecked(actions->markersShowEadPeakTimeStamps->isChecked());

	m_chkTime->setChecked(actions->markersShowTimeMarkers->isChecked());

	m_lblPeakTip->setText(tr(
			"<hr/>"
			"<center><b>Marker Tips</b></center>"
			"<p style='margin-top: .5em'><b>Verify auto-detected peak:</b><br/> Click on &quot;<span style='color:red'>Add</span>&quot;</p>"
			"<p style='margin-top: .5em'><b>Add peak marker:</b><br/> Ctrl-click wave</p>"
			"<p style='margin-top: .5em'><b>Add time marker:</b><br/> 1) Right-click wave<br/> 2) Select &quot;Add Time Marker&quot;</p>"
			"<p style='margin-top: .5em'><b>Adjust postition:</b><br/> Drag square handles</p>"
			"<p style='margin-top: .5em'><b>Remove marker:</b><br/> 1) Right-click marker<br/> 2) Select &quot;Remove&quot;</p>"
			"<p style='margin-top: .5em'><b>Add EAD peak end-point:</b><br/> 1) Right-click marker<br/> 2) Select &quot;Add End-Point&quot;</p>"
			));
	m_lblPeakTip->setWordWrap(true);
	m_lblPeakTip->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

	// FID -> action
	connect(m_grpFid, SIGNAL(toggled(bool)), actions->markersShowFidPeakMarkers, SLOT(setChecked(bool)));
	connect(m_chkShowFidTime, SIGNAL(toggled(bool)), actions->markersShowFidPeakTime, SLOT(setChecked(bool)));
	connect(m_chkShowFidArea, SIGNAL(toggled(bool)), actions->markersShowFidPeakArea, SLOT(setChecked(bool)));
	connect(m_chkAutoDetect, SIGNAL(toggled(bool)), this, SLOT(on_chkAutoDetect_toggled()));
	connect(m_cmbPeakFid, SIGNAL(activated(int)), this, SLOT(on_cmbPeakFid_activated()));

	// EAD -> action
	connect(m_grpEad, SIGNAL(toggled(bool)), actions->markersShowEadPeakMarkers, SLOT(setChecked(bool)));
	connect(m_chkShowEadAmplitude, SIGNAL(toggled(bool)), actions->markersShowEadPeakAmplitude, SLOT(setChecked(bool)));
	connect(m_chkShowEadTimeSpans, SIGNAL(toggled(bool)), actions->markersShowEadPeakTimeSpans, SLOT(setChecked(bool)));
	connect(m_chkShowEadTimeStamps, SIGNAL(toggled(bool)), actions->markersShowEadPeakTimeStamps, SLOT(setChecked(bool)));

	// Time -> action
	connect(m_chkTime, SIGNAL(toggled(bool)), actions->markersShowTimeMarkers, SLOT(setChecked(bool)));

	// Actions -> FID
	connect(actions->markersShowFidPeakMarkers, SIGNAL(toggled(bool)), m_grpFid, SLOT(setChecked(bool)));
	connect(actions->markersShowFidPeakTime, SIGNAL(toggled(bool)), m_chkShowFidTime, SLOT(setChecked(bool)));
	connect(actions->markersShowFidPeakArea, SIGNAL(toggled(bool)), m_chkShowFidArea, SLOT(setChecked(bool)));

	// Actions -> EAD
	connect(actions->markersShowEadPeakMarkers, SIGNAL(toggled(bool)), m_grpEad, SLOT(setChecked(bool)));
	connect(actions->markersShowEadPeakAmplitude, SIGNAL(toggled(bool)), m_chkShowEadAmplitude, SLOT(setChecked(bool)));
	connect(actions->markersShowEadPeakTimeSpans, SIGNAL(toggled(bool)), m_chkShowEadTimeSpans, SLOT(setChecked(bool)));
	connect(actions->markersShowEadPeakTimeStamps, SIGNAL(toggled(bool)), m_chkShowEadTimeStamps, SLOT(setChecked(bool)));

	// Actions -> Time
	connect(actions->markersShowTimeMarkers, SIGNAL(toggled(bool)), m_chkTime, SLOT(setChecked(bool)));

	connect(m_scope, SIGNAL(fileChanged(EadFile*)), this, SLOT(updateCmbPeakFid()));
	connect(m_scope, SIGNAL(taskTypeChanged(EadTask)), this, SLOT(updateCmbPeakFid()));
	connect(m_scope, SIGNAL(viewTypeChanged(EadView)), this, SLOT(updateCmbPeakFid()));
	connect(m_scope, SIGNAL(waveListChanged()), this, SLOT(updateCmbPeakFid()));
	connect(m_scope, SIGNAL(peakModeChanged(EadMarkerMode)), this, SLOT(on_scope_peakModeChanged()));
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
