#ifndef IMPORTEADDIALOG_H
#define IMPORTEADDIALOG_H

#include <QDialog>
#include <QList>
#include <QMap>

#include <WaveInfo.h>


class EadFile;
class QRadioButton;


class ImportEadDialog : public QDialog
{
    Q_OBJECT
public:
	ImportEadDialog(const EadFile& file, QWidget *parent = 0);

	/**
	 * @brief Get map of record index to wave types to import from that record.
	 */
	const QMultiMap<int, WaveType>& recordToWaveTypes() const { return m_map; }

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

	const EadFile& m_file;
	QMultiMap<int, WaveType> m_map;
};

#endif // IMPORTRECORDDIALOG_H
