#ifndef TASKFILTERWIDGET_H
#define TASKFILTERWIDGET_H

#include "TaskWidget.h"


class MainScope;


class TaskFilterWidget : public TaskWidget
{
    Q_OBJECT
public:
	TaskFilterWidget(MainScope* scope, QWidget *parent = 0);

signals:

public slots:

private:
	void setupWidgets();

private:
	MainScope* const m_scope;
};

#endif // TASKFILTERWIDGET_H
