#ifndef __WAITFORHARDWAREDIALOG_H
#define __WAITFORHARDWAREDIALOG_H

#include <QObject>


class QEventLoop;

class IdacProxy;


/// Show hardware status and errors while waiting for hardware to become available.
class WaitForHardwareDialog : public QObject
{
	Q_OBJECT
public:
	/// @param bCloseOnAvailable if true, the dialog will automatically close as soon as the hardware becomes available.
	WaitForHardwareDialog(IdacProxy* idac, bool bCloseOnAvailable, QObject* parent = NULL);

	bool exec();

private:
	void accept();
	void reject();

private slots:
	void updateStatus();
	void on_idac_statusErrorChanged(QString sError);
	void on_idac_isAvailableChanged(bool b);

private:
	IdacProxy* m_idac;
	QEventLoop* m_loop;
	bool m_bAccepted;
};

#endif
