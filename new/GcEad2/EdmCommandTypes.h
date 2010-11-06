#ifndef __EDMCOMMANDTYPES_H
#define __EDMCOMMANDTYPES_H

// REFACTOR: Rename file to EdmCommandType

enum EdmCommandType {
	EdmCommandType_ItemProperty,
	EdmCommandType_ItemCreate,
	EdmCommandType_ItemActivate,
	EdmCommandType_ItemInactivate,
	EdmCommandType_ClearHistory,
};

#endif
