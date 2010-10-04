#include "TaskFilterWidget.h"

#include <QComboBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include <Scope/MainScope.h>


TaskFilterWidget::TaskFilterWidget(MainScope* scope, QWidget *parent) :
	TaskWidget(parent),
	m_scope(scope)
{
	m_filterEad = NULL;
	m_filterFid = NULL;
	setupWidgets();
	connect(m_scope, SIGNAL(fileChanged(EadFile*)), this, SLOT(on_scope_fileChanged()));
}

void TaskFilterWidget::setupWidgets()
{
	QPushButton* btnEad;
	QPushButton* btnFid;
//	m_rdoOff = new QRadioButton(tr("No filter"));
//	m_rdoDefault = new QRadioButton(tr("Default filter"));
//	m_rdoAdvanced = new QRadioButton(tr("Advanced filter setup"));
	m_cmbEad = new QComboBox();
	m_edtEadProperties = new QPlainTextEdit();
	btnEad = new QPushButton("Run Algorithm");
	m_cmbFid = new QComboBox();
	m_edtFidProperties = new QPlainTextEdit();
	btnFid = new QPushButton("Run Algorithm");

	// Get current EAD and FID filters
	on_scope_fileChanged();

	for (int i = 0; i <= m_filterEad->filterCount(); i++) {
		m_cmbEad->addItem(QString::number(i));
	}
	m_cmbEad->setCurrentIndex(0);
	for (int i = 0; i <= m_filterFid->filterCount(); i++) {
		m_cmbFid->addItem(QString::number(i));
	}

	QVBoxLayout* vbox = new QVBoxLayout();

	vbox->addWidget(new QLabel("EAD Filter ID:"));
	vbox->addWidget(m_cmbEad);
	vbox->addWidget(m_edtEadProperties);
	vbox->addWidget(btnEad);

	vbox->addWidget(new QLabel("FID Filter ID:"));
	vbox->addWidget(m_cmbFid);
	vbox->addWidget(m_edtFidProperties);
	vbox->addWidget(btnFid);

	setLayout(vbox);

	updateEditWidget(m_filterEad, m_edtEadProperties);
	updateEditWidget(m_filterFid, m_edtFidProperties);

	connect(m_cmbEad, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cmbEad_currentIndexChanged()));
	connect(btnEad, SIGNAL(clicked()), this, SLOT(on_btnEad_clicked()));
	connect(m_cmbFid, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cmbFid_currentIndexChanged()));
	connect(btnFid, SIGNAL(clicked()), this, SLOT(on_btnFid_clicked()));
}

void TaskFilterWidget::on_scope_fileChanged()
{
	foreach (FilterTesterInfo* filter, m_scope->file()->filters()) {
		if (filter->waveType() == WaveType_EAD)
			m_filterEad = filter;
		else if (filter->waveType() == WaveType_FID)
			m_filterFid = filter;
	}
}

void TaskFilterWidget::on_cmbEad_currentIndexChanged()
{
	int id = m_cmbEad->currentIndex();
	m_filterEad->setFilterId(id);
	updateEditWidget(m_filterEad, m_edtEadProperties);
}

void TaskFilterWidget::on_btnEad_clicked()
{
	run(m_filterEad, m_edtEadProperties->toPlainText());
}

void TaskFilterWidget::on_cmbFid_currentIndexChanged()
{
	int id = m_cmbFid->currentIndex();
	m_filterFid->setFilterId(id);
	updateEditWidget(m_filterFid, m_edtFidProperties);
}

void TaskFilterWidget::on_btnFid_clicked()
{
	run(m_filterFid, m_edtFidProperties->toPlainText());
}

void TaskFilterWidget::updateEditWidget(FilterTesterInfo* filter, QPlainTextEdit* edt)
{
	const QVariantMap& mapProperties = filter->properties(filter->filterId());
	QStringList asVarVals;
	foreach (QString sVar, mapProperties.keys()) {
		asVarVals << sVar + "=" + mapProperties[sVar].toString();
	}
	QString sProperties = asVarVals.join(", ");
	edt->setPlainText(sProperties);
}

void TaskFilterWidget::run(FilterTesterInfo* filter, QString sProperties)
{
	QStringList asVarVals = sProperties.split(",");
	foreach (QString sVarVal, asVarVals) {
		QStringList as = sVarVal.split("=");
		if (as.size() == 2) {
			QString sVar = as[0].trimmed();
			QString sVal = as[1].trimmed();
			QVariantMap& mapProperties = filter->properties(filter->filterId());
			if (mapProperties.contains(sVar))
				mapProperties.insert(sVar, sVal);
		}
	}
	m_scope->file()->updateDisplay();
	m_scope->file()->updateAveWaves();
	m_scope->chart()->redraw(); // NOTE: This doesn't work, because there is a check in ChartPixmap::drawWaveform() which doesn't re-render waveforms if their position hasn't changed -- ellis, 2010-10-04
}
