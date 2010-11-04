#include "Project.h"

#include <Check.h>

#include "ItemList.h"
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
	m_items = new ItemList(this);
	m_items->setObjectName("items");
	m_trash = new ItemList(this);
	m_trash->setObjectName("trash");
}

Project::~Project() {
}

Item* Project::findItem(int itemId) const {
	return m_items->find(itemId);
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
	m_trash->add(wave);

	CommandDataGeneric* data = new CommandDataGeneric(CommandType_WaveCreate);
	data->o = wave;
	emit logCommand(QString("project.waveCreate()"));
	m_commands->push(new Command(tr("Create wave (#%0)").arg(waveD->itemId()), this, data));

	return wave;
}

void Project::itemDelete(int itemId) {
	Item* item = findItem(itemId);
	CHECK_PARAM_RET(item != NULL);
	CommandDataGeneric* data = new CommandDataGeneric(CommandType_ItemDelete);
	data->o = item;
	emit logCommand(QString("project.itemDelete(%0)").arg(itemId));
	m_commands->push(new Command(tr("Delete item (#%0)").arg(itemId), this, data));
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
		emit logCommand(QString("project.findItem(%0).%1 = \"%2\"").arg(item->itemId()).arg(sProperty).arg(vNew.toString()));
		m_commands->push(new Command("Set property " + sProperty, this, data));
	}
}

void Project::handleCommand(CommandData* data, bool bDo) {
	CHECK_PARAM_RET(data != NULL);
	if (!data->bRunOnce)
		data->bRunOnce = true;
	else {
		emit logCommand(QString("project.%0()").arg(bDo ? "redo" : "undo"));
	}

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

void Project::_setProperty(Item* item, const QString& sProperty, const QVariant& v) {
	CHECK_PARAM_RET(item != NULL)

	bool b = item->getItemData()->setProperty(sProperty.toLatin1(), v);
	CHECK_ASSERT_RET(b)

	if (b) {
		//emit wavePropertyChanged(id, sProperty);
		//emit propertyChanged(sTable, id, sProperty);
		m_items->emit_itemPropertyChanged(item->itemId(), sProperty);
	}
}

void Project::_itemDelete(CommandDataGeneric* data, bool bDo) {
	CHECK_PRECOND_RET(!data->o.isNull());
	Item* item = data->o;
	CHECK_ASSERT_RET(item != NULL);
	CHECK_PRECOND_RET(m_trash->contains(item) != bDo);
	CHECK_PRECOND_RET(m_items->contains(item) == bDo);
	if (bDo) {
		m_items->remove(item);
		m_trash->add(item);
	}
	else {
		m_items->add(item);
		m_trash->remove(item);
	}
}

void Project::_waveCreate(CommandDataGeneric* data, bool bDo) {
	_itemDelete(data, !bDo);
}
