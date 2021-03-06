#include "Project.h"

#include "WaveProxy.h"


Project::SetPropertyCommand::SetPropertyCommand(Project* proj, const QString& sTable, int id, const QString& sProperty, const QVariant& vOld, const QVariant& vNew)
	: proj(proj), sTable(sTable), id(id), sProperty(sProperty), vOld(vOld), vNew(vNew)
{
	CHECK_PARAM_NORET(!this->proj.isNull())
	CHECK_PARAM_NORET(vOld.isValid())
	CHECK_PARAM_NORET(vNew.isValid())
	setText("Set " + sTable + " " + sProperty);
}

void Project::SetPropertyCommand::redo() {
	proj->setProperty(sTable, id, sProperty, vNew);
}

void Project::SetPropertyCommand::undo() {
	proj->setProperty(sTable, id, sProperty, vOld);
}

void Project::CommandWaveCreate(Project *proj)
	: proj(proj)
{
	CHECK_PARAM_NORET(!proj.isNull())
	setText("Create new wave");
}

void Project::CommandWaveCreate::redo() {
	CHECK_PRECOND_RET(!proj.isNull())
	if (wave.isNull()) {
		wave = proj->createWave();
	}
	else {
		proj->attachWave(wave);
	}
}

void Project::CommandWaveCreate::undo() {
	CHECK_PRECOND_RET(!proj.isNull())
	CHECK_PRECOND_RET(!wave.isNull())
	proj->detachWave(wave);
}

void Project::CommandWaveDelete(Project *proj, int waveId)
	: proj(proj), waveId(waveId)
{
	CHECK_PARAM_NORET(!proj.isNull())
	setText("Delete wave");
}

void Project::CommandWaveCreate::redo() {
	CHECK_PRECOND_RET(!proj.isNull())
	proj->detachWave(waveId);
	if (wave.isNull()) {
		wave = proj->createWave();
	}
	else {
		proj->attachWave(wave);
	}
}

void Project::CommandWaveCreate::undo() {
	CHECK_PRECOND_RET(!proj.isNull())
	CHECK_PRECOND_RET(!wave.isNull())
	proj->detachWave(wave);
}




Project::Project(ProjectData* projD, QObject* parent)
	: QObject(parent), m_projD(projD)
{
	m_commands = new QUndoStack(this);
}

QVariant Project::getProperty(const QString& sTable, int id, const QString& sProperty) {
	CHECK_PRECOND_RETVAL(!m_projD.isNull(), QVariant())

	QObject* o = NULL;
	if (sTable == "wave")
		o = m_projD->getWaveData(id);
	else {
		CHECK_FAILURE_RETVAL(QVariant())
	}
	CHECK_ASSERT_RETVAL(o != NULL, QVariant())

	QVariant v = o->property(sProperty.toLatin1());
	CHECK_ASSERT_RETVAL(v.isValid(), QVariant())

	return v;
}

WaveProxy* Project::getWave(int waveId) {
	CHECK_PRECOND_RETVAL(!m_projD.isNull(), NULL);

	WaveProxy* waveP = m_mapWaves.value(waveId, NULL);
	if (waveP == NULL) {
		WaveData* waveD = m_projD->getWaveData(waveId);
		if (waveD != NULL) {
			waveP = new WaveProxy(this, waveD, this);
			m_mapWaves[waveId] = waveP;
		}
	}
	return waveP;
}

bool Project::cmdSetProperty(const QString& sTable, int id, const QString& sProperty, const QVariant& v) {
	QVariant vOld = getProperty(sTable, id, sProperty);
	if (v != vOld) {
		SetPropertyCommand* cmd = new SetPropertyCommand(this, "wave", id, sProperty, vOld, v);
		m_commands->push(cmd);
		return true;
	}
	return false;
}

void Project::setProperty(const QString& sTable, int id, const QString& sProperty, const QVariant& v) {
	CHECK_PRECOND_RET(!m_projD.isNull())

	QObject* o = NULL;
	if (sTable == "wave")
		o = m_projD->getWaveData(id);
	else {
		CHECK_FAILURE_RET()
	}
	CHECK_ASSERT_RET(o != NULL)

	bool b = o->setProperty(sProperty.toLatin1(), v);
	//CHECK_ASSERT_RET(b)

	if (b) {
		emit wavePropertyChanged(id, sProperty);
		emit propertyChanged(sTable, id, sProperty);
	}
}

WaveProxy* Project::cmdCreateWave() {

}
