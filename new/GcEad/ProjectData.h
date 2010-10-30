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

	WaveData* getWaveData(int waveId);

private:
	QList<WaveData*> m_waves;
};

#endif // PROJECTDATA_H
