AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(CONFIGDB_CFLAGS) \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
        $(QMICLIENTHELPER_CFLAGS) \
        $(DATAOSS_CFLAGS)

#refer to Androik.mk for staging header files
AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_DSUTILS_OFFTARGET \
        -DFEATURE_CONFIGDB_OFFTARGET \
        -DFEATURE_DS_LINUX_NO_RPC \
        -DFEATURE_DATA_LINUX_LE \
        -DFEATURE_DATA_LINUX_EMBMS_ODU \
        -DFEATURE_DATA_SOFTAP_V6 \
        -DFEATURE_LINUX \
        -DFEATURE_QMUXD_DEPRECATION \
        -DFEATURE_DATA_PREFIX_DELEGATION \
        -I../inc \
        -I./plugins/inc \
        -I$(topdir)/dsutils/inc \
        -I$(topdir)/configdb/inc \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
        $(QMICLIENTHELPER_CFLAGS) \
        $(DATAOSS_CFLAGS)

libnetmgr_c_sources = netmgr_client.c \
        netmgr_netlink.c \
        netmgr_unix.c \
        netmgr_util.c

requiredlibs = -lpthread -lconfigdb -ldsutils

library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libnetmgr.la
libnetmgr_la_CC = @CC@
libnetmgr_la_SOURCES = $(libnetmgr_c_sources)
libnetmgr_la_CFLAGS  = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libnetmgr_la_LDFLAGS  = $(DATAOSS_LIBS)  @GLIB_LIBS@
libnetmgr_la_CPPFLAGS = $(AM_CPPFLAGS)
libnetmgr_la_LIBADD = $(requiredlibs)

#####################netmgrd##################

netmgrd_sources = netmgr.c \
        netmgr_exec.c \
        netmgr_kif.c \
        netmgr_main.c \
        netmgr_platform.c \
        netmgr_qmi.c \
        netmgr_sm.c \
        netmgr_test.c \
        netmgr_sm_int.c \
        netmgr_tc.c \
        netmgr_qmi_dpm.c \
        netmgr_qmi_wda.c \
        netmgr_qmi_wds.c \
        netmgr_qmi_dsd.c \
        netmgr_qmi_qos.c \
        netmgr_qmi_dfs.c \
        netmgr_rmnet.c \
        netmgr_config.c \
        netmgr_cmdq.c \
	netmgr_cb.c

netmgrd_h_sources = ../inc/netmgr.h

bin_PROGRAMS = netmgrd
netmgrd_CC = @CC@
netmgrd_SOURCES = $(netmgrd_sources) $(netmgrd_h_sources)
netmgrd_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
netmgrd_LDFLAGS = $(DIAG_LIBS) $(QMICLIENTHELPER_LIBS) -shared  @GLIB_LIBS@
netmgrd_CPPFLAGS = $(AM_CPPFLAGS)
netmgrd_LDADD = $(requiredlibs) libnetmgr.la
