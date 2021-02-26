# sources and intermediate files are separated
# The inclusion of qmi/core/lib/inc is a hack
# qmi/core and data/qmi have a circular depedency which
# needs to be resolved

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs

AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_SINGLE_LINK \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DFEATURE_LINUX \
        -DFEATURE_NATIVELINUX \
        -DFEATURE_DSM_DUP_ITEMS \
        -DFEATURE_QMUXD_DEPRECATION \
        $(CONFIGDB_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS)

requiredlibs = -lpthread -lconfigdb -ldsutils -lqmi
library_includedir = $(pkgincludedir)

c_sources  =  qmi_platform.c \
              linux_qmi_qmux_if_server.c \
              qmi_platform_config.c \
              qmi_platform_qmux_io.c \
              qmi_qmux.c

h_sources =

bin_PROGRAMS = qmuxd
qmuxd_CC = @CC@
qmuxd_SOURCES = $(c_sources) $(h_sources)

qmuxd_CFLAGS = -DUSE_GLIB -Dstrlcpy=g_strlcpy $(AM_CFLAGS) $(DIAG_CFLAGS) @GLIB_CFLAGS@
qmuxd_LDFLAGS = $(DIAG_LIBS) $(QMIFRAMEWORK_LIBS) $(QMI_LIBS) -shared @GLIB_LIBS@

qmuxd_CPPFLAGS =$(AM_CPPFLAGS)
qmuxd_LDADD = $(requiredlibs)

