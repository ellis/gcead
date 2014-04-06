TEMPLATE = app
TARGET = GcEad
QT += printsupport \
    xml \
    svg \
	qml \
	quick
CONFIG += warn_on link_prl

unix:!macx {
	QMAKE_CFLAGS += -static-libgcc
	QMAKE_CXXFLAGS += -static-libgcc
	QMAKE_LFLAGS += -static-libgcc
	pg {
		QMAKE_CFLAGS += -pg
		QMAKE_CXXFLAGS += -pg
		QMAKE_LFLAGS += -pg
	}
}

## Copy IDAC2 hex file
#!macx:idac2hex.target = idc2fpga.hex
#win32:idac2hex.commands = ${COPY_FILE} \
#	$$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc2fpga.hex \
#	.
#unix:!macx:idac2hex.commands = ${COPY_FILE} \
#    $${PWD}/../Installables/idc2fpga.hex \
#	.

## Copy IDAC4 hex file
#!macx:idac4hex.target = idc4fpga.hex
#win32:idac4hex.commands = ${COPY_FILE} \
#    $$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc4fpga.hex \
#	.
#unix:!macx:idac4hex.commands = ${COPY_FILE} \
#    $${PWD}/../Installables/idc4fpga.hex \
#	.

#!macx:QMAKE_EXTRA_TARGETS += idac2hex idac4hex

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
    ImportRecordDialog.h \
	ImportEadDialog.h
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
    ImportRecordDialog.cpp \
	ImportEadDialog.cpp

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
	GcEad.rc \
	ImportDialog.qml


## Copy IDAC2 hex file
#!macx:idac2hex.target = idc2fpga.hex
#win32:idac2hex.commands = ${COPY_FILE} \
#	$$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc2fpga.hex \
#	.
#unix:!macx:idac2hex.commands = ${COPY_FILE} \
#    $${PWD}/../Installables/idc2fpga.hex \
#	.

## Copy IDAC4 hex file
#!macx:idac4hex.target = idc4fpga.hex
#win32:idac4hex.commands = ${COPY_FILE} \
#    $$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc4fpga.hex \
#	.
#unix:!macx:idac4hex.commands = ${COPY_FILE} \
#    $${PWD}/../Installables/idc4fpga.hex \
#	.

#!macx:QMAKE_EXTRA_TARGETS += idac2hex idac4hex

#POST_TARGETDEPS += idc2fpga.hex \
#	idc4fpga.hex


win32:CONFIG(release, debug|release): LIBS += \
    -L$$OUT_PWD/../Scope/release/ -lScope \
    -L$$OUT_PWD/../Model/release/ -lModel \
    -L$$OUT_PWD/../Filters/release/ -lFilters \
    -L$$OUT_PWD/../Idac/release/ -lIdac \
    -L$$OUT_PWD/../IdacDriverES/release/ -lIdacDriverES \
    -L$$OUT_PWD/../IdacDriver4/release/ -lIdacDriver4 \
    -L$$OUT_PWD/../IdacDriver2/release/ -lIdacDriver2 \
    -L$$OUT_PWD/../IdacDriver/release/ -lIdacDriver \
    -L$$OUT_PWD/../Core/release/ -lCore
else:win32:CONFIG(debug, debug|release): LIBS += \
    -L$$OUT_PWD/../Scope/debug/ -lScope \
    -L$$OUT_PWD/../Model/debug/ -lModel \
    -L$$OUT_PWD/../Filters/debug/ -lFilters \
    -L$$OUT_PWD/../Idac/debug/ -lIdac \
    -L$$OUT_PWD/../IdacDriverES/debug/ -lIdacDriverES \
    -L$$OUT_PWD/../IdacDriver4/debug/ -lIdacDriver4 \
    -L$$OUT_PWD/../IdacDriver2/debug/ -lIdacDriver2 \
    -L$$OUT_PWD/../IdacDriver/debug/ -lIdacDriver \
    -L$$OUT_PWD/../Core/debug/ -lCore
else:unix: LIBS += \
    -L$$OUT_PWD/../Scope/ -lScope \
    -L$$OUT_PWD/../Model/ -lModel \
    -L$$OUT_PWD/../Filters/ -lFilters \
    -L$$OUT_PWD/../Idac/ -lIdac \
    -L$$OUT_PWD/../IdacDriver4/ -lIdacDriver4 \
    -L$$OUT_PWD/../IdacDriver2/ -lIdacDriver2 \
    -L$$OUT_PWD/../IdacDriver/ -lIdacDriver \
    -L$$OUT_PWD/../Core/ -lCore

INCLUDEPATH += \
    $$PWD/.. \
    $$PWD/../Scope \
    $$PWD/../Model \
    $$PWD/../Filters \
    $$PWD/../Idac \
    $$PWD/../IdacDriver4 \
    $$PWD/../IdacDriver2 \
    $$PWD/../IdacDriver \
    $$PWD/../Core
DEPENDPATH += \
    $$PWD/.. \
    $$PWD/../Scope \
    $$PWD/../Model \
    $$PWD/../Filters \
    $$PWD/../Idac \
    $$PWD/../IdacDriver4 \
    $$PWD/../IdacDriver2 \
    $$PWD/../IdacDriver \
    $$PWD/../Core

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += \
    $$OUT_PWD/../Scope/release/libScope.a \
    $$OUT_PWD/../Model/release/libModel.a \
    $$OUT_PWD/../Filters/release/libFilters.a \
    $$OUT_PWD/../Idac/release/libIdac.a \
    $$OUT_PWD/../IdacDriverES/release/libIdacDriverES.a \
    $$OUT_PWD/../IdacDriver4/release/libIdacDriver4.a \
    $$OUT_PWD/../IdacDriver2/release/libIdacDriver2.a \
    $$OUT_PWD/../IdacDriver/release/libIdacDriver.a \
    $$OUT_PWD/../Core/release/libCore.a \
    $$PWD/../extern/win32/libusb.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += \
    $$OUT_PWD/../Scope/debug/libScope.a \
    $$OUT_PWD/../Model/debug/libModel.a \
    $$OUT_PWD/../Filters/debug/libFilters.a \
    $$OUT_PWD/../Idac/debug/libIdac.a \
    $$OUT_PWD/../IdacDriverES/debug/libIdacDriverES.a \
    $$OUT_PWD/../IdacDriver4/debug/libIdacDriver4.a \
    $$OUT_PWD/../IdacDriver2/debug/libIdacDriver2.a \
    $$OUT_PWD/../IdacDriver/debug/libIdacDriver.a \
    $$OUT_PWD/../Core/debug/libCore.a \
    $$PWD/../extern/win32/libusb.a
else:unix: PRE_TARGETDEPS += \
    $$OUT_PWD/../Scope/libScope.a \
    $$OUT_PWD/../Model/libModel.a \
    $$OUT_PWD/../Filters/libFilters.a \
    $$OUT_PWD/../Idac/libIdac.a \
    $$OUT_PWD/../IdacDriver4/libIdacDriver4.a \
    $$OUT_PWD/../IdacDriver2/libIdacDriver2.a \
    $$OUT_PWD/../IdacDriver/libIdacDriver.a \
    $$OUT_PWD/../Core/libCore.a


# For libusb
win32 {
	LIBS += -L$$PWD/../extern/win32/ -lusb
	INCLUDEPATH += $$PWD/../extern/win32
	DEPENDPATH += $$PWD/../extern/win32
}
else:macx {
	LIBS += -Wl,-framework \
		-Wl,IOKit -Wl,-framework -Wl,CoreFoundation \
		-lobjc \
		$${PWD}/../extern/libusb/lib/libusb-1.0.a
}
else {
	LIBS += \
		-Wl,-Bstatic \
		-lstdc++ \
		-Wl,-Bdynamic \
		$${PWD}/../extern/libusb/lib/libusb-1.0.a \
		-l:libudev.so.1
}

# Copy IDAC hex files
win32 {
	idac2hex.target = idc2fpga.hex
	idac2hex.commands = ${COPY_FILE} \"$$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc2fpga.hex\" \"$${OUT_PWD}\"
	idac4hex.target = idc4fpga.hex
	idac4hex.commands = ${COPY_FILE} \"$$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc4fpga.hex\" \"$${OUT_PWD}\"
	QMAKE_EXTRA_TARGETS += idac2hex idac4hex
	PRE_TARGETDEPS += idc2fpga.hex idc4fpga.hex
}
else:macx {
	hexfiles.files = $${PWD}/../Installables/idc2fpga.hex $${PWD}/../Installables/idc4fpga.hex
	hexfiles.path = Contents/MacOS
	QMAKE_BUNDLE_DATA += hexfiles
}
