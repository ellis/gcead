#include "MainWindow.h"

#include <QDataWidgetMapper>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QSpinBox>
#include <QTableView>
#include <QTreeView>
#include <QUndoView>

#include "ObjectPropertiesModel.h"
#include "Project.h"
#include "Item.h"
#include "ItemListModel.h"
#include "Wave.h"


static QScriptValue itemToScript(QScriptEngine* engine, Item* const& in) { return engine->newQObject(in); }
static void scriptToItem(const QScriptValue& val, Item*& out) { out = qobject_cast<Item*>(val.toQObject()); }

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

	qRegisterMetaType<Wave*>("Wave*");

	QScriptValue val = m_engine->newQObject(m_proj);
	m_engine->globalObject().setProperty("project", val);
	qScriptRegisterMetaType(m_engine, itemToScript, scriptToItem);
	qScriptRegisterMetaType(m_engine, waveToScript, scriptToWave);

	createWidgets(wave);

	connect(m_proj, SIGNAL(logCommand(QString)), this, SLOT(on_proj_logCommand(QString)));
}

MainWindow::~MainWindow()
{

}

void MainWindow::createWidgets(Wave* wave) {
	ItemListModel* tableModel = new ItemListModel(this);
	//ObjectPropertiesModel* objModel = new ObjectPropertiesModel(this);

	QGridLayout* layout = new QGridLayout();
	QTableView* tbl = new QTableView();
	//QTreeView* tree = new QTreeView();
	m_lstLog = new QListWidget();
	QUndoView* undoView = new QUndoView(m_proj->undoStack());
	m_debugger = new QScriptEngineDebugger(this);
	QWidget* w = new QWidget();

	tableModel->setProject(m_proj);
	tableModel->setProperties(QStringList() << "name" << "comment" << "sensitivity");

	//objModel->setProperties(QStringList() << "name" << "comment" << "sensitivity");
	//objModel->addObject(proxy);


	int iCol = 0;

	tbl->setModel(tableModel);
	layout->addWidget(tbl, 0, iCol++);

	//tree->setModel(tableModel);
	//layout->addWidget(tree, 0, iCol++);
	//layout->addWidget(m_lstLog, 0, iCol++);

	layout->addWidget(createForm(tableModel), 0, iCol++);

	//undoView->setWindowTitle(tr("Command List"));
	//layout->addWidget(undoView, 0, iCol++);

	w->setLayout(layout);
	setCentralWidget(w);

	QDockWidget* dock;

	dock = new QDockWidget(tr("Undo/Redo List"), this);
	dock->setWidget(undoView);
	addDockWidget(Qt::RightDockWidgetArea, dock);

	dock = new QDockWidget(tr("Log"), this);
	dock->setWidget(m_lstLog);
	addDockWidget(Qt::RightDockWidgetArea, dock);

	m_debugger->attachTo(m_engine);
	dock = new QDockWidget(tr("Script console"), this);
	dock->setWidget(m_debugger->widget(QScriptEngineDebugger::ConsoleWidget));
	addDockWidget(Qt::BottomDockWidgetArea, dock);

	//m_debugger->standardWindow()->show();
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

void MainWindow::on_proj_logCommand(const QString& s) {
	m_lstLog->addItem(s);
}
