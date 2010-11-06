#ifndef __ITEMREPOSITORY_H
#define __ITEMREPOSITORY_H

#include "EdmObject.h"
#include "ItemList.h"


class ItemRepository : public EdmObject
{
    Q_OBJECT
public:
	explicit ItemRepository(QObject *parent = 0);

	const ItemList* activeItems() const { return m_active; }
	const ItemList* inactiveItems() const { return m_inactive; }

	Item* createInactiveItem();
	bool activateItem(int itemId, bool bActive);

protected:
	virtual Item* newItem(EdmCommandItemCreate *cmd) = 0;

// EdmObject overrides
protected:
	//bool handleCommand(EdmCommand* cmd);
	bool handleItemCreate(EdmCommandItemCreate *cmd);

private:
	ItemList* m_active;
	ItemList* m_inactive;
};

#endif // ITEMREPOSITORY_H
