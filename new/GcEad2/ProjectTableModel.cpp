#include "ProjectTableModel.h"

#include <Check.h>

#include "ItemList.h"
#include "Project.h"


ProjectTableModel::ProjectTableModel(QObject *parent)
	: QAbstractTableModel(parent)
{
}

void ProjectTableModel::setProject(Project *proj) {
	if (!m_proj.isNull()) {
		m_proj->disconnect(this);
	}

	m_proj = proj;
	m_items = NULL;
	m_anRows.clear();

	if (!m_proj.isNull()) {
		m_items = m_proj->itemList();
		connect(m_items, SIGNAL(itemAdded(int)), this, SLOT(on_items_itemAdded(int)));
		connect(m_items, SIGNAL(itemRemoved(int)), this, SLOT(on_items_itemRemoved(int)));
		connect(m_items, SIGNAL(itemPropertyChanged(int,QString)), this, SLOT(on_items_itemPropertyChanged(int,QString)));
		foreach (const Item* item, m_items->getUnderlyingSet())
			m_anRows << item->itemId();
	}

	reset();
}

void ProjectTableModel::setProperties(const QStringList &asProperties) {
	m_asProperties = asProperties;
	reset();
}

int ProjectTableModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return m_anRows.size();
}

int ProjectTableModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return m_asProperties.size();
}

QVariant ProjectTableModel::data(const QModelIndex &index, int role) const
{
	CHECK_PARAM_RETVAL(index.isValid(), QVariant());
	CHECK_PARAM_RETVAL(index.column() < m_asProperties.size(), QVariant());

	if (m_proj.isNull() || index.row() >= m_anRows.size())
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		int itemId = m_anRows[index.row()];
		return m_proj->getProperty(itemId, m_asProperties[index.column()]);
	}
	else {
		return QVariant();
	}
}

/*
QModelIndex ProjectTableModel::index(int row, int column, const QModelIndex &parent) const
{
	QModelIndex idx;
	if (!parent.isValid())
		idx = createIndex(row, column, 0);
	return idx;
}

QModelIndex ProjectTableModel::parent(const QModelIndex &child) const
{
	return QModelIndex();
}
*/

Qt::ItemFlags ProjectTableModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::NoItemFlags;
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool ProjectTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	CHECK_PARAM_RETVAL(index.isValid(), false);
	CHECK_PARAM_RETVAL(index.column() < m_asProperties.size(), false);

	if (m_proj.isNull() || index.row() >= m_anRows.size())
		return false;

	if (role == Qt::EditRole) {
		int itemId = m_anRows[index.row()];
		m_proj->setProperty(itemId, m_asProperties[index.column()], value);
		return true;
	}

	return false;
}

void ProjectTableModel::on_items_itemAdded(int itemId) {
	int iRow = m_anRows.size();
	beginInsertRows(QModelIndex(), iRow, iRow);
	m_anRows << itemId;
	endInsertRows();
}

void ProjectTableModel::on_items_itemRemoved(int itemId) {
	int iRow = m_anRows.indexOf(itemId);
	if (iRow >= 0) {
		beginRemoveRows(QModelIndex(), iRow, iRow);
		m_anRows.removeAll(itemId);
		endRemoveRows();
	}
}

void ProjectTableModel::on_items_itemPropertyChanged(int itemId, const QString& sProperty) {
	int iRow = m_anRows.indexOf(itemId);
	if (iRow >= 0) {
		int iCol = m_asProperties.indexOf(sProperty);
		if (iCol >= 0) {
			QModelIndex idx = index(iRow, iCol);
			emit dataChanged(idx, idx);
		}
	}
}
