common_sources := testEngine1.cc
common_sources += ../common/VAMUtilities.cc

lib_LTLIBRARIES = libEngine_Test1.la
libEngine_Test1_la_SOURCES = $(common_sources)
libEngine_Test1_la_CPPFLAGS :=
libEngine_Test1_la_CPPFLAGS += -std=c++11
libEngine_Test1_la_LDFLAGS := -lcutils -lpthread -llog -lrt

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libEngine_Test1.pc
EXTRA_DIST = $(pkgconfig_DATA)
