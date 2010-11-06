#ifndef WAVELISTCONTROLLER_H
#define WAVELISTCONTROLLER_H

#include <QObject>

#include "WaveList.h"


class Wave;


class WaveListController : public QObject
{
    Q_OBJECT
public:
    explicit WaveListController(QObject *parent = 0);

	void setRepository(WaveRepository* repo);

	Q_INVOKABLE Wave* find(int itemId);

signals:

public slots:
	Wave* add();
	void remove(Wave* wave);
	void remove(int itemId);
	//Wave* copy(Wave* wave);

private:
	QPointer<WaveRepository> m_repo;
};

#endif // WAVELISTCONTROLLER_H
