#include "ObjectPropertiesModel.h"

#include <QMetaMethod>
#include <QMetaProperty>

#include <Check.h>


ObjectPropertiesModel::ObjectPropertiesModel(QObject *parent)
	: QAbstractTableModel(parent)
{
	m_iSlot = -1;
	const QMetaObject* m = metaObject();
	int n = m->methodCount();
	for (int i = 0; i < n; i++) {
		const QMetaMethod& p = m->method(i);
		if (QString(p.signature()).startsWith("on_obj_propertyChanged")) {
			m_iSlot = i;
			break;
		}
	}
}

void ObjectPropertiesModel::setProperties(const QStringList &asProperties) {
	m_asProperties = asProperties;
	reset();
}

void ObjectPropertiesModel::addObject(QObject* o) {
	CHECK_PARAM_RET(o != NULL);

	m_objs << o;

	const QMetaObject* m = o->metaObject();
	int n = m->propertyCount();
	for (int i = 0; i < n; i++) {
		const QMetaProperty& p = m->property(i);
		if (m_asProperties.contains(p.name()) && p.hasNotifySignal()) {
			QMetaObject::connect(o, p.notifySignalIndex(), this, m_iSlot);
		}
	}

	reset();
}

int ObjectPropertiesModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return m_objs.size();
}

int ObjectPropertiesModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return m_asProperties.size();
}

QVariant ObjectPropertiesModel::data(const QModelIndex &index, int role) const
{
	CHECK_PARAM_RETVAL(index.isValid(), QVariant());
	CHECK_PARAM_RETVAL(index.column() < m_asProperties.size(), QVariant());

	if (index.row() >= m_objs.size())
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		QObject* o = m_objs[index.row()];
		CHECK_ASSERT_RETVAL(o != NULL, QVariant());
		QString sPropertyName = m_asProperties[index.column()];
		CHECK_ASSERT_RETVAL(!sPropertyName.isEmpty(), QVariant());
		return o->property(sPropertyName.toLatin1());
	}

	return QVariant();
}

Qt::ItemFlags ObjectPropertiesModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::NoItemFlags;
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool ObjectPropertiesModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	CHECK_PARAM_RETVAL(index.isValid(), false);
	CHECK_PARAM_RETVAL(index.column() < m_asProperties.size(), false);

	if (index.row() >= m_objs.size())
		return false;

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		QObject* o = m_objs[index.row()];
		CHECK_ASSERT_RETVAL(o != NULL, false);
		QString sPropertyName = m_asProperties[index.column()];
		CHECK_ASSERT_RETVAL(!sPropertyName.isEmpty(), false);
		o->setProperty(sPropertyName.toLatin1(), value);
		return true;
	}

	return false;
}

void ObjectPropertiesModel::on_obj_propertyChanged() {
	QObject* o = sender();
	int iObj = m_objs.indexOf(o);
	CHECK_ASSERT_RET(iObj >= 0);
	emit dataChanged(index(iObj, 0), index(iObj, m_asProperties.size() - 1));
}
