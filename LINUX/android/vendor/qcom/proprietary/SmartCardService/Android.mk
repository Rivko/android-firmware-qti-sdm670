ifeq ($(strip $(TARGET_ENABLE_PROPRIETARY_SMARTCARD_SERVICE)),true)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

define all-java-files-oma-only
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.java" -and -not -name ".*" -and -not -name "eSETerminal*"
          -and -not -name "NfcIntentsService.java" -and -not -name "UiccTerminal*") \
 )
endef

define all-java-files-but-uicc-ese-term
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.java" -and -not -name ".*" -and -not -name "UiccTerminal*"
          -and -not -name "eSETerminal*") \
 )
endef


ifeq ($(call is-platform-sdk-version-at-least,26),true)
define uicc-terminal-to-build
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "UiccTerminalO.java" -and -not -name "UiccTerminal.java" \
    )
)
endef
define ese-terminal-to-build
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "eSETerminalO.java" -and -not -name "eSETerminal.java" \
    )
)
endef
else
define uicc-terminal-to-build
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "UiccTerminal.java" -and -not -name "UiccTerminalO.java" \
    )
 )
endef
define ese-terminal-to-build
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "eSETerminal.java" -and -not -name "eSETerminalO.java" \
    )
 )
endef
endif

uicc_term := $(strip $(call uicc-terminal-to-build, src))
ese_term := $(strip $(call ese-terminal-to-build, src))
ifeq ($(uicc_term),)
  $(error No UICC TERMINAL FOUND)
else
  $(info $(uicc_term))
endif

ifeq ($(ese_term),)
  $(info No ESE TERMINAL FOUND)
else
  $(info $(ese_term))
endif

ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
LOCAL_SRC_FILES := $(call all-java-files-but-uicc-ese-term, src)
else
LOCAL_SRC_FILES := $(call all-java-files-oma-only, src)
endif

ifeq ($(strip $(TARGET_USES_QPAY)),true)
LOCAL_SRC_FILES += $(ese_term)
endif

LOCAL_SRC_FILES += $(uicc_term)

LOCAL_AIDL_INCLUDES := $(LOCAL_PATH)/common/src/

LOCAL_PACKAGE_NAME := SmartcardService
LOCAL_CERTIFICATE := platform
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional

LOCAL_JAVA_LIBRARIES := org.simalliance.openmobileapi

ifeq ($(strip $(TARGET_USES_QPAY)),true)
ifeq ($(call is-platform-sdk-version-at-least,26),true)
LOCAL_JAVA_LIBRARIES += com.qualcomm.qti.libesewrapper
LOCAL_JNI_SHARED_LIBRARIES := libeSEWrapperJNI
else
LOCAL_JAVA_LIBRARIES += com.android.qti.qpay
endif
endif

ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
LOCAL_JAVA_LIBRARIES += com.nxp.nfc.nq com.android.nfc_extras
endif

LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))
endif
