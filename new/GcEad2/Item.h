#ifndef __ITEM_H
#define __ITEM_H

#include <QMetaType>

#include "Data.h"
#include "EdmObject.h"


class Item;
class IItemPropertySetter;


class IItemPropertySetter {
public:
	virtual void setProperty(Item* o, const QString& sProperty, const QVariant& vOld, const QVariant& vNew) = 0;
};


class Item : public EdmObject
{
	Q_OBJECT
public:
	explicit Item(Data* data, QObject* parent = NULL);

	inline Data* getItemData() { return m_data; }
	inline int itemId() const { return m_data->itemId(); }

protected:
	//void setItemData(Data* data);

// EdmObject overrides
protected:
	bool handleItemProperty(EdmCommandItemProperty* cmd, bool bDo);

private:
	Data* m_data;
};

Q_DECLARE_METATYPE(Item*);

#endif
