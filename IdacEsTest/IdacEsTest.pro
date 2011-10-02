#-------------------------------------------------
#
# Project created by QtCreator 2011-10-02T18:59:25
#
#-------------------------------------------------

QT       += core gui

TARGET = IdacEsTest
TEMPLATE = app

INCLUDEPATH += ..

SOURCES += main.cpp\
        MainWindow.cpp

HEADERS  += MainWindow.h

FORMS    += MainWindow.ui

CONFIG(debug, debug|release):DESTDIR = $${OUT_PWD}/../debug
else:DESTDIR = $${OUT_PWD}/../release
LIBS += -L$${DESTDIR}

LIBS += -luser32 \
	$${DESTDIR}/IdacEs.dll

PRE_TARGETDEPS +=  \
	$${DESTDIR}/IdacEs.dll
