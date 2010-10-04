#ifndef TASKFILTERWIDGET_H
#define TASKFILTERWIDGET_H

#include "TaskWidget.h"


class QComboBox;
class QPlainTextEdit;

class FilterTesterInfo;
class MainScope;


class TaskFilterWidget : public TaskWidget
{
    Q_OBJECT
public:
	TaskFilterWidget(MainScope* scope, QWidget *parent = 0);

signals:

private:
	void setupWidgets();

private slots:
	void on_scope_fileChanged();
	void on_cmbEad_currentIndexChanged();
	void on_btnEad_clicked();
	void on_cmbFid_currentIndexChanged();
	void on_btnFid_clicked();

private:
	void updateEditWidget(FilterTesterInfo* filter, QPlainTextEdit* edt);
	void run(FilterTesterInfo* filter, QString sProperties);

private:
	MainScope* const m_scope;

	QComboBox* m_cmbEad;
	QPlainTextEdit* m_edtEadProperties;
	QComboBox* m_cmbFid;
	QPlainTextEdit* m_edtFidProperties;
	FilterTesterInfo* m_filterEad;
	FilterTesterInfo* m_filterFid;
};

#endif // TASKFILTERWIDGET_H
