#include "Wave.h"

#include "Check.h"
#include "Project.h"
#include "WaveData.h"


Wave::Wave(WaveData* data, IItemPropertySetter* setter, QObject* parent) :
	Item(data, parent), setter(setter), m_data(data)
{
	CHECK_PARAM_RET(setter != NULL)
	CHECK_PARAM_RET(data != NULL)

	/*#define C(name) connect(data, SIGNAL(name##Changed()), this, SIGNAL(name##Changed()))
	C(id);
	C(recId);
	C(typeId);
	C(name);
	C(comment);
	C(source);
	C(time);
	C(rate);
	C(offset);
	C(sensitivity);
	C(shift);*/
}

/*
Wave::Wave()
	: proj(NULL), data(NULL)
{
}
*/

//int Wave::objId() const { return data->objId(); }
//int Wave::id() const { return data->id(); }
int Wave::recId() const { return m_data->recId(); }
int Wave::typeId() const { return m_data->typeId(); }
QString Wave::name() const { return m_data->name(); }
QString Wave::comment() const { return m_data->comment(); }
QString Wave::source() const { return m_data->source(); }
QDateTime Wave::time() const { return m_data->time(); }
double Wave::rate() const { return m_data->rate(); }
double Wave::offset() const { return m_data->offset(); }
double Wave::sensitivity() const { return m_data->sensitivity(); }
double Wave::shift() const { return m_data->shift(); }

void Wave::setDataProperty(const QString& sProperty, const QVariant& v) {
	CHECK_PARAM_RET(!sProperty.isEmpty())
	QVariant vOld = property(sProperty.toLatin1());
	setter->setProperty(this, sProperty, vOld, v);
}

//void Wave::setId(int id) { setDataProperty("id", id); }
void Wave::setRecId(int recId) { setDataProperty("recId", recId); }
void Wave::setTypeId(int typeId) { setDataProperty("typeId", typeId); }
void Wave::setName(const QString& sName) { setDataProperty("name", sName); }
void Wave::setComment(const QString& sComment) { setDataProperty("comment", sComment); }
void Wave::setSource(const QString& sSource) { setDataProperty("source", sSource); }
void Wave::setTime(const QDateTime& time) { setDataProperty("time", time); }
void Wave::setRate(double nRate) { setDataProperty("rate", nRate); }
void Wave::setOffset(double nOffset) { setDataProperty("offset", nOffset); }
void Wave::setSensitivity(double nSensitivity) { setDataProperty("sensitivity", nSensitivity); }
void Wave::setShift(double nShift) { setDataProperty("shift", nShift); }
