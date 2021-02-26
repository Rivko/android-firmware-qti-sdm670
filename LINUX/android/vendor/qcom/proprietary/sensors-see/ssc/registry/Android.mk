LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#list of sensors filenames
sns_config_files := $(wildcard $(LOCAL_PATH)/config/*.json)
sns_config_files += $(wildcard $(LOCAL_PATH)/config/*.txt)

$(info "adding sensors config files to persist.img  = $(sns_config_files)")
define _add-config-files
 include $$(CLEAR_VARS)
 LOCAL_MODULE := $(notdir $(1))
 LOCAL_MODULE_STEM := $(notdir $(1))
 sns_config_modules += $$(LOCAL_MODULE)
 LOCAL_SRC_FILES := config/$(notdir $(1))
 LOCAL_MODULE_TAGS := optional
 LOCAL_MODULE_CLASS := TEMP
 LOCAL_MODULE_PATH := $$(PRODUCT_OUT)/persist/sensors/registry/config
include $$(BUILD_PREBUILT)
endef

sns_config_modules :=
sns_config :=
$(foreach _conf-file, $(sns_config_files), $(eval $(call _add-config-files,$(_conf-file))))


$(info "adding sensors config files to persist.img  = $(sns_config_files)")
define _add-etc-config-files
 include $$(CLEAR_VARS)
 LOCAL_MODULE := $(notdir $(1))
 LOCAL_MODULE_STEM := $(notdir $(1))
 sns_config_modules += $$(LOCAL_MODULE)
 LOCAL_SRC_FILES := config/$(notdir $(1))
 LOCAL_MODULE_TAGS := optional
 LOCAL_MODULE_CLASS := ETC
 LOCAL_MODULE_PATH := $$(PRODUCT_OUT)/vendor/etc/sensors/config
include $$(BUILD_PREBUILT)
endef

sns_config_modules :=
sns_config :=
$(foreach _conf-file, $(sns_config_files), $(eval $(call _add-etc-config-files,$(_conf-file))))


include $(CLEAR_VARS)
LOCAL_MODULE := sensors_config_module
LOCAL_MODULE_TAGS := optional
LOCAL_REQUIRED_MODULES := $(sns_config_modules)
include $(BUILD_PHONY_PACKAGE)

include $(CLEAR_VARS)
LOCAL_MODULE := sensors_registry
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/persist/sensors/registry/registry
LOCAL_SRC_FILES := registry/$(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := sensors_settings
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/persist/sensors
LOCAL_SRC_FILES := registry/$(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := sns_reg_config
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/persist/sensors/registry
LOCAL_SRC_FILES := registry/$(LOCAL_MODULE)
include $(BUILD_PREBUILT)
