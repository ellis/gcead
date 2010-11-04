#include "WaveCommands.h"

using namespace WaveCommands;

WavePropertyCommand::WavePropertyCommand(ProjectData* proj, int waveId, const QString& sProperty, const QVariant& v, const QString& sCommandName)
	: proj(proj), waveId(waveId), sProperty(sProperty), vNew(v)
{
	setText(sCommandName);
	vOld = proj->getWaveProperty(waveId, sProperty.toLatin1());
}

void WavePropertyCommand::redo() {
	proj->setWaveProperty(waveId, sProperty.toLatin1(), vNew);
}

void WavePropertyCommand::undo() {
	proj->setWaveProperty(waveId, sProperty.toLatin1(), vOld);
}
