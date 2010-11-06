#ifndef __PROJECT_H
#define __PROJECT_H

#include <QObject>
#include <QPointer>
#include <QUndoCommand>
#include <QUndoStack>
#include <QVariantList>

#include "Item.h"


class ItemList;
class ProjectData;
class Wave;
class WaveRepository;


class Project : public QObject
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
	const ItemList* itemList() const;
	const ItemList* trashList() const;

	Q_INVOKABLE Item* find(int itemId) const;
	Q_INVOKABLE Wave* findWave(int itemId) const;

	Q_INVOKABLE QVariant getProperty(int itemId, const QString& sProperty);
	Q_INVOKABLE void setProperty(int itemId, const QString& sProperty, const QVariant& v);

	Q_INVOKABLE void itemDelete(int itemId);
	Q_INVOKABLE Wave* waveCreate();

	Q_INVOKABLE void undo();
	Q_INVOKABLE void redo();

signals:
	void logCommand(const QString& s);

public slots:

protected:
	friend class Command;
	friend class Wave;

	void setProperty(Item* o, const QString& sProperty, const QVariant& vOld, const QVariant& vNew);
	void handleCommand(CommandData* data, bool bDo);

private:
	WaveRepository* m_waves;
	QUndoStack* m_commands;
};

#endif
