/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <string.h>
#include "modules/uim/UimQmiUimRequestMsg.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "modules/uim/qcril_uim.h"
#include "modules/uim/qcril_uim_sap.h"
#include "modules/uim/qcril_uim_lpa.h"
#include "modules/uim/qcril_uim_gba.h"
#include "qcril_uim_util.h"

typedef union
{
  qmi_uim_read_transparent_params_type          read_transparent;
  qmi_uim_read_record_params_type               read_record;
  qmi_uim_write_transparent_params_type         write_transparent;
  qmi_uim_write_record_params_type              write_record;
  qmi_uim_get_file_attributes_params_type       get_attributes;
  qmi_uim_refresh_register_params_type          refresh_register;
  qmi_uim_set_service_status_params_type        set_service_status;
  qmi_uim_get_service_status_params_type        get_service_status;
  qmi_uim_verify_pin_params_type                verify_pin;
  qmi_uim_unblock_pin_params_type               unblock_pin;
  qmi_uim_change_pin_params_type                change_pin;
  qmi_uim_set_pin_protection_params_type        set_pin;
  qmi_uim_depersonalization_params_type         deperso;
  qmi_uim_power_up_params_type                  power_up;
  qmi_uim_power_down_params_type                power_down;
  qmi_uim_change_prov_session_params_type       change_prov_session;
  qmi_uim_logical_channel_params_type           logical_channel;
  qmi_uim_open_logical_channel_params_type      open_logical_channel;
  qmi_uim_send_apdu_params_type                 send_apdu;
  qmi_uim_get_atr_params_type                   get_atr;
  qmi_uim_authenticate_params_type              authenticate;
  qmi_uim_status_cmd_params_type                send_status;
  qmi_uim_reselect_params_type                  reselect;
  qmi_uim_supply_voltage_params_type            supply_voltage;
  qmi_uim_personalization_params_type           perso;
  qmi_uim_sap_connection_params_type            sap_connection;
  qmi_uim_sap_request_params_type               sap_request;
  qmi_uim_gba_params_type                       gba_init;
  qmi_uim_gba_impi_params_type                  gba_impi;
  qmi_uim_physical_slot_mapping_type            phy_slot_map;
  qmi_uim_set_apdu_behavior_params_type         set_apdu_behavior;
  qmi_uim_terminal_capability_params_type       terminal_capability;
  qmi_uim_remote_unlock_params_type             remote_unlock_req;
  qmi_uim_add_profile_req_type                  add_profile;
  qmi_uim_set_sim_profile_req_type              set_profile;
  qmi_uim_get_sim_profiles_type                 get_profiles;
  qmi_uim_delete_profile_req_type               delete_profile;
  qmi_uim_update_profile_nickname_req_type      update_nickname;
  qmi_uim_euicc_memory_reset_req_type           euicc_reset;
  qmi_uim_profile_user_consent_req_type         user_consent;
  qmi_uim_get_eid_req_type                      get_eid;
  qmi_uim_get_profile_info_req_type             profile_info;
  qmi_uim_euicc_default_server_address_req_type euicc_server_address;
} req_params;

/*---------------------------------------------------------------------------
  Internal functions
---------------------------------------------------------------------------*/
static int deep_copy
(
  uint32_t            request_type,
  const void        * param_data_ptr,
  req_params        * request_ptr
)
{
  uint32_t i = 0;
  /* Perform a deep copy where needed */
  switch(request_type)
  {
    case QCRIL_UIM_REQUEST_READ_TRANSPARENT:
      memcpy(&request_ptr->read_transparent,
             param_data_ptr,
             sizeof(qmi_uim_read_transparent_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->read_transparent.session_info.aid.data_ptr,
                          ((qmi_uim_read_transparent_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_read_transparent_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->read_transparent.file_id.path.data_ptr,
                          ((qmi_uim_read_transparent_params_type*)param_data_ptr)->file_id.path.data_ptr,
                          ((qmi_uim_read_transparent_params_type*)param_data_ptr)->file_id.path.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_READ_RECORD:
      memcpy(&request_ptr->read_record,
             param_data_ptr,
             sizeof(qmi_uim_read_record_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->read_record.session_info.aid.data_ptr,
                          ((qmi_uim_read_record_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_read_record_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->read_record.file_id.path.data_ptr,
                          ((qmi_uim_read_record_params_type*)param_data_ptr)->file_id.path.data_ptr,
                          ((qmi_uim_read_record_params_type*)param_data_ptr)->file_id.path.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_WRITE_TRANSPARENT:
      memcpy(&request_ptr->write_transparent,
             param_data_ptr,
             sizeof(qmi_uim_write_transparent_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->write_transparent.session_info.aid.data_ptr,
                          ((qmi_uim_write_transparent_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_write_transparent_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->write_transparent.file_id.path.data_ptr,
                          ((qmi_uim_write_transparent_params_type*)param_data_ptr)->file_id.path.data_ptr,
                          ((qmi_uim_write_transparent_params_type*)param_data_ptr)->file_id.path.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->write_transparent.data.data_ptr,
                          ((qmi_uim_write_transparent_params_type*)param_data_ptr)->data.data_ptr,
                          ((qmi_uim_write_transparent_params_type*)param_data_ptr)->data.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_WRITE_RECORD:
      memcpy(&request_ptr->write_record,
             param_data_ptr,
             sizeof(qmi_uim_write_record_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->write_record.session_info.aid.data_ptr,
                          ((qmi_uim_write_record_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_write_record_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->write_record.file_id.path.data_ptr,
                          ((qmi_uim_write_record_params_type*)param_data_ptr)->file_id.path.data_ptr,
                          ((qmi_uim_write_record_params_type*)param_data_ptr)->file_id.path.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->write_record.data.data_ptr,
                          ((qmi_uim_write_record_params_type*)param_data_ptr)->data.data_ptr,
                          ((qmi_uim_write_record_params_type*)param_data_ptr)->data.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_GET_RESPONSE:
      memcpy(&request_ptr->get_attributes,
             param_data_ptr,
             sizeof(qmi_uim_get_file_attributes_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->get_attributes.session_info.aid.data_ptr,
                          ((qmi_uim_get_file_attributes_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_get_file_attributes_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->get_attributes.file_id.path.data_ptr,
                          ((qmi_uim_get_file_attributes_params_type*)param_data_ptr)->file_id.path.data_ptr,
                          ((qmi_uim_get_file_attributes_params_type*)param_data_ptr)->file_id.path.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_REFRESH_REGISTER:
      memcpy(&request_ptr->refresh_register,
             param_data_ptr,
             sizeof(qmi_uim_refresh_register_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->refresh_register.session_info.aid.data_ptr,
                          ((qmi_uim_refresh_register_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_refresh_register_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->refresh_register.files_ptr,
                          ((qmi_uim_refresh_register_params_type*)param_data_ptr)->files_ptr,
                          (((qmi_uim_refresh_register_params_type*)param_data_ptr)->num_files) * sizeof(qmi_uim_file_id_type),
                          (qmi_uim_file_id_type *));
      break;

    case QCRIL_UIM_REQUEST_GET_FDN:
      memcpy(&request_ptr->get_service_status,
             param_data_ptr,
             sizeof(qmi_uim_get_service_status_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->get_service_status.session_info.aid.data_ptr,
                          ((qmi_uim_get_service_status_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_get_service_status_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_SET_FDN:
      memcpy(&request_ptr->set_service_status,
             param_data_ptr,
             sizeof(qmi_uim_set_service_status_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->set_service_status.session_info.aid.data_ptr,
                          ((qmi_uim_set_service_status_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_set_service_status_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_VERIFY_PIN:
    case QCRIL_UIM_REQUEST_VERIFY_PIN2:
      memcpy(&request_ptr->verify_pin,
             param_data_ptr,
             sizeof(qmi_uim_verify_pin_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->verify_pin.session_info.aid.data_ptr,
                          ((qmi_uim_verify_pin_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_verify_pin_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->verify_pin.pin_data.data_ptr,
                          ((qmi_uim_verify_pin_params_type*)param_data_ptr)->pin_data.data_ptr,
                          ((qmi_uim_verify_pin_params_type*)param_data_ptr)->pin_data.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_UNBLOCK_PIN:
      memcpy(&request_ptr->unblock_pin,
             param_data_ptr,
             sizeof(qmi_uim_unblock_pin_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->unblock_pin.session_info.aid.data_ptr,
                          ((qmi_uim_unblock_pin_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_unblock_pin_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->unblock_pin.puk_data.data_ptr,
                          ((qmi_uim_unblock_pin_params_type*)param_data_ptr)->puk_data.data_ptr,
                          ((qmi_uim_unblock_pin_params_type*)param_data_ptr)->puk_data.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->unblock_pin.new_pin_data.data_ptr,
                          ((qmi_uim_unblock_pin_params_type*)param_data_ptr)->new_pin_data.data_ptr,
                          ((qmi_uim_unblock_pin_params_type*)param_data_ptr)->new_pin_data.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_CHANGE_PIN:
      memcpy(&request_ptr->change_pin,
             param_data_ptr,
             sizeof(qmi_uim_change_pin_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->change_pin.session_info.aid.data_ptr,
                          ((qmi_uim_change_pin_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_change_pin_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->change_pin.old_pin_data.data_ptr,
                          ((qmi_uim_change_pin_params_type*)param_data_ptr)->old_pin_data.data_ptr,
                          ((qmi_uim_change_pin_params_type*)param_data_ptr)->old_pin_data.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->change_pin.new_pin_data.data_ptr,
                          ((qmi_uim_change_pin_params_type*)param_data_ptr)->new_pin_data.data_ptr,
                          ((qmi_uim_change_pin_params_type*)param_data_ptr)->new_pin_data.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_SET_PIN:
      memcpy(&request_ptr->set_pin,
             param_data_ptr,
             sizeof(qmi_uim_set_pin_protection_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->set_pin.session_info.aid.data_ptr,
                          ((qmi_uim_set_pin_protection_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_set_pin_protection_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->set_pin.pin_data.data_ptr,
                          ((qmi_uim_set_pin_protection_params_type*)param_data_ptr)->pin_data.data_ptr,
                          ((qmi_uim_set_pin_protection_params_type*)param_data_ptr)->pin_data.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_DEPERSO:
      memcpy(&request_ptr->deperso,
             param_data_ptr,
             sizeof(qmi_uim_depersonalization_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->deperso.ck_data.data_ptr,
                          ((qmi_uim_depersonalization_params_type*)param_data_ptr)->ck_data.data_ptr,
                          ((qmi_uim_depersonalization_params_type*)param_data_ptr)->ck_data.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_POWER_UP:
      memcpy(&request_ptr->power_up,
             param_data_ptr,
             sizeof(qmi_uim_power_up_params_type));
      break;

    case QCRIL_UIM_REQUEST_POWER_DOWN:
      memcpy(&request_ptr->power_down,
             param_data_ptr,
             sizeof(qmi_uim_power_down_params_type));
      break;

    case QCRIL_UIM_REQUEST_CHANGE_PROV_SESSION:
      memcpy(&request_ptr->change_prov_session,
             param_data_ptr,
             sizeof(qmi_uim_change_prov_session_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->change_prov_session.app_info.aid.data_ptr,
                          ((qmi_uim_change_prov_session_params_type*)param_data_ptr)->app_info.aid.data_ptr,
                          ((qmi_uim_change_prov_session_params_type*)param_data_ptr)->app_info.aid.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_AUTHENTICATE:
      memcpy(&request_ptr->authenticate,
             param_data_ptr,
             sizeof(qmi_uim_authenticate_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->authenticate.session_info.aid.data_ptr,
                          ((qmi_uim_authenticate_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_authenticate_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->authenticate.auth_data.data_ptr,
                          ((qmi_uim_authenticate_params_type*)param_data_ptr)->auth_data.data_ptr,
                          ((qmi_uim_authenticate_params_type*)param_data_ptr)->auth_data.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_LOGICAL_CHANNEL:
      memcpy(&request_ptr->logical_channel,
             param_data_ptr,
             sizeof(qmi_uim_logical_channel_params_type));
      if (request_ptr->logical_channel.operation_type == QMI_UIM_LOGICAL_CHANNEL_OPEN)
      {
        QCRIL_UIM_DUPLICATE(request_ptr->logical_channel.channel_data.aid.data_ptr,
                            ((qmi_uim_logical_channel_params_type*)param_data_ptr)->channel_data.aid.data_ptr,
                            ((qmi_uim_logical_channel_params_type*)param_data_ptr)->channel_data.aid.data_len,
                            (unsigned char *));
      }
      break;

    case QCRIL_UIM_REQUEST_OPEN_LOGICAL_CHANNEL:
      memcpy(&request_ptr->open_logical_channel,
             param_data_ptr,
             sizeof(qmi_uim_open_logical_channel_params_type));
      if (((qmi_uim_open_logical_channel_params_type*)param_data_ptr)->aid.data_ptr)
      {
        QCRIL_UIM_DUPLICATE(request_ptr->logical_channel.channel_data.aid.data_ptr,
                            ((qmi_uim_open_logical_channel_params_type*)param_data_ptr)->aid.data_ptr,
                            ((qmi_uim_open_logical_channel_params_type*)param_data_ptr)->aid.data_len,
                            (unsigned char *));
      }
      break;

    case QCRIL_UIM_REQUEST_SEND_APDU:
      memcpy(&request_ptr->send_apdu,
             param_data_ptr,
             sizeof(qmi_uim_send_apdu_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->send_apdu.apdu.data_ptr,
                          ((qmi_uim_send_apdu_params_type*)param_data_ptr)->apdu.data_ptr,
                          ((qmi_uim_send_apdu_params_type*)param_data_ptr)->apdu.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_GET_ATR:
      memcpy(&request_ptr->get_atr,
             param_data_ptr,
             sizeof(qmi_uim_get_atr_params_type));
      break;

    case QCRIL_UIM_REQUEST_SEND_STATUS:
      memcpy(&request_ptr->send_status,
             param_data_ptr,
             sizeof(qmi_uim_status_cmd_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->send_status.session_info.aid.data_ptr,
                          ((qmi_uim_status_cmd_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_status_cmd_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_RESELECT:
      memcpy(&request_ptr->reselect,
             param_data_ptr,
             sizeof(qmi_uim_reselect_params_type));
      break;

    case QCRIL_UIM_REQUEST_SUPPLY_VOLTAGE:
      memcpy(&request_ptr->supply_voltage,
             param_data_ptr,
             sizeof(qmi_uim_supply_voltage_params_type));
      break;

    case QCRIL_UIM_REQUEST_PERSO:
      memcpy(&request_ptr->perso,
             param_data_ptr,
             sizeof(qmi_uim_personalization_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->perso.ck_data.data_ptr,
                          ((qmi_uim_personalization_params_type*)param_data_ptr)->ck_data.data_ptr,
                          ((qmi_uim_personalization_params_type*)param_data_ptr)->ck_data.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_GET_PHYSICAL_SLOT_MAPPING:
    case QCRIL_UIM_REQUEST_GET_SLOTS_STATUS:
      break;

    case QCRIL_UIM_REQUEST_SWITCH_SLOT_EXT:
      memcpy(&request_ptr->phy_slot_map,
             param_data_ptr,
             sizeof(qmi_uim_physical_slot_mapping_type));
      break;

    case QCRIL_UIM_REQUEST_SET_APDU_BEHAVIOR:
      memcpy(&request_ptr->set_apdu_behavior,
             param_data_ptr,
             sizeof(qmi_uim_set_apdu_behavior_params_type));
      break;

    case QCRIL_UIM_REQUEST_SAP_CONNECTION:
      memcpy(&request_ptr->sap_connection,
             param_data_ptr,
             sizeof(qmi_uim_sap_connection_params_type));
      break;

    case QCRIL_UIM_REQUEST_SAP_REQUEST:
      memcpy(&request_ptr->sap_request,
             param_data_ptr,
             sizeof(qmi_uim_sap_request_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->sap_request.apdu.data_ptr,
                          ((qmi_uim_sap_request_params_type*)param_data_ptr)->apdu.data_ptr,
                          ((qmi_uim_sap_request_params_type*)param_data_ptr)->apdu.data_len,
                          (unsigned char *));
      break;

      case QCRIL_UIM_REQUEST_GBA:
      memcpy(&request_ptr->gba_init,
             param_data_ptr,
             sizeof(qmi_uim_gba_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->gba_init.session_info.aid.data_ptr,
                          ((qmi_uim_gba_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_gba_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      QCRIL_UIM_DUPLICATE(request_ptr->gba_init.naf_id.fqdn.data_ptr,
                          ((qmi_uim_gba_params_type*)param_data_ptr)->naf_id.fqdn.data_ptr,
                          ((qmi_uim_gba_params_type*)param_data_ptr)->naf_id.fqdn.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_GBA_IMPI:
      memcpy(&request_ptr->gba_impi,
             param_data_ptr,
             sizeof(qmi_uim_gba_impi_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->gba_impi.session_info.aid.data_ptr,
                          ((qmi_uim_gba_impi_params_type*)param_data_ptr)->session_info.aid.data_ptr,
                          ((qmi_uim_gba_impi_params_type*)param_data_ptr)->session_info.aid.data_len,
                          (unsigned char *));
      break;

    case QCRIL_UIM_REQUEST_GET_TERMINAL_CAPABILITY:
      request_ptr->terminal_capability.slot =
        ((qmi_uim_terminal_capability_params_type*)param_data_ptr)->slot;
      break;

    case QCRIL_UIM_REQUEST_SET_TERMINAL_CAPABILITY:
      memcpy(&request_ptr->terminal_capability,
             param_data_ptr,
             sizeof(qmi_uim_terminal_capability_params_type));
      for (i = 0; i < ((qmi_uim_terminal_capability_params_type*)param_data_ptr)->num_tlvs && i < QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX; i++)
      {
        QCRIL_UIM_DUPLICATE(request_ptr->terminal_capability.tlvs[i].value.data_ptr,
                            ((qmi_uim_terminal_capability_params_type*)param_data_ptr)->tlvs[i].value.data_ptr,
                            ((qmi_uim_terminal_capability_params_type*)param_data_ptr)->tlvs[i].value.data_len,
                            (unsigned char *));
      }
      break;

    case QCRIL_UIM_REQUEST_REMOTE_UNLOCK:
      memcpy(&request_ptr->remote_unlock_req,
             param_data_ptr,
             sizeof(qmi_uim_remote_unlock_params_type));
      QCRIL_UIM_DUPLICATE(request_ptr->remote_unlock_req.simlock_data.data_ptr,
                       ((qmi_uim_remote_unlock_params_type*)param_data_ptr)->simlock_data.data_ptr,
                       ((qmi_uim_remote_unlock_params_type*)param_data_ptr)->simlock_data.data_len,
                       (unsigned char *));
      break;

   case QCRIL_UIM_REQUEST_ADD_PROFILE:
      memcpy(&request_ptr->add_profile,
             param_data_ptr,
             sizeof(qmi_uim_add_profile_req_type));
      QCRIL_UIM_DUPLICATE(request_ptr->add_profile.activation_code_ptr,
                          ((qmi_uim_add_profile_req_type*)param_data_ptr)->activation_code_ptr,
                          strlen(((qmi_uim_add_profile_req_type*)param_data_ptr)->activation_code_ptr) + 1,
                          (char *));
      QCRIL_UIM_DUPLICATE(request_ptr->add_profile.confirmation_code_ptr,
                          ((qmi_uim_add_profile_req_type*)param_data_ptr)->confirmation_code_ptr,
                          strlen(((qmi_uim_add_profile_req_type*)param_data_ptr)->confirmation_code_ptr) + 1,
                          (char *));
      break;

    case QCRIL_UIM_REQUEST_SET_SIM_PROFILE:
      memcpy(&request_ptr->set_profile,
             param_data_ptr,
             sizeof(qmi_uim_set_sim_profile_req_type));
      break;

    case QCRIL_UIM_REQUEST_GET_SIM_PROFILES:
      memcpy(&request_ptr->get_profiles,
             param_data_ptr,
             sizeof(qmi_uim_get_sim_profiles_type));
      break;

    case QCRIL_UIM_REQUEST_DELETE_PROFILE:
      memcpy(&request_ptr->delete_profile,
             param_data_ptr,
             sizeof(qmi_uim_delete_profile_req_type));
      break;

    case QCRIL_UIM_REQUEST_UPDATE_PROFILE_NICKNAME:
      memcpy(&request_ptr->update_nickname,
             param_data_ptr,
             sizeof(qmi_uim_update_profile_nickname_req_type));
      QCRIL_UIM_DUPLICATE(request_ptr->update_nickname.nickname,
                          ((qmi_uim_update_profile_nickname_req_type*)param_data_ptr)->nickname,
                          strlen(((qmi_uim_update_profile_nickname_req_type*)param_data_ptr)->nickname) + 1,
                          (char *));
      break;

    case QCRIL_UIM_REQUEST_EUICC_MEMORY_RESET:
      memcpy(&request_ptr->euicc_reset,
             param_data_ptr,
             sizeof(qmi_uim_euicc_memory_reset_req_type));
      break;

    case QCRIL_UIM_REQUEST_USER_CONSENT:
      memcpy(&request_ptr->user_consent,
             param_data_ptr,
             sizeof(qmi_uim_profile_user_consent_req_type));
      break;

    case QCRIL_UIM_REQUEST_GET_EID:
      memcpy(&request_ptr->get_eid,
             param_data_ptr,
             sizeof(qmi_uim_get_eid_req_type));
      break;

    case QCRIL_UIM_REQUEST_GET_SET_SERVER_ADDRESS:
      memcpy(&request_ptr->euicc_server_address,
             param_data_ptr,
             sizeof(qmi_uim_euicc_default_server_address_req_type));
      QCRIL_UIM_DUPLICATE(request_ptr->euicc_server_address.default_smdp_address,
                          ((qmi_uim_euicc_default_server_address_req_type*)param_data_ptr)->default_smdp_address,
                          strlen(((qmi_uim_euicc_default_server_address_req_type*)param_data_ptr)->default_smdp_address) + 1,
                          (char *));
      break;

    case QCRIL_UIM_REQUEST_GET_PROFILE_INFO:
      memcpy(&request_ptr->profile_info,
       param_data_ptr,
       sizeof(qmi_uim_get_profile_info_req_type));
      break;

    default:
      /* This should never happen! */
      return -1;
  }
  return 0;
} /* deep_copy */


static void deep_clean
(
  uint32_t    request_type,
  req_params *params_ptr
)
{
  uint32_t i = 0;

 /* Free the request */
  switch(request_type)
  {
    case QCRIL_UIM_REQUEST_READ_TRANSPARENT:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->read_transparent.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->read_transparent.file_id.path.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_READ_RECORD:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->read_record.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->read_record.file_id.path.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_WRITE_TRANSPARENT:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->write_transparent.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->write_transparent.file_id.path.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->write_transparent.data.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_WRITE_RECORD:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->write_record.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->write_record.file_id.path.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->write_record.data.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_RESPONSE:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->get_attributes.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->get_attributes.file_id.path.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_REFRESH_REGISTER:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->refresh_register.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->refresh_register.files_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_FDN:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->get_service_status.session_info.aid.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SET_FDN:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->set_service_status.session_info.aid.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_VERIFY_PIN:
    case QCRIL_UIM_REQUEST_VERIFY_PIN2:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->verify_pin.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->verify_pin.pin_data.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_UNBLOCK_PIN:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->unblock_pin.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->unblock_pin.puk_data.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->unblock_pin.new_pin_data.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_CHANGE_PIN:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->change_pin.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->change_pin.old_pin_data.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->change_pin.new_pin_data.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SET_PIN:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->set_pin.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->set_pin.pin_data.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_DEPERSO:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->deperso.ck_data.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_CHANGE_PROV_SESSION:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->change_prov_session.app_info.aid.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_AUTHENTICATE:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->authenticate.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->authenticate.auth_data.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SAP_REQUEST:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->sap_request.apdu.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_LOGICAL_CHANNEL:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->logical_channel.channel_data.aid.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_OPEN_LOGICAL_CHANNEL:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->open_logical_channel.aid.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SEND_APDU:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->send_apdu.apdu.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_ATR:
      break;

    case QCRIL_UIM_REQUEST_SEND_STATUS:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->send_status.session_info.aid.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GBA:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->gba_init.session_info.aid.data_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->gba_init.naf_id.fqdn.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GBA_IMPI:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->gba_impi.session_info.aid.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_PERSO:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->perso.ck_data.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SET_TERMINAL_CAPABILITY:
      for (i = 0; i < params_ptr->terminal_capability.num_tlvs && i < QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX; i++)
      {
        QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->terminal_capability.tlvs[i].value.data_ptr);
      }
      break;

    case QCRIL_UIM_REQUEST_REMOTE_UNLOCK:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->remote_unlock_req.simlock_data.data_ptr);
      break;

    case QCRIL_UIM_REQUEST_ADD_PROFILE:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->add_profile.activation_code_ptr);
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->add_profile.confirmation_code_ptr);
      break;

    case QCRIL_UIM_REQUEST_UPDATE_PROFILE_NICKNAME:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->update_nickname.nickname);
      break;

    case QCRIL_UIM_REQUEST_GET_SET_SERVER_ADDRESS:
      QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr->euicc_server_address.default_smdp_address);
      break;

    case QCRIL_UIM_REQUEST_POWER_UP:
    case QCRIL_UIM_REQUEST_POWER_DOWN:
    case QCRIL_UIM_REQUEST_RESELECT:
    case QCRIL_UIM_REQUEST_SUPPLY_VOLTAGE:
    case QCRIL_UIM_REQUEST_SAP_CONNECTION:
    case QCRIL_UIM_REQUEST_GET_PHYSICAL_SLOT_MAPPING:
    case QCRIL_UIM_REQUEST_SWITCH_SLOT_EXT:
    case QCRIL_UIM_REQUEST_SET_APDU_BEHAVIOR:
    case QCRIL_UIM_REQUEST_GET_TERMINAL_CAPABILITY:
    case QCRIL_UIM_REQUEST_GET_SLOTS_STATUS:
    case QCRIL_UIM_REQUEST_SET_SIM_PROFILE:
    case QCRIL_UIM_REQUEST_GET_SIM_PROFILES:
    case QCRIL_UIM_REQUEST_DELETE_PROFILE:
    case QCRIL_UIM_REQUEST_EUICC_MEMORY_RESET:
    case QCRIL_UIM_REQUEST_USER_CONSENT:
    case QCRIL_UIM_REQUEST_GET_EID:
    case QCRIL_UIM_REQUEST_GET_PROFILE_INFO:
    default:
      break;
  }
} /* deep clean */


/*---------------------------------------------------------------------------
  Class functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  UimQmiUimRequestMsg - UimQmiUimRequestMsg destructor
===========================================================================*/
UimQmiUimRequestMsg::~UimQmiUimRequestMsg()
{
  if (mMsgPtr != nullptr)
  {
    deep_clean(mMsgId, (req_params *)mMsgPtr);
    delete (req_params *)mMsgPtr;
    mMsgPtr = nullptr;
  }
  mOrigPtr = nullptr;
  mCbPtr = nullptr;
} /* UimQmiUimRequestMsg::~UimQmiUimRequestMsg() */


/*===========================================================================
  FUNCTION  UimQmiUimRequestMsg - UimQmiUimRequestMsg constructor
===========================================================================*/
UimQmiUimRequestMsg::UimQmiUimRequestMsg
(
  uint32_t                      msg_id,
  const void                   *dataPtr,
  const void                   *orig_ptr,
  qmi_uim_user_async_cb_type    cb_ptr
) : SolicitedSyncMessage<int>(get_class_message_id(), MESSAGE_EXPIRY_TIMEOUT)
{
  req_params  *req_data_ptr;

  mName = MESSAGE_NAME;
  mMsgId = msg_id;
  mOrigPtr = orig_ptr;
  mCbPtr   = cb_ptr;

  req_data_ptr = new req_params;
  if (req_data_ptr != nullptr)
  {
    memset(req_data_ptr, 0x00, sizeof(req_params));
    if (deep_copy(msg_id, dataPtr, req_data_ptr) != 0)
    {
      delete req_data_ptr;
      req_data_ptr = nullptr;
    }
  }
  mMsgPtr = (void *)req_data_ptr;
} /* UimQmiUimRequestMsg::UimQmiUimRequestMsg */
