common_sources := vam_engine_wrapper_template.cc


lib_LTLIBRARIES = libEngine_CustomEngineTemplate.la
libEngine_CustomEngineTemplate_la_SOURCES = $(common_sources)
libEngine_CustomEngineTemplate_la_CPPFLAGS := -std=c++11
libEngine_CustomEngineTemplate_la_LDFLAGS := -lcutils -lpthread -llog -lrt

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libEngine_CustomEngineTemplate.pc
EXTRA_DIST = $(pkgconfig_DATA)
