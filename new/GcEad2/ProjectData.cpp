#include "ProjectData.h"

#include <QVariant>

#include <Check.h>

#include "ItemList.h"
#include "WaveData.h"
#include "Wave.h"


ProjectData::ProjectData(IItemPropertySetter* setter, QObject *parent)
	: QObject(parent), m_setter(setter)
{
	m_itemIdNext = 0;
	m_items = new ItemList(this);
	//m_items->setObjectName("items");
	m_trash = new ItemList(this);
	//m_trash->setObjectName("trash");
}

ProjectData::~ProjectData() {
}

Item* ProjectData::find(int itemId) const {
	return m_items->find(itemId);
}

bool ProjectData::activate(int itemId, bool bActive) {
	return activate((bActive) ? m_trash->find(itemId) : m_items->find(itemId), bActive);
}

bool ProjectData::activate(Item* item, bool bActive) {
	CHECK_PARAM_RETVAL(item != NULL, false);

	ItemList* from = (bActive) ? m_trash : m_items;
	ItemList* to = (bActive) ? m_items: m_trash;
	CHECK_PRECOND_NORET(from->contains(item));
	CHECK_PRECOND_NORET(!to->contains(item));

	to->add(item);
	from->remove(item);
	return true;
}

QVariant ProjectData::getProperty(int itemId, const QString& sProperty) {
	Item* o = find(itemId);
	CHECK_ASSERT_RETVAL(o != NULL, QVariant())

	QVariant v = o->getItemData()->property(sProperty.toLatin1());
	CHECK_ASSERT_RETVAL(v.isValid(), QVariant())

	return v;
}

void ProjectData::setProperty(int itemId, const QString& sProperty, const QVariant& v) {
	setProperty(find(itemId), sProperty, v);
}

void ProjectData::setProperty(Item* item, const QString& sProperty, const QVariant& vNew) {
	CHECK_PARAM_RET(item != NULL);
	CHECK_PARAM_RET(item->parent() == this);

	QVariant vOld = item->property(sProperty.toLatin1());
	if (vNew != vOld) {
		bool b = item->getItemData()->setProperty(sProperty.toLatin1(), vNew);
		CHECK_ASSERT_RET(b)

		if (b) {
			m_items->emit_itemPropertyChanged(item->itemId(), sProperty);
		}
	}
}

Wave* ProjectData::waveFind(int itemId) const {
	Item* item = find(itemId);
	if (item != NULL)
		return dynamic_cast<Wave*>(item);
	return NULL;
}

Wave* ProjectData::waveCreateInactive() {
	WaveData* waveD = new WaveData(m_itemIdNext++, this);
	Wave* wave = new Wave(waveD, m_setter, this);
	m_trash->add(wave);
	return wave;
}
