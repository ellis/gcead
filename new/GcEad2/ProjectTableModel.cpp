#include "ProjectTableModel.h"

#include <Check.h>

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
	m_anRows.clear();

	if (!m_proj.isNull()) {
		connect(m_proj, SIGNAL(propertyChanged(int,QString)), this, SLOT(on_proj_propertyChanged(int,QString)));
	}

	reset();
}

void ProjectTableModel::setTable(const QString &sTable) {
	m_sTable = sTable;
	m_asProperties.clear();
	m_anRows.clear();
	reset();
}

void ProjectTableModel::setProperties(const QStringList &asProperties) {
	m_asProperties = asProperties;
	reset();
}

void ProjectTableModel::setRows(const QList<int> &anRows) {
	m_anRows = anRows;
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

	if (m_proj.isNull() || m_sTable.isEmpty() || index.row() >= m_anRows.size())
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

	if (m_proj.isNull() || m_sTable.isEmpty() || index.row() >= m_anRows.size())
		return false;

	if (role == Qt::EditRole) {
		int itemId = m_anRows[index.row()];
		m_proj->setProperty(itemId, m_asProperties[index.column()], value);
		return true;
	}

	return false;
}

void ProjectTableModel::on_proj_propertyChanged(int itemId, const QString& sProperty) {
	int iRow = m_anRows.indexOf(itemId);
	if (iRow >= 0) {
		int iCol = m_asProperties.indexOf(sProperty);
		if (iCol >= 0) {
			QModelIndex idx = index(iRow, iCol);
			emit dataChanged(idx, idx);
		}
	}
}
