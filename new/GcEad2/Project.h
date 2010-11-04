#ifndef __PROJECT_H
#define __PROJECT_H

#include <QObject>
#include <QPointer>
#include <QUndoCommand>
#include <QUndoStack>
#include <QVariantList>

#include "Item.h"


class Wave;


class Project : public QObject, private IItemPropertySetter
{
    Q_OBJECT
public:
	struct CommandData {
		virtual ~CommandData() {}
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

	Item* findItem(int itemId) const;
	Wave* findWave(int itemId) const;

	QVariant getProperty(int itemId, const QString& sProperty);
	void setProperty(int itemId, const QString& sProperty, const QVariant& v);

	Wave* waveCreate();
	void itemDelete(int itemId);

signals:
	//void propertyChanged(const QString& sTable, int id, const QString& sProperty);
	void propertyChanged(int objId, const QString& sProperty);
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
	QList<Item*> m_items;
	QList<Item*> m_trash;
};

#endif
