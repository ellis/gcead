#ifndef PROJECTTABLEMODEL_H
#define PROJECTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QMap>
#include <QPair>
#include <QPointer>
#include <QStringList>


class ItemList;
class Project;


class ProjectTableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit ProjectTableModel(QObject *parent = 0);

	void setProject(Project* proj);
	//void setTable(const QString& sTable);
	void setProperties(const QStringList& asProperties);

// Overrides for QAbstractItemModel
public:
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	//QModelIndex index(int row, int column, const QModelIndex &parent) const;
	//QModelIndex parent(const QModelIndex &child) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);

private slots:
	void on_items_itemAdded(int itemId);
	void on_items_itemRemoved(int itemId);
	void on_items_itemPropertyChanged(int itemId, const QString& sProperty);

private:
	QPointer<Project> m_proj;
	const ItemList* m_items;
	//QString m_sTable;
	QStringList m_asProperties;
	// REFACTOR: Rename to m_itemIds
	QList<int> m_anRows;
};

#endif // PROJECTTABLEMODEL_H
