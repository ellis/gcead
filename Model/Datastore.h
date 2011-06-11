#ifndef __DATASTORE_H
#define __DATASTORE_H

#include <QAbstractListModel>
#include <QObject>
#include <QVariantHash>


/*
class DatastoreEntity : public QObject {
	Q_OBJECT
public:
	const QString& key() const { return m_key; }
	const QVariant value(const QString& name) const { return m_fields.value(name); }

signals:
	void valueChanged(const QString& sName, const QVariant& v);

private:
	friend class Datastore;

	void setKey(const QString& key) { m_key = key; }
	void setValue(const QString& sName, const QVariant& v) { m_fields.insert(sName, v); }

private:
	QString m_key;
	QVariantHash m_fields;
};
*/


class Datastore : public QObject
{
    Q_OBJECT
public:
    explicit Datastore(QObject *parent = 0);

	void loadFile(const QString& sFilename);

	//const DatastoreEntity* getEntity(const QString& key);
	const QObject* getEntity(const QString& key);

signals:
	void valueChanged(const QString& key, const QString& name, const QVariantHash& record);

public slots:

private:
	void add(Entity* e);

private:
	//QHash<QString, DatastoreEntity*> m_records;
	QHash<QString, class Entity*> m_entities;
};


class Entity : public QObject {
	Q_OBJECT
public:
	const QString& key() const { return m_key; }
	const QVariant value(const QString& name) const { return m_fields.value(name); }

signals:
	void valueChanged(const QString& sName, const QVariant& v);

private:
	friend class Datastore;

	void setKey(const QString& key) { m_key = key; }
	void setValue(const QString& sName, const QVariant& v) { m_fields.insert(sName, v); }

private:
	QString m_key;
	QVariantHash m_fields;
};


class EntityTop : public Entity {
	Q_OBJECT
	Q_PROPERTY(QString comment READ comment NOTIFY commentChanged)
public:
	const QString& comment() const { return value("comment"); }

signals:
	void commentChanged();
};

class EntityRec : public Entity {
	Q_OBJECT
	Q_PROPERTY(int id READ id)
	Q_PROPERTY(QDateTime time READ time);
public:
	static const char* ID = "id";
	static const char* TIME = "time";
public:
	const QString& id() const { return value(ID); }
	const QDateTime& timeOfRecording() const { return value(TIME).toDateTime(); }
};

class RecModel : public QAbstractListModel {
	Q_OBJECT
public:
	enum RecRoles {
		Role_Id = Qt::UserRole + 1,
		Role_Time
	};

public:
	explicit RecModel(QObject* parent = NULL);
};

class WaveModel : public QAbstractListModel {
	Q_OBJECT
public:
	enum WaveRoles {
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
	};
public:
	explicit WaveModel(QObject* parent = NULL);
};

#endif
