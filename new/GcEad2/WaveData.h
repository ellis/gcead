#ifndef __WAVEDATA_H
#define __WAVEDATA_H

#include <QDateTime>
#include <QObject>
#include <QString>


struct WaveData : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
	Q_PROPERTY(int recId READ recId WRITE setRecId NOTIFY recIdChanged)
	Q_PROPERTY(int typeId READ typeId WRITE setTypeId NOTIFY typeIdChanged)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY commentChanged)
	Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
	Q_PROPERTY(QDateTime time READ time WRITE setTime NOTIFY timeChanged)
	Q_PROPERTY(double rate READ rate WRITE setRate NOTIFY rateChanged)
	Q_PROPERTY(double offset READ offset WRITE setOffset NOTIFY offsetChanged)
	Q_PROPERTY(double sensitivity READ sensitivity WRITE setSensitivity NOTIFY sensitivityChanged)
	Q_PROPERTY(double shift READ shift WRITE setShift NOTIFY shiftChanged)

public:
	explicit WaveData(QObject *parent = 0);

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

signals:
	void idChanged();
	void recIdChanged();
	void typeIdChanged();
	void nameChanged();
	void commentChanged();
	void sourceChanged();
	void timeChanged();
	void rateChanged();
	void offsetChanged();
	void sensitivityChanged();
	void shiftChanged();

public slots:
	virtual void setId(int id);
	virtual void setRecId(int recId);
	virtual void setTypeId(int typeId);
	virtual void setName(const QString& sName);
	virtual void setComment(const QString& sComment);
	virtual void setSource(const QString& sSource);
	virtual void setTime(const QDateTime& time);
	virtual void setRate(double nRate);
	virtual void setOffset(double nOffset);
	virtual void setSensitivity(double nSensitivity);
	virtual void setShift(double nShift);

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
