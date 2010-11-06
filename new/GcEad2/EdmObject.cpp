#include "EdmObject.h"

#include <Check.h>

#include "EdmCommand.h"
#include "EdmCommandTypes.h"


EdmObject::EdmObject(QObject *parent) :
    QObject(parent)
{
}

EdmObject* EdmObject::findPath(EdmCommand* cmd) {
	CHECK_PARAM_RETVAL(cmd != NULL, NULL);
	CHECK_PARAM_RETVAL(cmd->path.isEmpty(), NULL);

	EdmObject* top = this;
	QObject* parent = this->parent();
	cmd->path << top;
	while (parent != NULL) {
		EdmObject* obj = dynamic_cast<EdmObject*>(parent);
		if (obj != NULL) {
			top = obj;
			cmd->path.insert(0, top);
		}
		parent = parent->parent();
	}
	return top;
}

bool EdmObject::sendCommand(EdmCommand* cmd, bool bDo) {
	EdmObject* top = findPath(cmd);
	bool b = top->startCommand(cmd, bDo);
	if (cmd->bDelete)
		delete cmd;
	return b;
}

bool EdmObject::startCommand(EdmCommand* cmd, bool bDo) {
	return handleCommand(cmd, 0, bDo);
}

bool EdmObject::handleCommand(EdmCommand* cmd, int iPath, bool bDo) {
	CHECK_PARAM_RETVAL(cmd != NULL, false);
	if (cmd->path.size()) {
		switch (cmd->type) {
		case EdmCommandType_ItemProperty:
			return handleItemProperty((EdmCommandItemProperty*) cmd, bDo);
		case EdmCommandType_ItemCreate:
			return handleItemCreate((EdmCommandItemCreate*) cmd);
		}
	}
	else {
		return forwardCommand(cmd, iPath + 1, bDo);
	}
	return false;
}

bool EdmObject::forwardCommand(EdmCommand* cmd, int iPath, bool bDo) {
	CHECK_ASSERT_RETVAL(cmd != NULL, false);
	if (iPath >= cmd->path.size())
		return false;
	EdmObject* next = cmd->path[iPath];
	CHECK_ASSERT_RETVAL(next != NULL, false);
	return next->handleCommand(cmd, iPath, bDo);
}

bool EdmObject::handleItemProperty(EdmCommandItemProperty* cmd, bool bDo) {
	Q_UNUSED(cmd);
	Q_UNUSED(bDo);
	return false;
}

bool EdmObject::handleItemCreate(EdmCommandItemCreate *cmd) {
	Q_UNUSED(cmd);
	return false;
}
