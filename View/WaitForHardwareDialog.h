#ifndef __WAITFORHARDWAREDIALOG_H
#define __WAITFORHARDWAREDIALOG_H

#include <QDialog>


class QLabel;
class QPushButton;

class IdacProxy;


/// Show hardware status and errors while waiting for hardware to become available.
class WaitForHardwareDialog : public QDialog
{
	Q_OBJECT
public:
	/// @param bCloseOnAvailable if true, the dialog will automatically close as soon as the hardware becomes available.
	WaitForHardwareDialog(IdacProxy* idac, bool bCloseOnAvailable, QWidget* parent = NULL);

private:
	void setupWidgets();

private slots:
	void updateRetryEnabled();
	void updateStatus();
	void on_idac_statusErrorChanged(QString sError);
	void on_idac_isAvailableChanged(bool b);
	void on_btnRetry_clicked();
	void on_btnClose_clicked();

private:
	QLabel* m_lblNotice;
	QLabel* m_lblStatus;
	QLabel* m_lblError;
	QPushButton* m_btnRetry;
	QPushButton* m_btnClose;

	IdacProxy* m_idac;
	bool m_bCloseOnAvailable;
};

#endif
