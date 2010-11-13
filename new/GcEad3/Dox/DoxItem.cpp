#include "DoxItem.h"

#include <Check.h>
#include "DoxChange.h"


DoxItem::DoxItem(QObject *parent) :
    QObject(parent)
{
}

/*
void DoxItem::setDoxProperty(const QString& sProperty, const QVariant& vOld, const QVariant& vNew) {
	DoxChange* chg = new DoxChange();
	chg->item = this;
	chg->sProperty = sProperty;
	chg->avArgs << vNew;
	chg->avUndo << vOld;
	m_root->startChange(chg);
}
*/
