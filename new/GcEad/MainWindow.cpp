#include "MainWindow.h"

#include <QTableView>

#include "Project.h"
#include "ProjectData.h"
#include "ProjectTableModel.h"
#include "WaveData.h"
#include "WaveProxy.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	ProjectData* projD = new ProjectData();
	Project* proj = new Project(projD, this);

	WaveData* waveD;

	waveD = new WaveData();
	waveD->setName("One");
	waveD->setComment("Yay");
	waveD->setSensitivity(1);
	projD->addWaveData(waveD);

	waveD = new WaveData();
	waveD->setName("Two");
	waveD->setComment("Yay");
	waveD->setSensitivity(2);
	projD->addWaveData(waveD);

	ProjectTableModel* model = new ProjectTableModel(this);
	model->setProject(proj);
	model->setTable("wave");
	model->setProperties(QStringList() << "name" << "comment" << "sensitivity");
	model->setRows(QList<int>() << 1 << 2);

	QTableView* tbl;
	tbl = new QTableView();
	tbl->setModel(model);

	setCentralWidget(tbl);

	// TODO:
	// - add some sample waves to the project
	// - create a Model of the waves
	// - show the Model in an editable table
	// - show the Model in an editable form
	// - add a button to modify something in a wave
	// - test that updates by any method leads to changes in all views
	// - test undo/redo
}

MainWindow::~MainWindow()
{

}
