common_sources := engineGenerator.cc
common_sources += ../common/VAMUtilities.cc

lib_LTLIBRARIES = libEngine_GTRenderer.la
libEngine_GTRenderer_la_SOURCES = $(common_sources)
libEngine_GTRenderer_la_CPPFLAGS := -I$(top_srcdir)/vam_utils/JSON/json_apis/include/
libEngine_GTRenderer_la_CPPFLAGS += -std=c++11
libEngine_GTRenderer_la_LDFLAGS := -lcutils -lpthread -llog -lrt

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libEngine_GTRenderer.pc
EXTRA_DIST = $(pkgconfig_DATA)
