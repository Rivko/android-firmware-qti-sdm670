LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := qcril.db
LOCAL_MODULE_CLASS := ETC
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/radio/qcril_database
include $(BUILD_SYSTEM)/base_rules.mk

$(LOCAL_BUILT_MODULE): $(LOCAL_PATH)/qcril.sql $(HOST_OUT_EXECUTABLES)/sqlite3
	@rm -f $@
	$(hide) cat $< | $(HOST_OUT_EXECUTABLES)/sqlite3 $@


include $(CLEAR_VARS)

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_PREBUILT_EXECUTABLES := qcril_database_upgrade
LOCAL_MODULE_TAG := debug

include $(BUILD_HOST_PREBUILT)


$(foreach script,$(call all-named-files-under,*.sql,upgrade), \
$(eval include $(CLEAR_VARS)) \
$(eval LOCAL_MODULE := $(script)) \
$(eval LOCAL_PROPRIETARY_MODULE := true) \
$(eval LOCAL_MODULE_OWNER := qti) \
$(eval LOCAL_MODULE_CLASS := SCRIPT) \
$(eval LOCAL_MODULE_TAG := optional) \
$(eval LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/radio/qcril_database) \
$(eval LOCAL_SRC_FILES := $(LOCAL_MODULE)) \
$(eval include $(BUILD_PREBUILT)) \
)

LOCAL_ADDITIONAL_DEPENDENCIES += qcril.db
