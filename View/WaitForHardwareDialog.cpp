#include "WaitForHardwareDialog.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

#include <Idac/IdacProxy.h>


WaitForHardwareDialog::WaitForHardwareDialog(IdacProxy* idac, bool bCloseOnAvailable, QWidget* parent)
	: QDialog(parent)
{
	m_idac = idac;
	m_bCloseOnAvailable = bCloseOnAvailable;

	setWindowTitle(tr("Waiting for Hardware"));

	connect(m_idac, SIGNAL(stateChanged(IdacState)), this, SLOT(updateRetryEnabled()));
	connect(m_idac, SIGNAL(statusTextChanged(QString)), this, SLOT(updateStatus()));
	connect(m_idac, SIGNAL(statusErrorChanged(QString)), this, SLOT(on_idac_statusErrorChanged(QString)));
	connect(m_idac, SIGNAL(isAvailableChanged(bool)), this, SLOT(on_idac_isAvailableChanged(bool)));

	setupWidgets();
	updateStatus();
	updateRetryEnabled();

	// Automatically try to connect if appropriate
	if (m_btnRetry->isEnabled())
		on_btnRetry_clicked();
}

void WaitForHardwareDialog::setupWidgets()
{
	m_lblNotice = new QLabel(tr("The action you requested cannot be performed until the hardware is ready"));
	m_lblStatus = new QLabel();
	m_lblError = new QLabel(tr("(None)"));
	m_btnRetry = new QPushButton(tr("&Retry"));
	m_btnClose = new QPushButton(tr("&Cancel"));

	setMinimumWidth(m_lblNotice->sizeHint().width() / 2);
	//m_lblNotice->setStyle("margin-bottom: 1em");
	m_lblNotice->setContentsMargins(0, 0, 0, m_lblNotice->sizeHint().height());
	m_lblNotice->setWordWrap(true);
	m_lblNotice->setVisible(m_bCloseOnAvailable);

	connect(m_btnRetry, SIGNAL(clicked()), this, SLOT(on_btnRetry_clicked()));
	connect(m_btnClose, SIGNAL(clicked()), this, SLOT(on_btnClose_clicked()));

	QGridLayout* layout = new QGridLayout();
	QFormLayout* form = new QFormLayout();

	form->addRow(tr("Status:"), m_lblStatus);
	form->addRow(tr("Error:"), m_lblError);

	layout->addWidget(m_lblNotice, 0, 0, 1, 3);
	layout->addLayout(form, 1, 0, 1, 3);
	layout->addItem(new QSpacerItem(1, 20, QSizePolicy::Expanding, QSizePolicy::Fixed), 2, 0);
	layout->addWidget(m_btnRetry, 3, 1);
	layout->addWidget(m_btnClose, 3, 2);

	setLayout(layout);
}

void WaitForHardwareDialog::updateRetryEnabled()
{
	bool bCanRetry = false;
	switch (m_idac->state())
	{
	case IdacState_None:
	case IdacState_NotPresent:
	case IdacState_InitError:
		bCanRetry = true;
		break;
	default:
		break;
	}
	m_btnRetry->setEnabled(bCanRetry);
}

void WaitForHardwareDialog::updateStatus()
{
	m_lblStatus->setText(m_idac->statusText());
}

void WaitForHardwareDialog::on_idac_statusErrorChanged(QString sError)
{
	m_lblError->setText(sError);
}

void WaitForHardwareDialog::on_idac_isAvailableChanged(bool b)
{
	if (b)
	{
		if (m_bCloseOnAvailable)
			accept();
		else
			m_btnClose->setText(tr("&OK"));
	}
	else
		m_btnClose->setText(tr("&Cancel"));

	m_btnClose->setDefault(b);
}

void WaitForHardwareDialog::on_btnRetry_clicked()
{
	m_idac->setup();
	m_lblError->setText(tr("(None)"));
}

void WaitForHardwareDialog::on_btnClose_clicked()
{
	if (m_idac->isAvailable())
		accept();
	else
		reject();
}
