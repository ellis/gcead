#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QVariant>

class WaveData;


class ProjectData : public QObject
{
	Q_OBJECT

public:
	explicit ProjectData(QObject* parent = NULL);

	WaveData* getWaveData(int waveId);

	WaveData* createWaveData();
	void detachWaveData(int waveId);
	void attachWaveData(WaveData* wave);

private:
	int nextWaveId();

private:
	int m_waveIdNext;
	QList<WaveData*> m_waves;
	QMap<int, WaveData*> m_mapWaves;
};

#endif // PROJECTDATA_H
