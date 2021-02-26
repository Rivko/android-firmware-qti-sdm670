LOCAL_PATH:= $(call my-dir)

ifeq ($(TARGET_USES_IOPHAL),true)
    include $(CLEAR_VARS)
    LOCAL_MODULE_RELATIVE_PATH := hw
    LOCAL_PROPRIETARY_MODULE := true
    LOCAL_MODULE := vendor.qti.hardware.iop@2.0-service
    LOCAL_INIT_RC := vendor.qti.hardware.iop@2.0-service.rc

    LOCAL_SRC_FILES := \
            service.cpp

    LOCAL_SHARED_LIBRARIES := \
            liblog \
            libcutils \
            libdl \
            libbase \
            libutils \
            libhardware_legacy \
            libhardware \
            libhwbinder \
            libhidlbase \
            libhidltransport \
            vendor.qti.hardware.iop@2.0 \

    include $(BUILD_EXECUTABLE)
endif
