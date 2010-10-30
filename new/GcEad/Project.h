#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QPointer>
#include <QUndoCommand>
#include <QUndoStack>

#include "Check.h"
#include "ProjectData.h"
#include "WaveData.h"


class Project : public QObject
{
    Q_OBJECT
public:
	class SetPropertyCommand : public QUndoCommand {
	public:
		SetPropertyCommand(Project* proj, const QString& sTable, int id, const QString& sProperty, const QVariant& vOld, const QVariant& vNew);

		virtual void redo();
		virtual void undo();

	private:
		QPointer<Project> proj;
		const QString sTable;
		const int id;
		const QString sProperty;
		const QVariant vOld;
		const QVariant vNew;
	};

public:
	explicit Project(ProjectData* projD, QObject *parent = 0);

	bool cmdSetProperty(const QString& sTable, int id, const QString& sProperty, const QVariant& v);

signals:
	void propertyChanged(const QString& sTable, int id, const QString& sProperty);
	void wavePropertyChanged(int waveId, const QString& sProperty);

public slots:

private:
	friend class SetPropertyCommand;

	QVariant getProperty(const QString& sTable, int id, const QString& sProperty);
	void setProperty(const QString& sTable, int id, const QString& sProperty, const QVariant& v);

private:
	QPointer<ProjectData> m_projD;
	QUndoStack* m_commands;

};

#endif // PROJECT_H
