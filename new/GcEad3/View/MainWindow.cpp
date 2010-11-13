#include "MainWindow.h"

#include <QDataWidgetMapper>
#include <QDockWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
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

#include "GraphicsWidget1.h"
#include "Item.h"
#include "ItemListModel.h"
#include "ItemRepository.h"
//#include "ObjectPropertiesModel.h"
#include "Wave.h"
#include "WaveListController.h"
#include "WaveList.h"
#include "Workspace.h"


static QScriptValue itemToScript(QScriptEngine* engine, Item* const& in) { return engine->newQObject(in); }
static void scriptToItem(const QScriptValue& val, Item*& out) { out = qobject_cast<Item*>(val.toQObject()); }

static QScriptValue waveToScript(QScriptEngine* engine, Wave* const& in) { return engine->newQObject(in); }
static void scriptToWave(const QScriptValue& val, Wave*& out) { out = qobject_cast<Wave*>(val.toQObject()); }

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	m_engine = new QScriptEngine(this);

	m_space = new Workspace(this);
	m_waveRepo = m_space->waveRepository();

	m_waves = new WaveListController(this);
	m_waves->setObjectName("waves");
	m_waves->setRepository(m_waveRepo);

	Wave* wave = m_waves->add();
	wave->setName("One");
	wave->setComment("Yay");
	wave->setSensitivity(1);

	wave = (Wave*) m_waves->add();
	wave->setName("Two");
	wave->setComment("Yay");
	wave->setSensitivity(2);

	m_itemModel = new ItemListModel(this);
	m_itemModel->setItemList(m_waveRepo->activeItems());
	m_itemModel->setProperties(QStringList() << "name" << "comment" << "sensitivity");

	qRegisterMetaType<Wave*>("Wave*");

	QScriptValue val = m_engine->newQObject(m_waves);
	m_engine->globalObject().setProperty("waves", val);
	qScriptRegisterMetaType(m_engine, itemToScript, scriptToItem);
	qScriptRegisterMetaType(m_engine, waveToScript, scriptToWave);

	createWidgets();

	connect(m_space, SIGNAL(logCommand(QString)), this, SLOT(on_proj_logCommand(QString)));
}

MainWindow::~MainWindow()
{

}

void MainWindow::createWidgets() {
	m_lstLog = new QListWidget();
	QUndoView* undoView = new QUndoView(m_space->undoStack());
	m_debugger = new QScriptEngineDebugger(this);

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

	QWidget* w = createCentralWidget1();
	setCentralWidget(w);
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

QWidget* MainWindow::createCentralWidget1() {
	QGridLayout* layout = new QGridLayout();
	QGraphicsScene* scene = new QGraphicsScene(this);
	QGraphicsView* view = new QGraphicsView(scene);
	QTableView* tbl = new QTableView();
	QWidget* w = new QWidget();

	int iCol = 0;

	scene->addItem(new GraphicsWidget1());
	layout->addWidget(view, 0, iCol++);

	tbl->setModel(m_itemModel);
	layout->addWidget(tbl, 0, iCol++);

	layout->addWidget(createForm(m_itemModel), 0, iCol++);

	w->setLayout(layout);
	return w;
}

QWidget* MainWindow::createCentralWidget2() {
	return NULL;
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
