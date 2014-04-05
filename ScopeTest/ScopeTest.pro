TEMPLATE = app
QT += xml svg
CONFIG += warn_on link_prl

# VPATH += ../debug
#PRE_TARGETDEPS += ../Core/libCore.a \
#	../IdacDriver/libIdacDriver.a \
#	../IdacDriver2/libIdacDriver2.a \
#	../IdacDriver4/libIdacDriver4.a \
#	../Idac/libIdac.a \
#	../Filters/libFilters.a \
#	../Model/libModel.a \
#	../Scope/libScope.a
#win32:PRE_TARGETDEPS += $${DESTDIR}/libIdacDriverES.a
#win32:LIBS += $${PWD}/../extern/win32/libusb.a $${PWD}/../IdacDriverES/IdacControl/IDAC8_32.lib
#win32:LIBS += $${PWD}/../extern/win32/libusb.a
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
#win32:LIBS += -lIdacDriverES

unix:QMAKE_CFLAGS += -static-libgcc
unix:QMAKE_CXXFLAGS += -static-libgcc
unix:QMAKE_LFLAGS += -static-libgcc
unix:pg { 
    QMAKE_CFLAGS += -pg
    QMAKE_CXXFLAGS += -pg
    QMAKE_LFLAGS += -pg
}

# Copy IDAC2 hex file
#!macx:idac2hex.target = ./idc2fpga.hex
#macx:idac2hex.target = $${DESTDIR}/ScopeTest.app/Contents/MacOS/idc2fpga.hex
#win32:idac2hex.commands = ${COPY_FILE} \
#    $$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc2fpga.hex \
#    ${DESTDIR}
#unix:!macx:idac2hex.commands = ${COPY_FILE} \
#    $${PWD}/../Installables/idc2fpga.hex \
#    .
#macx:idac2hex.commands = ${COPY_FILE} \
#    $${PWD}/../Installables/idc2fpga.hex \
#    ${DESTDIR}
##    ${DESTDIR}/ScopeTest.app/Contents/MacOS/

## Copy IDAC4 hex file
#!mac:idac4hex.target = idc4fpga.hex
#mac:idac4hex.target = $${DESTDIR}/ScopeTest.app/Contents/MacOS/idc4fpga.hex
#win32:idac4hex.commands = ${COPY_FILE} \
#    $$replace(_PRO_FILE_PWD_, '/', '\\')\..\Installables\idc4fpga.hex \
#    ${DESTDIR}
#unix:!macx:idac4hex.commands = ${COPY_FILE} \
#    $${PWD}/../Installables/idc4fpga.hex \
#    .
#macx:idac4hex.commands = ${COPY_FILE} \
#    $${PWD}/../Installables/idc4fpga.hex \
#    ${DESTDIR}
##    ${DESTDIR}/ScopeTest.app/Contents/MacOS/

#QMAKE_EXTRA_TARGETS += idac2hex \
#    idac4hex
#!mac:POST_TARGETDEPS += idc2fpga.hex \
#	idc4fpga.hex
#mac:POST_TARGETDEPS += $${DESTDIR}/ScopeTest.app/Contents/MacOS/idc2fpga.hex \
#    $${DESTDIR}/ScopeTest.app/Contents/MacOS/idc4fpga.hex

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

# Copy IDAC hex files
install_hex.files = $${PWD}/../Installables/idc*.hex
install_hex.path = $${OUT_PWD}
INSTALLS += install_hex

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

# For libusb on windows
win32:LIBS += -L$$PWD/../extern/win32/ -lusb
win32:INCLUDEPATH += $$PWD/../extern/win32
win32:DEPENDPATH += $$PWD/../extern/win32
