# Run this makefile (make -f libusbx.mak) in order to build
# libusbx 1.x for linux and mac.

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

${LIBUSB}:
	mkdir -p ${LIBUSB_OBJDIR}
	cd ${LIBUSB_OBJDIR} && \
	${LIBUSB_SRCDIR}/configure --prefix ${LIBUSB_OUTDIR} && \
	make install

${COMPAT}:
	mkdir -p ${COMPAT_OBJDIR}
	cd ${COMPAT_OBJDIR} && \
	export LIBUSB_1_0_LIBS="-L${LIBUSB_OUTDIR}/lib" && \
	export LIBUSB_1_0_CFLAGS="-I${LIBUSB_OUTDIR}/include/libusb-1.0" && \
	${COMPAT_SRCDIR}/configure --prefix ${LIBUSB_OUTDIR} && \
	make install
