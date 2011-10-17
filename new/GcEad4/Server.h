#ifndef __SERVER_H
#define __SERVER_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>

#include "Defines.h"


class Server : public QObject
{
	Q_OBJECT
public:
	explicit Server(QObject *parent = 0);

signals:
	void entryAdded(const QVariantMap& entry);
	void entriesRemoved(const QStringList& keys);

public slots:
	void add(const QVariantMap& entry);
	void remove(const QString& key);

private:
	QMap<QString, QVariantMap> m_database;
};

#endif
