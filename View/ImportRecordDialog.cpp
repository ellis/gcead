#include "ImportRecordDialog.h"

#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>

ImportRecordDialog::ImportRecordDialog(const QStringList& asNames, QWidget *parent) :
    QDialog(parent)
{
	setupWidgets(asNames);
}

void ImportRecordDialog::setupWidgets(const QStringList& asNames)
{
	QGridLayout* grid = new QGridLayout();

	int iRow = 0;

	QLabel* lblHelp = new QLabel("Please categorize the signal(s) you wish to import.  You may import a maximum of one signal per type.");
	grid->addWidget(lblHelp);

	grid->addWidget(new QLabel("Signal Name"), iRow, 0);
	grid->addWidget(new QLabel("EAD"), iRow, 1);
	grid->addWidget(new QLabel("FID"), iRow, 2);
	grid->addWidget(new QLabel("Digital"), iRow, 3);
	grid->addWidget(new QLabel("Skip"), iRow, 3);
	iRow++;

	for (int i = 0; i < asNames.size(); i++) {
		QString sName = asNames[i];
		QLabel* lbl = new QLabel(sName);
		grid->addWidget(lbl, iRow, 0);

		QRadioButton rdo;
		QButtonGroup* grp= new QButtonGroup(this);

		rdo = new QRadioButton(grp);
		m_eads << rdo;
		grid->addWidget(rdo, iRow, 1);

		rdo = new QRadioButton(grp);
		m_fids << rdo;
		grid->addWidget(rdo, iRow, 2);

		rdo = new QRadioButton(grp);
		m_digs << rdo;
		grid->addWidget(rdo, iRow, 3);

		rdo = new QRadioButton(grp);
		rdo.setChecked(true);
		m_skps << rdo;
		grid->addWidget(rdo, iRow, 4);

		iRow++;
	}

	setLayout(grid);
}
