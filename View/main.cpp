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

#include <QtDebug>
#include <QApplication>
#include <QMutex>
#include <QSettings>

#include <AppDefines.h>
#include <Globals.h>
#include <Idac/IdacFactory.h>

#include "MainWindow.h"


void checkFailure(const char* sFile, int iLine)
{
	qDebug() << "CHECK FAILURE: " << sFile << ", line " << iLine;
}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	qDebug() << QString::number(5.0, 'e', 1);
	qDebug() << QString::number(5.0, 'f', 1);
	qDebug() << QString::number(5.0, 'g', 1);

	a.setWindowIcon(QIcon(":/images/GcEad.ico"));

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
