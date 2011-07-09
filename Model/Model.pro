TEMPLATE = lib
QT += xml svg
CONFIG += warn_on staticlib create_prl debug_and_release
DEFINES += QT_XML_LIB QT_SVG_LIB
INCLUDEPATH += . .. ../Core
DEPENDPATH += . .. ../Core

HEADERS += AppDefines.h ChartPixmap.h EadEnums.h EadFile.h Globals.h PublisherSettings.h RecInfo.h RenderData.h ViewInfo.h ViewSettings.h WaveInfo.h \
	FilterInfo.h
	#PropertyRowModel.h \
	#Datastore.h
SOURCES += ChartPixmap.cpp EadFile.cpp FakeData.cpp Globals.cpp PublisherSettings.cpp RecInfo.cpp RenderData.cpp ViewInfo.cpp WaveInfo.cpp \
    FilterInfo.cpp \
    PropertyRowModel.cpp \
	#Datastore.cpp

CONFIG(debug, debug|release) {
    DESTDIR = ../debug
} else {
    DESTDIR = ../release
}
