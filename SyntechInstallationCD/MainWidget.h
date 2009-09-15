#ifndef _MAINWIDGET_H
#define _MAINWIDGET_H

#include <QtGui/QWidget>

namespace Ui
{
	class MainWidget;
}

class MainWidget : public QWidget
{
	Q_OBJECT

public:
	MainWidget(QWidget *parent = 0);
	~MainWidget();

private slots:
	void on_btnUsbDriver_clicked();
	void on_btnIdacDriver_clicked();
	void on_btnAutoSpike_clicked();
	void on_btnEag_clicked();
	void on_btnEagPro_clicked();
	void on_btnGcEad_clicked();
	void on_btnGcEadOpen_clicked();
	void on_btnGcEadPro_clicked();
	void on_btnUsbView_clicked();
	void on_btnClose_clicked();

private:
	void loadSettings();
	void run(const QString& sExe);

private:
	Ui::MainWidget *ui;
	QString m_sUsbDriver;
	QString m_sIdacDriver;
	QString m_sAutoSpike;
	QString m_sEag;
	QString m_sEagPro;
	QString m_sGcEad;
	QString m_sGcEadOpen;
	QString m_sGcEadPro;
	QString m_sUsbView;
};

#endif
