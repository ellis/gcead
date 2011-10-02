#include <windows.h>

#include <Idac/IdacDriverManager.h>
#include <Idac/IdacFactory.h>
#include <IdacDriver/IdacDriverUsbEs.h>
#include <IdacDriver2/IdacDriver2Es.h>
#include <IdacDriver4/IdacDriver4Es.h>

#include "qmfcapp.h"
//#include "qwinwidget.h"

//#include <qmessagebox.h>

static QPointer<IdacDriverManager> m_manager;
IdacDriverUsbEs* g_wrapper;

IdacDriverUsbEs* wrapper() {
	if (m_manager.isNull())
		return NULL;

	if (g_wrapper == NULL) {
		IdacDriver2* driver2 = dynamic_cast<IdacDriver2*>(m_manager->driver());
		IdacDriver4* driver4 = dynamic_cast<IdacDriver4*>(m_manager->driver());
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
	static bool ownApplication = FALSE;

	if ( dwReason == DLL_PROCESS_ATTACH ) {
		ownApplication = QMfcApp::pluginInstance( hInstance );
		m_manager = IdacFactory::getDriverManager(false);
	}
	if ( dwReason == DLL_PROCESS_DETACH && ownApplication )
		delete qApp;

	return TRUE;
}


#include <QFile>
#include <QFileInfo>
//#include <QMutex>
//#include <QSettings>
#include <QTextStream>
#include <QDateTime>
#include <iostream>

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
