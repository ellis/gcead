#include "Item.h"

#include <Check.h>

//#include "EdmCommandTypes.h"


Item::Item(Data* data, QObject* parent)
	: EdmObject(parent), m_data(data)
{
	CHECK_PARAM_NORET(data != NULL);
	setObjectName(QString("item%0").arg(data->itemId()));
	data->setParent(this);
}

bool Item::handleItemProperty(EdmCommandItemProperty* cmd, bool bDo) {
	CHECK_PARAM_RETVAL(cmd != NULL, false);
	CHECK_PARAM_RETVAL(cmd->item == this, false);
	bool b = cmd->item->getItemData()->setProperty(cmd->sProperty.toLatin1(), bDo ? cmd->vNew : cmd->vOld);
	return b;
}
