ifneq ($(BUILD_TINY_ANDROID),true)

ROOT_DIR := $(call my-dir)
LOCAL_PATH := $(ROOT_DIR)
include $(CLEAR_VARS)


# ------------------------------------------------------------------------------
#       FORM_FACTOR mtp
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE:= form_factor_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Mixer profiles
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE:= mixer_paths_mtp.xml
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/mixer
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Tester daemon configuration files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_tester_epos_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/tester/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Tester daemon transparent data files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_tester_epos_mtp_tx_transparent_data.bin
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/tester/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       EPOS daemon configuration files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_epos_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/epos/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_epos_mtp_ps_disabled.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/epos/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Power save standy mode tuning parameters (EPOS) file
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= ps_tuning1_standby_mtp.bin
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := ps_tuning1_standby.bin
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/epos/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Power save idle mode tuning parameters (EPOS) file
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= ps_tuning1_idle_mtp.bin
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := ps_tuning1_idle.bin
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/epos/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       P2P daemon configuration files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_p2p_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/p2p/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Hovering daemon configuration files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_hovering_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/hovering/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# 2 speakers
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_hovering_mtp_2_speaker.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/hovering/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Gesture daemon configuration files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_gesture_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/gesture/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Sync Gesture daemon configuration files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_sync_gesture_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/sync_gesture/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_sync_gesture_apps_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/sync_gesture/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_sync_gesture_lpass_rec_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/sync_gesture/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_sync_gesture_fw_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/sync_gesture/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_sync_gesture_fw_apps_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/sync_gesture/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_sync_gesture_apps_0bom_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/sync_gesture/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Sync Gesture daemon transparent data binary files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_sync_gesture_mtp_tx_transparent_data.bin
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/sync_gesture/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_sync_gesture_mtp_algo_transparent_data.bin
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/sync_gesture/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)


# ------------------------------------------------------------------------------
#       Proximity daemon transparent data binary files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_proximity_mtp_algo_transparent_data.bin
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/proximity/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_ranging_mtp_algo_transparent_data.bin
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/proximity/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Proximity daemon configuration files
# ------------------------------------------------------------------------------
# mtp digital

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_proximity_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/proximity/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_proximity_apps_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/proximity/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_proximity_mtp_debug.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/proximity/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_proximity_recording_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/proximity/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_ranging_apps_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/proximity/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Proximity daemon transparent data files
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_proximity_mtp_rx_transparent_data.bin
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/proximity/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_proximity_mtp_tx_transparent_data.bin
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/proximity/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       Pairing daemon configuration files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_pairing_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/pairing/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       SW Calib daemon configuration files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= usf_sw_calib_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/sw_calib/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= usf_sw_calib_tester_mtp.cfg
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/sw_calib/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
#       DigitalPen Service configuration files
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE:= service_settings_mtp.xml
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/epos/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= service_settings_mtp_folio.xml
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/epos/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE:= service_settings_mtp_1_to_1.xml
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/usf/epos/cfg_mtp
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

# Build all of the sub-targets
include $(call all-makefiles-under, $(LOCAL_PATH))

endif #BUILD_TINY_ANDROID

