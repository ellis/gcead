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

public slots:
	void on_checked(bool checked);

private:
	const EadFile& m_file;
	QMultiMap<int, WaveType> m_map;
};

#endif // IMPORTRECORDDIALOG_H
