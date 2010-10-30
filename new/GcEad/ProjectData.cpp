#include "ProjectData.h"

#include "Check.h"

#include "WaveData.h"


ProjectData::ProjectData(QObject* parent)
	: QObject(parent)
{
}

WaveData* ProjectData::getWaveData(int waveId) {
	CHECK_PARAM_RETVAL(waveId >= 0, NULL)
	CHECK_PARAM_RETVAL(waveId < m_waves.size(), NULL)

	foreach (WaveData* wave, m_waves) {
		if (wave->id() == waveId)
			return wave;
	}
	return NULL;
}
