#ifndef __WAVEREPOSITORY_H
#define __WAVEREPOSITORY_H

#include "ItemRepository.h"


class WaveRepository : public ItemRepository
{
    Q_OBJECT
public:
	explicit WaveRepository(QObject *parent = 0);

// ItemList overrides
protected:
	Item* newItem(EdmCommandItemCreate *cmd);
};

#endif
