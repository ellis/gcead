#-------------------------------------------------
#
# Project created by QtCreator 2011-05-17T23:38:13
#
#-------------------------------------------------

TARGET = IdacEs
TEMPLATE = lib
CONFIG += qt

DEFINES += IDACES_LIBRARY
DEFINES += QT_QTWINMIGRATE_EXPORT

INCLUDEPATH += ..

SOURCES += IdacEs.cpp \
 #   IdacBrd.cpp \
    qwinwidget.cpp \
    qwinhost.cpp \
    qmfcapp.cpp \
#    IdacInt.cpp \
#    IdacInfo.cpp \
#    IdacDll.cpp \
#    IdacDbg.cpp \
#    IdacData.cpp \
#    IdacCtrl.cpp
    IdacExports.cpp

HEADERS += IdacEs.h\
	IdacEs_global.h \
    qwinwidget.h \
    qwinhost.h \
    qmfcapp.h \
    IdacExports.h

LIBS += -luser32
