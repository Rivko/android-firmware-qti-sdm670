/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <map>
#include <framework/legacy.h>
#include <framework/Dispatcher.h>
#include <framework/qcril_event.h>
#include "qcril_qmi_oem_events.h"
#include "qcril_qmi_oem_reqlist.h"


#undef QCRIL_LEGACY_EVENT
#define QCRIL_LEGACY_EVENT QCRIL_DEFINE_LEGACY_EVENT
#include "qcril_qmi_oem_eventlist.h"

static std::map<int, qcril_evt_e_type> qcril_qmi_oem_hook_map;

void qcril_qmi_oem_events_init()
{
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_BASE]                                 = QCRIL_EVT_HOOK_BASE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_NV_READ]                              = QCRIL_EVT_HOOK_NV_READ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_NV_WRITE]                             = QCRIL_EVT_HOOK_NV_WRITE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_DATA_GO_DORMANT]                      = QCRIL_EVT_HOOK_DATA_GO_DORMANT;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_ME_DEPERSONALIZATION]                 = QCRIL_EVT_HOOK_ME_DEPERSONALIZATION;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_TUNE_AWAY]                        = QCRIL_EVT_HOOK_SET_TUNE_AWAY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_TUNE_AWAY]                        = QCRIL_EVT_HOOK_GET_TUNE_AWAY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_PAGING_PRIORITY]                  = QCRIL_EVT_HOOK_SET_PAGING_PRIORITY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_PAGING_PRIORITY]                  = QCRIL_EVT_HOOK_GET_PAGING_PRIORITY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_NEIGHBORING_CELLS_INFO]           = QCRIL_EVT_HOOK_GET_NEIGHBORING_CELLS_INFO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_CDMA_SUB_SRC_WITH_SPC]            = QCRIL_EVT_HOOK_SET_CDMA_SUB_SRC_WITH_SPC;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_DEFAULT_VOICE_SUB]                = QCRIL_EVT_HOOK_SET_DEFAULT_VOICE_SUB;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_LOCAL_CALL_HOLD]                  = QCRIL_EVT_HOOK_SET_LOCAL_CALL_HOLD;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CDMA_AVOID_CUR_NWK]                   = QCRIL_EVT_HOOK_CDMA_AVOID_CUR_NWK;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CDMA_CLEAR_AVOIDANCE_LIST]            = QCRIL_EVT_HOOK_CDMA_CLEAR_AVOIDANCE_LIST;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CDMA_GET_AVOIDANCE_LIST]              = QCRIL_EVT_HOOK_CDMA_GET_AVOIDANCE_LIST;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_BUILTIN_PLMN_LIST]                = QCRIL_EVT_HOOK_SET_BUILTIN_PLMN_LIST;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_PERFORM_INCREMENTAL_NW_SCAN]          = QCRIL_EVT_HOOK_PERFORM_INCREMENTAL_NW_SCAN;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CANCEL_QUERY_AVAILABLE_NETWORK]       = QCRIL_EVT_HOOK_CANCEL_QUERY_AVAILABLE_NETWORK;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_ENABLE_ENGINEER_MODE]                 = QCRIL_EVT_HOOK_ENABLE_ENGINEER_MODE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_REJECT_INCOMING_CALL_CAUSE_21]        = QCRIL_EVT_HOOK_REJECT_INCOMING_CALL_CAUSE_21;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_MODEM_TEST_MODE]                  = QCRIL_EVT_HOOK_SET_MODEM_TEST_MODE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_QUERY_MODEM_TEST_MODE]                = QCRIL_EVT_HOOK_QUERY_MODEM_TEST_MODE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_AVAILABLE_CONFIGS]                = QCRIL_EVT_HOOK_GET_AVAILABLE_CONFIGS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_CSG_ID]                           = QCRIL_EVT_HOOK_GET_CSG_ID;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_RAT_RAC]                          = QCRIL_EVT_HOOK_GET_RAT_RAC;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS]                   = QCRIL_EVT_HOOK_SET_ATEL_UI_STATUS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_ACQ_ORDER]      = QCRIL_EVT_HOOK_SET_PREFERRED_NETWORK_ACQ_ORDER;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_ACQ_ORDER]      = QCRIL_EVT_HOOK_GET_PREFERRED_NETWORK_ACQ_ORDER;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_CURRENT_SETUP_CALLS]              = QCRIL_EVT_HOOK_GET_CURRENT_SETUP_CALLS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_REQUEST_SETUP_ANSWER]                 = QCRIL_EVT_HOOK_REQUEST_SETUP_ANSWER;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CLEANUP_LOADED_CONFIGS]               = QCRIL_EVT_HOOK_CLEANUP_LOADED_CONFIGS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SEL_CONFIG]                           = QCRIL_EVT_HOOK_SEL_CONFIG;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_META_INFO]                        = QCRIL_EVT_HOOK_GET_META_INFO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_MODEM_CAPABILITY]                 = QCRIL_EVT_HOOK_GET_MODEM_CAPABILITY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UPDATE_SUB_BINDING]                   = QCRIL_EVT_HOOK_UPDATE_SUB_BINDING;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_DATA_SUBSCRIPTION]                = QCRIL_EVT_HOOK_SET_DATA_SUBSCRIPTION;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF]      = QCRIL_EVT_HOOK_SET_PREFERRED_NETWORK_BAND_PREF;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF]      = QCRIL_EVT_HOOK_GET_PREFERRED_NETWORK_BAND_PREF;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_IS_DATA_ENABLED]                  = QCRIL_EVT_HOOK_SET_IS_DATA_ENABLED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_IS_DATA_ROAMING_ENABLED]          = QCRIL_EVT_HOOK_SET_IS_DATA_ROAMING_ENABLED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_APN_INFO]                         = QCRIL_EVT_HOOK_SET_APN_INFO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_LTE_TUNE_AWAY]                    = QCRIL_EVT_HOOK_SET_LTE_TUNE_AWAY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_DEACTIVATE_CONFIGS]                   = QCRIL_EVT_HOOK_DEACTIVATE_CONFIGS;
#ifndef QMI_RIL_UTF
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_QC_VERSION_OF_FILE]               = QCRIL_EVT_HOOK_GET_QC_VERSION_OF_FILE;
#endif
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VALIDATE_CONFIG]                      = QCRIL_EVT_HOOK_VALIDATE_CONFIG;
#ifndef QMI_RIL_UTF
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_QC_VERSION_OF_CONFIGID]           = QCRIL_EVT_HOOK_GET_QC_VERSION_OF_CONFIGID;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_OEM_VERSION_OF_FILE]              = QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_FILE;
#endif
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_OEM_VERSION_OF_CONFIGID]          = QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_CONFIGID;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_ACTIVATE_CONFIGS]                     = QCRIL_EVT_HOOK_ACTIVATE_CONFIGS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_REQ_GENERIC]                          = QCRIL_EVT_HOOK_REQ_GENERIC;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_PROTOBUF_MSG]                         = QCRIL_EVT_HOOK_PROTOBUF_MSG;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_LTE_DIRECT_DISC_REQ]                  = QCRIL_EVT_HOOK_LTE_DIRECT_DISC_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_LTE_DIRECT_DISC]                = QCRIL_EVT_HOOK_UNSOL_LTE_DIRECT_DISC;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN]                  = QCRIL_EVT_HOOK_CSG_PERFORM_NW_SCAN;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF]                 = QCRIL_EVT_HOOK_CSG_SET_SYS_SEL_PREF;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CSG_GET_SYS_INFO]                     = QCRIL_EVT_HOOK_CSG_GET_SYS_INFO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_SAR_REV_KEY]                      = QCRIL_EVT_HOOK_GET_SAR_REV_KEY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_TRANSMIT_POWER]                   = QCRIL_EVT_HOOK_SET_TRANSMIT_POWER;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_DATA_CALL_PROFILE]                = QCRIL_EVT_HOOK_GET_DATA_CALL_PROFILE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SLOT_GET_CARD_STATE_REQ]              = QCRIL_EVT_HOOK_SLOT_GET_CARD_STATE_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SLOT_GET_ATR_REQ]                     = QCRIL_EVT_HOOK_SLOT_GET_ATR_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SLOT_SEND_APDU_REQ]                   = QCRIL_EVT_HOOK_SLOT_SEND_APDU_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SWITCH_SLOT_REQ]                      = QCRIL_EVT_HOOK_SWITCH_SLOT_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ]        = QCRIL_EVT_HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SLOT_CARD_POWER_REQ]                  = QCRIL_EVT_HOOK_SLOT_CARD_POWER_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_ENTER_DEPERSONALIZATION_REQ]          = QCRIL_EVT_HOOK_ENTER_DEPERSONALIZATION_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ]           = QCRIL_EVT_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_SLOTS_STATUS_REQ]                 = QCRIL_EVT_HOOK_GET_SLOTS_STATUS_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ]  = QCRIL_EVT_HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ]  = QCRIL_EVT_HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_EXTENDED_DBM_INTL]              = QCRIL_EVT_HOOK_UNSOL_EXTENDED_DBM_INTL;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_CDMA_BURST_DTMF]                = QCRIL_EVT_HOOK_UNSOL_CDMA_BURST_DTMF;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_CDMA_CONT_DTMF_START]           = QCRIL_EVT_HOOK_UNSOL_CDMA_CONT_DTMF_START;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_CDMA_CONT_DTMF_STOP]            = QCRIL_EVT_HOOK_UNSOL_CDMA_CONT_DTMF_STOP;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_CALL_EVT_PROGRESS_INFO_IND]     = QCRIL_EVT_HOOK_UNSOL_CALL_EVT_PROGRESS_INFO_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_NSS_RELEASE]                    = QCRIL_EVT_HOOK_UNSOL_NSS_RELEASE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_NEIGHBOR_CELL_INFO_RCVD]              = QCRIL_EVT_HOOK_NEIGHBOR_CELL_INFO_RCVD;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_EUTRA_STATUS]                   = QCRIL_EVT_HOOK_UNSOL_EUTRA_STATUS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_LTE_COEX]                       = QCRIL_EVT_HOOK_UNSOL_LTE_COEX;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_WMS_READY]                      = QCRIL_EVT_HOOK_UNSOL_WMS_READY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_VOICE_SYSTEM_ID]                = QCRIL_EVT_HOOK_UNSOL_VOICE_SYSTEM_ID;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_INCREMENTAL_NW_SCAN_IND]        = QCRIL_EVT_HOOK_UNSOL_INCREMENTAL_NW_SCAN_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_ENGINEER_MODE]                  = QCRIL_EVT_HOOK_UNSOL_ENGINEER_MODE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_SS_ERROR_CODE]                  = QCRIL_EVT_HOOK_UNSOL_SS_ERROR_CODE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PDC_CONFIG]                     = QCRIL_EVT_HOOK_UNSOL_PDC_CONFIG;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_AUDIO_STATE_CHANGED]            = QCRIL_EVT_HOOK_UNSOL_AUDIO_STATE_CHANGED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_SIM_REFRESH]                    = QCRIL_EVT_HOOK_UNSOL_SIM_REFRESH;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PDC_CLEAR_CONFIGS]              = QCRIL_EVT_HOOK_UNSOL_PDC_CLEAR_CONFIGS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_WWAN_AVAILABLE]                 = QCRIL_EVT_HOOK_UNSOL_WWAN_AVAILABLE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_MODEM_CAPABILITY]               = QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_UICC_VOLTAGE_STATUS]            = QCRIL_EVT_HOOK_UNSOL_UICC_VOLTAGE_STATUS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UICC_VOLTAGE_STATUS_REQ]              = QCRIL_EVT_HOOK_UICC_VOLTAGE_STATUS_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PDC_VALIDATE_CONFIGS]           = QCRIL_EVT_HOOK_UNSOL_PDC_VALIDATE_CONFIGS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PDC_VALIDATE_DUMPED]            = QCRIL_EVT_HOOK_UNSOL_PDC_VALIDATE_DUMPED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PDC_LIST_CONFIG]                = QCRIL_EVT_HOOK_UNSOL_PDC_LIST_CONFIG;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UICC_POWER_REQ]                       = QCRIL_EVT_HOOK_UICC_POWER_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_PERSONALIZATION_REACTIVATE_REQ]       = QCRIL_EVT_HOOK_PERSONALIZATION_REACTIVATE_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_PERSONALIZATION_STATUS_REQ]           = QCRIL_EVT_HOOK_PERSONALIZATION_STATUS_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_SUB_PROVISION_STATUS]           = QCRIL_EVT_HOOK_UNSOL_SUB_PROVISION_STATUS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_SIMLOCK_TEMP_UNLOCK_EXPIRED]    = QCRIL_EVT_HOOK_UNSOL_SIMLOCK_TEMP_UNLOCK_EXPIRED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_CARD_STATE_CHANGE_IND]          = QCRIL_EVT_HOOK_UNSOL_CARD_STATE_CHANGE_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_REMOTE_SIM_STATUS_IND]          = QCRIL_EVT_HOOK_UNSOL_REMOTE_SIM_STATUS_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_SLOT_STATUS_CHANGE_IND]         = QCRIL_EVT_HOOK_UNSOL_SLOT_STATUS_CHANGE_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_CSG_SYS_INFO_IND]               = QCRIL_EVT_HOOK_UNSOL_CSG_SYS_INFO_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_CSG_NETWORK_REJ_IND]            = QCRIL_EVT_HOOK_UNSOL_CSG_NETWORK_REJ_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_CSG_ID_CHANGE_IND]              = QCRIL_EVT_HOOK_UNSOL_CSG_ID_CHANGE_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_RAT_RAC_CHANGE_IND]             = QCRIL_EVT_HOOK_UNSOL_RAT_RAC_CHANGE_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_MAX_DATA_CHANGE_IND]            = QCRIL_EVT_HOOK_UNSOL_MAX_DATA_CHANGE_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PCSCF_ADDR]                     = QCRIL_EVT_HOOK_UNSOL_PCSCF_ADDR;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_OPERATOR_RESERVED_PCO]          = QCRIL_EVT_HOOK_UNSOL_OPERATOR_RESERVED_PCO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_GENERIC]                        = QCRIL_EVT_HOOK_UNSOL_GENERIC;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_OEM_SOCKET_CONNECTED]           = QCRIL_EVT_HOOK_UNSOL_OEM_SOCKET_CONNECTED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_BASE]                              = QCRIL_EVT_HOOK_VT_BASE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_DIAL_CALL]                         = QCRIL_EVT_HOOK_VT_DIAL_CALL;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_END_CALL]                          = QCRIL_EVT_HOOK_VT_END_CALL;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_ANSWER_CALL]                       = QCRIL_EVT_HOOK_VT_ANSWER_CALL;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_GET_CALL_INFO]                     = QCRIL_EVT_HOOK_VT_GET_CALL_INFO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_UNSOL_CALL_STATUS_IND]             = QCRIL_EVT_HOOK_VT_UNSOL_CALL_STATUS_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_PRESENCE_BASE]                        = QCRIL_EVT_HOOK_PRESENCE_BASE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_ENABLER_STATE_REQ]                = QCRIL_EVT_HOOK_IMS_ENABLER_STATE_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_PUBLISH_REQ]                 = QCRIL_EVT_HOOK_IMS_SEND_PUBLISH_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_PUBLISH_XML_REQ]             = QCRIL_EVT_HOOK_IMS_SEND_PUBLISH_XML_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_UNPUBLISH_REQ]               = QCRIL_EVT_HOOK_IMS_SEND_UNPUBLISH_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_SUBSCRIBE_REQ]               = QCRIL_EVT_HOOK_IMS_SEND_SUBSCRIBE_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_SUBSCRIBE_XML_REQ]           = QCRIL_EVT_HOOK_IMS_SEND_SUBSCRIBE_XML_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_UNSUBSCRIBE_REQ]             = QCRIL_EVT_HOOK_IMS_SEND_UNSUBSCRIBE_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SET_NOTIFY_FMT_REQ_V01]           = QCRIL_EVT_HOOK_IMS_SET_NOTIFY_FMT_REQ_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_GET_NOTIFY_FMT_REQ_V01]           = QCRIL_EVT_HOOK_IMS_GET_NOTIFY_FMT_REQ_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SET_EVENT_REPORT_REQ_V01]         = QCRIL_EVT_HOOK_IMS_SET_EVENT_REPORT_REQ_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_GET_EVENT_REPORT_REQ_V01]         = QCRIL_EVT_HOOK_IMS_GET_EVENT_REPORT_REQ_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_PUBLISH_TRIGGER_IND_V01]          = QCRIL_EVT_HOOK_IMS_PUBLISH_TRIGGER_IND_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_NOTIFY_XML_IND_V01]               = QCRIL_EVT_HOOK_IMS_NOTIFY_XML_IND_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_NOTIFY_IND_V01]                   = QCRIL_EVT_HOOK_IMS_NOTIFY_IND_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_ENABLER_STATUS_IND]               = QCRIL_EVT_HOOK_IMS_ENABLER_STATUS_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_SUB_PROVISION_PREFERENCE_REQ]     = QCRIL_EVT_HOOK_GET_SUB_PROVISION_PREFERENCE_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_SUB_PROVISION_PREFERENCE_REQ]     = QCRIL_EVT_HOOK_SET_SUB_PROVISION_PREFERENCE_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_SIM_ICCID_REQ]                    = QCRIL_EVT_HOOK_GET_SIM_ICCID_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ]             = QCRIL_EVT_HOOK_GET_MAX_DATA_ALLOWED_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ] = QCRIL_EVT_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_BASE]                           = QCRIL_EVT_HOOK_EMBMS_BASE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_ENABLE]                         = QCRIL_EVT_HOOK_EMBMS_ENABLE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_DISABLE]                        = QCRIL_EVT_HOOK_EMBMS_DISABLE;
    // DATA REFACTOR TODO: enable after data integration is complete
#ifndef QMI_RIL_UTF
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_ACTIVATE_TMGI]                  = QCRIL_EVT_HOOK_EMBMS_ACTIVATE_TMGI;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_DEACTIVATE_TMGI]                = QCRIL_EVT_HOOK_EMBMS_DEACTIVATE_TMGI;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_RSSI_IND]                 = QCRIL_EVT_HOOK_EMBMS_UNSOL_RSSI_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_GET_AVAILABLE_TMGI]             = QCRIL_EVT_HOOK_EMBMS_GET_AVAILABLE_TMGI;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_GET_ACTIVE_TMGI]                = QCRIL_EVT_HOOK_EMBMS_GET_ACTIVE_TMGI;
#endif
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_ENABLE_RSSI]                    = QCRIL_EVT_HOOK_EMBMS_ENABLE_RSSI;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_DISABLE_RSSI]                   = QCRIL_EVT_HOOK_EMBMS_DISABLE_RSSI;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_GET_COVERAGE_STATE]             = QCRIL_EVT_HOOK_EMBMS_GET_COVERAGE_STATE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_GET_RSSI]                       = QCRIL_EVT_HOOK_EMBMS_GET_RSSI;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_GET_EMBMS_SVC_STATE]            = QCRIL_EVT_HOOK_EMBMS_GET_EMBMS_SVC_STATE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_SVC_STATE]                = QCRIL_EVT_HOOK_EMBMS_UNSOL_SVC_STATE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_ACTIVE_TMGI]              = QCRIL_EVT_HOOK_EMBMS_UNSOL_ACTIVE_TMGI;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_COVERAGE]                 = QCRIL_EVT_HOOK_EMBMS_UNSOL_COVERAGE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_AVAILABLE_TMGI]           = QCRIL_EVT_HOOK_EMBMS_UNSOL_AVAILABLE_TMGI;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_OSS_WARNING]              = QCRIL_EVT_HOOK_EMBMS_UNSOL_OSS_WARNING;
#ifndef QMI_RIL_UTF
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_ACTIVATE_DEACTIVATE_TMGI]       = QCRIL_EVT_HOOK_EMBMS_ACTIVATE_DEACTIVATE_TMGI;
#endif
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_CELL_INFO_CHANGED]        = QCRIL_EVT_HOOK_EMBMS_UNSOL_CELL_INFO_CHANGED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_RADIO_STATE_CHANGED]      = QCRIL_EVT_HOOK_EMBMS_UNSOL_RADIO_STATE_CHANGED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_SAI_LIST]                 = QCRIL_EVT_HOOK_EMBMS_UNSOL_SAI_LIST;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_SET_SNTP_TIME]                  = QCRIL_EVT_HOOK_EMBMS_SET_SNTP_TIME;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_GET_SIB16_COVERAGE]             = QCRIL_EVT_HOOK_EMBMS_GET_SIB16_COVERAGE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_SIB16_COVERAGE]           = QCRIL_EVT_HOOK_EMBMS_UNSOL_SIB16_COVERAGE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_GET_UTC_TIME]                   = QCRIL_EVT_HOOK_EMBMS_GET_UTC_TIME;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_GET_ACTIVE_LOG_PACKET_IDS]      = QCRIL_EVT_HOOK_EMBMS_GET_ACTIVE_LOG_PACKET_IDS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_DELIVER_LOG_PACKET]             = QCRIL_EVT_HOOK_EMBMS_DELIVER_LOG_PACKET;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_GET_E911_STATE]                 = QCRIL_EVT_HOOK_EMBMS_GET_E911_STATE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_E911_STATE_CHANGED]       = QCRIL_EVT_HOOK_EMBMS_UNSOL_E911_STATE_CHANGED;
#ifndef QMI_RIL_UTF
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_CONTENT_DESC_UPDATE]            = QCRIL_EVT_HOOK_EMBMS_CONTENT_DESC_UPDATE;
#endif
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_CONTENT_DESC_CONTROL]     = QCRIL_EVT_HOOK_EMBMS_UNSOL_CONTENT_DESC_CONTROL;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_GET_SIB_PLMN]                   = QCRIL_EVT_HOOK_EMBMS_GET_SIB_PLMN;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_EMBMS_STATUS]             = QCRIL_EVT_HOOK_EMBMS_UNSOL_EMBMS_STATUS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_GET_EMBMS_STATUS]               = QCRIL_EVT_HOOK_EMBMS_GET_EMBMS_STATUS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_UNSOL_GET_INTERESTED_TMGI_LIST] = QCRIL_EVT_HOOK_EMBMS_UNSOL_GET_INTERESTED_TMGI_LIST;
#ifndef QMI_RIL_UTF
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_SEND_INTERESTED_TMGI_LIST]      = QCRIL_EVT_HOOK_EMBMS_SEND_INTERESTED_TMGI_LIST;
#endif
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_EMBMS_MAX]                            = QCRIL_EVT_HOOK_MAX;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_RFPE_BASE]                            = QCRIL_EVT_HOOK_RFPE_BASE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_RFM_SCENARIO_REQ]                 = QCRIL_EVT_HOOK_SET_RFM_SCENARIO_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_RFM_SCENARIO_REQ]                 = QCRIL_EVT_HOOK_GET_RFM_SCENARIO_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_PROVISIONED_TABLE_REVISION_REQ]   = QCRIL_EVT_HOOK_GET_PROVISIONED_TABLE_REVISION_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_LOCAL_CALL_HOLD_v01]              = QCRIL_EVT_HOOK_SET_LOCAL_CALL_HOLD_v01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_ADN_RECORD]                       = QCRIL_EVT_HOOK_GET_ADN_RECORD;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UPDATE_ADN_RECORD]                    = QCRIL_EVT_HOOK_UPDATE_ADN_RECORD;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_ADN_INIT_DONE]                  = QCRIL_EVT_HOOK_UNSOL_ADN_INIT_DONE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_ADN_RECORDS_IND]                = QCRIL_EVT_HOOK_UNSOL_ADN_RECORDS_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_SPEECH_CODEC_INFO]              = QCRIL_EVT_HOOK_UNSOL_SPEECH_CODEC_INFO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_MAX]                                  = QCRIL_EVT_HOOK_MAX;
}

qcril_evt_e_type qcril_qmi_oem_get_event(int oemhook_msg)
{
  std::map<int, qcril_evt_e_type>::iterator it;

  it = qcril_qmi_oem_hook_map.find(oemhook_msg);

  if (it != qcril_qmi_oem_hook_map.end())
      return it->second;
  else
      return QCRIL_EVT_HOOK_BASE;
}

int qcril_qmi_oem_get_oemhook_msg(qcril_evt_e_type event)
{
  std::map<int, qcril_evt_e_type>::iterator it;
  int request = QCRIL_REQ_HOOK_BASE;
  for (it=qcril_qmi_oem_hook_map.begin(); it!=qcril_qmi_oem_hook_map.end(); ++it)
  {
    if (it->second == event)
     {
         request = it->first;
         break;
     }
  }
  return request;
}
