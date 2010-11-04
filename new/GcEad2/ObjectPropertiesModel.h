#ifndef __OBJECTPROPERTIESMODEL_H
#define __OBJECTPROPERTIESMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QPointer>
#include <QStringList>


class ObjectPropertiesModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	ObjectPropertiesModel(QObject* parent = NULL);

	void setProperties(const QStringList& asProperties);
	void addObject(QObject* o);

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
	void on_obj_propertyChanged();

private:
	int m_iSlot;
	QStringList m_asProperties;
	QList< QPointer<QObject> > m_objs;
};

#endif
