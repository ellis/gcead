#include "MainWindow.h"

#include "ProjectData.h"
#include "Project.h"
#include "WaveData.h"
#include "WaveProxy.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	ProjectData* projD = new ProjectData();
	Project* proj = new Project(projD, this);

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
