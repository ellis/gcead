TEMPLATE = app
TARGET = GcEad
QT += xml \
    svg
CONFIG += warn_on \
    debug_and_release \
    link_prl
DEFINES += QT_XML_LIB \
    QT_SVG_LIB
INCLUDEPATH += . \
    .. \
    ../Core \
    ../Model \
    ../Scope \
    ../Idac
DEPENDPATH += . \
    ../Core \
    ../Model \
    ../Scope \
    ../Idac
CONFIG(debug, debug|release): LIBS += -L../debug
else: LIBS += -L../release

# VPATH += ../debug
LIBS += -lCore \
    -lIdac \
    -lIdacDriver4 \
    -lIdacDriver2 \
    -lIdacDriver \
    -lModel \
    -lScope
PRE_TARGETDEPS += ../debug/libCore.a \
    ../debug/libIdacDriver.a \
    ../debug/libIdacDriver2.a \
    ../debug/libIdacDriver4.a \
    ../debug/libIdac.a \
    ../debug/libModel.a \
    ../debug/libScope.a
win32:LIBS += ../extern/win32/libusb.a
unix:LIBS += /usr/lib/libusb.a -static-libgcc -L../release -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic
unix:QMAKE_CFLAGS += -static-libgcc
unix:QMAKE_CXXFLAGS += -static-libgcc
unix:QMAKE_LFLAGS += -static-libgcc
unix:pg { 
    QMAKE_CFLAGS += -pg
    QMAKE_CXXFLAGS += -pg
    QMAKE_LFLAGS += -pg
}
CONFIG(debug, debug|release):DESTDIR = ../debug
else:DESTDIR = ../release

# Copy IDAC2 hex file
idac2hex.target = $${DESTDIR}/idc2fpga.hex
win32:idac2hex.commands = ${COPY_FILE} ..\Installables\idc2fpga.hex ${DESTDIR}
unix:idac2hex.commands = ${COPY_FILE} ../Installables/idc2fpga.hex ${DESTDIR}
# Copy IDAC4 hex file
idac4hex.target = $${DESTDIR}/idc4fpga.hex
win32:idac4hex.commands = ${COPY_FILE} ..\Installables\idc4fpga.hex ${DESTDIR}
unix:idac4hex.commands = ${COPY_FILE} ../Installables/idc4fpga.hex ${DESTDIR}
QMAKE_EXTRA_TARGETS += idac2hex idac4hex
POST_TARGETDEPS += $${DESTDIR}/idc2fpga.hex $${DESTDIR}/idc4fpga.hex


HEADERS += ./WaitCursor.h \
    ./ChartWidget.h \
    ./SuffixSpinBox.h \
    ./ViewTabs.h \
    ./MainWindowUi.h \
    ./RecordSettingsDialog.h \
    ./PanelTabs.h \
    ./RecordDialog.h \
    ./Widgets/SignalWidget.h \
    ./Widgets/SweepWidget.h \
    ./Widgets/GroupWidget.h \
    ./Widgets/TabWidget.h \
    ./Widgets/DigitalSignalsWidget.h \
    ./Options.h \
    ./TaskPublishWidget.h \
    ./TaskWidget.h \
    ./DataListItem.h \
    ./TaskReviewWidget.h \
    ./TaskPanel.h \
    ./MainWindow.h \
    ./WaveEditorWidget.h \
    ./WaveEditorDialog.h
SOURCES += ./DataListItem.cpp \
    ./TaskReviewWidget.cpp \
    ./MainWindow.cpp \
    ./Options.cpp \
    ./RecordDialog.cpp \
    ./TaskPublishWidget.cpp \
    ./PanelTabs.cpp \
    ./main.cpp \
    ./TaskPanel.cpp \
    ./MainWindowUi.cpp \
    ./ChartWidget.cpp \
    ./ViewTabs.cpp \
    ./Widgets/SignalWidget.cpp \
    ./Widgets/SweepWidget.cpp \
    ./Widgets/TabWidget.cpp \
    ./Widgets/GroupWidget.cpp \
    ./Widgets/DigitalSignalsWidget.cpp \
    ./RecordSettingsDialog.cpp \
    ./TaskWidget.cpp \
    ./WaveEditorWidget.cpp \
    ./WaveEditorDialog.cpp

# Forms
FORMS += ./MainWindow.ui \
    ./Options.ui \
    ./RecordDialog.ui \
    ./RecordSettingsDialog.ui

# Resource file(s)
RESOURCES += ./View.qrc
