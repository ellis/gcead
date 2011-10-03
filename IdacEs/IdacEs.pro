#-------------------------------------------------
#
# Project created by QtCreator 2011-05-17T23:38:13
#
#-------------------------------------------------

TARGET = IDAC8_32
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
    IdacExports.cpp \
    dllmain.cpp

HEADERS += IdacEs.h\
	IdacEs_global.h \
    qwinwidget.h \
    qwinhost.h \
    qmfcapp.h \
    IdacExports.h

OTHER_FILES += Idacdll.def

CONFIG(debug, debug|release):DESTDIR = $${OUT_PWD}/../debug
else:DESTDIR = $${OUT_PWD}/../release
LIBS += -L$${DESTDIR}

LIBS += $${PWD}/Idacdll.def -Wl,--kill-at,--output-def,Idacdll.def \
	-luser32 \
	-lIdac \
	-lIdacDriver2 \
	-lIdacDriver4 \
	-lIdacDriverES \
	-lIdacDriver \
	$${PWD}/../extern/win32/libusb.a

PRE_TARGETDEPS +=  \
	$${DESTDIR}/libIdac.a \
	$${DESTDIR}/libIdacDriver.a \
	$${DESTDIR}/libIdacDriver2.a \
	$${DESTDIR}/libIdacDriver4.a \
	$${DESTDIR}/libIdacDriverES.a
