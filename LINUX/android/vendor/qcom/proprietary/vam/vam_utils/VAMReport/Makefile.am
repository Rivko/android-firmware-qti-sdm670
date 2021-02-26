common_sources := source/test.cc
common_sources += source/metadata_comparisor.cc

bin_PROGRAMS := VAMReport

VAMReport_SOURCES  := $(common_sources)
VAMReport_CPPFLAGS := -I$(top_srcdir)/vam_utils/VAMReport/include
VAMReport_CPPFLAGS += -I$(top_srcdir)/vam_utils/JSON/common/include
VAMReport_CPPFLAGS += -I$(top_srcdir)/vam_utils/JSON/json_apis/include
VAMReport_CPPFLAGS += -std=c++11
VAMReport_LDADD    :=  ../JSON/libjson_apis.la
VAMReport_LDFLAGS  := -lcutils -lpthread -llog -lrt -luuid