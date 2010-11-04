#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class QAbstractTableModel;
class QScriptEngine;

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
	QWidget* createForm(QAbstractTableModel* model);

private:
	Project* m_proj;
	QScriptEngine* m_engine;
};

#endif // MAINWINDOW_H
