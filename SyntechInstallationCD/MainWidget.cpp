#include "MainWidget.h"
#include "ui_MainWidget.h"

#include <QProcess>


MainWidget::MainWidget(QWidget *parent)
	: QWidget(parent), ui(new Ui::MainWidget)
{
	ui->setupUi(this);
}

MainWidget::~MainWidget()
{
	delete ui;
}

void MainWidget::on_btnUsbDriver_clicked() { run("Drivers/DriverSetup.exe"); }
void MainWidget::on_btnIdacDriver_clicked() { run("DeviceInterfaces/DeviceInterfaces8_Setup.exe"); }
void MainWidget::on_btnAutoSpike_clicked() { run("AutoSpike/AutoSpike8_Setup.exe"); }
void MainWidget::on_btnEag_clicked() { run("Eag2000/EAG2000Setup.exe"); }
void MainWidget::on_btnEagPro_clicked() { run("EagPro/EagPro8_Setup.exe"); }
void MainWidget::on_btnGcEad_clicked() { run("GcEad2000/Ead2000Setup.exe"); }
void MainWidget::on_btnGcEadPro_clicked() { run("CgEadPro/GcEAD8_Setup.exe"); }
void MainWidget::on_btnUsbView_clicked() { run("Utilities/usbview.exe"); }
void MainWidget::on_btnClose_clicked() { close(); }

void MainWidget::run(const QString& sExe)
{
	QProcess* process = new QProcess(this);
	process->start(sExe);
}

