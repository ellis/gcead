#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QtGui/QMainWindow>

class QAbstractTableModel;
class QListWidget;
class QScriptEngine;
class QScriptEngineDebugger;

class ItemList;
class ItemListModel;
class Wave;
class WaveListController;
class WaveRepository;
class Workspace;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	void createWidgets();
	QWidget* createCentralWidget1();
	QWidget* createCentralWidget2();
	QWidget* createForm(QAbstractTableModel* model);

private slots:
	void on_proj_logCommand(const QString& s);

private:
	Workspace* m_space;
	WaveRepository* m_waveRepo;
	//const ItemList* m_waves;
	WaveListController* m_waves;
	ItemListModel* m_itemModel;
	QScriptEngine* m_engine;

	QScriptEngineDebugger* m_debugger;
	QListWidget* m_lstLog;
};

#endif
