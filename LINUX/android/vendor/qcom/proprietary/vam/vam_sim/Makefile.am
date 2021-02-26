common_sources := src/VASim.cc
common_sources += src/ConfigBase.cc
common_sources += src/VASimConfig.cc
common_sources += src/VAMExtRuleConfig.cc
common_sources += src/VAMJSONRuleConfig.cc
common_sources += src/VASimFRConfig.cc

bin_PROGRAMS := VASim

VASim_SOURCES  := $(common_sources)
VASim_CPPFLAGS := -I$(top_srcdir)/vam_sim/inc
VASim_CPPFLAGS += -I$(top_srcdir)/vam_utils/JSON/json_apis/include
VASim_CPPFLAGS += -std=c++14 -Wall -Wextra -Werror
VASim_LDADD    :=  ../vam_utils/JSON/libjson_apis.la
VASim_LDFLAGS  := -lVAManager -lz -lm \
	-lcutils -lpthread -llog -ldl -lrt -luuid

