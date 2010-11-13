#ifndef __DOXCHANGE_H
#define __DOXCHANGE_H

#include <QPointer>
#include <QString>
#include <QVariantList>


class DoxItem;


class DoxChange
{
public:
    DoxChange();

	QPointer<DoxItem> item;
	QString sProperty;
	QString sCommand;
	QVariantList avArgs;
	QVariantList avUndo;
	QString sNofity;
};

#endif
