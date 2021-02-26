LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
# build a library for external parties to include
LOCAL_MODULE := libsnsapi
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

# Local source files
c_sources =
c_sources += sns_client_api_v01.c

# Auto-generated sources from proto files
## find all .proto files in proto/ folder
proto_files := $(call all-proto-files-under, proto)
proto_path := $(LOCAL_PATH)/proto
sns_gen_sources_dir := $(local-generated-sources-dir)

## all auto-generated sources, to be created before compilation
LOCAL_GENERATED_SOURCES := $(patsubst %.proto, $(sns_gen_sources_dir)/%.pb.cpp, $(proto_files))

sns_protoc_flags := --proto_path=$(proto_path) \
		--proto_path=external/protobuf/src \
		--cpp_out=$(sns_gen_sources_dir)/proto

$(info sns_gen_sources_dir = $(sns_gen_sources_dir))

## Custom rule to generate .cpp file from .proto file
$(sns_gen_sources_dir)/proto/%.pb.cpp:$(LOCAL_PATH)/proto/%.proto $(PROTOC)
	echo "[PROTOC RUNNING on]" $@"<="$<
	$(PROTOC) $(sns_protoc_flags) $<
	echo "[PROTOC DONE]" $@ "<=" $<
	cp $(patsubst %.pb.cpp, %.pb.cc, $@) $@
	# copy the generated sources in src_dir to help with indexing
	#mkdir -p $(proto_path)/gen
	#cp -f $@ $(proto_path)/gen/
	#cp -f $(patsubst %.pb.cpp, %.pb.h, $@) $(proto_path)/gen/

# Include paths
c_includes =
c_includes += $(sns_gen_sources_dir)/proto
c_includes += $(TARGET_OUT_HEADERS)/qmi-framework/inc
c_includes += $(LOCAL_PATH)

# shared libraries
shared_libs =
shared_libs += libprotobuf-cpp-full
shared_libs += libqmi_common_so
shared_libs += libqmi_cci
shared_libs += libqmi_encdec
shared_libs += libcutils
shared_libs += libutils


c_export_includes =
c_export_includes += $(sns_gen_sources_dir)/proto
c_export_includes += $(LOCAL_PATH)

## Wno-unused-parameter required to ignore some warnings
## coming from google protobuf code
c_flags = -Werror -Wall -Wno-unused-parameter -fexceptions
ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf
endif
# auto-generated protobuf files require this preprocessor macro
cpp_flags = -DGOOGLE_PROTOBUF_NO_RTTI

## C/C++ compiler flags
LOCAL_CFLAGS   = $(c_flags)


LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES = $(c_sources)
LOCAL_CPPFLAGS = $(cpp_flags)
LOCAL_C_INCLUDES = $(c_includes)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(c_export_includes)
LOCAL_SHARED_LIBRARIES = $(shared_libs)

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libsnsapi_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(c_export_includes)
include $(BUILD_HEADER_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libssc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

# Local source files
c_sources =
c_sources += wakelock_utils.cpp
c_sources += ssc_connection.cpp
c_sources += ssc_utils.cpp
c_sources += sns_client_api_v01.c

# Auto-generated sources from proto files
## find all .proto files in proto/ folder
proto_files := $(call all-proto-files-under, proto)
proto_path := $(LOCAL_PATH)/proto
gen_sources_dir := $(local-generated-sources-dir)

## all auto-generated sources, to be created before compilation
LOCAL_GENERATED_SOURCES := $(patsubst %.proto, $(gen_sources_dir)/%.pb.cpp, $(proto_files))

protoc_flags := --proto_path=$(proto_path) \
                --proto_path=external/protobuf/src \
                --cpp_out=$(gen_sources_dir)/proto

$(info gen_sources_dir = $(gen_sources_dir))

## Custom rule to generate .cpp file from .proto file
$(gen_sources_dir)/proto/%.pb.cpp: $(LOCAL_PATH)/proto/%.proto $(PROTOC)
	echo "[PROTOC RUNNING on]" $@ "<=" $<
	$(PROTOC) $(protoc_flags) $<
	echo "[PROTOC DONE]" $@ "<=" $<
	cp $(patsubst %.pb.cpp, %.pb.cc, $@) $@
	# copy the generated sources in src_dir to help with indexing
	#mkdir -p $(proto_path)/gen
	#cp -f $@ $(proto_path)/gen/
	#cp -f $(patsubst %.pb.cpp, %.pb.h, $@) $(proto_path)/gen/

# Include paths
c_includes =
c_includes += $(gen_sources_dir)/proto
c_includes += $(TARGET_OUT_HEADERS)/qmi-framework/inc
c_includes += $(LOCAL_PATH)
c_includes += $(LOCAL_PATH)/../sensors-log

# shared libraries
shared_libs =
shared_libs += liblog
shared_libs += libsensorslog
shared_libs += libprotobuf-cpp-full
shared_libs += libqmi_common_so
shared_libs += libqmi_cci
shared_libs += libqmi_encdec
shared_libs += libcutils

c_export_includes =
c_export_includes += $(gen_sources_dir)/proto
c_export_includes += $(LOCAL_PATH)

## Wno-unused-parameter required to ignore some warnings
## coming from google protobuf code
c_flags = -Werror -Wall -Wno-unused-parameter -fexceptions
ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf
endif
# auto-generated protobuf files require this preprocessor macro
cpp_flags = -DGOOGLE_PROTOBUF_NO_RTTI

## C/C++ compiler flags
LOCAL_CFLAGS   = $(c_flags)

ifeq ($(call is-board-platform-in-list, sdm710),true)
    LOCAL_CFLAGS += -DADSPRPC
endif

LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES = $(c_sources)
LOCAL_CPPFLAGS = $(cpp_flags)
LOCAL_C_INCLUDES = $(c_includes)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(c_export_includes)
LOCAL_SHARED_LIBRARIES = $(shared_libs)

include $(BUILD_SHARED_LIBRARY)

# TODO: add export_includes for this library

include $(CLEAR_VARS)
#LOCAL_PATH := $(call my-dir)

LOCAL_MODULE := libssc_system
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

# Local source files
c_sources =
c_sources += wakelock_utils.cpp
c_sources += ssc_connection.cpp
c_sources += ssc_utils.cpp
c_sources += sns_client_api_v01.c

# Auto-generated sources from proto files
## find all .proto files in proto/ folder
proto_files := $(call all-proto-files-under, proto)
proto_path := $(LOCAL_PATH)/proto
gen_sources_dir_system := $(local-generated-sources-dir)

## all auto-generated sources, to be created before compilation
LOCAL_GENERATED_SOURCES := $(patsubst %.proto, $(gen_sources_dir_system)/%.pb.cpp, $(proto_files))

protoc_flags_system := --proto_path=$(proto_path) \
                --proto_path=external/protobuf/src \
                --cpp_out=$(gen_sources_dir_system)/proto

$(info gen_sources_dir_system = $(gen_sources_dir_system))

## Custom rule to generate .cpp file from .proto file
$(gen_sources_dir_system)/proto/%.pb.cpp: $(LOCAL_PATH)/proto/%.proto $(PROTOC)
	echo "[PROTOC RUNNING on]" $@ "<=" $<
	$(PROTOC) $(protoc_flags_system) $<
	echo "[PROTOC DONE]" $@ "<=" $<
	cp $(patsubst %.pb.cpp, %.pb.cc, $@) $@
	# copy the generated sources in src_dir to help with indexing
	#mkdir -p $(proto_path)/gen
	#cp -f $@ $(proto_path)/gen/
	#cp -f $(patsubst %.pb.cpp, %.pb.h, $@) $(proto_path)/gen/

# Include paths
c_includes =
c_includes += $(gen_sources_dir_system)/proto
c_includes += $(TARGET_OUT_HEADERS)/qmi-framework/inc
c_includes += $(LOCAL_PATH)
c_includes += $(LOCAL_PATH)/../sensors-log

# shared libraries
shared_libs =
shared_libs += liblog
shared_libs += libsensorslog_system
shared_libs += libprotobuf-cpp-full
shared_libs += libqmi_cci_system
shared_libs += libdiag_system
shared_libs += libcutils

c_export_includes =
c_export_includes += $(gen_sources_dir_system)/proto
c_export_includes += $(LOCAL_PATH)

## Wno-unused-parameter required to ignore some warnings
## coming from google protobuf code
c_flags = -Werror -Wall -Wno-unused-parameter -fexceptions
ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf
endif
# auto-generated protobuf files require this preprocessor macro
cpp_flags = -DGOOGLE_PROTOBUF_NO_RTTI

## C/C++ compiler flags
LOCAL_CFLAGS   = $(c_flags)

ifeq ($(call is-board-platform-in-list, sdm710),true)
    LOCAL_CFLAGS += -DADSPRPC
endif

LOCAL_STATIC_LIBRARIES := libqmi_common
LOCAL_STATIC_LIBRARIES += libqmi_encdec
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_SRC_FILES = $(c_sources)
LOCAL_CPPFLAGS = $(cpp_flags)
LOCAL_C_INCLUDES = $(c_includes)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(c_export_includes)
LOCAL_SHARED_LIBRARIES = $(shared_libs)
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
LOCAL_INSTALLED_MODULE_STEM := libssc.so
endif

include $(BUILD_SHARED_LIBRARY)

# TODO: add export_includes for this library

include $(call all-makefiles-under,$(LOCAL_PATH))
