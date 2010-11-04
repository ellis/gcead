#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>


class ObjectPropertiesModel;
class Project;
class WaveData;
class WaveProxy;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	void createWidgets(WaveData* waveD);
	QWidget* createForm(ObjectPropertiesModel* model);

private:
	Project* m_proj;
};

#endif // MAINWINDOW_H
