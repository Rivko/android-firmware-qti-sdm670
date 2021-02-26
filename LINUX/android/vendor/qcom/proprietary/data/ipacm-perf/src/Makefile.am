AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat

AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DAEEINTERFACE_CPLUSPLUS \
        -DFEATURE_SMD \
        -DFEATURE_DIAG_MP_MASTER_APPS \
        -DFEATURE_DS_LINUX_ANDROID \
        -DFEATURE_DSS_LINUX \
        -DFEATURE_LINUX \
        -DFEATURE_MULTIPROCESSOR \
        -DFEATURE_DSUTILS_OFFTARGET \
        -DFEATURE_NO_REX \
        -I$(top_srcdir)/dsutils/inc \
        -I./../inc \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(CFLAGS) \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat

library_includedir = $(pkgincludedir)

requiredlibs = -lpthread

ipacm_perf_sources = ipacm_perf_qmi.cpp \
                     ipacm_perf_netlink.cpp \
                     ipacm_perf_rm.cpp \
                     ipacm_perf_main.cpp

bin_PROGRAMS = ipacm_perf
ipatest_CC = @CC@
ipacm_perf_SOURCES = $(ipacm_perf_sources)
if USE_GLIB
ipacm_perf_CFLAGS  = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
ipacm_perf_LDFLAGS  = $(DIAG_LIBS) $(DSUTILS_LIBS) -lpthread -lqmi_encdec -lqmiservices -lqmi_csi -lqmi_cci @GLIB_LIBS@ -shared -version-info 1:0:0
ipacm_perf_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
ipacm_perf_CFLAGS = $(AM_CFLAGS) $(DIAG_CFLAGS) -fPIC
ipacm_perf_LDFLAGS = -lpthread $(DIAG_LIBS) $(DSUTILS_LIBS) -lqmi_encdec -lqmiservices -lqmi_csi -lqmi_cci -shared -version-info 1:0:0
ipacm_perf_CPPFLAGS = $(AM_CPPFLAGS)
endif
ipacm_perf_CPPFLAGS = $(AM_CPPFLAGS)
ipacm_perf_LDADD = $(requiredlibs)
