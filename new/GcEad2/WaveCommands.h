#ifndef __WAVECOMMANDS_H
#define __WAVECOMMANDS_H

#include <QUndoCommand>

#include "ProjectData.h"
#include "WaveData.h"


namespace WaveCommands {
/*
	class WavePropertyCommand : public QUndoCommand {
	public:
		WavePropertyCommand(ProjectData* proj, int waveId, const QString& sProperty, const QVariant& v, const QString& sCommandName);
		virtual void redo();
		virtual void undo();

	private:
		ProjectData* proj;
		int waveId;
		QString sProperty;
		QVariant vNew;
		QVariant vOld;
	};

#define MK(Name, property, type) \
	class Name : public WavePropertyCommand {\
	public:\
		Name(ProjectData* proj, int waveId, const type& val)\
			: WavePropertyCommand(proj, waveId, property, val, QString("Set wave %1").arg(property))\
		{}\
	}
	MK(WaveNameCommand, "name", QString);
	MK(WaveCommentCommand, "comment", QString);
	MK(WaveSourceCommand, "source", QString);
#undef MK
*/

	template <typename T>
	class WaveDataCommand : public QUndoCommand {
	public:
		WaveDataCommand(ProjectData* proj, int waveId, T WaveData::*ptr, const T& v, const QString& sCommandName)
			: proj(proj), waveId(waveId), ptr(ptr), vNew(v)
		{
			setText(sCommandName);
		}

		virtual void redo() {
			proj->setWaveProperty(waveId, sProperty, vNew);
		}

		virtual void undo() {
			proj->setWaveProperty(waveId, sProperty, vOld);
		}

		/*WavePropertyCommand* setName(ProjectData* proj, int waveId, const QString& sName) {
			return new WavePropertyCommand(proj, waveId, "name", sName, "Set wave name");
		}*/

	private:
		ProjectData* proj;
		int waveId;
		T WaveData::*ptr;
		QVariant vNew;
		QVariant vOld;
	};

/*#define MK(Name, property, type) \
	class Name : public WavePropertyCommand {\
	public:\
		Name(ProjectData* proj, int waveId, const type& val)\
			: WavePropertyCommand(proj, waveId, property, val, QString("Set wave %1").arg(property))\
		{}\
	}
	MK(WaveNameCommand, "name", QString);
	MK(WaveCommentCommand, "comment", QString);
	MK(WaveSourceCommand, "source", QString);
#undef MK*/

	/*
	template <const char* sProperty, typename T>
	class WavePropertyCommand : public QUndoCommand {
	public:
		WavePropertyCommand(ProjectData* proj, int waveId, const T& v)
			: proj(proj), waveId(waveId), vNew(v)
		{
			QString sCommandName = QString("Set wave %1").arg(sProperty);
			setText(sCommandName);
			vOld = proj->getWaveProperty(waveId, sProperty);
		}

		virtual void redo() {
			proj->setWaveProperty(waveId, sProperty, vNew);
		}

		virtual void undo() {
			proj->setWaveProperty(waveId, sProperty, vOld);
		}

	private:
		ProjectData* proj;
		int waveId;
		QVariant vNew;
		QVariant vOld;
	};

	typedef WavePropertyCommand<"name", QString> WaveNameCommand;
	typedef WavePropertyCommand<"comment", QString> WaveNameCommand;
	typedef WavePropertyCommand<"source", QString> WaveNameCommand;
	*/
};

#endif
