#ifndef __NODE_H
#define __NODE_H

#include <QList>
#include <QObject>
#include <QPointer>
#include <QVariantMap>


class Node : public QObject
{
    Q_OBJECT
public:
	explicit Node(QObject *parent = 0);
	explicit Node(const QVariantMap& entry, QObject *parent = 0);

	Node* parentNode() const { return m_parent; }
	void setParent(Node* parent);

signals:

public slots:

private:
	void addChild(Node* child);
	void removeChild(Node* child);

private:
	QVariantMap m_entry;
	QList<Node*> m_children;
	QPointer<Node> m_parent;
};

#endif
