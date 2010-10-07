#ifndef IMPORTRECORDDIALOG_H
#define IMPORTRECORDDIALOG_H

#include <QDialog>
#include <QList>
#include <QMap>

#include <WaveInfo.h>


class QRadioButton;


class ImportRecordDialog : public QDialog
{
    Q_OBJECT
public:
	ImportRecordDialog(const QStringList& asNames, QWidget *parent = 0);

	const QMap<QString, WaveType>& map() const { return m_map; }

private:
	void setupWidgets();
	void updateEnabled();
	void updateEnabled(const QList<QRadioButton*>& rdos, WaveType waveType);

public slots:
	void on_ead();
	void on_fid();
	void on_dig();
	void on_skp();

private:
	QList<QRadioButton*> m_eads;
	QList<QRadioButton*> m_fids;
	QList<QRadioButton*> m_digs;
	QList<QRadioButton*> m_skps;

	const QStringList m_asNames;
	QMap<QString, WaveType> m_map;
};

#endif // IMPORTRECORDDIALOG_H
