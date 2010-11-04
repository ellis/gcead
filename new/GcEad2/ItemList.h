#ifndef __ITEMLIST_H
#define __ITEMLIST_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QSet>

#include "Item.h"


class Project;


class ItemList : public QObject
{
	Q_OBJECT
public:
	ItemList(Project* proj);

	const QSet<Item*>& getUnderlyingSet() const { return m_items; }

	Q_INVOKABLE bool contains(Item* item) const { return m_items.contains(item); }
	Q_INVOKABLE bool contains(int itemId) const { return m_map.contains(itemId); }
	Q_INVOKABLE Item* find(int itemId) const { return m_map.value(itemId, NULL); }

	void add(Item* item);
	void remove(Item* item);
	//void remove(int itemId);

signals:
	void itemAdded(int itemId);
	void itemRemoved(int itemId);
	void itemPropertyChanged(int itemId, const QString& sProperty);

private:
	friend class Project;
	void emit_itemPropertyChanged(int itemId, const QString& sProperty);

private:
	Project* const m_proj;
	QHash<int, Item*> m_map;
	QSet<Item*> m_items;
};

#endif
