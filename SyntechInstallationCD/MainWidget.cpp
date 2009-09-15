#include "MainWidget.h"
#include "ui_MainWidget.h"

#include <QProcess>
#include <QSettings>

MainWidget::MainWidget(QWidget *parent)
	: QWidget(parent), ui(new Ui::MainWidget)
{
	ui->setupUi(this);
	loadSettings();
}

MainWidget::~MainWidget()
{
	delete ui;
}

// The following corresponds (as of 2009-09-15) to following SyntechInstallationCD.ini:
//[Software]
//UsbDriver=Software/DriverSetup.exe
//IdacDriver=Software/DeviceInterfaces8_Setup.exe
//AutoSpike=Software/AutoSpike8_Setup_3V9.exe
//Eag=Software/EAG2000Setup.exe
//EagPro=Software/EagPro8_Setup.exe
//GcEad=Software/Ead2000Setup.exe
//GcEadOpen=Software/GcEad-1.1.0-windows-installer-20090525.exe
//GcEadPro=Software/GcEAD8_Setup_4V6.exe
//UsbView=usbview.exe
void MainWidget::loadSettings()
{
	QSettings settings("SyntechInstallationCD.ini", QSettings::IniFormat);
	settings.beginGroup("Software");
	m_sUsbDriver = settings.value("UsbDriver", "Software/DriverSetup.exe").toString();
	m_sIdacDriver = settings.value("IdacDriver", "Software/DeviceInterfaces8_Setup.exe").toString();
	m_sAutoSpike = settings.value("AutoSpike", "Software/AutoSpike8_Setup_3V9.exe").toString();
	m_sEag = settings.value("Eag", "Software/EAG2000Setup.exe").toString();
	m_sEagPro = settings.value("EagPro", "Software/EagPro8_Setup.exe").toString();
	m_sGcEad = settings.value("GcEad", "Software/Ead2000Setup.exe").toString();
	m_sGcEadOpen = settings.value("GcEadOpen", "Software/GcEad-1.1.0-windows-installer-20090525.exe").toString();
	m_sGcEadPro = settings.value("GcEadPro", "Software/GcEAD8_Setup_4V6.exe").toString();
	m_sUsbView = settings.value("UsbView", "usbview.exe").toString();
}

void MainWidget::on_btnUsbDriver_clicked() { run(m_sUsbDriver); }
void MainWidget::on_btnIdacDriver_clicked() { run(m_sIdacDriver); }
void MainWidget::on_btnAutoSpike_clicked() { run(m_sAutoSpike); }
void MainWidget::on_btnEag_clicked() { run(m_sEag); }
void MainWidget::on_btnEagPro_clicked() { run(m_sEagPro); }
void MainWidget::on_btnGcEad_clicked() { run(m_sGcEad); }
void MainWidget::on_btnGcEadOpen_clicked() { run(m_sGcEadOpen); }
void MainWidget::on_btnGcEadPro_clicked() { run(m_sGcEadPro); }
void MainWidget::on_btnUsbView_clicked() { run(m_sUsbView); }
void MainWidget::on_btnClose_clicked() { close(); }

void MainWidget::run(const QString& sExe)
{
	QProcess* process = new QProcess(this);
	process->start(sExe);
}

