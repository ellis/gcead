#ifndef IMPORTRECORDDIALOG_H
#define IMPORTRECORDDIALOG_H

#include <QDialog>
#include <QList>

class QRadioButton;


class ImportRecordDialog : public QDialog
{
    Q_OBJECT
public:
	ImportRecordDialog(const QStringList& asNames, QWidget *parent = 0);

signals:

public slots:

private:
	void setupWidgets(const QStringList& asNames);

private:
	QList<QRadioButton*> m_eads;
	QList<QRadioButton*> m_fids;
	QList<QRadioButton*> m_digs;
	QList<QRadioButton*> m_skps;
};

#endif // IMPORTRECORDDIALOG_H
