#ifndef __WAVEPROXY_H
#define __WAVEPROXY_H

#include <QDateTime>
#include <QObject>
#include <QPointer>

class Project;
class WaveData;


class WaveProxy : public QObject
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
	//static const WaveProxy null;

public:
	explicit WaveProxy(Project* proj, WaveData* data, QObject *parent = 0);

	int id() const;
	int recId() const;
	int typeId() const;
	QString name() const;
	QString comment() const;
	QString source() const;
	QDateTime time() const;
	double rate() const;
	double offset() const;
	double sensitivity() const;
	double shift() const;

signals:
	//void propertyChanged(const QString& sPropertyName);
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

protected:
	void setDataProperty(const QString& sProperty, const QVariant& v);

private:
	//WaveProxy();

private:
	QPointer<Project> proj;
	QPointer<WaveData> data;
};

#endif
