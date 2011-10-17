#include "NodeManager.h"

#include "Check.h"

#include "Key.h"
#include "Node.h"
#include "Server.h"


NodeManager::NodeManager(Server* server, QObject *parent) :
    QObject(parent)
{
	m_server = server;
	connect(m_server, SIGNAL(entryAdded(const QVariantMap&)), this, SLOT(on_server_add(QVariantMap)));
}

void NodeManager::on_server_add(const QVariantMap& entry) {
	CHECK_PARAM_RET(entry.contains(KEY));
	const QString& sKey = entry.value(KEY).toString();
	CHECK_PARAM_RET(!sKey.isEmpty());

	Key key = Key::parse(sKey);
	Node* node = new Node(entry, this);

	const QString& sKeyParent = key.sParent;
	if (m_nodes.contains(sKeyParent)) {
		Node* parent = m_nodes[sKeyParent];
		node->setParent(parent);
	}
	else {
		m_roots.insert(sKey, node);
		m_nodes.insert(sKey, node);
	}

	foreach (Node* root, m_roots) {
		root->printTree(0);
	}
}
