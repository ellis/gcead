#include "WaitForHardwareDialog.h"

#include <QtDebug>
#include <QEventLoop>
#include <QPointer>

#include <Idac/IdacProxy.h>


WaitForHardwareDialog::WaitForHardwareDialog(IdacProxy* idac, bool /*bCloseOnAvailable*/, QObject* parent)
	: QObject(parent)
{
	m_idac = idac;
	m_loop = NULL;
	//m_bCloseOnAvailable = bCloseOnAvailable;

	m_bAccepted = false;

	//connect(m_idac, SIGNAL(stateChanged(IdacState)), this, SLOT(updateRetryEnabled()));
	connect(m_idac, SIGNAL(statusTextChanged(QString)), this, SLOT(updateStatus()));
	connect(m_idac, SIGNAL(statusErrorChanged(QString)), this, SLOT(on_idac_statusErrorChanged(QString)));
	connect(m_idac, SIGNAL(isAvailableChanged(bool)), this, SLOT(on_idac_isAvailableChanged(bool)));

	m_idac->setup();

	updateStatus();
}

bool WaitForHardwareDialog::exec()
{
	QEventLoop loop;
	m_loop = &loop;
	QPointer<WaitForHardwareDialog> guard = this;
	loop.exec();
	if (guard.isNull())
		return false;

	if (!m_bAccepted)
		m_idac->stopSampling();
	return m_bAccepted;
}

void WaitForHardwareDialog::accept()
{
	m_bAccepted = true;
	m_loop->exit();
}

void WaitForHardwareDialog::reject()
{
	m_bAccepted = false;
	m_loop->exit();
}

void WaitForHardwareDialog::updateStatus()
{
	qDebug() << "WaitForHardwareDialog::updateStatus():" << m_idac->statusText();
}

void WaitForHardwareDialog::on_idac_statusErrorChanged(QString sError)
{
	qDebug() << "WaitForHardwareDialog::on_idac_statusErrorChanged():" << sError;
}

void WaitForHardwareDialog::on_idac_isAvailableChanged(bool b)
{
	if (b)
		accept();
	else
		reject();
}
