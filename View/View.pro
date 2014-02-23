TEMPLATE = app
TARGET = GcEad
QT += printsupport \
    xml \
    svg \
	qml \
	quick
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
#CONFIG(debug, debug|release):DESTDIR = $${OUT_PWD}/../debug
#else:DESTDIR = $${OUT_PWD}/../release
#LIBS += -L$${DESTDIR}

# VPATH += ../debug
PRE_TARGETDEPS += ../Core/libCore.a \
	../IdacDriver/libIdacDriver.a \
	../IdacDriver2/libIdacDriver2.a \
	../IdacDriver4/libIdacDriver4.a \
	../Idac/libIdac.a \
	../Filters/libFilters.a \
	../Model/libModel.a \
	../Scope/libScope.a
win32:LIBS += $${DESTDIR}/libIdacDriverES.a
#win32:LIBS += $${PWD}/../extern/win32/libusb.a $${PWD}/../IdacDriverES/IdacControl/IDAC8_32.lib
win32:LIBS += $${PWD}/../extern/win32/libusb.a
unix:!macx:LIBS += -static-libgcc \
	../Scope/libScope.a \
	../Model/libModel.a \
	../Filters/libFilters.a \
	../Idac/libIdac.a \
	../IdacDriver4/libIdacDriver4.a \
	../IdacDriver2/libIdacDriver2.a \
	../IdacDriver/libIdacDriver.a \
	../Core/libCore.a \
	-Wl,-Bstatic \
    -lstdc++ \
    -Wl,-Bdynamic \
    $${PWD}/../extern/libusb/lib/libusb-1.0.a \
    -l:libudev.so.1
unix:macx:LIBS += -Wl,-framework \
    -Wl,IOKit -Wl,-framework -Wl,CoreFoundation \
    -lobjc \
    $${PWD}/../extern/libusb/lib/libusb-1.0.a
#LIBS += \
#	-lScope \
#	-lModel \
#	-lFilters \
#	-lIdac \
#	-lIdacDriver2 \
#	-lIdacDriver4 \
#	-lIdacDriver \
#	-lCore
win32:LIBS += -lIdacDriverES
unix:QMAKE_CFLAGS += -static-libgcc
unix:QMAKE_CXXFLAGS += -static-libgcc
unix:QMAKE_LFLAGS += -static-libgcc
unix:pg { 
    QMAKE_CFLAGS += -pg
    QMAKE_CXXFLAGS += -pg
    QMAKE_LFLAGS += -pg
}

# Copy IDAC2 hex file
!macx:idac2hex.target = idc2fpga.hex
win32:idac2hex.commands = ${COPY_FILE} \
	$$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc2fpga.hex \
	.
unix:!macx:idac2hex.commands = ${COPY_FILE} \
    $${PWD}/../Installables/idc2fpga.hex \
	.

# Copy IDAC4 hex file
!macx:idac4hex.target = idc4fpga.hex
win32:idac4hex.commands = ${COPY_FILE} \
    $$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc4fpga.hex \
	.
unix:!macx:idac4hex.commands = ${COPY_FILE} \
    $${PWD}/../Installables/idc4fpga.hex \
	.

!macx:QMAKE_EXTRA_TARGETS += idac2hex idac4hex

POST_TARGETDEPS += idc2fpga.hex \
	idc4fpga.hex
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
    ./WaveEditorDialog.h \
    WaitForHardwareDialog.h \
    TestRecording.h \
    TaskMarkersWidget.h \
    TaskFilterWidget.h \
    TaskFilterWidgetModel.h \
    ImportRecordDialog.h
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
    ./WaveEditorDialog.cpp \
    WaitForHardwareDialog.cpp \
    TestRecording.cpp \
    TaskMarkersWidget.cpp \
    TaskFilterWidget.cpp \
    TaskFilterWidgetModel.cpp \
    ImportRecordDialog.cpp

# Forms
FORMS += ./MainWindow.ui \
    ./Options.ui \
    ./RecordDialog.ui \
    ./RecordSettingsDialog.ui

# Resource file(s)
RESOURCES += ./View.qrc
macx:ICON = ../Installables/Mac/GcEad.icns
win32:RC_FILE = ./GcEad.rc

OTHER_FILES += \
    GcEad.rc

hexfiles.files = $${PWD}/../Installables/idc2fpga.hex $${PWD}/../Installables/idc4fpga.hex
mac:hexfiles.path = Contents/MacOS
#!mac:hexfiles.path =
QMAKE_BUNDLE_DATA += hexfiles
