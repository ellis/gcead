/**
 * Copyright (C) 2008, 2009  Ellis Whitehead
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include <QtDebug>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QSettings>
#include <QTextStream>
#include <QDateTime>

#include <AppDefines.h>
#include <Globals.h>
#include <Idac/IdacFactory.h>

#include "MainWindow.h"
#include "TestRecording.h"


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
#ifdef Q_WS_X11
	QApplication::setGraphicsSystem("raster");
#endif
	QApplication a(argc, argv);

	a.setWindowIcon(QIcon(":/images/GcEad.ico"));

	//X x;
	//x.show();
	//return a.exec();

	Globals = new GlobalVars();

	MainWindow* w = new MainWindow();

	// This used to be handled in MainWindow::readSettings(),
	// but there were sometimes layout problems upon maximization.
	// This bug might have been fixed in Qt in the meantime, but
	// I don't know, so I'm trying this as a workaround -- ellis, 2009-05-04
	QSettings settings("Syntech", APPSETTINGSKEY);
	settings.beginGroup("MainWindow");
	bool bMax = settings.value("Maximized", true).toBool();
	if (bMax)
		w->showMaximized();
	else
		w->show();

	if (QFile::exists(QCoreApplication::applicationDirPath() + "/flag.TestRecording")) {
		TestRecording* test = new TestRecording(w->scope());
		test->show();
	}

	// Open file from command line
	if (argc >= 2)
		w->open(argv[1]);

	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	int ret = a.exec();

	delete w;

	IdacFactory::exitIdacThreads();

	delete Globals;

	return ret;
}
