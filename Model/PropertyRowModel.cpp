#include "PropertyRowModel.h"

#include <Check.h>


PropertyRowModel::PropertyRowModel()
{
	m_nCols = 0;
}

void PropertyRowModel::addObject(QObject* o, const QStringList& asProperties)
{
	CHECK_PARAM_RET(o != NULL);
	CHECK_PARAM_RET(asProperties.size() > 0);

	m_data << qMakePair(QPointer<QObject>(o), asProperties);
	m_nCols += asProperties.size();
}

int PropertyRowModel::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid() && m_data.size() > 0)
		return 1;
	else
		return 0;
}

int PropertyRowModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return m_nCols;
}

QVariant PropertyRowModel::data(const QModelIndex &index, int role) const
{
	CHECK_PARAM_RETVAL(index.column() < m_nCols, QVariant());

	int iObj = 0;
	int iProperty = 0;
	int d = index.column();
	while (d >= 0 && iObj < m_data.size()) {
		const QPair<QPointer<QObject>, QStringList>& pair = m_data[iObj];
		QObject* o = pair.first;
		const QStringList& asProperties = pair.second;
		if (d >= asProperties.size()) {
			d -= asProperties.size();
			iObj++;
		}
		else {
			iProperty = d;
			QString sPropertyName = asProperties[iProperty];
			if (role == Qt::DisplayRole)
				return o->property(sPropertyName.toLatin1());
			break;
		}
	}

	return QVariant();
}

QModelIndex PropertyRowModel::index(int row, int column, const QModelIndex &parent) const
{
	QModelIndex idx;
	if (!parent.isValid())
        idx = createIndex(row, column, (void*)0);
	return idx;
}

QModelIndex PropertyRowModel::parent(const QModelIndex &/*child*/) const
{
	return QModelIndex();
}
