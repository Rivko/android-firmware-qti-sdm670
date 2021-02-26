#default prebuilt
include $(BUILD_PREBUILT)
#generate javalib.jar if DEX_PREOPT is true
ifeq ($(WITH_DEXPREOPT),true)
common_javalib_jar := $(intermediates.COMMON)/javalib.jar
built_dex_intermediate_leaf := with-local
built_dex_intermediate := $(intermediates.COMMON)/$(built_dex_intermediate_leaf)/classes.dex
built_dex := $(intermediates.COMMON)/classes.dex

LOCAL_INTERMEDIATE_TARGETS += \
    $(built_dex_intermediate) \
    $(built_dex)

$(built_dex_intermediate): PRIVATE_INTERMEDIATES_DIR := $(intermediates.COMMON)
$(built_dex_intermediate): PRIVATE_DX_FLAGS := $(LOCAL_DX_FLAGS)


$(built_dex_intermediate): $(common_classes_jar) $(DX)
	$(transform-classes.jar-to-dex)

$(built_dex): $(built_dex_intermediate) | $(ACP)
	@echo Copying: $@
	$(hide) mkdir -p $(dir $@)
	$(hide) rm -f $(dir $@)/classes*.dex
	$(hide) $(ACP) -fp $(dir $<)/classes*.dex $(dir $@)

$(common_javalib_jar): PRIVATE_DEX_FILE := $(built_dex)
$(common_javalib_jar) : $(built_dex) $(java_resource_sources)
	@echo "target Jar: $(PRIVATE_MODULE) ($@)"
	$(create-empty-package)
	$(add-dex-to-package)

$(LOCAL_BUILT_MODULE) : $(common_javalib_jar)
endif
