#include "Node.h"

#include <iostream>
using namespace std;

#include "Check.h"


Node::Node(QObject *parent) :
    QObject(parent)
{
}

Node::Node(const QVariantMap& entry, QObject *parent) :
	QObject(parent),
	m_entry(entry)
{
}

void Node::setParent(Node* parent) {
	if (!m_parent.isNull()) {
		m_parent->removeChild(this);
	}
	m_parent = parent;
	if (!m_parent.isNull()) {
		m_parent->addChild(this);
	}
}

void Node::addChild(Node* child) {
	CHECK_PRECOND_RET(!m_children.contains(child));
	m_children.append(child);
}

void Node::removeChild(Node* child) {
	CHECK_PRECOND_RET(m_children.contains(child));
	m_children.removeAll(child);
}

void Node::printTree(int nIndent) const {
	const QString sIndent = QString(nIndent, ' ');
	const QString sKey = m_entry[KEY].toString();
	cout << qPrintable(sIndent) << qPrintable(sKey) << endl;
	foreach (Node* child, m_children) {
		child->printTree(nIndent + 1);
	}
}
