#ifndef __EDMOBJECT_H
#define __EDMOBJECT_H

#include <QObject>

#include "EdmCommand.h"


class EdmObject : public QObject
{
    Q_OBJECT
public:
    explicit EdmObject(QObject *parent = 0);

signals:

public slots:

protected:
	EdmObject* findPath(EdmCommand* cmd);
	bool sendCommand(EdmCommand* cmd, bool bDo = true);

	virtual bool startCommand(EdmCommand* cmd, bool bDo);
	virtual bool handleCommand(EdmCommand* cmd, int iPath, bool bDo);
	bool forwardCommand(EdmCommand* cmd, int iPath, bool bDo);
	virtual bool handleItemProperty(EdmCommandItemProperty* cmd, bool bDo);
	virtual bool handleItemCreate(EdmCommandItemCreate* cmd);
};

#endif
