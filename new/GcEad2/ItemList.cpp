#include "ItemList.h"

#include <Check.h>

#include "EdmCommand.h"
#include "EdmCommandTypes.h"


ItemList::ItemList(QObject* parent)
	: EdmObject(parent)
{
}

void ItemList::add(Item* item) {
	CHECK_PARAM_RET(item != NULL);
	CHECK_PARAM_RET(!contains(item));
	CHECK_PARAM_RET(!contains(item->itemId()));
	//CHECK_PARAM_RET(item->parent() == m_proj);

	m_items.insert(item);
	m_map.insert(item->itemId(), item);
	emit itemAdded(item->itemId());
}

void ItemList::remove(Item* item) {
	CHECK_PARAM_RET(item != NULL);
	CHECK_PARAM_RET(contains(item));

	m_items.remove(item);
	m_map.remove(item->itemId());
	emit itemRemoved(item->itemId());
}

void ItemList::emit_itemPropertyChanged(int itemId, const QString& sProperty) {
	emit itemPropertyChanged(itemId, sProperty);
}
