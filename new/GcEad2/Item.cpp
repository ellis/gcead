#include "Item.h"

#include <Check.h>


Item::Item(Data* data, QObject* parent)
	: QObject(parent), m_data(data)
{
	CHECK_PARAM_NORET(data != NULL);
	setObjectName(QString("item%0").arg(data->itemId()));
}
