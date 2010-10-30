#include "ProjectData.h"

#include "Check.h"

#include "WaveData.h"


ProjectData::ProjectData(QObject* parent)
	: QObject(parent)
{
	m_waveIdNext = 1;
}

void ProjectData::addWaveData(WaveData *wave) {
	CHECK_PARAM_RET(wave != NULL);
	wave->setId(nextWaveId());
	wave->setParent(this);
	// TODO: catch as an error if the wave is already part of another project
	m_waves << wave;
}

int ProjectData::nextWaveId() {
	return m_waveIdNext++;
}

WaveData* ProjectData::getWaveData(int waveId) {
	CHECK_PARAM_RETVAL(waveId >= 0, NULL)
	//CHECK_PARAM_RETVAL(waveId < m_waves.size(), NULL)

	foreach (WaveData* wave, m_waves) {
		if (wave->id() == waveId)
			return wave;
	}
	return NULL;
}
