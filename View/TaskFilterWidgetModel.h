/*
#ifndef TASKFILTERWIDGETMODEL_H
#define TASKFILTERWIDGETMODEL_H

#include <QObject>

class TaskFilterWidgetModel : public QObject
{
    Q_OBJECT

	Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
	Q_PROPERTY(int type READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
public:
    explicit TaskFilterWidgetModel(QObject *parent = 0);



signals:

public slots:

private:
	bool m_bEnabled;

};

#endif // TASKFILTERWIDGETMODEL_H
*/
