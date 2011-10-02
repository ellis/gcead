#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <IdacEs/IdacExports.h>

#include <iostream>
using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn1_clicked()
{
	cout << "IdacLibVersion(): " << IdacLibVersion() << endl;
}
