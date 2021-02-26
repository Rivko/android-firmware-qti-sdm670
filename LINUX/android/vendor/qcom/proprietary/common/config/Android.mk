LOCAL_PATH := $(my-dir)

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO   := common/inc
LOCAL_COPY_HEADERS      := ../inc/armasm.h
LOCAL_COPY_HEADERS      += ../inc/comdef.h
LOCAL_COPY_HEADERS      += ../inc/common_log.h
LOCAL_COPY_HEADERS      += ../inc/customer.h
LOCAL_COPY_HEADERS      += ../inc/msm_ipc.h
LOCAL_COPY_HEADERS      += ../inc/rex.h
LOCAL_COPY_HEADERS      += ../inc/stringl.h
LOCAL_COPY_HEADERS      += ../inc/target.h
LOCAL_COPY_HEADERS      += ../inc/qsocket.h
LOCAL_COPY_HEADERS      += ../inc/qsocket_ipcr.h

include build/core/copy_headers.mk

#----------------------------------------------------------------------
# Copy public.libraries.txt to /system/vendor/etc
#----------------------------------------------------------------------
$(shell mkdir -p $(TARGET_OUT_VENDOR_ETC))
$(shell cat device/qcom/$(TARGET_PRODUCT)/public.libraries.vendor.txt > $(TARGET_OUT_VENDOR_ETC)/public.libraries.txt)
$(shell cat $(QC_PROP_ROOT)/common/config/public.libraries.txt >> $(TARGET_OUT_VENDOR_ETC)/public.libraries.txt)
ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
$(shell echo "libOpenCL.so" >> $(TARGET_OUT_VENDOR_ETC)/public.libraries.txt)
endif
#----------------------------------------------------------------------

#Added to include QuickSettingsService
ifeq ($(TARGET_USES_QSSERVICE), true)
include vendor/qcom/proprietary/qrdplus/Extension/apps/QuickSettingsService/Android.mk
endif