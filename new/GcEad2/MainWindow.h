#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class QAbstractTableModel;

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
};

#endif // MAINWINDOW_H
