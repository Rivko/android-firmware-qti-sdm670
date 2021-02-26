ifeq ($(TARGET_USES_LM), true)
PRODUCT_COPY_FILES += \
					  $(foreach file,$(wildcard vendor/qcom/proprietary/android-perf/configs/$(TARGET_BOARD_PLATFORM)/lm/*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/lm/$(notdir $(file)))
endif
PRODUCT_COPY_FILES += \
					  $(foreach file,$(wildcard vendor/qcom/proprietary/android-perf/configs/$(TARGET_BOARD_PLATFORM)/*.conf),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
					  $(foreach file,$(wildcard vendor/qcom/proprietary/android-perf/configs/$(TARGET_BOARD_PLATFORM)/whitelist*.xml),$(file):/system/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
					  $(foreach file,$(wildcard vendor/qcom/proprietary/android-perf/configs/$(TARGET_BOARD_PLATFORM)/app*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
					  $(foreach file,$(wildcard vendor/qcom/proprietary/android-perf/configs/$(TARGET_BOARD_PLATFORM)/perfboosts*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
					  $(foreach file,$(wildcard vendor/qcom/proprietary/android-perf/configs/$(TARGET_BOARD_PLATFORM)/power*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
					  $(foreach file,$(wildcard vendor/qcom/proprietary/android-perf/configs/$(TARGET_BOARD_PLATFORM)/perfmap*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
					  $(foreach file,$(wildcard vendor/qcom/proprietary/android-perf/configs/$(TARGET_BOARD_PLATFORM)/target*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
					  $(foreach file,$(wildcard vendor/qcom/proprietary/android-perf/configs/$(TARGET_BOARD_PLATFORM)/targetresourceconfigs.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
					  $(foreach file,$(wildcard vendor/qcom/proprietary/android-perf/configs/common/commonresourceconfigs.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))
