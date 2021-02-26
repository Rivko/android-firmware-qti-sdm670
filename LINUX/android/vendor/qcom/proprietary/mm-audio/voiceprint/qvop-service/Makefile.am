
AM_CFLAGS = -Werror -Wno-unused-parameter -Wno-ignored-attributes
AM_CFLAGS += -DQVOP_DEBUG
AM_CFLAGS += -DQVOP_LNX_ONLY
AM_CFLAGS += -I $(top_srcdir)/../../../securemsm/QSEEComAPI/
AM_CFLAGS += -I $(top_srcdir)/../
AM_CFLAGS += -I $(top_srcdir)/../../../diag/include

c_sources = logfile.cpp \
            QvopCommandService.cpp \
            QvopService.cpp \
            qvop_interface.c \
            qvop_qsee_interface.c \
            qvop_qsee.c

lib_LTLIBRARIES = libqvop_lnx_service.la
libqvop_lnx_service_la_SOURCES = $(c_sources)
libqvop_lnx_service_la_LIBADD = -lc -lcutils -ldiag -lutils -lQseeComApi $(GLIB_LIBS)
libqvop_lnx_service_la_CFLAGS = $(AM_CFLAGS) -Dstrlcpy=g_strlcpy $(GLIB_CFLAGS) -include glib.h -include string.h
libqvop_lnx_service_la_CPPFLAGS = $(AM_CFLAGS) -Dstrlcpy=g_strlcpy $(GLIB_CFLAGS) -include glib.h -include string.h -include stdint.h
libqvop_lnx_service_la_LDFLAGS = -module -shared -avoid-version

bin_PROGRAMS = qvop-tester-lnx

QVOP_CPPFLAGS = -Werror -Wno-unused-parameter
QVOP_CPPFLAGS += -DQVOP_DEBUG
QVOP_CPPFLAGS += -DQVOP_LNX_ONLY -std=c++11

QVOP_INCLUDES = -I $(top_srcdir)/../../../../securemsm/QSEEComAPI
QVOP_INCLUDES += -I $(top_srcdir)/../
QVOP_INCLUDES += -I $(top_srcdir)/../../../hardware/qcom/audio/qahw_api/inc/
QVOP_INCLUDES += -I $(WORKSPACE)/hardware/qcom/audio/qahw_api/inc/
QVOP_INCLUDES += -I $(WORKSPACE)/audio/mm-audio/qsthw_api/

qvop_tester_lnx_SOURCES = test-app-lnx/QvopTesterLnx.cpp logfile.cpp
qvop_tester_lnx_CPPFLAGS = $(QVOP_CPPFLAGS) $(QVOP_INCLUDES) -include string.h
qvop_tester_lnx_CPPFLAGS += -Dstrlcpy=g_strlcpy  -Dstrlcat=g_strlcat $(GLIB_CFLAGS) -include glib.h
qvop_tester_lnx_LDADD = -lc -lcutils -lutil libqvop_lnx_service.la -llog -lqahw $(GLIB_LIBS) -lqtisoundtriggerhw
