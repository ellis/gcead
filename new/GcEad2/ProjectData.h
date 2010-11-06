#ifndef __PROJECTDATA_H
#define __PROJECTDATA_H

#include <QObject>

#include "Item.h"


class Item;
class ItemList;
class Wave;


class ProjectData : public QObject
{
	Q_OBJECT
public:
	explicit ProjectData(IItemPropertySetter* setter, QObject *parent = 0);
	~ProjectData();

	const ItemList* itemList() const { return m_items; }
	const ItemList* trashList() const { return m_trash; }

	Item* find(int itemId) const;
	bool activate(int itemId, bool bActive);
	bool activate(Item* item, bool bActive);

	QVariant getProperty(int itemId, const QString& sProperty);
	void setProperty(int itemId, const QString& sProperty, const QVariant& v);
	void setProperty(Item* item, const QString& sProperty, const QVariant& v);
	//void setProperty(Item* item, const QString& sProperty, const QVariant& vOld, const QVariant& vNew);

	Wave* waveFind(int itemId) const;
	Wave* waveCreateInactive();

private:
	IItemPropertySetter* m_setter;
	int m_itemIdNext;
	ItemList* m_items;
	ItemList* m_trash;
};

#endif
