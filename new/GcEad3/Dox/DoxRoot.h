#ifndef DOXROOT_H
#define DOXROOT_H

#include <QList>
#include <QMap>
#include <QObject>

#include "DoxItem.h"


class DoxChange;
class DoxChangeReport;
class DoxItem;


class DoxRoot : public DoxItem
{
    Q_OBJECT
public:
    explicit DoxRoot(QObject *parent = 0);

signals:

public slots:

public:
	void beginDoxReport(const QString& sTitle);
	void addDoxChange(DoxChange* chg);
	void endDoxReport();

private:
	int m_nReporters;
	DoxChangeReport* m_report;
	QMap<DoxItem*, QList<DoxChange*> > m_mapTargetToChanges;
};


class DoxChangeReport {
public:
	DoxChangeReport();

	QString sTitle;
	QList<DoxChange*> changes;
};

#endif // DOXROOT_H
