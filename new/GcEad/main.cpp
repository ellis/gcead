#include <iostream>

#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "MainWindow.h"


void checkLog(const char* sFile, int iLine, const QString& sType, const QString& sMessage)
{
	QFile file(QCoreApplication::applicationDirPath() + "/GcEad.log");
	if (file.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
	{
		QFileInfo fi(sFile);
		QTextStream out(&file);
		QString s = QString("%0: %1 at %2:%3: %4")
					.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
					.arg(sType)
					.arg(fi.fileName())
					.arg(iLine)
					.arg(sMessage.trimmed());
		out << s << "\n";
		std::cerr << qPrintable(s) << std::endl;
	}
}

void checkFailure(const char* sFile, int iLine, const char* s)
{
	checkLog(sFile, iLine, "CHECK FAILURE", s);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
