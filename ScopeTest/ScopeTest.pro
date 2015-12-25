TEMPLATE = app
QT += xml svg
CONFIG += warn_on link_prl

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
    $$PWD/../extern/win32/libusb-1.0.a
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
    $$PWD/../extern/win32/libusb-1.0.a
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
        LIBS += $$PWD/../extern/win32/libusb-1.0.a
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
