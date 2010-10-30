# -------------------------------------------------
# Project created by QtCreator 2009-04-17T11:33:42
# -------------------------------------------------
QT -= gui
CONFIG += warn_on staticlib create_prl debug_and_release
TEMPLATE = lib
INCLUDEPATH += . .. ../Core ../IdacDriver
HEADERS += \
    IdacControlThread.h \
    IdacFactory.h \
    IdacProxy.h \
    IdacDriverManager.h 
SOURCES += \
    IdacDriverManager.cpp \
    IdacControlThread.cpp \
    IdacProxy.cpp \
    IdacFactory.cpp
win32:DEFINES += WIN32
win32:INCLUDEPATH += ../extern/win32
unix:INCLUDEPATH += ../extern/libusb-compat-0.1.3/libusb

CONFIG(debug, debug|release) {
    DESTDIR = ../debug
} else {
    DESTDIR = ../release
}
