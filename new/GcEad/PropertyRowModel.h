#ifndef PROPERTYROWMODEL_H
#define PROPERTYROWMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QMap>
#include <QPair>
#include <QPointer>
#include <QStringList>


class PropertyRowModel : public QAbstractItemModel
{
public:
    PropertyRowModel();

	void addObject(QObject* o, const QStringList& asProperties);

// Overrides for QAbstractItemModel
public:
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QModelIndex index(int row, int column, const QModelIndex &parent) const;
	QModelIndex parent(const QModelIndex &child) const;

private:
	QPair< QPointer<QObject>, QStringList > hoya;
	QList< QPair< QPointer<QObject>, QStringList > > m_data;
	int m_nCols;
};

#endif // PROPERTYROWMODEL_H
