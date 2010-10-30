#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include <QObject>
#include <QList>
#include <QVariant>

class WaveData;


class ProjectData : public QObject
{
	Q_OBJECT

public:
	explicit ProjectData(QObject* parent = NULL);

	void addWaveData(WaveData* wave);
	WaveData* getWaveData(int waveId);

private:
	int nextWaveId();

private:
	int m_waveIdNext;
	QList<WaveData*> m_waves;
};

#endif // PROJECTDATA_H
