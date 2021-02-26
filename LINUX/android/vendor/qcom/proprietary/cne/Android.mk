CNE_PATH := $(call my-dir)

ifeq ($(BOARD_USES_QCNE),true)
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-platform-sdk-version-at-least,18),true)

ifneq ($(TARGET_PRODUCT),full)
  include $(call first-makefiles-under, $(CNE_PATH))
endif

endif
endif
endif
endif
