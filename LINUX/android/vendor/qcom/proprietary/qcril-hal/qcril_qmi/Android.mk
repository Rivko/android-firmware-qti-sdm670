# LOCAL_PATH and the include path need to be hard-coded because wmsts is inside
# the qcril directory (bug in the Android makefile system).
# LOCAL_PATH reflects parent directory to ensure common objects are in
# separate pools for each RIL variant.


ril_header = "hardware/ril/include/telephony/ril.h"
ril_version := $(shell cat $(ril_header) | grep '\#define[[:space:]]\+\<RIL_VERSION\>' | cut -d' ' -f3 )
$(info ril_version: ${ril_version})
ril_v12 := $(shell [ "${ril_version}" -ge 12 ] && echo true )

$(info Is v12 $(ril_v12))

LOCAL_PATH := $(call my-dir)
QCRIL_HEADER_DIR := $(TARGET_OUT_HEADERS)/qcril
PWD:=$(shell pwd)


include $(CLEAR_VARS)

##
## START build_qcril
##
LOCAL_CPP_EXTENSION := .cc
LOCAL_CFLAGS += -Wall -Werror $(qcril_cflags)
LOCAL_CPPFLAGS += -std=c++14 $(qcril_cppflags)
LOCAL_LDFLAGS += $(qcril_ldflags)

LOCAL_SRC_FILES += $(call all-c-files-under, .)
#LOCAL_SRC_FILES += $(call all-cpp-files-under, .)
LOCAL_SRC_FILES += $(call all-named-files-under,*.cc,.)
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/oem_socket
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/radio_config
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/services
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/utilities
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/utilities/timer
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/qcril
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_EXPORT_C_INCLUDE_DIRS += external/nanopb-c

###->LOCAL_SRC_FILES += ims_socket/qcril_qmi_ims_packing.c
###->LOCAL_SRC_FILES += ims_socket/qcril_qmi_ims_misc.c
###->LOCAL_SRC_FILES += ims_socket/imsIF.pb.c
###->LOCAL_SRC_FILES += ims_socket/qcril_qmi_ims_msg_meta.c
###->LOCAL_SRC_FILES += ims_socket/qcril_qmi_ims_flow_control.c
###->LOCAL_SRC_FILES += nanopb_utils/qcril_qmi_npb_encode.c
###->LOCAL_SRC_FILES += nanopb_utils/qcril_qmi_npb_decode.c
###->LOCAL_SRC_FILES += nanopb_utils/qcril_qmi_npb_utils.c
###->LOCAL_SRC_FILES += oem_socket/qcril_qmi_oem_socket.cc
###->LOCAL_SRC_FILES += oem_socket/qcril_qmi_oem_packing.c
###->LOCAL_SRC_FILES += oem_socket/qcril_qmi_oem_msg_meta.c
###->LOCAL_SRC_FILES += oem_socket/qcril_qmi_oem_misc.c
###->LOCAL_SRC_FILES += oem_socket/qtiTelephonyService.pb.c
###->LOCAL_SRC_FILES += qcril_qmi_generic_socket.cc
###->LOCAL_SRC_FILES += qcril_qmi_cell_info.c
###->ifeq ($(ril_v12),true)
###->LOCAL_SRC_FILES += qcril_qmi_cell_info_v12.c
###->endif
###->
###->LOCAL_SRC_FILES += radio_config/qcril_qmi_radio_config_socket.cc
###->LOCAL_SRC_FILES += radio_config/qcril_qmi_radio_config_imss.c
###->LOCAL_SRC_FILES += radio_config/qcril_qmi_radio_config_imss_v02.c
###->LOCAL_SRC_FILES += radio_config/qcril_qmi_radio_config_data.c
###->LOCAL_SRC_FILES += radio_config/qcril_qmi_radio_config_meta.c
###->LOCAL_SRC_FILES += radio_config/qcril_qmi_radio_config_meta_table.c
###->LOCAL_SRC_FILES += radio_config/qcril_qmi_radio_config_meta_table_v02.c
###->LOCAL_SRC_FILES += radio_config/qcril_qmi_radio_config_dispatch_helper.c
###->LOCAL_SRC_FILES += radio_config/qcril_qmi_radio_config_packing.c
###->LOCAL_SRC_FILES += radio_config/qcril_qmi_radio_config_misc.c
###->
###->LOCAL_SRC_FILES += utilities/timer/util_timer.c
###->LOCAL_SRC_FILES += utilities/ref_counted.c
###->
###->LOCAL_SRC_FILES += lte_direct/qcril_qmi_lte_direct_disc_packing.c
###->LOCAL_SRC_FILES += lte_direct/qcril_qmi_lte_direct_disc_msg_meta.c
###->LOCAL_SRC_FILES += lte_direct/qcril_qmi_lte_direct_disc_misc.c
###->LOCAL_SRC_FILES += lte_direct/qcril_qmi_lte_direct_disc.c
###->LOCAL_SRC_FILES += lte_direct/qcril_qmi_lte_direct_disc_dispatcher.c

# for asprinf
LOCAL_CFLAGS += -D_GNU_SOURCE

# for embms
LOCAL_CFLAGS += -DFEATURE_DATA_EMBMS

# for LCE(LQE)
LOCAL_CFLAGS += -DFEATURE_DATA_LQE

###->ifeq ($(call is-board-platform-in-list,msm7630_surf msm7630_fusion msm8660 msm8960 msm8974 msm8610 apq8084 msm8226 msm8916 msm8994 msm8996 msm8992 msm8909 msm8952 msm8937 msm8953 msm7627_surf msm7627a msm8998),true)
###-># These targets have route look up available on modem
###->  LOCAL_SRC_FILES += ../common/data/qcril_data_netctrl.c
###->  LOCAL_SRC_FILES += ../common/data/qcril_data_req_resp.c
###->  LOCAL_SRC_FILES += ../common/data/qcril_data_qos.c
###->  LOCAL_SRC_FILES += ../common/data/qcril_data_embms.c
###->  LOCAL_SRC_FILES += ../common/data/qcril_data_client.c
###->  LOCAL_SRC_FILES += ../common/data/qcril_data_utils.c
###->  LOCAL_SRC_FILES += ../common/data/qcril_data_qmi_wds.c
###->  LOCAL_SRC_FILES += ../common/data/qcril_data_lqe.c
###->  LOCAL_CFLAGS += -DFEATURE_QCRIL_USE_NETCTRL
###->else
###-># These targets do not have route look up available on modem
###->  LOCAL_SRC_FILES += ../common/data/qcril_data.c
###->  LOCAL_SRC_FILES += ../common/data/qcril_datai.c
###->endif

LOCAL_WHOLE_STATIC_LIBRARIES += qcrilAndroidModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilAndroidTranslators
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilVoiceModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilLpaModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNasModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilSmsModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilQmiModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilCellInfoModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilUimModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilImsModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilPbmModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilDataModule
#LOCAL_WHOLE_STATIC_LIBRARIES += qcrilAndroidAudioModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilMbnModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilSapModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilUimRemoteClientModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilQtiRadio

LOCAL_SHARED_LIBRARIES	   += libhidlbase
LOCAL_SHARED_LIBRARIES	   += libhidltransport
LOCAL_SHARED_LIBRARIES	   += libhwbinder
LOCAL_SHARED_LIBRARIES	   += vendor.qti.hardware.radio.ims@1.0_vendor
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.am@1.0_vendor
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qcrilhook@1.0_vendor
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim@1.0_vendor
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim@1.1_vendor
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim_remote_server@1.0_vendor
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim_remote_client@1.0_vendor
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.lpa@1.0_vendor
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qtiradio@1.0_vendor
LOCAL_SHARED_LIBRARIES += libqcrilFramework

LOCAL_SHARED_LIBRARIES += libdsutils  # must preceed libcutils in ICS build
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog
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
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_SHARED_LIBRARIES += libmdmdetect
LOCAL_SHARED_LIBRARIES += libperipheral_client
LOCAL_SHARED_LIBRARIES += libsettings
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

$(info is-board-platform-in-list=$is-board-platform-in-list)
# These targets have route look up available on modem
  LOCAL_SHARED_LIBRARIES += libdsi_netctrl
  LOCAL_SHARED_LIBRARIES += libqdp
  LOCAL_SHARED_LIBRARIES += liblqe

LOCAL_C_INCLUDES += $(QCRIL_HEADER_DIR)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/nanopb_utils/
# IMS REFACTOR: TODO: export only required files
LOCAL_C_INCLUDES += $(LOCAL_PATH)/oem_socket/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/radio_config/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/services/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utilities/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utilities/control
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utilities/list
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utilities/log
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utilities/memory
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utilities/bit_field
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utilities/synchronization
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utilities/timer
LOCAL_C_INCLUDES += $(LOCAL_PATH)/core/core/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/cri/core/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/cri/csvt/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/cri/data/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/cri/dms/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/cri/nas/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/cri/voice/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/cri/wms/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/hlos/core/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/hlos/csvt/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/lte_direct/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcril/settingsd
LOCAL_C_INCLUDES += external/sqlite/dist/
LOCAL_C_INCLUDES += external/nanopb-c/
ifneq (${LIBXML_SUPPORTED},false)
LOCAL_C_INCLUDES += external/libxml2/include/
endif
LOCAL_C_INCLUDES += external/icu/icu4c/source/common
LOCAL_C_INCLUDES += hardware/ril/include/telephony/

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/auth/inc/

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../common/data/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcril/qcrilhook_oem/
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
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libpdmapper/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libpdnotifier/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcril/modules/nas

# For API Definitions and enables
LOCAL_CFLAGS   += $(remote_api_defines)
LOCAL_CFLAGS   += $(remote_api_enables)

# defines necessary for QCRIL code
LOCAL_CFLAGS += -DRIL_SHLIB
LOCAL_CFLAGS += -DFEATURE_MMGSDI_GSM
LOCAL_CFLAGS += -DFEATURE_AUTH
LOCAL_CFLAGS += -DPACKED=

ifdef FEATURE_QCRIL_TOOLKIT_SKIP_SETUP_EVT_LIST_FILTER
LOCAL_CFLAGS += -DFEATURE_QCRIL_TOOLKIT_SKIP_SETUP_EVT_LIST_FILTER
endif

ifneq ($(SYS_HEALTH_MON_STATUS), false)
LOCAL_CFLAGS += -DFEATURE_QCRIL_SHM
endif
ifneq ($(TARGET_HAS_LOW_RAM), true)
LOCAL_CFLAGS += -DFEATURE_QCRIL_LTE_DIRECT
LOCAL_CFLAGS += -DFEATURE_QCRIL_OEMHOOK_IMS_PRESENCE
LOCAL_CFLAGS += -DFEATURE_QCRIL_OEMHOOK_IMS_VT
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

LOCAL_REQUIRED_MODULES     += qcril_features_def.h
LOCAL_ADDITIONAL_DEPENDENCIES += qcril_features_def.h
LOCAL_REQUIRED_MODULES     += qcril.db
LOCAL_ADDITIONAL_DEPENDENCIES += qcril.db

LOCAL_COPY_HEADERS_TO      := qcril/framework
LOCAL_COPY_HEADERS         += qcrili.h
LOCAL_COPY_HEADERS         += oem_socket/qcril_qmi_oem_events.h
LOCAL_COPY_HEADERS         += oem_socket/qcril_qmi_oem_eventlist.h
LOCAL_COPY_HEADERS         += qcril_eventlist.h
LOCAL_COPY_HEADERS         += qcril_am.h
LOCAL_COPY_HEADERS         += qcrili.h
LOCAL_COPY_HEADERS         += ./radio_config/qcril_qmi_radio_config_meta.h
LOCAL_COPY_HEADERS         += nanopb_utils/qcril_qmi_npb_encode.h 
LOCAL_COPY_HEADERS         += nanopb_utils/qcril_qmi_npb_decode.h 
LOCAL_COPY_HEADERS         += nanopb_utils/qcril_qmi_npb_utils.h 
LOCAL_COPY_HEADERS         += qcril_log.h
LOCAL_COPY_HEADERS         += qcril_reqlist.h
LOCAL_COPY_HEADERS         += qcril_qmi_singleton_agent.h

LOCAL_CLANG := true
ifndef QCRIL_DSDA_INSTANCE
   LOCAL_MODULE:= libril-qc-hal-qmi
else
   LOCAL_CFLAGS += -DFEATURE_DSDA_RIL_INSTANCE=$(QCRIL_DSDA_INSTANCE)
   LOCAL_MODULE:= libril-qc-hal-qmi-$(QCRIL_DSDA_INSTANCE)
endif
#LOCAL_EXPORT_CFLAGS += $(LOCAL_CFLAGS)
include $(BUILD_SHARED_LIBRARY)
##
## END build_qcril
##
