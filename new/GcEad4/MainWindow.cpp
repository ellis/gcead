#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Node.h"
#include "NodeManager.h"
#include "Server.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	m_server = new Server(this);
	m_nodeManager = new NodeManager(m_server, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAdd_clicked()
{
	const QString sKey = ui->edtName->text();
	const QStringList lsTuples = ui->edtFields->text().split(';');
	QVariantMap entry;
	entry[KEY] = sKey;
	foreach (QString sTuple, lsTuples) {
		QStringList ls = sTuple.split('=');
		if (ls.size() == 2)
			entry.insert(ls[0], ls[1]);
	}
	m_server->add(entry);
}
