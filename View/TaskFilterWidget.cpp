#include "TaskFilterWidget.h"

#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

#include <Scope/MainScope.h>


TaskFilterWidget::TaskFilterWidget(MainScope* scope, QWidget *parent) :
	TaskWidget(parent),
	m_scope(scope)
{
	setupWidgets();
}

void TaskFilterWidget::setupWidgets()
{
	QRadioButton* m_rdoOff;
	QRadioButton* m_rdoDefault;
	QRadioButton* m_rdoAdvanced;

	m_rdoOff = new QRadioButton(tr("No filter"));
	m_rdoDefault = new QRadioButton(tr("Default filter"));
	m_rdoAdvanced = new QRadioButton(tr("Advanced filter setup"));

	QVBoxLayout* vbox = new QVBoxLayout();

	vbox->addWidget(m_rdoOff);
	vbox->addWidget(m_rdoDefault);
	vbox->addWidget(m_rdoAdvanced);

	QLabel* lbl = new QLabel();
	vbox->addWidget(lbl);

	setLayout(vbox);
}
