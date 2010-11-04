#include "MainWindow.h"

#include <QDataWidgetMapper>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QSpinBox>
#include <QTableView>
#include <QTreeView>
#include <QUndoView>

#include "ObjectPropertiesModel.h"
#include "Project.h"
#include "ProjectTableModel.h"
#include "Wave.h"


static QScriptValue waveToScript(QScriptEngine* engine, Wave* const& in) { return engine->newQObject(in); }
static void scriptToWave(const QScriptValue& val, Wave*& out) { out = qobject_cast<Wave*>(val.toQObject()); }

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	m_engine = new QScriptEngine(this);

	m_proj = new Project(this);

	Wave* wave = m_proj->waveCreate();
	wave->setName("One");
	wave->setComment("Yay");
	wave->setSensitivity(1);

	wave = m_proj->waveCreate();
	wave->setName("Two");
	wave->setComment("Yay");
	wave->setSensitivity(2);

	createWidgets(wave);

	qRegisterMetaType<Wave*>("Wave*");

	QScriptValue val = m_engine->newQObject(m_proj);
	m_engine->globalObject().setProperty("project", val);
	qScriptRegisterMetaType(m_engine, waveToScript, scriptToWave);

	QScriptEngineDebugger* debugger = new QScriptEngineDebugger(this);
	debugger->attachTo(m_engine);
	debugger->standardWindow()->show();
	//m_engine->registerCustomType();
	//m_engine->evaluate("debugger; 5 + 4");

}

MainWindow::~MainWindow()
{

}

void MainWindow::createWidgets(Wave* wave) {
	ProjectTableModel* tableModel = new ProjectTableModel(this);
	//ObjectPropertiesModel* objModel = new ObjectPropertiesModel(this);

	QGridLayout* layout = new QGridLayout();
	QTableView* tbl = new QTableView();
	QTreeView* tree = new QTreeView();
	QUndoView* undoView = new QUndoView(m_proj->undoStack());
	QWidget* w = new QWidget();

	tableModel->setProject(m_proj);
	tableModel->setProperties(QStringList() << "name" << "comment" << "sensitivity");

	//objModel->setProperties(QStringList() << "name" << "comment" << "sensitivity");
	//objModel->addObject(proxy);


	int iCol = 0;

	tbl->setModel(tableModel);
	layout->addWidget(tbl, 0, iCol++);

	tree->setModel(tableModel);
	layout->addWidget(tree, 0, iCol++);

	layout->addWidget(createForm(tableModel), 0, iCol++);

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

QWidget* MainWindow::createForm(QAbstractTableModel* model) {
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
