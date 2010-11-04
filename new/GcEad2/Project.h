#ifndef __PROJECT_H
#define __PROJECT_H

#include <QObject>
#include <QPointer>
#include <QUndoCommand>
#include <QUndoStack>

#include "Check.h"
#include "ProjectData.h"
#include "WaveData.h"


class WaveProxy;


class Project : public QObject
{
    Q_OBJECT
public:
	class CommandBase : public QUndoCommand {
	public:
		CommandBase(Project* proj);
	protected:
		Project* const proj;
	};

	class CommandProperty : public CommandBase {
		CommandProperty(Project* proj, QObject* o, const QString& sProperty, const QVariant& vOld, const QVariant& vNew);
		void redo();
		void undo();
	private:
		QObject const* o;
		const QString sProperty;
		const QVariant vOld;
		const QVariant vNew;
	};

	/*class SetPropertyCommand : public QUndoCommand {
	public:
		SetPropertyCommand(Project* proj, const QString& sTable, int id, const QString& sProperty, const QVariant& vOld, const QVariant& vNew);

		virtual void redo();
		virtual void undo();

	private:
		const QString sTable;
		const int id;
		const QString sProperty;
		const QVariant vOld;
		const QVariant vNew;
	};*/


	class CommandWaveCreate : public CommandBase {
	public:
		CommandWaveCreate(Project* proj);
		void redo();
		void undo();
	private:
		int waveId;
	};

public:
	explicit Project(ProjectData* projD, QObject *parent = 0);

	QUndoStack* undoStack() { return m_commands; }

	QVariant getProperty(const QString& sTable, int id, const QString& sProperty);
	WaveProxy* getWave(int waveId);

	bool cmdSetProperty(const QString& sTable, int id, const QString& sProperty, const QVariant& v);

	WaveProxy* cmdCreateWave();
	void cmdDeleteWave(int waveId);

signals:
	void propertyChanged(const QString& sTable, int id, const QString& sProperty);
	void wavePropertyChanged(int waveId, const QString& sProperty);

public slots:

private:
	friend class SetPropertyCommand;

	void setProperty(const QString& sTable, int id, const QString& sProperty, const QVariant& v);

private:
	QUndoStack* m_commands;

	int m_waveIdNext;
	QList<WaveProxy*> m_waves;
	QMap<int, int> m_mapWaveIdToIndex;
};

#endif
