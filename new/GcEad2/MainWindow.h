#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class QAbstractTableModel;
class QListWidget;
class QScriptEngine;
class QScriptEngineDebugger;

class ItemListModel;
class Project;
class Wave;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	void createWidgets(Wave* wave);
	QWidget* createCentralWidget1();
	QWidget* createCentralWidget2();
	QWidget* createForm(QAbstractTableModel* model);

private slots:
	void on_proj_logCommand(const QString& s);

private:
	Project* m_proj;
	ItemListModel* m_itemModel;
	QScriptEngine* m_engine;

	QScriptEngineDebugger* m_debugger;
	QListWidget* m_lstLog;
};

#endif // MAINWINDOW_H
