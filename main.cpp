/**
 * Copyright (C) 2008  Ellis Whitehead
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

#include "Globals.h"
//#include <Idac/Idac.h>
#include "MainWindow.h"
#include "IdacTest/IdacTestDlg.h"


QMutex g_exitMutex;


void checkFailure(const char* sFile, int iLine)
{
	qDebug() << "CHECK FAILURE: " << sFile << ", line " << iLine;
}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setWindowIcon(QIcon(":/images/GcEad.ico"));

	Globals = new GlobalVars();

	QWidget* w2 = NULL;
	if (false)
	{
		//Idac idac;
		//CIdacTestDlg* dlg = new CIdacTestDlg();
		//dlg->show();
	}
	else
	{
		MainWindow* w = new MainWindow();
		w->show();
		w2 = w;
	}
	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	int ret = a.exec();

	delete w2;

	g_exitMutex.lock();
	g_exitMutex.unlock();

	delete Globals;

	return ret;
}
