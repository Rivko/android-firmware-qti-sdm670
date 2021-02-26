common_sources := testEngine2.cc
common_sources += ../common/VAMUtilities.cc

lib_LTLIBRARIES = libEngine_Test2.la
libEngine_Test2_la_SOURCES = $(common_sources)
libEngine_Test2_la_CPPFLAGS :=
libEngine_Test2_la_CPPFLAGS += -std=c++11
libEngine_Test2_la_LDFLAGS := -lcutils -lpthread -llog -lrt

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libEngine_Test2.pc
EXTRA_DIST = $(pkgconfig_DATA)
