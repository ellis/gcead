#ifndef __NODEMANAGER_H
#define __NODEMANAGER_H

#include <QMap>
#include <QObject>
#include <QPointer>
#include <QVariantMap>

class Node;
class Server;


class NodeManager : public QObject
{
    Q_OBJECT
public:
	explicit NodeManager(Server* server, QObject *parent = 0);

signals:

private slots:
	void on_server_add(const QVariantMap& entry);

private:
	QPointer<Server> m_server;
	/// Root nodes, mapped by key
	QMap<QString, Node*> m_roots;
	/// All nodes, mapped by key
	QMap<QString, Node*> m_nodes;
};

#endif
