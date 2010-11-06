#include "EdmCommand.h"

#include "EdmCommandTypes.h"
#include "Item.h"


EdmCommandItem::EdmCommandItem(int type, Item* item)
	: EdmCommand(type), item(item)
{
}

/*Item* EdmCommandItem::item() const {
	return (Item*) m_item.data();
}*/

EdmCommandItemProperty::EdmCommandItemProperty(Item* item, const QString& sProperty, const QVariant& vOld, const QVariant& vNew)
	: EdmCommandItem(EdmCommandType_ItemProperty, item), sProperty(sProperty), vOld(vOld), vNew(vNew)
{
}

EdmCommandItemCreate::EdmCommandItemCreate()
	: EdmCommandItem(EdmCommandType_ItemCreate, NULL)
{
	itemId = -1;
}
