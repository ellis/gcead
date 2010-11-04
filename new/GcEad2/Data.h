#ifndef __DATA_H
#define __DATA_H

#include <QObject>


class Data : public QObject {
	Q_OBJECT
public:
	Data(int itemId, QObject* parent = NULL)
		: QObject(parent), m_itemId(itemId)
	{
	}

	int itemId() const { return m_itemId; }

private:
	const int m_itemId;
};

#endif
