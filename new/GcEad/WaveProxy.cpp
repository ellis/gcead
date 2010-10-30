#include "WaveProxy.h"

#include "Check.h"
#include "Project.h"
#include "WaveData.h"


WaveProxy::WaveProxy(Project* proj, WaveData* data, QObject *parent) :
	QObject(parent), proj(proj), data(data)
{
	CHECK_PARAM_RET(proj != NULL)
	CHECK_PARAM_RET(data != NULL)

	#define C(name) connect(data, SIGNAL(name##Changed()), this, SIGNAL(name##Changed()))
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
	C(shift);
}

int WaveProxy::id() const { CHECK_PRECOND_RETVAL(!data.isNull(), -1) return data->id(); }
int WaveProxy::recId() const { CHECK_PRECOND_RETVAL(!data.isNull(), -1) return data->recId(); }
int WaveProxy::typeId() const { CHECK_PRECOND_RETVAL(!data.isNull(), -1) return data->typeId(); }
QString WaveProxy::name() const { CHECK_PRECOND_RETVAL(!data.isNull(), QString::null) return data->name(); }
QString WaveProxy::comment() const { CHECK_PRECOND_RETVAL(!data.isNull(), QString::null) return data->comment(); }
QString WaveProxy::source() const { CHECK_PRECOND_RETVAL(!data.isNull(), QString::null) return data->source(); }
QDateTime WaveProxy::time() const { CHECK_PRECOND_RETVAL(!data.isNull(), QDateTime()) return data->time(); }
double WaveProxy::rate() const { CHECK_PRECOND_RETVAL(!data.isNull(), 0) return data->rate(); }
double WaveProxy::offset() const { CHECK_PRECOND_RETVAL(!data.isNull(), 0) return data->offset(); }
double WaveProxy::sensitivity() const { CHECK_PRECOND_RETVAL(!data.isNull(), 0) return data->sensitivity(); }
double WaveProxy::shift() const { CHECK_PRECOND_RETVAL(!data.isNull(), 0) return data->shift(); }

void WaveProxy::setDataProperty(const QString& sProperty, const QVariant& v) {
	CHECK_PRECOND_RET(!proj.isNull())
	CHECK_PRECOND_RET(!data.isNull())
	CHECK_PARAM_RET(!sProperty.isEmpty())
	proj->cmdSetProperty("wave", data->id(), sProperty, v);
}

void WaveProxy::setId(int id) { setDataProperty("id", id); }
void WaveProxy::setRecId(int recId) { setDataProperty("recId", recId); }
void WaveProxy::setTypeId(int typeId) { setDataProperty("typeId", typeId); }
void WaveProxy::setName(const QString& sName) { setDataProperty("name", sName); }
void WaveProxy::setComment(const QString& sComment) { setDataProperty("comment", sComment); }
void WaveProxy::setSource(const QString& sSource) { setDataProperty("source", sSource); }
void WaveProxy::setTime(const QDateTime& time) { setDataProperty("time", time); }
void WaveProxy::setRate(double nRate) { setDataProperty("rate", nRate); }
void WaveProxy::setOffset(double nOffset) { setDataProperty("offset", nOffset); }
void WaveProxy::setSensitivity(double nSensitivity) { setDataProperty("sensitivity", nSensitivity); }
void WaveProxy::setShift(double nShift) { setDataProperty("shift", nShift); }
