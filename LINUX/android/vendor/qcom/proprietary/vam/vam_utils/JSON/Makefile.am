src := common/source/simple_scene_calibration.cc
src += common/source/vamath.cc
src += json_generator/source/configuration_generator.cc
src += json_generator/source/configuration_generator_apis.cc
src += json_generator/source/event_generator.cc
src += json_generator/source/event_generator_apis.cc
src += json_generator/source/metadata_generator.cc
src += json_generator/source/metadata_generator_apis.cc
src += json_parser/source/configuration_parser.cc
src += json_parser/source/configuration_parser_apis.cc
src += json_parser/source/event_parser.cc
src += json_parser/source/event_parser_apis.cc
src += json_parser/source/metadata_parser.cc
src += json_parser/source/metadata_parser_apis.cc

lib_LTLIBRARIES = libjson_apis.la
libjson_apis_la_SOURCES = $(src)
libjson_apis_la_CPPFLAGS := -I$(top_srcdir)/vam_utils/JSON/common/include
libjson_apis_la_CPPFLAGS += -I$(top_srcdir)/vam_utils/JSON/json_apis/include
libjson_apis_la_CPPFLAGS += -I$(top_srcdir)/vam_utils/JSON/json_generator/include
libjson_apis_la_CPPFLAGS += -I$(top_srcdir)/vam_utils/JSON/json_parser/include
libjson_apis_la_CPPFLAGS += -std=c++11
libjson_apis_la_LDFLAGS := -lcutils -lpthread -llog -ljsoncpp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libjson_apis.pc
EXTRA_DIST = $(pkgconfig_DATA)

jsonincdir=$(includedir)
jsoninc_HEADERS := $(top_srcdir)/vam_utils/JSON/json_apis/include/json_configuration.h
jsoninc_HEADERS += $(top_srcdir)/vam_utils/JSON/json_apis/include/configuration_parser_apis.h
jsoninc_HEADERS += $(top_srcdir)/vam_utils/JSON/json_apis/include/json_metadata.h
jsoninc_HEADERS += $(top_srcdir)/vam_utils/JSON/json_apis/include/json_event.h
jsoninc_HEADERS += $(top_srcdir)/vam_utils/JSON/json_apis/include/json_common_types.h
