# Run this makefile (make -f libusb.mak) in order to build
# libusb 1.x for linux and mac.

BASEDIR := $(shell pwd)
LIBUSB_SRCDIR := ${BASEDIR}/extern/libusb-1
LIBUSB_OBJDIR := ${BASEDIR}/extern/libusb-obj
LIBUSB_OUTDIR := ${BASEDIR}/extern/libusb
LIBUSB := ${LIBUSB_OUTDIR}/lib/libusb-1.0.a
COMPAT_SRCDIR := ${BASEDIR}/extern/libusb-compat-0.1
COMPAT_OBJDIR := ${BASEDIR}/extern/libusb-compat-obj
COMPAT := ${LIBUSB_OUTDIR}/lib/libusb.a

all: ${LIBUSB} ${COMPAT}

clean:
	rm -rf ${LIBUSB_OBJDIR} ${LIBUSB_OUTDIR}

${LIBUSB_SRCDIR}/configure:
	cd ${LIBUSB_SRCDIR} && ./bootstrap.sh

${LIBUSB}: ${LIBUSB_SRCDIR}/configure
	mkdir -p ${LIBUSB_OBJDIR}
	cd ${LIBUSB_OBJDIR} && \
	${LIBUSB_SRCDIR}/configure --prefix ${LIBUSB_OUTDIR} && \
	make install

${COMPAT_SRCDIR}/configure:
	cd ${COMPAT_SRCDIR} && ./bootstrap.sh

${COMPAT}: ${COMPAT_SRCDIR}/configure
	#mkdir -p ${COMPAT_OBJDIR}
	cd ${COMPAT_SRCDIR} && \
	export LIBUSB_1_0_LIBS="-L${LIBUSB_OUTDIR}/lib" && \
	export LIBUSB_1_0_CFLAGS="-I${LIBUSB_OUTDIR}/include/libusb-1.0" && \
	./configure --prefix ${LIBUSB_OUTDIR} && \
	make install
