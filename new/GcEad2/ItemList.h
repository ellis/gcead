#ifndef __ITEMLIST_H
#define __ITEMLIST_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QSet>

#include "EdmObject.h"
#include "Item.h"


class ItemList : public EdmObject
{
	Q_OBJECT
public:
	ItemList(QObject* parent);

	const QSet<Item*>& getUnderlyingSet() const { return m_items; }

	Q_INVOKABLE bool contains(Item* item) const { return m_items.contains(item); }
	Q_INVOKABLE bool contains(int itemId) const { return m_map.contains(itemId); }
	Q_INVOKABLE Item* find(int itemId) const { return m_map.value(itemId, NULL); }

	//void remove(int itemId);

signals:
	void itemAdded(int itemId);
	void itemRemoved(int itemId);
	void itemPropertyChanged(int itemId, const QString& sProperty);

//protected:
//	virtual Item* newItem(EdmCommandItemCreate *cmd) = 0;

private:
	friend class ItemRepository;
	void add(Item* item);
	void remove(Item* item);
	void emit_itemPropertyChanged(int itemId, const QString& sProperty);

private:
	QHash<int, Item*> m_map;
	QSet<Item*> m_items;
};

/*
template <class T>
class ItemList<T> : public ItemListBase {
public:
	void add(T* item) { ItemListBase::add(item); }
	void remove(T* item) { ItemListBase::remove(item); }

};*/

#endif
