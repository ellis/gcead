#include "ProjectData.h"

#include "Check.h"

#include "WaveData.h"


ProjectData::ProjectData(QObject* parent)
	: QObject(parent)
{
	m_waveIdNext = 1;
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

WaveData* ProjectData::createWaveData() {
	WaveData* waveD = new WaveData(this);
	waveD->setId(nextWaveId());
	m_waves << waveD;
	m_mapWaves[waveD->id()] = waveD;
}

void ProjectData::detachWaveData(int waveId) {
	CHECK_PRECOND_RET(m_mapWaves.contains(waveId));
	for (int i = 0; i < m_waves.size(); i++) {
		if (m_waves[i]->id() == waveId) {
			m_waves.removeAt(i);
			break;
		}
	}
	m_mapWaves.remove(waveId);
}

void ProjectData::attachWaveData(WaveData *waveD) {
	CHECK_PARAM_RET(wave != NULL);
	CHECK_PRECOND_RET(!m_mapWaves.contains(wave->id()));
	waveD->setParent(this);
	m_waves << waveD;
	m_mapWaves[waveD->id()] = waveD;
	// TODO: catch as an error if the wave is already part of another project
}

int ProjectData::nextWaveId() {
	return m_waveIdNext++;
}
