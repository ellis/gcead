#include "Project.h"

#include <Check.h>

#include "ItemList.h"
#include "ProjectData.h"
#include "WaveData.h"
#include "Wave.h"
#include "WaveList.h"


Project::CommandBase::CommandBase(Project* proj)
	: proj(proj)
{
	CHECK_PARAM_NORET(proj != NULL);
}

Project::CommandBase::~CommandBase() {
}

Project::Command::Command(const QString& sName, Project* proj, CommandData* data)
	: CommandBase(proj), data(data)
{
	CHECK_PARAM_NORET(data != NULL);
	setText(sName);
}

Project::Command::~Command() {
	delete data;
}

void Project::Command::redo() {
	CHECK_PRECOND_RET(proj != NULL);
	proj->handleCommand(data, true);
}

void Project::Command::undo() {
	CHECK_PRECOND_RET(proj != NULL);
	proj->handleCommand(data, false);
}

Project::Project(QObject *parent)
	: QObject(parent)
{
	//m_waves = new ProjectData(this);
	m_waves = new WaveRepository(this);
	m_commands = new QUndoStack(this);
}

Project::~Project() {
}

const ItemList* Project::itemList() const { return m_waves->activeItems(); }
const ItemList* Project::trashList() const { return m_waves->inactiveItems(); }

Item* Project::find(int itemId) const {
	return m_waves->activeItems()->find(itemId);
}

Wave* Project::findWave(int itemId) const {
	Item* item = find(itemId);
	if (item != NULL)
		return dynamic_cast<Wave*>(item);
	return NULL;
}

QVariant Project::getProperty(int itemId, const QString& sProperty) {
	Item* o = find(itemId);
	CHECK_ASSERT_RETVAL(o != NULL, QVariant())

	QVariant v = o->getItemData()->property(sProperty.toLatin1());
	CHECK_ASSERT_RETVAL(v.isValid(), QVariant())

	return v;
}

void Project::setProperty(int itemId, const QString& sProperty, const QVariant& v) {
	Item* o = find(itemId);
	CHECK_ASSERT_RET(o != NULL)
	QVariant vOld = o->property(sProperty.toLatin1());
	setProperty(o, sProperty, vOld, v);
}

void Project::itemDelete(int itemId) {
	Item* item = find(itemId);
	CHECK_PARAM_RET(item != NULL);
	CommandDataGeneric* data = new CommandDataGeneric(CommandType_ItemDelete);
	data->o = item;
	emit logCommand(QString("project.itemDelete(%0)").arg(itemId));
	m_commands->push(new Command(tr("Delete item (#%0)").arg(itemId), this, data));
}

Wave* Project::waveCreate() {
	Wave* wave = m_waves->waveCreateInactive();

	CommandDataGeneric* data = new CommandDataGeneric(CommandType_WaveCreate);
	data->o = wave;
	emit logCommand(QString("project.waveCreate()"));
	m_commands->push(new Command(tr("Create wave (#%0)").arg(wave->itemId()), this, data));

	return wave;
}

void Project::undo() {
	m_commands->undo();
}

void Project::redo() {
	m_commands->redo();
}

void Project::setProperty(Item* item, const QString& sProperty, const QVariant& vOld, const QVariant& vNew) {
	if (vNew != vOld) {
		CommandDataProperty* data = new CommandDataProperty;
		data->o = item;
		data->sProperty = sProperty;
		data->vNew = vNew;
		data->vOld = vOld;
		emit logCommand(QString("project.find(%0).%1 = \"%2\"").arg(item->itemId()).arg(sProperty).arg(vNew.toString()));
		m_commands->push(new Command("Set property " + sProperty, this, data));
	}
}

void Project::handleCommand(CommandData* data, bool bDo) {
	CHECK_PARAM_RET(data != NULL);
	if (!data->bRunOnce)
		data->bRunOnce = true;
	else {
		emit logCommand(QString("ui.%0()").arg(bDo ? "redo" : "undo"));
	}

	{
		CommandDataProperty* d = dynamic_cast<CommandDataProperty*>(data);
		if (d != NULL) {
			m_waves->setProperty(d->o, d->sProperty, (bDo) ? d->vNew : d->vOld);
			return;
		}
	}
	{
		CommandDataGeneric* d = dynamic_cast<CommandDataGeneric*>(data);
		if (d != NULL) {
			switch (d->type) {
			case CommandType_ItemDelete: m_waves->activate(d->o, false); break;
			case CommandType_WaveCreate: m_waves->activate(d->o, true); break;
			}
			return;
		}
	}
}
