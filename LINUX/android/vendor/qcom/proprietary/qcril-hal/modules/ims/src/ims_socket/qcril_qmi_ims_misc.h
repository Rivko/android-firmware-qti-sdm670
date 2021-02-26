/******************************************************************************
#  Copyright (c) 2012-2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_ims_misc.h

  @brief

*/


#ifndef QCRIL_QMI_IMS_MISC_H
#define QCRIL_QMI_IMS_MISC_H

#include "modules/ims/qcril_qmi_imsa.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <framework/legacy.h>
#include "telephony/ril.h"
#include "qcrili.h"
#include "qcril_qmi_voice.h"
#include "ip_multimedia_subsystem_application_v01.h"
#include "ip_multimedia_subsystem_settings_v01.h"
#include "network_access_service_v01.h"
#include "wireless_messaging_service_v01.h"
#include "qcril_qmi_radio_config_meta.h"
#include "modules/ims/qcril_qmi_imss_internal.h"

ims_MsgId qcril_qmi_ims_map_event_to_request(qcril_evt_e_type event);

boolean qcril_qmi_ims_is_valid_event(qcril_evt_e_type event);

ims_Error qcril_qmi_ims_map_ril_error_to_ims_error(int ril_error);
qcril_evt_e_type qcril_qmi_ims_map_request_to_event(ims_MsgId msg_id);

ims_Error qcril_qmi_ims_map_radio_config_error_to_ims_error(qcril_qmi_radio_config_error_type radio_config_error);
qcril_evt_e_type qcril_qmi_ims_map_request_to_event(ims_MsgId msg_id);

ims_Registration_RegState qcril_qmi_ims_map_ril_reg_state_to_ims_reg_state(int state);

ims_CallType qcril_qmi_ims_map_ril_call_type_to_ims_call_type(RIL_Call_Type call_type);
RIL_Call_Type qcril_qmi_ims_map_ims_call_type_to_ril_call_type(boolean has_call_type, ims_CallType call_type);

ims_CallDomain qcril_qmi_ims_map_ril_call_domain_to_ims_call_domain(RIL_Call_Domain call_domain);
RIL_Call_Domain qcril_qmi_ims_map_ims_call_domain_to_ril_call_domain(boolean has_call_domain, ims_CallDomain call_domain);

RIL_Token qcril_qmi_ims_convert_ims_token_to_ril_token(uint32_t ims_token);
uint32_t qcril_qmi_ims_free_and_convert_ril_token_to_ims_token(RIL_Token ril_token);

void qcril_qmi_ims_translate_ril_callmodify_to_ims_callmodify(const RIL_Call_Modify* ril_data, ims_CallModify* ims_data);

void qcril_qmi_ims_translate_ril_calldetails_to_ims_calldetails(const RIL_Call_Details* ril_data, ims_CallDetails* ims_data);
void qcril_qmi_ims_translate_ims_calldetails_to_ril_calldetails(const ims_CallDetails *ims_data, RIL_Call_Details* ril_data);

void qcril_qmi_ims_translate_ril_callforwdinfo_to_ims_callforwdinfo
(
 const qcril_qmi_voice_callforwd_info_param_u_type* ril_data, int num,
 voice_time_type_v02 *call_fwd_start_time,
 voice_time_type_v02 *call_fwd_end_time,
 ims_CallForwardInfoList* ims_data
);

void qcril_qmi_ims_translate_ril_service_status_class_to_ims_callwaitinginfo(int service_status, int service_class, ims_CallWaitingInfo* ims_data);
ims_Registration_RegState qcril_qmi_ims_map_qmi_ims_reg_state_to_ims_reg_state(uint8_t ims_registered);

void qcril_qmi_ims_translate_ril_suppsvcnotification_to_ims_suppsvcnotification(const RIL_SuppSvcNotification* ril_data, ims_SuppSvcNotification* ims_data);

void qcril_qmi_ims_translate_ril_callcapabilities_to_ims_srvstatusinfo
(
 const uint8_t ril_data_valid,
 const voice_ip_call_capabilities_info_type_v02* ril_data,
 const uint8 rtt_capabilities_info_valid,
 const rtt_mode_type_v02 rtt_capabilities_info,
 ims_SrvStatusList* ims_data,
 ims_CallType current_call_type,
 call_mode_enum_v02 call_mode
);

ims_Info* qcril_qmi_ims_create_ims_info(
    ims_CallType type,
    imsa_service_status_enum_v01 status,
    boolean rat_valid,
    imsa_service_rat_enum_v01 rat
);
ims_SrvStatusList* qcril_qmi_ims_create_ims_srvstatusinfo(const qcril_qmi_imsa_srv_status_type* qmi_data);

void qcril_qmi_ims_free_srvstatuslist(ims_SrvStatusList* ims_srv_status_list_ptr);
void qcril_qmi_ims_free_ims_info(ims_Info* ims_info_ptr);

ims_Handover* qcril_qmi_ims_create_ims_handover_from_imsa_rat_info(const imsa_rat_handover_status_info_v01* qmi_data);
void qcril_qmi_ims_free_ims_handover(ims_Handover* ims_handover_ptr);
void qcril_qmi_ims_free_ims_registration(ims_Registration* ims_reg_ptr);

ims_CallFailCause qcril_qmi_ims_map_ril_failcause_to_ims_failcause(RIL_LastCallFailCause ril_failcause, int ims_extended_error_code);

boolean qcril_qmi_ims_map_qmi_call_state_to_ims_conf_call_state(call_state_enum_v02 qmi_state, ims_ConfCallState *ims_state_ptr);

ims_ConfigFailureCause qcril_qmi_ims_map_radio_config_settings_resp_to_ims_config_failcause
(
  qcril_qmi_radio_config_settings_resp_type radio_config_error
);

boolean qcril_qmi_voice_map_qmi_reason_to_ims_facility
(
  voice_cc_sups_result_reason_enum_v02 reason,
  ims_SuppSvcFacilityType *ims_reason
);

ims_CallSubstate qcril_qmi_ims_map_ril_call_substate_to_ims_call_substate
(
 RIL_Call_Sub_State  ril_call_substate
);

ims_MwiPriority qcril_qmi_sms_map_qmi_mwi_priority_to_ims_priority
(
 wms_mwi_priority_type_enum_v01 wms_mwi_priority
);

ims_MwiMessageType qcril_qmi_sms_map_qmi_mwi_msg_type_to_ims_msg_type
(
 transport_mwi_wms_message_type_enum_v01 mwi_wms_msg_type
);

boolean qcril_qmi_ims_translate_ims_ttymodetype_to_qmi_tty_mode
(
 ims_Tty_Mode_Type mode,
 tty_mode_enum_v02 *tty_mode
);

boolean qcril_qmi_ims_translate_ims_callfwdtimerinfo_to_voice_time_type
(
 const ims_CallFwdTimerInfo *callfwdtimerinfo,
 voice_time_type_v02 *call_fwd_timer
);

boolean qcril_qmi_ims_translate_voice_time_type_to_ims_callfwdtimerinfo
(
 const voice_time_type_v02 *call_fwd_timer,
 ims_CallFwdTimerInfo *callfwdtimerinfo
);

int32_t qcril_qmi_ims_map_ims_failcause_qmi_reject_cause
(
  ims_CallFailCause failcause
);

ims_CauseCodes qcril_qmi_ims_map_qmi_call_modified_cause_to_ims_cause_code
(
  voice_call_modified_cause_enum_v02 call_modified_cause
);

uint8_t qcril_qmi_ims_map_wificallingstatus_to_ims_settings_wfc_status
(
 ims_WifiCallingStatus             status,
 ims_settings_wfc_status_enum_v01 *wifi_call
);

boolean qcril_qmi_ims_map_ims_settings_wfc_status_to_wificallingstatus
(
 ims_settings_wfc_status_enum_v01 wifi_call,
 ims_WifiCallingStatus           *status
);

uint8_t qcril_qmi_ims_map_wificallingpreference_to_ims_settings_wfc_preference
(
 ims_WifiCallingPreference        preference,
 ims_settings_wfc_preference_v01 *wifi_call_preference
);

boolean qcril_qmi_ims_map_ims_settings_wfc_preference_to_wificallingpreference
(
 ims_settings_wfc_preference_v01 wifi_call_preference,
 ims_WifiCallingPreference      *preference
);
/* Maps the ims config item to the radio config item */
qcril_qmi_radio_config_item_type qcril_qmi_ims_map_ims_config_to_radio_config_item
(
   ims_ConfigItem ims_config_item
);

/* Maps the radio config item to the ims config item */
ims_ConfigItem qcril_qmi_ims_map_radio_config_to_ims_config_item
(
   qcril_qmi_radio_config_item_type ims_config_item
);

boolean qcril_qmi_ims_map_imsa_rat_to_ims_rat
(
 imsa_service_rat_enum_v01 imsa_rat,
 ims_RadioTechType        *ims_rat
);

boolean qcril_qmi_ims_map_wificallingpreference_to_dsd_rat_preference
(
 ims_WifiCallingPreference  preference,
 int                       *dsd_rat_preference
);

boolean qcril_qmi_ims_map_ims_settings_wfc_preference_to_dsd_rat_preference
(
 qcril_ims_setting_wfc_preference_type wifi_call_preference,
 int                            *dsd_rat_preference
);

boolean qcril_qmi_ims_map_ims_ect_type_to_qmi_ect_type
(
 ims_EctType ims_ectType,
 voip_ect_type_enum_v02 *qmi_ectType
);

boolean qcril_qmi_ims_map_qmi_wifi_quality_to_ims_wifi_quality
(
 imsa_vowifiprofile_status_enum_v01 qmi_wifi_quality,
 int                                *ims_wifi_quality
);

boolean qcril_qmi_ims_map_qmi_conf_parti_op_to_ims_conf_parti_op
(
 conf_participant_operation_enum_v02 qmi_operation,
 ims_ConfParticipantOperation       *ims_operation
);

void copy_qmi_ssac_to_proto_ssac(nas_get_ssac_info_resp_msg_v01 *qmiSsacInfo, ims_SsacInfo *protoSsacInfo);

void copy_qmi_ssac_info_resp_to_proto_ssac(nas_get_ssac_info_resp_msg_v01 *qmiSsacInfo, ims_SsacInfo *protoSsacInfo);

boolean qcril_qmi_ims_map_ims_volte_user_pref_to_qmi_nas_voice_domain_pref
(
 ims_VoltePref                       ims_volte_user_pref,
 nas_voice_domain_pref_enum_type_v01 *qmi_voice_domain_pref
);

boolean qcril_qmi_ims_map_qmi_nas_voice_domain_pref_to_ims_volte_user_pref
(
 nas_voice_domain_pref_enum_type_v01 qmi_voice_domain_pref,
 ims_VoltePref                       *ims_volte_user_pref
);

boolean qcril_qmi_ims_is_msg_supported
(
  int message_id, uint8_t supported_msgs[], uint32_t supported_msgs_len
);

boolean qcril_qmi_ims_map_ims_settings_ho_config_to_ims_ho_config
(
 ims_settings_handover_option_enum_v01 qmi_imss_ho_config,
 ims_HandoverConfig           *ims_ho_config
);

boolean qcril_qmi_ims_map_ims_ho_config_to_qmi_imss_ho_config
(
 ims_HandoverConfig           ims_ho_config,
 ims_settings_handover_option_enum_v01 *qmi_imss_ho_config
);

ims_BlockReasonType qcril_qmi_ims_map_qmi_block_reason_to_ims_block_reason
(
  imsa_block_reason_type_mask_v01 qmi_block_reason
);


ims_RegFailureReasonType qcril_qmi_call_end_type_to_ims_reg_failure
(
  imsa_verbose_call_end_reason_type_enum_v01 qmi_block_reason
);

bool qcril_qmi_ims_map_ims_rtt_mode_to_qcril
(
  ims_RttMode rtt_mode_ims,
  rtt_mode_type_v02 *rtt_mode_qcril
);

bool qcril_qmi_ims_map_qcril_rtt_mode_to_ims
(
  rtt_mode_type_v02 rtt_mode_qcril,
  ims_RttMode* rtt_mode_ims
);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* QCRIL_QMI_IMS_MISC_H */
