LOCAL_PATH := $(call my-dir)

VAM_TOP_SRCDIR := $(LOCAL_PATH)/../..

include $(VAM_TOP_SRCDIR)/build.mk

ifneq (,$(BUILD_VAM))

# Build libjson_apis library
# libjson_apis.so

include $(CLEAR_VARS)
#LOCAL_COPY_HEADERS_TO := JSON
LOCAL_COPY_HEADERS := json_apis/include/json_configuration.h
LOCAL_COPY_HEADERS += json_apis/include/configuration_parser_apis.h
LOCAL_COPY_HEADERS += json_apis/include/json_metadata.h
LOCAL_COPY_HEADERS += json_apis/include/json_event.h
LOCAL_COPY_HEADERS += json_apis/include/json_common_types.h

include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)

include $(VAM_TOP_SRCDIR)/common.mk

LOCAL_C_INCLUDES := $(LOCAL_PATH)/common/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/json_apis/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/json_generator/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/json_parser/include

LOCAL_SRC_FILES := common/source/simple_scene_calibration.cc
LOCAL_SRC_FILES += common/source/vamath.cc
LOCAL_SRC_FILES += json_generator/source/configuration_generator.cc
LOCAL_SRC_FILES += json_generator/source/configuration_generator_apis.cc
LOCAL_SRC_FILES += json_generator/source/event_generator.cc
LOCAL_SRC_FILES += json_generator/source/event_generator_apis.cc
LOCAL_SRC_FILES += json_generator/source/metadata_generator.cc
LOCAL_SRC_FILES += json_generator/source/metadata_generator_apis.cc
LOCAL_SRC_FILES += json_parser/source/configuration_parser.cc
LOCAL_SRC_FILES += json_parser/source/configuration_parser_apis.cc
LOCAL_SRC_FILES += json_parser/source/event_parser.cc
LOCAL_SRC_FILES += json_parser/source/event_parser_apis.cc
LOCAL_SRC_FILES += json_parser/source/metadata_parser.cc
LOCAL_SRC_FILES += json_parser/source/metadata_parser_apis.cc

LOCAL_SHARED_LIBRARIES += libjsoncpp_vendor

LOCAL_MODULE = libjson_apis

include $(BUILD_SHARED_LIBRARY)

endif # BUILD_VAM