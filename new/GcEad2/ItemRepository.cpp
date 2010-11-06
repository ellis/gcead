#include "ItemRepository.h"

#include <Check.h>


ItemRepository::ItemRepository(QObject *parent) :
	EdmObject(parent)
{
	m_active = new ItemList(this);
	m_inactive = new ItemList(this);
}

Item* ItemRepository::createInactiveItem() {
	EdmCommandItemCreate* cmd = new EdmCommandItemCreate();
	sendCommand(cmd, true);
	return cmd->item;
}

bool ItemRepository::activateItem(int itemId, bool bActive) {
	Item* item = (bActive) ? m_inactive->find(itemId) : m_active->find(itemId);
	CHECK_PARAM_RETVAL(item != NULL, false);

	ItemList* from = (bActive) ? m_inactive : m_active;
	ItemList* to = (bActive) ? m_active: m_inactive;
	CHECK_PRECOND_NORET(from->contains(item));
	CHECK_PRECOND_NORET(!to->contains(item));

	to->add(item);
	from->remove(item);
	return true;
}

bool ItemRepository::handleItemCreate(EdmCommandItemCreate* cmd) {
	CHECK_PARAM_RETVAL(cmd != NULL, false);
	CHECK_PARAM_RETVAL(cmd->item == NULL, false);
	cmd->item = newItem(cmd);
	m_inactive->add(cmd->item);
	return true;
}
