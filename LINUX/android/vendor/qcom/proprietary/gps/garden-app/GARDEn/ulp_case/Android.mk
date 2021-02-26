LOCAL_PATH := $(call my-dir)
ifeq ($(BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET),true)
    BUILD_GARDEN_ULP := false
else ifeq ($(LW_FEATURE_SET),true)
    BUILD_GARDEN_ULP := false
endif

ifneq ($(BUILD_GARDEN_ULP),false)

# Build libgardencaseulp
include $(CLEAR_VARS)

LOCAL_MODULE := libgardencaseulp
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    UlpCase.cpp \

LOCAL_CFLAGS += \
    -fno-short-enums

LOCAL_HEADER_LIBRARIES := \
    libgarden_headers \
    libgps.utils_headers \
    libloc_pla_headers \
    liblocation_api_headers \
    liblocationservice_headers \

LOCAL_SHARED_LIBRARIES := \
    libgarden \
    libgps.utils \
    libloc_core \
    liblocation_api \
    liblog \
    liblocationservice \
    libulp2 \

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libgardencaseulp_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
include $(BUILD_HEADER_LIBRARY)

endif #BUILD_GARDEN_ULP
