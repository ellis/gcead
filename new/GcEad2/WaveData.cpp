#include "WaveData.h"


WaveData::WaveData(QObject *parent)
	: QObject(parent)
{
	m_id = 0;
	m_recId = 0;
	m_typeId = 0;
	m_nRate = 0;
	m_nOffset = 0;
	m_nSensitivity = 0;
	m_nShift = 0;
}

void WaveData::setId(int id) {
	if (id != m_id) {
		m_id = id;
		emit idChanged();
	}
}

void WaveData::setRecId(int recId) {
	if (recId != m_recId) {
		m_recId = recId;
		emit recIdChanged();
	}
}

void WaveData::setTypeId(int typeId) {
	if (typeId != m_typeId) {
		m_typeId = typeId;
		emit typeIdChanged();
	}
}

void WaveData::setName(const QString& sName) {
	if (sName != m_sName) {
		m_sName = sName;
		emit nameChanged();
	}
}

void WaveData::setComment(const QString& sComment) {
	if (sComment != m_sComment) {
		m_sComment = sComment;
		emit commentChanged();
	}
}

void WaveData::setSource(const QString& sSource) {
	if (sSource != m_sSource) {
		m_sSource = sSource;
		emit sourceChanged();
	}
}

#define AAA(Name, signal, var, type)\
void WaveData::set##Name(type var) {\
	if (var != m_##var) {\
	   m_##var = var;\
	   emit signal##Changed();\
   }\
}

#define AAC(Name, signal, var, type)\
void WaveData::set##Name(const type& var) {\
	if (var != m_##var) {\
	   m_##var = var;\
	   emit signal##Changed();\
   }\
}

AAC(Time, time, time, QDateTime)
AAA(Rate, rate, nRate, double)
AAA(Offset, offset, nOffset, double)
AAA(Sensitivity, sensitivity, nSensitivity, double)
AAA(Shift, shift, nShift, double)
