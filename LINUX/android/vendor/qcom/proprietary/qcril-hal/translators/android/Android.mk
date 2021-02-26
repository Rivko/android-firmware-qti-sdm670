LOCAL_PATH := $(call my-dir)
QCRIL_DIR := ${LOCAL_PATH}/../..

include $(CLEAR_VARS)

LOCAL_CFLAGS               += -Wall -Werror -DRIL_SHLIB $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++14 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++14 $(qcril_cppflags)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src) $(call all-c-files-under, src)
QCRIL_HEADER_DIR           := $(TARGET_OUT_HEADERS)/qcril
FEATURES_HEADER            := qcril_features_def.h

LOCAL_MODULE               := qcrilAndroidTranslators
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_C_INCLUDES           += hardware/ril/include
LOCAL_COPY_HEADERS_TO      := qcril/translators/android
LOCAL_COPY_HEADERS         += $(call all-named-files-under,*.h,../../include/translators/android)
LOCAL_SHARED_LIBRARIES     += libril-qc-hal-qmi
LOCAL_REQUIRED_MODULES     += $(FEATURES_HEADER)
LOCAL_ADDITIONAL_DEPENDENCIES += $(FEATURES_HEADER)

include $(BUILD_STATIC_LIBRARY)
