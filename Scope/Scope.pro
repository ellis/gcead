TEMPLATE = lib
QT += xml \
    svg
CONFIG += warn_on \
    staticlib \
    create_prl \
    debug_and_release
DEFINES += QT_XML_LIB \
    QT_SVG_LIB
INCLUDEPATH += . \
    .. \
    ../Core \
    ../Model
DEPENDPATH += . \
    .. \
    ../Core \
    ../Model
HEADERS += Actions.h \
    MainScope.h \
    MainScopeUi.h \
    RecordHandler.h \
    ChartScope.h
SOURCES += Actions.cpp \
    MainScope.cpp \
    RecordHandler.cpp \
    ChartScope.cpp
CONFIG(debug, debug|release):DESTDIR = ../debug
else:DESTDIR = ../release
