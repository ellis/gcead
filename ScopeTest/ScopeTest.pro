TEMPLATE = app
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
#CONFIG(debug, debug|release):DESTDIR = ../debug
#else:DESTDIR = ../release
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
win32:PRE_TARGETDEPS += $${DESTDIR}/libIdacDriverES.a
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
!macx:idac2hex.target = ./idc2fpga.hex
macx:idac2hex.target = $${DESTDIR}/ScopeTest.app/Contents/MacOS/idc2fpga.hex
win32:idac2hex.commands = ${COPY_FILE} \
    $$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc2fpga.hex \
    ${DESTDIR}
unix:!macx:idac2hex.commands = ${COPY_FILE} \
    $${PWD}/../Installables/idc2fpga.hex \
    .
macx:idac2hex.commands = ${COPY_FILE} \
    $${PWD}/../Installables/idc2fpga.hex \
    ${DESTDIR}
#    ${DESTDIR}/ScopeTest.app/Contents/MacOS/

# Copy IDAC4 hex file
!mac:idac4hex.target = idc4fpga.hex
mac:idac4hex.target = $${DESTDIR}/ScopeTest.app/Contents/MacOS/idc4fpga.hex
win32:idac4hex.commands = ${COPY_FILE} \
    $$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc4fpga.hex \
    ${DESTDIR}
unix:!macx:idac4hex.commands = ${COPY_FILE} \
    $${PWD}/../Installables/idc4fpga.hex \
    .
macx:idac4hex.commands = ${COPY_FILE} \
    $${PWD}/../Installables/idc4fpga.hex \
    ${DESTDIR}
#    ${DESTDIR}/ScopeTest.app/Contents/MacOS/

QMAKE_EXTRA_TARGETS += idac2hex \
    idac4hex
!mac:POST_TARGETDEPS += idc2fpga.hex \
	idc4fpga.hex
mac:POST_TARGETDEPS += $${DESTDIR}/ScopeTest.app/Contents/MacOS/idc2fpga.hex \
    $${DESTDIR}/ScopeTest.app/Contents/MacOS/idc4fpga.hex
HEADERS += \
	TestBase.h \
	WaitForHardwareDialog.h \
	RecordDialog.h \
	TestRecording.h
SOURCES += \
	TestBase.cpp \
	WaitForHardwareDialog.cpp \
	RecordDialog.cpp \
	TestRecording.cpp \
	./main.cpp
