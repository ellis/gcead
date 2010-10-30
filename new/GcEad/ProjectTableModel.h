#ifndef PROJECTTABLEMODEL_H
#define PROJECTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QMap>
#include <QPair>
#include <QPointer>
#include <QStringList>


class Project;


class ProjectTableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit ProjectTableModel(QObject *parent = 0);

	void setProject(Project* proj);
	void setTable(const QString& sTable);
	void setProperties(const QStringList& asProperties);
	void setRows(const QList<int>& anRows);

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
	void on_proj_propertyChanged(const QString& sTable, int id, const QString& sProperty);

private:
	QPointer<Project> m_proj;
	QString m_sTable;
	QStringList m_asProperties;
	QList<int> m_anRows;
};

#endif // PROJECTTABLEMODEL_H
