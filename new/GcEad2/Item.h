#ifndef __ITEM_H
#define __ITEM_H

#include <QObject>

#include "Data.h"


class Item;
class IItemPropertySetter;


class IItemPropertySetter {
public:
	virtual void setProperty(Item* o, const QString& sProperty, const QVariant& vOld, const QVariant& vNew) = 0;
};


class Item : public QObject
{
	Q_OBJECT
public:
	explicit Item(Data* data, QObject* parent = NULL);

	inline Data* getItemData() { return m_data; }
	inline int itemId() const { return m_data->itemId(); }

private:
	Data* m_data;
};

#endif
