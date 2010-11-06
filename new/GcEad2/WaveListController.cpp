#include "WaveListController.h"

#include <Check.h>

#include "Wave.h"


WaveListController::WaveListController(QObject *parent) :
    QObject(parent)
{
}

void WaveListController::setRepository(WaveRepository* repo) {
	m_repo = repo;
}

Wave* WaveListController::find(int itemId) {
	if (m_repo.isNull())
		return NULL;
	return static_cast<Wave*>(m_repo->activeItems()->find(itemId));
}

Wave* WaveListController::add() {
	Item* item = m_repo->createInactiveItem();
	m_repo->activateItem(item->itemId(), true);
	return static_cast<Wave*>(item);
}

void WaveListController::remove(Wave* wave) {
	CHECK_PARAM_RET(wave != NULL)
	m_repo->activateItem(wave->itemId(), false);
}

void WaveListController::remove(int itemId) {
	m_repo->activateItem(itemId, false);
}
