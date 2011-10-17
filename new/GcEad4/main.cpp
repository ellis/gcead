#include <iostream>

#include <QApplication>
#include <QDateTime>

#include "MainWindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

void checkLog(const char* sFile, int iLine, const QString& sType, const QString& sMessage)
{
	QString s = QString("%0: %1 at %2:%3: %4")
			.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
			.arg(sType)
			.arg(QString(sFile))
			.arg(iLine)
			.arg(sMessage.trimmed());
	std::cerr << qPrintable(s) << std::endl;
}

void checkFailure(const char* sFile, int iLine, const char* s)
{
	checkLog(sFile, iLine, "CHECK FAILURE", s);
}
