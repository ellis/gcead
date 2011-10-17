#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class NodeManager;
class Server;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
	void on_btnAdd_clicked();

private:
    Ui::MainWindow *ui;
	Server* m_server;
	NodeManager* m_nodeManager;
};

#endif // MAINWINDOW_H
