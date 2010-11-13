#include "GraphicsWidget1.h"

#include <QGraphicsTextItem>


GraphicsWidget1::GraphicsWidget1(QGraphicsItem* parent) :
    QGraphicsWidget(parent)
{
	new QGraphicsTextItem("Hi", this);
}
