#include "DoxRoot.h"

#include <Check.h>

#include "DoxChange.h"
#include "DoxItem.h"


DoxRoot::DoxRoot(QObject *parent)
	: DoxItem(parent)
{
	m_nReporters = 0;
	m_report = 0;
}

void DoxRoot::beginDoxReport(const QString& sTitle) {
	if (m_nReporters == 0) {
		m_report = new DoxChangeReport();
		m_report->sTitle = sTitle;
	}
	m_nReporters++;
}

void DoxRoot::addDoxChange(DoxChange* chg) {
	CHECK_PARAM_RET(chg != NULL);
	CHECK_PARAM_NORET(chg->item != NULL);
	if (chg->item == NULL) {
		delete chg;
		return;
	}

	m_report->changes << chg;
	DoxItem* item = dynamic_cast<DoxRoot*>(chg->item->parent());
	while (item != NULL) {
		m_mapTargetToChanges[item] << chg;
		if (item == this)
			break;
		item = dynamic_cast<DoxRoot*>(item->parent());
	}
}

void DoxRoot::endDoxReport() {
	CHECK_PRECOND_RET(m_report != NULL);

	bool bFirst = (m_nReporters == 1);
	if (bFirst) {
		int i = 0;
		// Allow parent items to ensure data consistency
		while (i < m_report->changes.size()) {
			DoxChange* chg = m_report->changes[i];
			DoxItem* item = dynamic_cast<DoxRoot*>(chg->item->parent());
			QList<DoxItem*> chain;
			while (item != NULL) {
				chain << item;
				QList<DoxChange*> changes = m_mapTargetToChanges[item];
				if (changes.size() > 0) {
					item->changeEvent(changes, chain);
					m_mapTargetToChanges[item].clear();
				}
				if (item == this)
					break;
				item = dynamic_cast<DoxRoot*>(item->parent());
			}
			i++;
		}

		// Emit all signals
		foreach (DoxChange* chg, m_report->changes) {
			QMetaObject::invokeMethod(chg->item, chg->sNofity.toAscii(), Qt::DirectConnection);
		}

		CHECK_ASSERT_NORET(m_nReporters == 1)
		m_nReporters = 0;
	}
	else {
		CHECK_ASSERT_NORET(m_nReporters > 0)
		if (m_nReporters > 0)
			m_nReporters--;
		else
			m_nReporters = 0;
	}
}
