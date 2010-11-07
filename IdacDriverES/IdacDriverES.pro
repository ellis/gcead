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
win32:HEADERS += IdacDriverES.h
win32:SOURCES += IdacDriverES.cpp
win32:DEFINES += WIN32
CONFIG(debug, debug|release):DESTDIR = ../debug
else:DESTDIR = ../release

win32:HEADERS += \
    IdacControl/Sample.h \
    IdacControl/Ipc.h \
    IdacControl/Idacpc.h \
    IdacControl/windows_types.h
