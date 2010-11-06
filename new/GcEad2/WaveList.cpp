#include "WaveList.h"

#include <Check.h>

#include "Wave.h"


WaveRepository::WaveRepository(QObject *parent) :
	ItemRepository(parent)
{
}

Item* WaveRepository::newItem(EdmCommandItemCreate *cmd) {
	CHECK_PARAM_RETVAL(cmd != NULL, NULL);
	CHECK_PARAM_RETVAL(cmd->itemId >= 0, NULL);
	WaveData* waveD = new WaveData(cmd->itemId);
	Wave* wave = new Wave(waveD, this);
	return wave;
}
