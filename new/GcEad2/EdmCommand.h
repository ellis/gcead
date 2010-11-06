#ifndef __EDMCOMMAND_H
#define __EDMCOMMAND_H

#include <QPointer>
#include <QStack>
#include <QString>
#include <QVariant>


class EdmObject;
class Item;


struct EdmCommand
{
public:
	EdmCommand(int type)
		: type(type), bDelete(true), m_bRan(false)
	{
	}

	const int type;
	bool bDelete;
	//EdmObject* sender;
	QList<EdmObject*> path;

	bool hasRun() const { return m_bRan; }
	void setHasRun() { m_bRan = true; }

private:
	bool m_bRan;
};


struct EdmCommandItem : public EdmCommand
{
public:
	EdmCommandItem(int type, Item* item = NULL);

	Item* item;
};


struct EdmCommandItemProperty : public EdmCommandItem {
	EdmCommandItemProperty(Item* item, const QString& sProperty, const QVariant& vOld, const QVariant& vNew);

	QString sProperty;
	QVariant vOld;
	QVariant vNew;
};


struct EdmCommandItemCreate : public EdmCommandItem {
	EdmCommandItemCreate();
	int itemId;
};

#endif
