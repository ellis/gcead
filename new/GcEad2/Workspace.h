#ifndef __WORKSPACE_H
#define __WORKSPACE_H

#include <QUndoCommand>

#include "EdmObject.h"


class QUndoStack;

class WaveRepository;


class Workspace : public EdmObject
{
    Q_OBJECT
public:
	class Command : public QUndoCommand {
	public:
		Command(const QString& sName, Workspace* space, EdmCommand* data, int iPath);
		~Command();

		void redo();
		void undo();

		Workspace* const space;
		EdmCommand* const data;
		const int iPath;
	};

public:
    explicit Workspace(QObject *parent = 0);

	QUndoStack* undoStack() { return m_commands; }
	WaveRepository* waveRepository() { return m_waves; }

signals:
	void logCommand(const QString& s);

public slots:

// EdmObject overrides
protected:
	bool startCommand(EdmCommand* cmd, bool bDo);

private:
	friend class Command;
	bool handleCommand(Command* cmd, bool bDo);

private:
	int m_itemId;
	WaveRepository* m_waves;
	QUndoStack* m_commands;
};

#endif
