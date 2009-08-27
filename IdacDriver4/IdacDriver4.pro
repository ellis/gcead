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
HEADERS += IdacDriver4.h \
    IdacDriver4Constants.h \
    IdacDriver4Channel.h \
    IdacDriver4ReqIds.h
SOURCES += IdacDriver4.cpp \
    IdacDriver4Channel.cpp \
    IdacDriver4Firmware.cpp
win32:INCLUDEPATH += ../extern/win32
win32:DEFINES += WIN32
unix:INCLUDEPATH += ../extern/libusb-1.0.2 ../extern/libusb-1.0.2/libusb ../extern/libusb-compat-0.1.3/libusb
CONFIG(debug, debug|release):DESTDIR = ../debug
else:DESTDIR = ../release
