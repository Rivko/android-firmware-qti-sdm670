if QTI_AUDIO_SERVER_ENABLED
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = qti-audio-server.pc
EXTRA_DIST = $(pkgconfig_DATA)

h_sources = qti_audio_server.h \
            qti_audio_server_interface.h \
            qti_audio_server_client.h \
            qti_audio_server_callback.h

AM_CPPFLAGS = -I $(PKG_CONFIG_SYSROOT_DIR)/usr/include/mm-audio/qahw_api/inc
AM_CPPFLAGS += -I $(PKG_CONFIG_SYSROOT_DIR)/usr/include/mm-audio/qahw/inc
AM_CPPFLAGS += -D__unused=__attribute__\(\(__unused__\)\)

lib_includedir = $(includedir)/mm-audio/qti-audio-server
lib_include_HEADERS = $(h_sources)

cpp_sources := qti_audio_server.cpp
cpp_sources += qti_audio_proxy.cpp
cpp_sources += qti_audio_server_callback.cpp

lib_LTLIBRARIES = libqtiaudioserver.la
libqtiaudioserver_la_SOURCES   = $(cpp_sources)
libqtiaudioserver_la_CPPFLAGS := $(AM_CPPFLAGS) -std=c++11 -DHAVE_PTHREADS -DHAVE_ANDROID_OS
libqtiaudioserver_la_CPPFLAGS += -DUSE_GLIB -D__linux__
libqtiaudioserver_la_CPPFLAGS += -DDEBUG_REFS_CALLSTACK_ENABLED=0
libqtiaudioserver_la_LDFLAGS   = -shared -module -avoid-version
libqtiaudioserver_la_LDFLAGS  += -ltinyalsa -lhardware -lcutils -llog -ldl -lbinder
libqtiaudioserver_la_LDFLAGS  += -llog -lcutils -lpthread -lutils -lqahwwrapper

# Build testapp
bin_PROGRAMS := test_qas

test_qas_SOURCES  := test_qas.cpp
test_qas_CPPFLAGS := $(libqtiaudioserver_la_CPPFLAGS)
test_qas_LDADD    := libqtiaudioserver.la
test_qas_LDFLAGS  := -lutils -lcutils -llog -lpthread -lbinder

# Build testapp
bin_PROGRAMS += qti_audio_server_daemon

qti_audio_server_daemon_SOURCES  := qti_audio_server_daemon.cpp
qti_audio_server_daemon_CPPFLAGS := $(libqtiaudioserver_la_CPPFLAGS)
qti_audio_server_daemon_LDADD    := libqtiaudioserver.la
qti_audio_server_daemon_LDFLAGS  := -lutils -lcutils -llog -lpthread -lbinder

endif
