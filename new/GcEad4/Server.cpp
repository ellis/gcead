#include "Server.h"

#include <QtDebug>

#include "Check.h"


Server::Server(QObject *parent) :
    QObject(parent)
{
}

void Server::add(const QVariantMap& entry) {
	CHECK_PARAM_RET(entry.contains(KEY));
	const QString& sKey = entry.value(KEY).toString();
	CHECK_PARAM_RET(!sKey.isEmpty());

	m_database.insert(sKey, entry);

	qDebug() << "database:";
	qDebug() << m_database;

	emit entryAdded(entry);
}

void Server::remove(const QString& key) {
	CHECK_PARAM_RET(m_database.contains(key));
	m_database.remove(key);
	emit entriesRemoved(QStringList(key));
}
