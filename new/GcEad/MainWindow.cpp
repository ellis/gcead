#include "MainWindow.h"

#include <QGridLayout>
#include <QTableView>
#include <QTreeView>
#include <QUndoView>

#include "ObjectPropertiesModel.h"
#include "Project.h"
#include "ProjectData.h"
#include "ProjectTableModel.h"
#include "WaveData.h"
#include "WaveProxy.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	ProjectData* projD = new ProjectData();

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

	m_proj = new Project(projD, this);

	ProjectTableModel* tableModel = new ProjectTableModel(this);
	tableModel->setProject(m_proj);
	tableModel->setTable("wave");
	tableModel->setProperties(QStringList() << "name" << "comment" << "sensitivity");
	tableModel->setRows(QList<int>() << 1 << 2);

	WaveProxy* proxy = new WaveProxy(m_proj, waveD, this);
	ObjectPropertiesModel* objModel = new ObjectPropertiesModel(this);
	objModel->setProperties(QStringList() << "name" << "comment" << "sensitivity");
	objModel->addObject(proxy);

	QGridLayout* layout = new QGridLayout();
	QTableView* tbl;
	tbl = new QTableView();
	tbl->setModel(tableModel);
	layout->addWidget(tbl, 0, 0);
	QTreeView* tree = new QTreeView();
	tree->setModel(objModel);
	layout->addWidget(tree, 0, 1);

	QWidget* w = new QWidget();
	w->setLayout(layout);
	setCentralWidget(w);

	createUndoView();

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

void MainWindow::createUndoView()
{
	QUndoView* undoView = new QUndoView(m_proj->undoStack());
	undoView->setWindowTitle(tr("Command List"));
	undoView->show();
	undoView->setAttribute(Qt::WA_QuitOnClose, false);
}
