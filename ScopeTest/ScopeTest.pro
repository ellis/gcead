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
CONFIG(debug, debug|release):DESTDIR = ../debug
else:DESTDIR = ../release
LIBS += -L$${DESTDIR}

# VPATH += ../debug
LIBS += \
	-lScope \
	-lModel \
	-lIdac \
    -lIdacDriver4 \
    -lIdacDriver2 \
    -lIdacDriver \
	-lCore
PRE_TARGETDEPS += $${DESTDIR}/libCore.a \
    $${DESTDIR}/libIdacDriver.a \
    $${DESTDIR}/libIdacDriver2.a \
    $${DESTDIR}/libIdacDriver4.a \
    $${DESTDIR}/libIdac.a \
    $${DESTDIR}/libModel.a \
    $${DESTDIR}/libScope.a
win32:LIBS += ../extern/win32/libusb.a
unix:!macx:LIBS += -static-libgcc \
    -L../release \
    -Wl,-Bstatic \
    -lstdc++ \
    -Wl,-Bdynamic \
    ../extern/libusb-compat-0.1.3/libusb/.libs/libusb.a \
    ../extern/libusb/libusb/.libs/libusb-1.0.a
unix:macx:LIBS += -Wl,-framework \
    -Wl,IOKit \
    ../extern/libusb-compat-0.1.3/libusb/.libs/libusb.a \
    ../extern/libusb/libusb/.libs/libusb-1.0.a
unix:QMAKE_CFLAGS += -static-libgcc
unix:QMAKE_CXXFLAGS += -static-libgcc
unix:QMAKE_LFLAGS += -static-libgcc
unix:pg { 
    QMAKE_CFLAGS += -pg
    QMAKE_CXXFLAGS += -pg
    QMAKE_LFLAGS += -pg
}

# Copy IDAC2 hex file
!macx:idac2hex.target = $${DESTDIR}/idc2fpga.hex
macx:idac2hex.target = $${DESTDIR}/GcEad.app/Contents/MacOS/idc2fpga.hex
win32:idac2hex.commands = ${COPY_FILE} \
    ..\Installables\idc2fpga.hex \
    ${DESTDIR}
unix:!macx:idac2hex.commands = ${COPY_FILE} \
    ../Installables/idc2fpga.hex \
    ${DESTDIR}
macx:idac2hex.commands = ${COPY_FILE} \
    ../Installables/idc2fpga.hex \
    ${DESTDIR}/GcEad.app/Contents/MacOS

# Copy IDAC4 hex file
idac4hex.target = $${DESTDIR}/idc4fpga.hex
win32:idac4hex.commands = ${COPY_FILE} \
    ..\Installables\idc4fpga.hex \
    ${DESTDIR}
unix:!macx:idac4hex.commands = ${COPY_FILE} \
    ../Installables/idc4fpga.hex \
    ${DESTDIR}
macx:idac4hex.commands = ${COPY_FILE} \
    ../Installables/idc4fpga.hex \
    ${DESTDIR}/GcEad.app/Contents/MacOS
QMAKE_EXTRA_TARGETS += idac2hex \
    idac4hex
POST_TARGETDEPS += $${DESTDIR}/idc2fpga.hex \
    $${DESTDIR}/idc4fpga.hex
HEADERS += \
	TestBase.h
SOURCES += \
	TestBase.cpp \
	./main.cpp
