ifeq ($(strip $(BOARD_HAS_QCOM_WIGIG)),true)
LOCAL_PATH := $(call my-dir)

# Build the java code
# ============================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, java)

LOCAL_STATIC_JAVA_LIBRARIES := wigig_manager vendor.qti.hardware.wigig.supptunnel-V1.0-java-static \
    vendor.qti.hardware.wigig.netperftuner-V1.0-java-static
LOCAL_JAVA_LIBRARIES := services android.hidl.base-V1.0-java  android.hidl.manager-V1.0-java
LOCAL_REQUIRED_MODULES := services
LOCAL_MODULE := wigig-service

include $(BUILD_JAVA_LIBRARY)

endif
