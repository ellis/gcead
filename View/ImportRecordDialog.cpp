#include "ImportRecordDialog.h"

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

ImportRecordDialog::ImportRecordDialog(const QStringList& asNames, QWidget *parent) :
	QDialog(parent),
	m_asNames(asNames)
{
	setupWidgets();
}

void ImportRecordDialog::setupWidgets()
{
	QVBoxLayout* vbox = new QVBoxLayout();
	QGridLayout* grid = new QGridLayout();

	vbox->addWidget(new QLabel("Please categorize the signal(s) you wish to import."));
	vbox->addWidget(new QLabel("You may import a maximum of one signal per type."));

	int iRow = 0;
	grid->addWidget(new QLabel("Signal Name"), iRow, 0);
	grid->addWidget(new QLabel("EAD"), iRow, 1);
	grid->addWidget(new QLabel("FID"), iRow, 2);
	//grid->addWidget(new QLabel("Digital"), iRow, 3);
	grid->addWidget(new QLabel("Skip"), iRow, 4);
	iRow++;

	for (int i = 0; i < m_asNames.size(); i++) {
		QString sName = m_asNames[i];
		QLabel* lbl = new QLabel(sName);
		grid->addWidget(lbl, iRow, 0);

		QRadioButton* rdo;
		QButtonGroup* grp = new QButtonGroup(this);

		rdo = new QRadioButton();
		grp->addButton(rdo);
		grid->addWidget(rdo, iRow, 1);
		m_eads << rdo;
		connect(rdo, SIGNAL(toggled(bool)), this, SLOT(on_ead()));

		rdo = new QRadioButton();
		grp->addButton(rdo);
		grid->addWidget(rdo, iRow, 2);
		m_fids << rdo;
		connect(rdo, SIGNAL(toggled(bool)), this, SLOT(on_fid()));

		/*rdo = new QRadioButton();
		grp->addButton(rdo);
		grid->addWidget(rdo, iRow, 3);
		m_digs << rdo;
		connect(rdo, SIGNAL(toggled(bool)), this, SLOT(on_dig()));
		*/
		rdo = new QRadioButton();
		rdo->setChecked(true);
		grp->addButton(rdo);
		grid->addWidget(rdo, iRow, 4);
		m_skps << rdo;
		connect(rdo, SIGNAL(toggled(bool)), this, SLOT(on_skp()));

		iRow++;
	}

	vbox->addLayout(grid);
	vbox->setAlignment(grid, Qt::AlignLeft);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
	vbox->addWidget(buttons);

	setLayout(vbox);
}

void ImportRecordDialog::updateEnabled()
{
	m_map.clear();
	updateEnabled(m_eads, WaveType_EAD);
	updateEnabled(m_fids, WaveType_FID);
	//updateEnabled(m_digs, WaveType_Digital);
}

void ImportRecordDialog::updateEnabled(const QList<QRadioButton*>& rdos, WaveType waveType) {
	int iRow = -1;
	for (int i = 0; i < rdos.size(); i++) {
		if (rdos[i]->isChecked()) {
			iRow = i;
			m_map.insert(m_asNames[i], waveType);
			break;
		}
	}

	for (int i = 0; i < rdos.size(); i++) {
		bool bEnabled = (iRow < 0 || i == iRow);
		rdos[i]->setEnabled(bEnabled);
	}
}

void ImportRecordDialog::on_ead() { updateEnabled(); }
void ImportRecordDialog::on_fid() { updateEnabled(); }
void ImportRecordDialog::on_dig() { updateEnabled(); }
void ImportRecordDialog::on_skp() { updateEnabled(); }
