#include <iostream>

#include <windows.h>

#include <Idac/IdacDriverManager.h>
#include <Idac/IdacFactory.h>
#include <IdacDriver/IdacDriverUsbEs.h>
#include <IdacDriver2/IdacDriver2Es.h>
#include <IdacDriver4/IdacDriver4Es.h>

#include "qmfcapp.h"
//#include "qwinwidget.h"

//#include <qmessagebox.h>

static QPointer<IdacDriverManager> g_manager;
IdacDriverUsbEs* g_wrapper;

IdacDriverManager* manager() {
	if (g_manager.isNull()) {
		g_manager = IdacFactory::getDriverManager(true);
		g_manager->command(IdacCommand_Connect);
	}
	return g_manager;
}

IdacDriverUsbEs* wrapper() {
	if (g_manager == NULL)
		return NULL;

	if (g_wrapper == NULL) {
		IdacDriver2* driver2 = dynamic_cast<IdacDriver2*>(g_manager->driver());
		IdacDriver4* driver4 = dynamic_cast<IdacDriver4*>(g_manager->driver());
		if (driver2 != NULL) {
			g_wrapper = new IdacDriver2Es(driver2, driver2->defaultChannelSettings());
		}
		else if (driver4 != NULL)
			g_wrapper = new IdacDriver4Es(driver4, driver4->defaultChannelSettings());
	}
	return g_wrapper;
}

BOOL WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved )
{
	std::cout << "DllMain" << std::endl;

	static bool ownApplication = FALSE;

	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		ownApplication = QMfcApp::pluginInstance( hInstance );
		manager();
		break;
	case DLL_THREAD_ATTACH:
		manager();
		break;
	case DLL_PROCESS_DETACH:
		if (ownApplication)
			delete qApp;
	}

	return TRUE;
}


#include <QFile>
#include <QFileInfo>
//#include <QMutex>
//#include <QSettings>
#include <QTextStream>
#include <QDateTime>

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
