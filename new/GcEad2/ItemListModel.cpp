#include "ItemListModel.h"

#include <Check.h>

#include "ItemList.h"


ItemListModel::ItemListModel(QObject *parent)
	: QAbstractTableModel(parent)
{
}

void ItemListModel::setItemList(const ItemList* items) {
	CHECK_PARAM_RET(items != NULL)

	if (m_items != NULL) {
		m_items->disconnect(this);
	}

	m_items = (ItemList*) items;
	m_anRows.clear();

	if (m_items != NULL) {
		connect(m_items, SIGNAL(itemAdded(int)), this, SLOT(on_items_itemAdded(int)));
		connect(m_items, SIGNAL(itemRemoved(int)), this, SLOT(on_items_itemRemoved(int)));
		connect(m_items, SIGNAL(itemPropertyChanged(int,QString)), this, SLOT(on_items_itemPropertyChanged(int,QString)));
		foreach (const Item* item, m_items->getUnderlyingSet())
			m_anRows << item->itemId();
	}

	reset();
}

void ItemListModel::setProperties(const QStringList &asProperties) {
	m_asProperties = asProperties;
	reset();
}

int ItemListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return m_anRows.size();
}

int ItemListModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return m_asProperties.size();
}

QVariant ItemListModel::data(const QModelIndex &index, int role) const
{
	CHECK_PARAM_RETVAL(index.isValid(), QVariant());
	CHECK_PARAM_RETVAL(index.column() < m_asProperties.size(), QVariant());

	if (m_items.isNull() || index.row() >= m_anRows.size())
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		int itemId = m_anRows[index.row()];
		Item* item = m_items->find(itemId);
		CHECK_ASSERT_RETVAL(item != NULL, QVariant());
		return item->getItemData()->property(m_asProperties[index.column()].toLatin1());
	}
	else {
		return QVariant();
	}
}

/*
QModelIndex ItemListModel::index(int row, int column, const QModelIndex &parent) const
{
	QModelIndex idx;
	if (!parent.isValid())
		idx = createIndex(row, column, 0);
	return idx;
}

QModelIndex ItemListModel::parent(const QModelIndex &child) const
{
	return QModelIndex();
}
*/

Qt::ItemFlags ItemListModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::NoItemFlags;
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool ItemListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	CHECK_PARAM_RETVAL(index.isValid(), false);
	CHECK_PARAM_RETVAL(index.column() < m_asProperties.size(), false);

	if (m_items.isNull() || index.row() >= m_anRows.size())
		return false;

	if (role == Qt::EditRole) {
		int itemId = m_anRows[index.row()];
		Item* item = m_items->find(itemId);
		CHECK_ASSERT_RETVAL(item != NULL, false);
		item->setProperty(m_asProperties[index.column()].toLatin1(), value);
		return true;
	}

	return false;
}

void ItemListModel::on_items_itemAdded(int itemId) {
	int iRow = m_anRows.size();
	beginInsertRows(QModelIndex(), iRow, iRow);
	m_anRows << itemId;
	endInsertRows();
}

void ItemListModel::on_items_itemRemoved(int itemId) {
	int iRow = m_anRows.indexOf(itemId);
	if (iRow >= 0) {
		beginRemoveRows(QModelIndex(), iRow, iRow);
		m_anRows.removeAll(itemId);
		endRemoveRows();
	}
}

void ItemListModel::on_items_itemPropertyChanged(int itemId, const QString& sProperty) {
	int iRow = m_anRows.indexOf(itemId);
	if (iRow >= 0) {
		int iCol = m_asProperties.indexOf(sProperty);
		if (iCol >= 0) {
			QModelIndex idx = index(iRow, iCol);
			emit dataChanged(idx, idx);
		}
	}
}
