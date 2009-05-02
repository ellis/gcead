TEMPLATE = lib
QT += xml svg
CONFIG += warn_on staticlib create_prl debug_and_release
DEFINES += QT_XML_LIB QT_SVG_LIB
INCLUDEPATH += . ..
DEPENDPATH += . ..

HEADERS += Check.h Utils.h
SOURCES += Utils.cpp

CONFIG(debug, debug|release) {
    DESTDIR = ../debug
} else {
    DESTDIR = ../release
}
