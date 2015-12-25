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
    IdacDriver4ReqIds.h \
    IdacDriver4Es.h
SOURCES += IdacDriver4.cpp \
    IdacDriver4Channel.cpp \
    IdacDriver4Firmware.cpp \
    IdacDriver4Es.cpp


win32:INCLUDEPATH += ../extern/win32
unix:INCLUDEPATH += ../extern/libusb/include

win32:DEFINES += WIN32
win32:DEFINES += LIBUSBX #LIBUSB0
unix:DEFINES += LIBUSBX

#CONFIG(debug, debug|release):DESTDIR = ../debug
#else:DESTDIR = ../release
