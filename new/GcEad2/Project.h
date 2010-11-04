#ifndef __PROJECT_H
#define __PROJECT_H

#include <QObject>
#include <QPointer>
#include <QUndoCommand>
#include <QUndoStack>
#include <QVariantList>

#include "Item.h"


class ItemList;
class Wave;


class Project : public QObject, private IItemPropertySetter
{
    Q_OBJECT
public:
	struct CommandData {
		CommandData() : bRunOnce(false) {}
		virtual ~CommandData() {}
		bool bRunOnce;
	};

	struct CommandDataProperty : public CommandData {
		QPointer<Item> o;
		//QString sTable;
		//int id;
		QString sProperty;
		QVariant vOld;
		QVariant vNew;
	};

	enum CommandType {
		CommandType_ItemDelete,
		CommandType_WaveCreate,
	};

	struct CommandDataGeneric : public CommandData {
		CommandDataGeneric(CommandType type) : type(type) {}
		CommandType type;
		QPointer<Item> o;
		//int itemId;
		QVariantList vars;
	};

	class CommandBase : public QUndoCommand {
	public:
		CommandBase(Project* proj);
		virtual ~CommandBase();
	protected:
		Project* const proj;
	};

	class Command : public CommandBase {
	public:
		Command(const QString& sName, Project* proj, CommandData* data);
		~Command();
		void redo();
		void undo();
	private:
		CommandData* const data;
	};

public:
	explicit Project(QObject *parent = 0);
	~Project();

	QUndoStack* undoStack() { return m_commands; }
	const ItemList* itemList() const { return m_items; }
	const ItemList* trashList() const { return m_trash; }

	Q_INVOKABLE Item* findItem(int itemId) const;
	Q_INVOKABLE Wave* findWave(int itemId) const;

	Q_INVOKABLE QVariant getProperty(int itemId, const QString& sProperty);
	Q_INVOKABLE void setProperty(int itemId, const QString& sProperty, const QVariant& v);

	Q_INVOKABLE Wave* waveCreate();
	Q_INVOKABLE void itemDelete(int itemId);

	Q_INVOKABLE void undo();
	Q_INVOKABLE void redo();

signals:
	void logCommand(const QString& s);
	//void propertyChanged(const QString& sTable, int id, const QString& sProperty);
	//void propertyChanged(int objId, const QString& sProperty);
	//void propertyChanged(int objId, int iProperty);

public slots:

protected:
	friend class Command;
	friend class Wave;

	void setProperty(Item* o, const QString& sProperty, const QVariant& vOld, const QVariant& vNew);
	void handleCommand(CommandData* data, bool bDo);

private:
	void _setProperty(Item* o, const QString& sProperty, const QVariant& v);
	void _itemDelete(CommandDataGeneric* data, bool bDo);
	void _waveCreate(CommandDataGeneric* data, bool bDo);

private:
	QUndoStack* m_commands;

	int m_itemIdNext;
	//int m_waveIdNext;
	//QList<WaveData*> m_waveDatas;
	//QMap<int, int> m_mapWaveIdToIndex;
	//QList<Item*> m_items;
	//QList<Item*> m_trash;
	ItemList* m_items;
	ItemList* m_trash;
};

#endif
