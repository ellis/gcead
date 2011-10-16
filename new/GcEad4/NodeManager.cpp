#include "NodeManager.h"

#include "Node.h"
#include "Server.h"


NodeManager::NodeManager(Server* server, QObject *parent) :
    QObject(parent)
{
	m_server = server;
}

void NodeManager::on_server_add(const QVariantMap& entry) {
	Node* node = new Node(entry, this);
	...
	look for parent
	if parent does not exist, make this a root node
}
