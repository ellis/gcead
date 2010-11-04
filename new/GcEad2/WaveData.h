#ifndef __WAVEDATA_H
#define __WAVEDATA_H

#include <QDateTime>
#include <QObject>
#include <QString>

#include "Data.h"


struct WaveData : public Data
{
	Q_OBJECT

	Q_PROPERTY(int id READ id WRITE setId)
	Q_PROPERTY(int recId READ recId WRITE setRecId)
	Q_PROPERTY(int typeId READ typeId WRITE setTypeId)
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(QString comment READ comment WRITE setComment)
	Q_PROPERTY(QString source READ source WRITE setSource)
	Q_PROPERTY(QDateTime time READ time WRITE setTime)
	Q_PROPERTY(double rate READ rate WRITE setRate)
	Q_PROPERTY(double offset READ offset WRITE setOffset)
	Q_PROPERTY(double sensitivity READ sensitivity WRITE setSensitivity)
	Q_PROPERTY(double shift READ shift WRITE setShift)

public:
	explicit WaveData(int objId, QObject *parent = 0);

	int id() const { return m_id; }
	int recId() const { return m_recId; }
	int typeId() const { return m_typeId; }
	const QString& name() const { return m_sName; }
	const QString& comment() const { return m_sComment; }
	/// Text description of the source from which this wave was obtained
	const QString& source() const { return m_sSource; }
	const QDateTime& time() const { return m_time; }
	double rate() const { return m_nRate; }
	double offset() const { return m_nOffset; }
	double sensitivity() const { return m_nSensitivity; }
	double shift() const { return m_nShift; }

//public slots:
	void setId(int id);
	void setRecId(int recId);
	void setTypeId(int typeId);
	void setName(const QString& sName);
	void setComment(const QString& sComment);
	void setSource(const QString& sSource);
	void setTime(const QDateTime& time);
	void setRate(double nRate);
	void setOffset(double nOffset);
	void setSensitivity(double nSensitivity);
	void setShift(double nShift);

private:
	int m_id;
	int m_recId;
	int m_typeId;
	QString m_sName;
	QString m_sComment;
	QString m_sSource;
	QDateTime m_time;
	double m_nRate;
	double m_nOffset;
	double m_nSensitivity;
	double m_nShift;
};

#endif
