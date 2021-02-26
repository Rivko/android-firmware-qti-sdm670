LOCAL_PATH := $(call my-dir)

# ------------------------------------------------------------------------------
#       Make the wigig HAL service
# ------------------------------------------------------------------------------
wigighalsvc_cflags := -Wno-unused-parameter
ifdef WIGIG_DRIVER_MODULE_PATH
wigighalsvc_cflags += -DWIGIG_DRIVER_MODULE_PATH=\"$(WIGIG_DRIVER_MODULE_PATH)\"
endif
ifdef WIGIG_DRIVER_MODULE_ARG
wigighalsvc_cflags += -DWIGIG_DRIVER_MODULE_ARG=\"$(WIGIG_DRIVER_MODULE_ARG)\"
endif
ifdef WIGIG_DRIVER_MODULE_NAME
wigighalsvc_cflags += -DWIGIG_DRIVER_MODULE_NAME=\"$(WIGIG_DRIVER_MODULE_NAME)\"
endif

include $(CLEAR_VARS)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/supptunnel
LOCAL_CFLAGS := $(wigighalsvc_cflags)
LOCAL_SRC_FILES := \
    wigighalsvc.cpp \
    supptunnel/SuppTunnelProvider.cpp \
    supptunnel/SuppTunnel.cpp \
    supptunnel/WigigSuppConnector.cpp \
    supptunnel/SuppTunnelCallback.cpp
LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libutils \
    libbase \
    libhwbinder \
    libwpa_client \
    libcrypto \
    libhidlbase \
    libhidltransport \
    vendor.qti.hardware.wigig.supptunnel@1.0
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := wigighalsvc

include $(BUILD_EXECUTABLE)
# ------------------------------------------------------------------------------
#       deploy wigig_supplicant.conf
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE       := wigig_supplicant.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/wifi
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       deploy wigig_p2p_supplicant.conf
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE       := wigig_p2p_supplicant.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/wifi
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
