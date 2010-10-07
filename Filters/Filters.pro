TEMPLATE = lib
CONFIG += warn_on staticlib create_prl debug_and_release

INCLUDEPATH += . .. ../Core
DEPENDPATH += . .. ../Core

SOURCES += ap.cpp AutoCov.cpp conv.cpp corr.cpp fft.cpp Filters.cpp ftbase.cpp sig_model.cpp WaveProcess.cpp

HEADERS += ap.h apvt.h AutoCov.h conv.h corr.h fft.h Filters.h ftbase.h ialglib.h sig_model.h stdafx.h WaveProcess.h

CONFIG(debug, debug|release) {
    DESTDIR = ../debug
} else {
    DESTDIR = ../release
}
