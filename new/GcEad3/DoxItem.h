#ifndef __DOXITEM_H
#define __DOXITEM_H

#include <QList>
#include <QObject>


class DoxChange;


class DoxItem : public QObject
{
    Q_OBJECT
public:
    explicit DoxItem(QObject *parent = 0);

signals:

public slots:

protected:
	friend class DoxRoot;

	void setDoxProperty(const QString& sProperty, const QVariant& vOld, const QVariant& vNew);

	virtual void changeEvent(QList<DoxChange*> changes, QList<DoxItem*> chain);

	//void emitDoxChangeSignal(DoxChange* chg);

};

#endif
