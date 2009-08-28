# -------------------------------------------------
# Project created by QtCreator 2009-04-17T11:33:42
# -------------------------------------------------
QT -= gui
CONFIG += warn_on \
    staticlib \
    create_prl \
    debug_and_release
TEMPLATE = lib
INCLUDEPATH += . \
    .. \
    ../Core \
    ../IdacDriver
HEADERS += IdacDriver2.h \
    IdacDriver2ReqIds.h \
    IdacDriver2Constants.h
SOURCES += IdacDriver2.cpp \
    IdacDriver2Firmware.cpp
win32:INCLUDEPATH += ../extern/win32
unix:INCLUDEPATH += ../extern/libusb ../extern/libusb-compat-0.1.3/libusb
win32:DEFINES += WIN32
CONFIG(debug, debug|release):DESTDIR = ../debug
else:DESTDIR = ../release
