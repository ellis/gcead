#include "ImportEadDialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <EadFile.h>


ImportEadDialog::ImportEadDialog(const EadFile& file, QWidget *parent) :
	QDialog(parent),
	m_file(file)
{
	setupWidgets();
}

void ImportEadDialog::setupWidgets()
{
	QVBoxLayout* vbox = new QVBoxLayout();
	QGridLayout* grid = new QGridLayout();

	vbox->addWidget(new QLabel("Please categorize the signal(s) you wish to import."));
	vbox->addWidget(new QLabel("You may import a maximum of one signal per type."));

	int iRow = 0;
	grid->addWidget(new QLabel("Import?"), iRow, 0);
	grid->addWidget(new QLabel("Record#"), iRow, 1);
	grid->addWidget(new QLabel("Type"), iRow, 2);
	grid->addWidget(new QLabel("Signal Name"), iRow, 3);
	iRow++;

	for (int i = 1; i < m_file.recs().size(); i++) {
		RecInfo* rec = m_file.recs()[i];

		foreach (WaveInfo* wave, rec->waves()) {
			if (!wave->raw.isEmpty()) {
				// Add wave to map (so that it's selected)
				m_map.insert(i, wave->type);
				// Create UI elements
				QCheckBox* chk = new QCheckBox(this);
				chk->setChecked(true);
				chk->setProperty("rec", i);
				chk->setProperty("type", (int) wave->type);
				connect(chk, SIGNAL(toggled(bool)), this, SLOT(on_checked(bool)));
				grid->addWidget(chk, iRow, 0);
				grid->addWidget(new QLabel(QString::number(i)), iRow, 1);
				grid->addWidget(new QLabel(WaveInfo::getWaveTypeName(wave->type)), iRow, 2);
				QString sName = (wave->sComment.isEmpty()) ? wave->sName : QString("%0: %1").arg(wave->sName).arg(wave->sComment);
				grid->addWidget(new QLabel(sName), iRow, 3);
				iRow++;
			}
		}
	}

	vbox->addLayout(grid);
	vbox->setAlignment(grid, Qt::AlignLeft);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
	vbox->addWidget(buttons);

	setLayout(vbox);
}

void ImportEadDialog::on_checked(bool checked) {
	const QCheckBox* chk = static_cast<QCheckBox*>(sender());
	const int iRec = chk->property("rec").toInt();
	const WaveType type = (WaveType) chk->property("type").toInt();
	if (checked) {
		m_map.insert(iRec, type);
	}
	else {
		m_map.remove(iRec, type);
	}
}
