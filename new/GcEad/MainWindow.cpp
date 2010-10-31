#include "MainWindow.h"

#include <QDataWidgetMapper>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
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

	createWidgets(waveD);
}

MainWindow::~MainWindow()
{

}

void MainWindow::createWidgets(WaveData* waveD) {
	ProjectTableModel* tableModel = new ProjectTableModel(this);
	WaveProxy* proxy = new WaveProxy(m_proj, waveD, this);
	ObjectPropertiesModel* objModel = new ObjectPropertiesModel(this);

	QGridLayout* layout = new QGridLayout();
	QTableView* tbl = new QTableView();
	QTreeView* tree = new QTreeView();
	QUndoView* undoView = new QUndoView(m_proj->undoStack());
	QWidget* w = new QWidget();

	tableModel->setProject(m_proj);
	tableModel->setTable("wave");
	tableModel->setProperties(QStringList() << "name" << "comment" << "sensitivity");
	tableModel->setRows(QList<int>() << 1 << 2);

	objModel->setProperties(QStringList() << "name" << "comment" << "sensitivity");
	objModel->addObject(proxy);


	int iCol = 0;

	tbl->setModel(tableModel);
	layout->addWidget(tbl, 0, iCol++);

	tree->setModel(objModel);
	layout->addWidget(tree, 0, iCol++);

	layout->addWidget(createForm(objModel), 0, iCol++);

	undoView->setWindowTitle(tr("Command List"));
	layout->addWidget(undoView, 0, iCol++);

	w->setLayout(layout);
	setCentralWidget(w);

	// TODO:
	// - add some sample waves to the project
	// - create a Model of the waves
	// - show the Model in an editable table
	// - show the Model in an editable form
	// - add a button to modify something in a wave
	// - test that updates by any method leads to changes in all views
	// - test undo/redo
}

QWidget* MainWindow::createForm(ObjectPropertiesModel* model) {
	QWidget* form = new QWidget();
	QGridLayout* grid = new QGridLayout();
	QLineEdit* edtName = new QLineEdit();
	QLineEdit* edtComment = new QLineEdit();
	QSpinBox* spnSensitivity = new QSpinBox();

	int iRow = 0;
	grid->addWidget(new QLabel(tr("Name:")), iRow, 0);
	grid->addWidget(edtName, iRow++, 1);

	grid->addWidget(new QLabel(tr("Comment:")), iRow, 0);
	grid->addWidget(edtComment, iRow++, 1);

	grid->addWidget(new QLabel(tr("Sensitivity:")), iRow, 0);
	grid->addWidget(spnSensitivity, iRow++, 1);

	QDataWidgetMapper* mapper = new QDataWidgetMapper(this);
	mapper->setModel(model);
	mapper->addMapping(edtName, 0);
	mapper->addMapping(edtComment, 1);
	mapper->addMapping(spnSensitivity, 2);
	mapper->toFirst();

	form->setLayout(grid);
	return form;
}
