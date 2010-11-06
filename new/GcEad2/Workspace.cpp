#include "Workspace.h"

#include <QUndoStack>

#include <Check.h>

#include "EdmCommand.h"
#include "EdmCommandTypes.h"
#include "Item.h"
#include "WaveList.h"


Workspace::Workspace(QObject *parent) :
	EdmObject(parent)
{
	m_itemId = 1;
	m_waves = new WaveRepository(this);
	m_commands = new QUndoStack(this);
}

bool Workspace::startCommand(EdmCommand* cmd, bool bDo) {
	CHECK_PARAM_RETVAL(cmd != NULL, false)

	if (!cmd->hasRun())
		cmd->setHasRun();
	else {
		emit logCommand(QString("project.%0()").arg(bDo ? "redo" : "undo"));
	}

	QString sLabel;
	QString sLog;

	switch (cmd->type) {
	case EdmCommandType_ItemProperty: {
		EdmCommandItemProperty* d = (EdmCommandItemProperty*) cmd;
		sLabel = QString("Set property %0").arg(d->sProperty);
		sLog = QString("project.find(%0).%1 = \"%2\"").arg(d->item->itemId()).arg(d->sProperty).arg(d->vNew.toString());
		break;
	}
	case EdmCommandType_ItemCreate: {
		EdmCommandItemCreate* d = (EdmCommandItemCreate*) cmd;
		d->itemId = m_itemId++;
		sLabel = QString("Create item (#%0)").arg(d->itemId);
		sLog = QString("project.waveCreate()");
		break;
	}
	default:
		CHECK_FAILURE_RETVAL(false);
	}

	emit logCommand(sLog);

	Command* qcmd = new Command(sLabel, this, cmd, 0);
	m_commands->push(qcmd);
	return true;
}

bool Workspace::handleCommand(Command* cmd, bool bDo) {
	CHECK_PARAM_RETVAL(cmd != NULL, false)
	CHECK_PARAM_RETVAL(cmd->data != NULL, false)
	if (cmd->data->hasRun())
		cmd->data->setHasRun();
	else {
		emit logCommand(QString("project.%0()").arg(bDo ? "redo" : "undo"));
	}

	return forwardCommand(cmd->data, cmd->iPath + 1, bDo);
}



//---------------------------------

Workspace::Command::Command(const QString& sName, Workspace* space, EdmCommand* data, int iPath)
	: space(space), data(data), iPath(iPath)
{
	CHECK_PARAM_NORET(space != NULL)
	CHECK_PARAM_NORET(data != NULL)
	setText(sName);
	data->bDelete = false;
}

Workspace::Command::~Command() {
	delete data;
}

void Workspace::Command::redo() {
	CHECK_PRECOND_RET(space != NULL)
	space->handleCommand(this, true);
}

void Workspace::Command::undo() {
	CHECK_PRECOND_RET(space != NULL)
	space->handleCommand(this, false);
}
