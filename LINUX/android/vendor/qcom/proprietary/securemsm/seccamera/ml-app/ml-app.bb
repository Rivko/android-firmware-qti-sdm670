inherit autotools

DESCRIPTION = "test app run in ML VM"
LICENSE          = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://app_main.c;beginline=1;endline=5;md5=9012efb83459d71d0621d514e80fc4ed"

SRC_URI = "file://app_main.c  \
           file://configure.ac \
           file://Makefile.am \
           file://seccamlib.c \
           file://seccamlib.h \
           file://sock_comm.c \
           file://sock_comm.h \
           file://dummy_ipc.h"

S = "${WORKDIR}"

PR = "1"

EXTRA_OEMAKE += "ARCH=${TARGET_ARCH} CROSS_COMPILE=${TARGET_PREFIX}"

EXTRA_OECONF += "--with-kernel=${STAGING_KERNEL_DIR} \
                --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"

FILES_${PN} += "${bindir}/*"
