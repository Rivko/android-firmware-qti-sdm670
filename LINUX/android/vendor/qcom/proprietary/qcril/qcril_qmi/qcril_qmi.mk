# LOCAL_PATH and the include path need to be hard-coded because wmsts is inside
# the qcril directory (bug in the Android makefile system).
# LOCAL_PATH reflects parent directory to ensure common objects are in
# separate pools for each RIL variant.


ril_header = "hardware/ril/include/telephony/ril.h"
ril_version := $(shell cat $(ril_header) | grep '\#define[[:space:]]\+\<RIL_VERSION\>' | cut -d' ' -f3 )
$(info ril_version: ${ril_version})
ril_v12 := $(shell [ "${ril_version}" -ge 12 ] && echo true )

$(info Is v12 $(ril_v12))
ifneq ($(filter address,$(SANITIZE_TARGET)),)
$(shell mkdir -p $(TARGET_OUT_HEADERS)/qcril/)
$(shell cp $(PRODUCT_OUT)/obj/include/qcril/qcril_features_def.h $(TARGET_OUT_HEADERS)/qcril/)
endif

LOCAL_PATH := $(QCRIL_DIR)
QCRIL_PARSER_PATH := $(QCRIL_DIR)/qcril_qmi/qcril_header_parser.py
PWD:=$(shell pwd)

ifeq ($(filter address,$(SANITIZE_TARGET)),)
include $(CLEAR_VARS)
QCRIL_HEADER_DIR := $(TARGET_OUT_HEADERS)/qcril
LOCAL_MODULE := qcril_features_def.h
LOCAL_MODULE_CLASS := ETC
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_PATH := $(QCRIL_HEADER_DIR)
include $(BUILD_SYSTEM)/base_rules.mk

FEATURES_HEADER := qcril_features_def.h

$(info LOCAL_BUILT_MODULE: $(LOCAL_BUILT_MODULE))
$(info LOCAL_INSTALLED_MODULE: $(LOCAL_INSTALLED_MODULE))
$(LOCAL_BUILT_MODULE): hardware/ril/include/telephony/ril.h $(QCRIL_PARSER_PATH)
	python $(QCRIL_PARSER_PATH) $(PWD) $(dir $@)
endif

include $(CLEAR_VARS)

LOCAL_MODULE:= librilqmiservices
LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES += qcril_qmi/services/qmi_embms_v01.c
LOCAL_SRC_FILES += qcril_qmi/services/qtuner_v01.c
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_SHARED_LIBRARIES += libqmi_common_so
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

##
## START build_qcril
##
define build_qcril
LOCAL_CPP_EXTENSION := .cc
LOCAL_CFLAGS += -Wall -Werror
LOCAL_CPPFLAGS += -std=c++14

LOCAL_SRC_FILES := qcril_qmi/qcril.c
LOCAL_SRC_FILES += qcril_qmi/qcril_pd_notifier.c
LOCAL_SRC_FILES += qcril_qmi/qcril_arb.c
LOCAL_SRC_FILES += qcril_qmi/qcril_cm_ss.c
LOCAL_SRC_FILES += qcril_qmi/qcril_cm_util.c
LOCAL_SRC_FILES += qcril_qmi/qcril_event.c
LOCAL_SRC_FILES += qcril_qmi/qcril_log.c
LOCAL_SRC_FILES += qcril_qmi/qcril_other.c
LOCAL_SRC_FILES += qcril_qmi/qcril_pbm.c
LOCAL_SRC_FILES += qcril_qmi/qcril_reqlist.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_voice.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_client.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_nas.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_prov.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_nas2.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_sms.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_sms_errors.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_ims.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_imss.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_imss_v02.c
LOCAL_SRC_FILES += qcril_qmi/qcril_db.c
LOCAL_SRC_FILES += qcril_qmi/qmi_ril_peripheral_mng.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_audio_service.cc
LOCAL_SRC_FILES += qcril_qmi/qcril_am.cc
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_pdc.c
LOCAL_SRC_FILES += qcril_qmi/qmi_ril_platform_dep.c
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_mbn_file.c
LOCAL_SRC_FILES += qcril_qmi/qcril_dir_utils.c
LOCAL_SRC_FILES += qcril_qmi/qcril_file_utils.c
LOCAL_SRC_FILES += qcril_qmi/qcril_mbn_db.c
LOCAL_SRC_FILES += qcril_qmi/qcril_mbn_hw_update.c
LOCAL_SRC_FILES += qcril_qmi/qcril_mbn_sw_update.c
LOCAL_SRC_FILES += qcril_qmi/qcril_mbn_meta.c
LOCAL_SRC_FILES += qcril_qmi/qmi_ril_file_observer.cc
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_mbn_ota_daemon.c
LOCAL_SRC_FILES += qcril_qmi/qcril_mbn_ota_file_list.c
LOCAL_SRC_FILES += qcril_qmi/mbn_utils.c
LOCAL_SRC_FILES += qcril_qmi/pdc_utils.c

LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_imsa.c

LOCAL_SRC_FILES += qcril_qmi/ims_socket/qcril_qmi_ims_packing.c
LOCAL_SRC_FILES += qcril_qmi/ims_socket/qcril_qmi_ims_misc.c
LOCAL_SRC_FILES += qcril_qmi/ims_socket/imsIF.pb.c
LOCAL_SRC_FILES += qcril_qmi/ims_socket/qcril_qmi_ims_msg_meta.c
LOCAL_SRC_FILES += qcril_qmi/ims_socket/qcril_qmi_ims_flow_control.c
LOCAL_SRC_FILES += qcril_qmi/ims_socket/qcril_qmi_ims_radio_service.cc
LOCAL_SRC_FILES += qcril_qmi/ims_socket/qcril_qmi_ims_radio_utils.cc
LOCAL_SRC_FILES += qcril_qmi/nanopb_utils/qcril_qmi_npb_encode.c
LOCAL_SRC_FILES += qcril_qmi/nanopb_utils/qcril_qmi_npb_decode.c
LOCAL_SRC_FILES += qcril_qmi/nanopb_utils/qcril_qmi_npb_utils.c
LOCAL_SRC_FILES += qcril_qmi/oem_socket/qcril_qmi_oemhook_agent.cc
LOCAL_SRC_FILES += qcril_qmi/oem_socket/qcril_qmi_oem_packing.c
LOCAL_SRC_FILES += qcril_qmi/oem_socket/qcril_qmi_oem_msg_meta.c
LOCAL_SRC_FILES += qcril_qmi/oem_socket/qcril_qmi_oem_misc.c
LOCAL_SRC_FILES += qcril_qmi/oem_socket/qtiTelephonyService.pb.c
LOCAL_SRC_FILES += qcril_qmi/oem_socket/qcril_qmi_oemhook_service.cc
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_generic_socket.cc
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_cell_info.c
LOCAL_SRC_FILES += qcril_qmi/qti_radio/qcril_qmi_qti_radio_service.cc

ifeq ($(ril_v12),true)
LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_cell_info_v12.c
endif
LOCAL_SRC_FILES += qcril_qmi/radio_config/qcril_qmi_radio_config_socket.cc
LOCAL_SRC_FILES += qcril_qmi/radio_config/qcril_qmi_radio_config_imss.c
LOCAL_SRC_FILES += qcril_qmi/radio_config/qcril_qmi_radio_config_imss_v02.c
LOCAL_SRC_FILES += qcril_qmi/radio_config/qcril_qmi_radio_config_data.c
LOCAL_SRC_FILES += qcril_qmi/radio_config/qcril_qmi_radio_config_meta.c
LOCAL_SRC_FILES += qcril_qmi/radio_config/qcril_qmi_radio_config_meta_table.c
LOCAL_SRC_FILES += qcril_qmi/radio_config/qcril_qmi_radio_config_meta_table_v02.c
LOCAL_SRC_FILES += qcril_qmi/radio_config/qcril_qmi_radio_config_dispatch_helper.c
LOCAL_SRC_FILES += qcril_qmi/radio_config/qcril_qmi_radio_config_packing.c
LOCAL_SRC_FILES += qcril_qmi/radio_config/qcril_qmi_radio_config_misc.c

LOCAL_SRC_FILES += qcril_qmi/qcril_qmi_pil_monitor.cc
LOCAL_SRC_FILES += common/uim/qcril_gstk_qmi.c
LOCAL_SRC_FILES += common/uim/qcril_uim.c
LOCAL_SRC_FILES += common/uim/qcril_uim_card.c
LOCAL_SRC_FILES += common/uim/qcril_uim_file.c
LOCAL_SRC_FILES += common/uim/qcril_uim_security.c
LOCAL_SRC_FILES += common/uim/qcril_uim_util.c
LOCAL_SRC_FILES += common/uim/qcril_uim_queue.c
LOCAL_SRC_FILES += common/uim/qcril_uim_refresh.c
LOCAL_SRC_FILES += common/uim/qcril_uim_restart.c
LOCAL_SRC_FILES += common/uim/qcril_uim_slot_mapping.c
LOCAL_SRC_FILES += common/uim/qcril_scws.c
LOCAL_SRC_FILES += common/uim/qcril_scws_opt.c
LOCAL_SRC_FILES += common/uim/qcril_uim_qcci.c
LOCAL_SRC_FILES += common/uim/qcril_uim_sap.c
LOCAL_SRC_FILES += common/uim/qcril_uim_sap_atel.c
LOCAL_SRC_FILES += common/uim/qcril_uim_gba.c
LOCAL_SRC_FILES += common/uim/qcril_uim_remote.c
LOCAL_SRC_FILES += common/uim/qcril_uim_lpa.c
LOCAL_SRC_FILES += common/uim/qcril_uim_http.c
LOCAL_SRC_FILES += common/uim/uim_remote_client_service.cc
LOCAL_SRC_FILES += common/uim/uim_remote_server_service.cc
LOCAL_SRC_FILES += common/uim/uim_lpa_service.cc
LOCAL_SRC_FILES += common/uim/uim_service.cc

LOCAL_SRC_FILES += qcril_qmi/settings.cc

LOCAL_SRC_FILES += qcril_qmi/lte_direct/qcril_qmi_lte_direct_disc_packing.c
LOCAL_SRC_FILES += qcril_qmi/lte_direct/qcril_qmi_lte_direct_disc_msg_meta.c
LOCAL_SRC_FILES += qcril_qmi/lte_direct/qcril_qmi_lte_direct_disc_misc.c
LOCAL_SRC_FILES += qcril_qmi/lte_direct/qcril_qmi_lte_direct_disc.c
LOCAL_SRC_FILES += qcril_qmi/lte_direct/qcril_qmi_lte_direct_disc_dispatcher.c

LOCAL_SRC_FILES += qcril_qmi/utilities/timer/util_timer.c
LOCAL_SRC_FILES += qcril_qmi/utilities/ref_counted.c
LOCAL_SRC_FILES += qcril_qmi/utilities/simple_list.c
# for asprinf
LOCAL_CFLAGS += -D_GNU_SOURCE

# for embms
LOCAL_CFLAGS += -DFEATURE_DATA_EMBMS

# for LCE(LQE)
LOCAL_CFLAGS += -DFEATURE_DATA_LQE

ifdef FEATURE_QCRIL_UIM_SAP_SERVER_MODE
LOCAL_CFLAGS += -DFEATURE_QCRIL_UIM_SAP_SERVER_MODE
endif

LOCAL_SRC_FILES += common/data/qcril_data_netctrl.c
LOCAL_SRC_FILES += common/data/qcril_data_req_resp.c
LOCAL_SRC_FILES += common/data/qcril_data_qos.c
LOCAL_SRC_FILES += common/data/qcril_data_embms.c
LOCAL_SRC_FILES += common/data/qcril_data_client.c
LOCAL_SRC_FILES += common/data/qcril_data_utils.c
LOCAL_SRC_FILES += common/data/qcril_data_qmi_wds.c
LOCAL_SRC_FILES += common/data/qcril_data_lqe.c
LOCAL_CFLAGS += -DFEATURE_QCRIL_USE_NETCTRL

ifndef NANOPB_LIBRARY_NAME
  NANOPB_LIBRARY_NAME := libprotobuf-c-nano-enable_malloc
endif

LOCAL_STATIC_LIBRARIES := ${NANOPB_LIBRARY_NAME}

LOCAL_SHARED_LIBRARIES += libdsutils  # must preceed libcutils in ICS build
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libril
LOCAL_SHARED_LIBRARIES += librilutils
LOCAL_SHARED_LIBRARIES += libril-qc-radioconfig
LOCAL_SHARED_LIBRARIES += libril-qc-ltedirectdisc

LOCAL_SHARED_LIBRARIES += libril-qcril-hook-oem
LOCAL_SHARED_LIBRARIES += libqmi_cci
LOCAL_SHARED_LIBRARIES += libqmi
LOCAL_SHARED_LIBRARIES += libqmi_client_qmux
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libhardware_legacy
LOCAL_SHARED_LIBRARIES += libqmiservices
LOCAL_SHARED_LIBRARIES += librilqmiservices
LOCAL_SHARED_LIBRARIES += libqmi_client_helper
LOCAL_SHARED_LIBRARIES += libidl
LOCAL_SHARED_LIBRARIES += libtime_genoff
LOCAL_SHARED_LIBRARIES += libsqlite
LOCAL_SHARED_LIBRARIES += libmdmdetect
LOCAL_SHARED_LIBRARIES += libperipheral_client
LOCAL_SHARED_LIBRARIES += libsettings
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libhidlbase
LOCAL_SHARED_LIBRARIES += libhidltransport
LOCAL_SHARED_LIBRARIES += libhwbinder
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.ims@1.0_vendor
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.qcrilhook@1.0_vendor
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.am@1.0_vendor
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.qtiradio@1.0_vendor
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.uim_remote_server@1.0_vendor
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.uim_remote_client@1.0_vendor
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.lpa@1.0_vendor
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.uim@1.0_vendor
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.uim@1.1_vendor

ifneq ($(SYS_HEALTH_MON_STATUS), false)
LOCAL_SHARED_LIBRARIES += libsystem_health_mon
endif

ifeq ($(TARGET_PD_SERVICE_ENABLED), true)
    LOCAL_SHARED_LIBRARIES += libpdmapper
    LOCAL_SHARED_LIBRARIES += libpdnotifier
    LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libpdnotifier/inc
    LOCAL_CFLAGS += -DQCRIL_USE_PD_NOTIFIER
endif

# Assume by default that libxml2 is available
# This prevents touching existing device
# config files that already support #
# libxml2. Device files not supporting it,
# would have to explicitly define it, but it should be clear when it is
# required (compilation will fail)
ifneq (${LIBXML_SUPPORTED},false)
LOCAL_SHARED_LIBRARIES += libxml2
else
LOCAL_CFLAGS += -DLIBXML_UNSUPPORTED
endif

# These targets have route look up available on modem
LOCAL_SHARED_LIBRARIES += libdsi_netctrl
LOCAL_SHARED_LIBRARIES += libqdp
LOCAL_SHARED_LIBRARIES += liblqe

LOCAL_C_INCLUDES += $(QCRIL_HEADER_DIR)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcril_qmi/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcril_qmi/nanopb_utils/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcril_qmi/ims_socket/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcril_qmi/oem_socket/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcril_qmi/radio_config/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcril_qmi/services/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcril_qmi/utilities/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcril_qmi/utilities/log
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcril_qmi/utilities/timer
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcril_qmi/lte_direct/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcril_qmi/qti_radio/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/settingsd
LOCAL_C_INCLUDES += bionic/libc/include/
LOCAL_C_INCLUDES += external/sqlite/dist/
LOCAL_C_INCLUDES += external/nanopb-c/
ifneq (${LIBXML_SUPPORTED},false)
LOCAL_C_INCLUDES += external/libxml2/include/
endif
LOCAL_C_INCLUDES += external/icu/icu4c/source/common
LOCAL_C_INCLUDES += hardware/ril/include/telephony/

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/auth/inc/

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/common/data/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/common/uim/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/qcrilhook_oem/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/data/inc/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libmdmdetect/inc/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libperipheralclient/inc/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcril/
ifneq ($(SYS_HEALTH_MON_STATUS), false)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/system_health_monitor/inc/
endif
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/time-services/
ifdef FEATURE_QCRIL_UIM_SAP_SERVER_MODE
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/librilutils
endif
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libpdmapper/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libpdnotifier/inc

# For API Definitions and enables
LOCAL_CFLAGS   += $(remote_api_defines)
LOCAL_CFLAGS   += $(remote_api_enables)

# defines necessary for QCRIL code
LOCAL_CFLAGS += -DRIL_SHLIB
LOCAL_CFLAGS += -DFEATURE_MMGSDI_GSM
LOCAL_CFLAGS += -DFEATURE_AUTH
LOCAL_CFLAGS += -DPACKED=

# Define for UIM files using QMI
LOCAL_CFLAGS += -DFEATURE_QCRIL_UIM_QMI
LOCAL_CFLAGS += -DFEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
LOCAL_CFLAGS += -DFEATURE_QCRIL_QMI_CAT
LOCAL_CFLAGS += -DFEATURE_QCRIL_UIM_ISIM_REFRESH

ifdef FEATURE_QCRIL_TOOLKIT_SKIP_SETUP_EVT_LIST_FILTER
LOCAL_CFLAGS += -DFEATURE_QCRIL_TOOLKIT_SKIP_SETUP_EVT_LIST_FILTER
endif

#ifdef MANUAL_PROVISIONING_NOT_SUPPORTED
LOCAL_CFLAGS += -DQCRIL_CONTROLLED_SUBSCRIPTION_PROVISIONING
#endif

ifneq ($(SYS_HEALTH_MON_STATUS), false)
LOCAL_CFLAGS += -DFEATURE_QCRIL_SHM
endif

ifeq ($(call is-board-platform,msm8960),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8960
endif

ifeq ($(call is-board-platform,msm7627a),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_7627A
endif

ifeq ($(call is-board-platform,msm8974),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8974
endif

ifeq ($(call is-board-platform,msm8610),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8610
endif

ifeq ($(call is-board-platform,msm8916),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8916
endif

ifeq ($(call is-board-platform,msm8909),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8909
endif

ifeq ($(call is-board-platform,msm8994),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8994
endif

ifeq ($(call is-board-platform,msm8996),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8996
endif

ifeq ($(call is-board-platform,msm8992),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8992
endif

ifeq ($(call is-board-platform,msm8952),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8952
endif

ifeq ($(call is-board-platform,msm8937),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8937
endif

ifeq ($(call is-board-platform,msm8953),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8953
endif

ifeq ($(call is-board-platform,msm8998),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8998
endif

ifeq ($(call is-board-platform,apq8084),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8084
endif

ifeq ($(call is-board-platform,msm8226),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_8226
endif

ifneq (,$(findstring sdm, $(TARGET_BOARD_PLATFORM)))
  LOCAL_CFLAGS += -DFEATURE_QCRIL_SDM
endif

ifeq ($(call is-chipset-in-board-platform,msm7630),true)
  LOCAL_CFLAGS += -DFEATURE_QCRIL_LTE
  LOCAL_CFLAGS += -DFEATURE_QCRIL_IMS
endif

LOCAL_CFLAGS += -DFEATURE_QCRIL_HDR_RELB
LOCAL_CFLAGS += -DFEATURE_QCRIL_NCELL

# defines common to all shared libraries
LOCAL_CFLAGS += \
  -DLOG_NDDEBUG=0 \
  -DIMAGE_APPS_PROC \
  -DFEATURE_Q_SINGLE_LINK \
  -DFEATURE_Q_NO_SELF_QPTR \
  -DFEATURE_NATIVELINUX \
  -DFEATURE_DSM_DUP_ITEMS \

# compiler options
LOCAL_CFLAGS += -g
LOCAL_CFLAGS += -O0
LOCAL_CFLAGS += -fno-inline
LOCAL_CFLAGS += -fno-short-enums

LOCAL_MODULE_TAGS := debug

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true

LOCAL_REQUIRED_MODULES += qcril_features_def.h
LOCAL_ADDITIONAL_DEPENDENCIES += qcril_features_def.h
LOCAL_REQUIRED_MODULES += qcril.db
LOCAL_ADDITIONAL_DEPENDENCIES += qcril.db

include $(BUILD_SHARED_LIBRARY)
endef
##
## END build_qcril
##

LOCAL_CLANG := true
ifndef QCRIL_DSDA_INSTANCE
   LOCAL_MODULE:= libril-qc-qmi-1
else
   LOCAL_CFLAGS += -DFEATURE_DSDA_RIL_INSTANCE=$(QCRIL_DSDA_INSTANCE)
   LOCAL_MODULE:= libril-qc-qmi-$(QCRIL_DSDA_INSTANCE)
endif

$(eval $(build_qcril))
