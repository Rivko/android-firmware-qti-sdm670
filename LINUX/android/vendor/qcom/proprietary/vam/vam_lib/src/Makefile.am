common_sources := VAAPI.cc
common_sources += VAManager.cc
common_sources += VAMUtilities.cc
common_sources += VAMEngineBase.cc
common_sources += VAMEngine_DynEngine.cc

lib_LTLIBRARIES = libVAManager.la
libVAManager_la_SOURCES = $(common_sources)
libVAManager_la_CPPFLAGS := -I$(top_srcdir)/inc
libVAManager_la_CPPFLAGS += -std=c++14 -Wall -Wextra -Werror
libVAManager_la_LDFLAGS := -lcutils -lpthread -llog -rdynamic -ldl

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libVAManager.pc
EXTRA_DIST = $(pkgconfig_DATA)
