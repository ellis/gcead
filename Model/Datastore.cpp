#include "Datastore.h"

#include "EadFile.h"
#include "RecInfo.h"


Datastore::Datastore(QObject *parent) :
	QObject(parent)
{
}

void Datastore::loadFile(const QString &sFilename) {
	EadFile file;
	file.load(sFilename);

	EntityTop* top = new EntityTop();
	top->setKey("top");
	top->setValue("comment", file.comment());
	add(top);

	foreach (const RecInfo* rec, file.recs()) {
		EntityRec const* e = new EntityRec();
		const QString sKeyRec = QString("r%1").arg(rec->id(), 2, '0');
		e->setKey(e->ID, sKeyRec);
		e->setValue(e->TIME, rec->timeOfRecording());
		add(e);
	}
}

void Datastore::add(Entity* e) {
	e->setParent(this);
	m_entities.insert(e->key(), e);
}

RecModel::RecModel(QObject *parent)
	: QAbstractListModel(parent)
{
	QHash<int, QByteArray> roles;
	roles[Role_Id] = "id";
	roles[Role_Time] = "timeOfRecording";
	setRoleNames(roles);
}

WaveModel::WaveModel(QObject *parent)
	: QAbstractListModel(parent)
{
	QHash<int, QByteArray> roles;
	roles[Role_Id] = "id";
	roles[Role_Time] = "timeOfRecording";
	setRoleNames(roles);
	Role_Key = Qt::UserRole + 1,
	Role_Type,
	Role_Name,
	Role_FactorNum,
	Role_FactorDen,
	Role_Shift,
	Role_Visible,
	Role_Volts,
	Role_YOffset,
	Role_Comment
}

elem.setAttribute("type", getWaveTypeNodeName(wave->type));
elem.setAttribute("name", wave->sName);
elem.setAttribute("factor_num", wave->nRawToVoltageFactorNum);
elem.setAttribute("factor_den", wave->nRawToVoltageFactorDen);
elem.setAttribute("shift", wave->shift());
elem.setAttribute("visible", (wave->pos.bVisible) ? "t" : "f");
elem.setAttribute("volts", wave->pos.nVoltsPerDivision);
elem.setAttribute("yOffset", wave->pos.nDivisionOffset);

if (!wave->sComment.isEmpty())
{
	QDomElement comment = doc.createElement("comment");
	QDomText text = doc.createTextNode(wave->sComment);
	comment.appendChild(text);
	elem.appendChild(comment);
}

foreach (const WavePeakChosenInfo& peak, wave->peaksChosen)
	createPeakNode(doc, elem, &peak);
