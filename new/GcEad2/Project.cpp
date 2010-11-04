#include "Project.h"

#include <Check.h>

#include "WaveData.h"
#include "Wave.h"


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
	m_commands = new QUndoStack(this);
	m_itemIdNext = 0;
	//m_waveIdNext = 0;
}

Project::~Project() {
}

Item* Project::findItem(int itemId) const {
	foreach (Item* item, m_items) {
		if (item->itemId() == itemId)
			return item;
	}
	return NULL;
}

Wave* Project::findWave(int itemId) const {
	Item* item = findItem(itemId);
	if (item != NULL)
		return dynamic_cast<Wave*>(item);
	return NULL;
}

QVariant Project::getProperty(int itemId, const QString& sProperty) {
	Item* o = findItem(itemId);
	CHECK_ASSERT_RETVAL(o != NULL, QVariant())

	QVariant v = o->getItemData()->property(sProperty.toLatin1());
	CHECK_ASSERT_RETVAL(v.isValid(), QVariant())

	return v;
}

void Project::setProperty(int itemId, const QString& sProperty, const QVariant& v) {
	Item* o = findItem(itemId);
	CHECK_ASSERT_RET(o != NULL)
	QVariant vOld = o->property(sProperty.toLatin1());
	setProperty(o, sProperty, vOld, v);
}

Wave* Project::waveCreate() {
	WaveData* waveD = new WaveData(m_itemIdNext++, this);
	Wave* wave = new Wave(waveD, this, this);
	m_trash << wave;

	CommandDataGeneric* data = new CommandDataGeneric(CommandType_WaveCreate);
	data->o = wave;
	m_commands->push(new Command(tr("Create wave (#%0)").arg(waveD->itemId()), this, data));

	return wave;
}

void Project::itemDelete(int itemId) {
	Item* item = findItem(itemId);
	CHECK_PARAM_RET(item != NULL);
	CommandDataGeneric* data = new CommandDataGeneric(CommandType_ItemDelete);
	data->o = item;
	m_commands->push(new Command(tr("Delete item (#%0)").arg(itemId), this, data));
}

void Project::setProperty(Item* o, const QString& sProperty, const QVariant& vOld, const QVariant& vNew) {
	if (vNew != vOld) {
		CommandDataProperty* data = new CommandDataProperty;
		data->o = o;
		data->sProperty = sProperty;
		data->vNew = vNew;
		data->vOld = vOld;
		m_commands->push(new Command("Set property " + sProperty, this, data));
	}
}

void Project::handleCommand(CommandData* data, bool bDo) {
	CHECK_PARAM_RET(data != NULL);
	{
		CommandDataProperty* d = dynamic_cast<CommandDataProperty*>(data);
		if (d != NULL) {
			_setProperty(d->o, d->sProperty, (bDo) ? d->vNew : d->vOld);
			return;
		}
	}
	{
		CommandDataGeneric* d = dynamic_cast<CommandDataGeneric*>(data);
		if (d != NULL) {
			switch (d->type) {
			case CommandType_ItemDelete: _itemDelete(d, bDo); break;
			case CommandType_WaveCreate: _waveCreate(d, bDo); break;
			}
			return;
		}
	}
}

void Project::_setProperty(Item* o, const QString& sProperty, const QVariant& v) {
	CHECK_PARAM_RET(o != NULL)

	bool b = o->getItemData()->setProperty(sProperty.toLatin1(), v);
	CHECK_ASSERT_RET(b)

	if (b) {
		//emit wavePropertyChanged(id, sProperty);
		//emit propertyChanged(sTable, id, sProperty);
		emit propertyChanged(o->itemId(), sProperty);
	}
}

void Project::_itemDelete(CommandDataGeneric* data, bool bDo) {
	CHECK_PRECOND_RET(!data->o.isNull());
	Item* item = data->o;
	CHECK_ASSERT_RET(item != NULL);
	CHECK_PRECOND_RET(m_trash.contains(item) != bDo);
	CHECK_PRECOND_RET(m_items.contains(item) == bDo);
	if (bDo) {
		m_items.removeOne(item);
		m_trash << item;
	}
	else {
		m_items << item;
		m_trash.removeOne(item);
	}
}

void Project::_waveCreate(CommandDataGeneric* data, bool bDo) {
	_itemDelete(data, !bDo);
}
