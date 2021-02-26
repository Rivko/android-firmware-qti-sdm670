utils_sources = \
    utils/src/dvpTest_tb.c \
    utils/src/dvpTest_android.c \
    utils/src/buf_pool.c \
    utils/src/test_utils.c \
    utils/src/test_mock_ip.c \
    utils/src/test_mock_registry.c

AM_CFLAGS = \
    -Wundef \
    -Werror \
    -Wall \
    -Wno-unused-but-set-variable \
    -Wno-strict-aliasing \
    -fno-short-enums \
    -g -O1 \
    -I../inc \
    -I../lib/core/inc \
    -I../lib/utils/inc \
    -Iutils/inc \
    -include stdint.h \
    -I$(WORKSPACE)/hardware/qcom/media/mm-core/inc/ \
    -I$(WORKSPACE)/system/core/include/ \
    -D__unused=__attribute__\(\(__unused__\)\) \
    -D_ANDROID_

AM_LDFLAGS = -ldl -lpthread -lglib-2.0 -llog -lcutils

if VPP_TARGET_USES_GPU
AM_CFLAGS += -I../lib/ip/gpu/inc/
endif

if VPP_TARGET_USES_HVX
AM_CFLAGS += -I../lib/ip/hvx/inc/
endif

lib_LTLIBRARIES = libvpptestutils.la
libvpptestutils_la_SOURCES = $(utils_sources)
libvpptestutils_la_CFLAGS = $(AM_CFLAGS) -DVPP_TEST_UTILS -Dstrlcat=g_strlcat -Wno-implicit-function-declaration
libvpptestutils_la_LDFLAGS = $(AM_LDFLAGS) -shared -avoid-version
libvpptestutils_la_LIBADD = ../lib/libvpplibrary.la

ut_sources = \
    unit_tests/src/test_core.c \
    unit_tests/src/test_uc.c \
    unit_tests/src/test_queue.c \
    unit_tests/src/test_buf.c \
    unit_tests/src/test_pipeline.c \
    unit_tests/src/test_vpp.c \
    unit_tests/src/test_ip.c \
    unit_tests/src/test_stats.c

if VPP_TARGET_USES_GPU
ut_sources += unit_tests/src/test_ip_gpu.c
endif

if VPP_TARGET_USES_HVX_CORE
AM_CFLAGS += -I../lib/ip/hvx_core/inc
endif

if VPP_TARGET_USES_FRC
ut_sources += unit_tests/src/test_ip_me.c \
              unit_tests/src/test_ip_mc.c
AM_CFLAGS += -I../lib/ip/frc/inc
endif

if VPP_TARGET_USES_HVX
ut_sources += unit_tests/src/test_ip_hvx.c
AM_CFLAGS += -I../lib/ip/hvx/inc
endif

bin_PROGRAMS = vpplibraryunittest
vpplibraryunittest_SOURCES = $(ut_sources)
vpplibraryunittest_CFLAGS = $(AM_CFLAGS) -DVPP_UNIT_TEST=1 -Dstrlcat=g_strlcat -Wno-implicit-function-declaration
vpplibraryunittest_LDFLAGS = $(AM_LDFLAGS)
vpplibraryunittest_LDADD = libvpptestutils.la ../lib/libvpplibrary.la

ft_sources = \
    functional_tests/src/test_main.cpp \
    functional_tests/src/test_buffer_exchange.cpp \
    functional_tests/src/test_concurrency.cpp

bin_PROGRAMS += vpplibraryfunctionaltest
vpplibraryfunctionaltest_SOURCES = $(ft_sources)
vpplibraryfunctionaltest_CPPFLAGS = $(AM_CFLAGS) -DVPP_FUNCTIONAL_TEST $(GLIB_CFLAGS) -include glib.h -Dstrlcpy=g_strlcpy
vpplibraryfunctionaltest_LDFLAGS = $(AM_LDFLAGS)
vpplibraryfunctionaltest_LDADD = libvpptestutils.la ../lib/libvpplibrary.la
