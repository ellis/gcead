# Run this makefile (make -f libusb.mak) in order to build
# libusb 1.0 for linux and mac.

BASEDIR := $(shell pwd)
LIBUSB1_BASEDIR := extern/libusb
LIBUSB1_SRCDIR := ${LIBUSB1_BASEDIR}/libusb
LIBUSB1_LIBDIR := ${LIBUSB1_SRCDIR}/.libs
LIBUSB1 := ${LIBUSB1_LIBDIR}/libusb-1.0.a
LIBCOMPAT := extern/libusb-compat-0.1.3/libusb/.libs/libusb.a

all: ${LIBUSB1} ${LIBCOMPAT}

${LIBUSB1}:
	cd ${LIBUSB1_BASEDIR} && \
        ./configure CC="gcc -arch i386" CXX="g++ -arch i386" && \
	make

${LIBCOMPAT}:
	cd extern/libusb-compat-0.1.3 && \
	export LIBUSB_1_0_LIBS="-L${BASEDIR}/${LIBUSB1_LIBDIR}" && \
	export LIBUSB_1_0_CFLAGS="-I${BASEDIR}/${LIBUSB1_SRCDIR}" && \
        ./configure CC="gcc -arch i386" CXX="g++ -arch i386" && \
	make
