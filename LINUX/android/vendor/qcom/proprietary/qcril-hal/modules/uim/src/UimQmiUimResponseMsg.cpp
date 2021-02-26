/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <string.h>
#include "modules/uim/qcril_uim_srvc.h"
#include "modules/uim/qcril_uim.h"
#include "modules/uim/qcril_uim_lpa.h"
#include "modules/uim/qcril_uim_sap.h"
#include "modules/uim/qcril_uim_gba.h"
#include "UimQmiUimResponseMsg.h"


/*---------------------------------------------------------------------------
  Internal functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  qcril_uim_copy_callback - Copy callback data
===========================================================================*/
static qcril_uim_callback_params_type* qcril_uim_copy_callback
(
  const qmi_uim_rsp_data_type        * rsp_data_ptr,
  const void                         * user_data,
  uint32                             * total_size_ptr,
  qmi_uim_rsp_id_type                * rsp_id_ptr
)
{
  qcril_uim_callback_params_type* out_ptr  = NULL;

  /* Sanity checks */
  if ((rsp_data_ptr == NULL) || (total_size_ptr == NULL) || (rsp_id_ptr == NULL))
  {
    return NULL;
  }

  *total_size_ptr = sizeof(qcril_uim_callback_params_type);
  *rsp_id_ptr = rsp_data_ptr->rsp_id;

  /* Following messages are the only responses that require a deep copy and
     additional memory for it */
  switch(rsp_data_ptr->rsp_id)
  {
    case QMI_UIM_SRVC_READ_TRANSPARENT_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.read_transparent_rsp.content.data_len;
      break;

    case QMI_UIM_SRVC_READ_RECORD_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.read_record_rsp.content.data_len;
      break;

    case QMI_UIM_SRVC_GET_FILE_ATTRIBUTES_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.get_file_attributes_rsp.raw_value.data_len;
      break;

    case QMI_UIM_SRVC_SEND_APDU_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.send_apdu_rsp.apdu_response.data_len;
      break;

    case QMI_UIM_SRVC_AUTHENTICATE_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.authenticate_rsp.auth_response.data_len;
      break;

    case QMI_UIM_SRVC_SET_PIN_PROTECTION_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.set_pin_protection_rsp.encr_pin_data.data_len;
      break;

    case QMI_UIM_SRVC_VERIFY_PIN_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.verify_pin_rsp.encr_pin_data.data_len;
      break;

    case QMI_UIM_SRVC_UNBLOCK_PIN_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.unblock_pin_rsp.encr_pin_data.data_len;
      break;

    case QMI_UIM_SRVC_CHANGE_PIN_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.change_pin_rsp.encr_pin_data.data_len;
      break;

    case QMI_UIM_SRVC_GET_ATR_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.get_atr_rsp.atr_response.data_len;
      break;

    case QMI_UIM_SRVC_LOGICAL_CHANNEL_RSP_MSG:
    case QMI_UIM_SRVC_OPEN_LOGICAL_CHANNEL_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.logical_channel_rsp.select_response.data_len;
      break;

    case QMI_UIM_SRVC_SEND_STATUS_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.send_status_rsp.status_response.data_len;
      break;

    case QMI_UIM_SRVC_RESELECT_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.reselect_rsp.select_response.data_len;
      break;

    case QMI_UIM_SRVC_SAP_REQUEST_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.sap_response_rsp.sap_response.data_len;
      break;

    case QMI_UIM_SRVC_GBA_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.gba_rsp.ks_naf.data_len +
                           rsp_data_ptr->rsp_data.gba_rsp.b_tid.data_len +
                           rsp_data_ptr->rsp_data.gba_rsp.key_lifetime.data_len;
      break;

    case QMI_UIM_SRVC_GBA_IMPI_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.gba_impi_rsp.impi.data_len;
      break;

    case QMI_UIM_SRVC_TERMINAL_CAPABILITY_RSP_MSG:
      {
        uint32_t     i = 0;
        for(i = 0; i < rsp_data_ptr->rsp_data.terminal_capability_rsp.num_tlvs && i < QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX; i++)
        {
          *total_size_ptr += rsp_data_ptr->rsp_data.terminal_capability_rsp.tlvs[i].value.data_len;
        }
      }
      break;

    case QMI_UIM_SRVC_REMOTE_UNLOCK_RSP_MSG:
      if (rsp_data_ptr->rsp_data.remote_unlock_rsp.operation !=
          QMI_UIM_REMOTE_UNLOCK_GET_MAX_SUPPORTED_VERSION)
      {
        *total_size_ptr += rsp_data_ptr->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_len;
      }
      break;

    case QMI_UIM_SRVC_GET_EID_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.eid_resp.eid_len;
      break;

    case QMI_UIM_SRVC_GET_SET_SERVER_ADDRESS_RSP_MSG:
      if (rsp_data_ptr->rsp_data.euicc_server_address_rsp.smds_address_ptr != nullptr)
      {
        *total_size_ptr += (strlen(rsp_data_ptr->rsp_data.euicc_server_address_rsp.smds_address_ptr) + 1);
      }
      if (rsp_data_ptr->rsp_data.euicc_server_address_rsp.smdp_address_ptr != nullptr)
      {
        *total_size_ptr += (strlen(rsp_data_ptr->rsp_data.euicc_server_address_rsp.smdp_address_ptr) + 1);
      }
      break;

    case QMI_UIM_SRVC_GET_PROFILE_INFO_RSP_MSG:
      *total_size_ptr += rsp_data_ptr->rsp_data.profile_info_rsp.iccid_len;
      if (rsp_data_ptr->rsp_data.profile_info_rsp.nickname != nullptr)
      {
        *total_size_ptr += (strlen(rsp_data_ptr->rsp_data.profile_info_rsp.nickname) + 1);
      }
      if (rsp_data_ptr->rsp_data.profile_info_rsp.spn != nullptr)
      {
        *total_size_ptr += (strlen(rsp_data_ptr->rsp_data.profile_info_rsp.spn) + 1);
      }
      if (rsp_data_ptr->rsp_data.profile_info_rsp.name != nullptr)
      {
        *total_size_ptr += (strlen(rsp_data_ptr->rsp_data.profile_info_rsp.name) + 1);
      }
      *total_size_ptr += rsp_data_ptr->rsp_data.profile_info_rsp.icon_len;
      break;

    default:
      break;
  }

  out_ptr = (qcril_uim_callback_params_type*)qcril_malloc(*total_size_ptr);
  if (out_ptr == NULL)
  {
    return NULL;
  }

  memset(out_ptr, 0, *total_size_ptr);

  /* Copy the response parameters */
  memcpy(&out_ptr->qmi_rsp_data, rsp_data_ptr, sizeof(qmi_uim_rsp_data_type));

  /* Update original request data */
  out_ptr->orig_req_data = user_data;

  /* Deep copy */
  switch(rsp_data_ptr->rsp_id)
  {
    case QMI_UIM_SRVC_READ_TRANSPARENT_RSP_MSG:
      out_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr =
        (uint8*)&out_ptr->payload;
      memcpy(out_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr,
             rsp_data_ptr->rsp_data.read_transparent_rsp.content.data_ptr,
             rsp_data_ptr->rsp_data.read_transparent_rsp.content.data_len);
      break;

    case QMI_UIM_SRVC_READ_RECORD_RSP_MSG:
      out_ptr->qmi_rsp_data.rsp_data.read_record_rsp.content.data_ptr =
        (uint8*)&out_ptr->payload;
      memcpy(out_ptr->qmi_rsp_data.rsp_data.read_record_rsp.content.data_ptr,
             rsp_data_ptr->rsp_data.read_record_rsp.content.data_ptr,
             rsp_data_ptr->rsp_data.read_record_rsp.content.data_len);
      break;

    case QMI_UIM_SRVC_GET_FILE_ATTRIBUTES_RSP_MSG:
      out_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.raw_value.data_ptr =
        (uint8*)&out_ptr->payload;
      memcpy(out_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.raw_value.data_ptr,
             rsp_data_ptr->rsp_data.get_file_attributes_rsp.raw_value.data_ptr,
             rsp_data_ptr->rsp_data.get_file_attributes_rsp.raw_value.data_len);
      break;

    case QMI_UIM_SRVC_SEND_APDU_RSP_MSG:
      out_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_ptr =
        (uint8*)&out_ptr->payload;
      memcpy(out_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_ptr,
             rsp_data_ptr->rsp_data.send_apdu_rsp.apdu_response.data_ptr,
             rsp_data_ptr->rsp_data.send_apdu_rsp.apdu_response.data_len);
      break;

    case QMI_UIM_SRVC_AUTHENTICATE_RSP_MSG:
      out_ptr->qmi_rsp_data.rsp_data.authenticate_rsp.auth_response.data_ptr =
        (uint8*)&out_ptr->payload;
      memcpy(out_ptr->qmi_rsp_data.rsp_data.authenticate_rsp.auth_response.data_ptr,
             rsp_data_ptr->rsp_data.authenticate_rsp.auth_response.data_ptr,
             rsp_data_ptr->rsp_data.authenticate_rsp.auth_response.data_len);
      break;

    case QMI_UIM_SRVC_SET_PIN_PROTECTION_RSP_MSG:
      if(rsp_data_ptr->rsp_data.set_pin_protection_rsp.encr_pin_data.data_len > 0)
      {
        out_ptr->qmi_rsp_data.rsp_data.set_pin_protection_rsp.encr_pin_data.data_ptr =
          (uint8*)&out_ptr->payload;
        memcpy(out_ptr->qmi_rsp_data.rsp_data.set_pin_protection_rsp.encr_pin_data.data_ptr,
               rsp_data_ptr->rsp_data.set_pin_protection_rsp.encr_pin_data.data_ptr,
               rsp_data_ptr->rsp_data.set_pin_protection_rsp.encr_pin_data.data_len);
      }
      break;

    case QMI_UIM_SRVC_VERIFY_PIN_RSP_MSG:
      if(rsp_data_ptr->rsp_data.verify_pin_rsp.encr_pin_data.data_len > 0)
      {
        out_ptr->qmi_rsp_data.rsp_data.verify_pin_rsp.encr_pin_data.data_ptr =
          (uint8*)&out_ptr->payload;
        memcpy(out_ptr->qmi_rsp_data.rsp_data.verify_pin_rsp.encr_pin_data.data_ptr,
               rsp_data_ptr->rsp_data.verify_pin_rsp.encr_pin_data.data_ptr,
               rsp_data_ptr->rsp_data.verify_pin_rsp.encr_pin_data.data_len);
      }
      break;

    case QMI_UIM_SRVC_UNBLOCK_PIN_RSP_MSG:
      if(rsp_data_ptr->rsp_data.unblock_pin_rsp.encr_pin_data.data_len > 0)
      {
        out_ptr->qmi_rsp_data.rsp_data.unblock_pin_rsp.encr_pin_data.data_ptr =
          (uint8*)&out_ptr->payload;
        memcpy(out_ptr->qmi_rsp_data.rsp_data.unblock_pin_rsp.encr_pin_data.data_ptr,
               rsp_data_ptr->rsp_data.unblock_pin_rsp.encr_pin_data.data_ptr,
               rsp_data_ptr->rsp_data.unblock_pin_rsp.encr_pin_data.data_len);
      }
      break;

    case QMI_UIM_SRVC_CHANGE_PIN_RSP_MSG:
      if(rsp_data_ptr->rsp_data.change_pin_rsp.encr_pin_data.data_len > 0)
      {
        out_ptr->qmi_rsp_data.rsp_data.change_pin_rsp.encr_pin_data.data_ptr =
          (uint8*)&out_ptr->payload;
        memcpy(out_ptr->qmi_rsp_data.rsp_data.change_pin_rsp.encr_pin_data.data_ptr,
               rsp_data_ptr->rsp_data.change_pin_rsp.encr_pin_data.data_ptr,
               rsp_data_ptr->rsp_data.change_pin_rsp.encr_pin_data.data_len);
      }
      break;

    case QMI_UIM_SRVC_GET_ATR_RSP_MSG:
      out_ptr->qmi_rsp_data.rsp_data.get_atr_rsp.atr_response.data_ptr =
        (uint8*)&out_ptr->payload;
      memcpy(out_ptr->qmi_rsp_data.rsp_data.get_atr_rsp.atr_response.data_ptr,
             rsp_data_ptr->rsp_data.get_atr_rsp.atr_response.data_ptr,
             rsp_data_ptr->rsp_data.get_atr_rsp.atr_response.data_len);
      break;

    case QMI_UIM_SRVC_LOGICAL_CHANNEL_RSP_MSG:
    case QMI_UIM_SRVC_OPEN_LOGICAL_CHANNEL_RSP_MSG:
      out_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.select_response.data_ptr =
        (uint8*)&out_ptr->payload;
      memcpy(out_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.select_response.data_ptr,
             rsp_data_ptr->rsp_data.logical_channel_rsp.select_response.data_ptr,
             rsp_data_ptr->rsp_data.logical_channel_rsp.select_response.data_len);
      break;

    case QMI_UIM_SRVC_SEND_STATUS_RSP_MSG:
      if (rsp_data_ptr->rsp_data.send_status_rsp.status_response.data_len > 0)
      {
        out_ptr->qmi_rsp_data.rsp_data.send_status_rsp.status_response.data_ptr =
          (uint8*)&out_ptr->payload;
        memcpy(out_ptr->qmi_rsp_data.rsp_data.send_status_rsp.status_response.data_ptr,
               rsp_data_ptr->rsp_data.send_status_rsp.status_response.data_ptr,
               rsp_data_ptr->rsp_data.send_status_rsp.status_response.data_len);
      }
      break;

    case QMI_UIM_SRVC_RESELECT_RSP_MSG:
      if (rsp_data_ptr->rsp_data.reselect_rsp.select_response.data_len > 0)
      {
        out_ptr->qmi_rsp_data.rsp_data.reselect_rsp.select_response.data_ptr =
          (uint8*)&out_ptr->payload;
        memcpy(out_ptr->qmi_rsp_data.rsp_data.reselect_rsp.select_response.data_ptr,
               rsp_data_ptr->rsp_data.reselect_rsp.select_response.data_ptr,
               rsp_data_ptr->rsp_data.reselect_rsp.select_response.data_len);
      }
      break;

    case QMI_UIM_SRVC_SAP_REQUEST_RSP_MSG:
      out_ptr->qmi_rsp_data.rsp_data.sap_response_rsp.sap_response.data_ptr =
        (uint8*)&out_ptr->payload;
      memcpy(out_ptr->qmi_rsp_data.rsp_data.sap_response_rsp.sap_response.data_ptr,
             rsp_data_ptr->rsp_data.sap_response_rsp.sap_response.data_ptr,
             rsp_data_ptr->rsp_data.sap_response_rsp.sap_response.data_len);
      break;

    case QMI_UIM_SRVC_GBA_RSP_MSG:
      {
        uint8  * offset_ptr = (uint8*)&out_ptr->payload;
        out_ptr->qmi_rsp_data.rsp_data.gba_rsp.ks_naf.data_ptr = offset_ptr;
        memcpy(out_ptr->qmi_rsp_data.rsp_data.gba_rsp.ks_naf.data_ptr,
               rsp_data_ptr->rsp_data.gba_rsp.ks_naf.data_ptr,
               rsp_data_ptr->rsp_data.gba_rsp.ks_naf.data_len);

        offset_ptr += rsp_data_ptr->rsp_data.gba_rsp.ks_naf.data_len;
        out_ptr->qmi_rsp_data.rsp_data.gba_rsp.b_tid.data_ptr = offset_ptr;
        memcpy(out_ptr->qmi_rsp_data.rsp_data.gba_rsp.b_tid.data_ptr,
               rsp_data_ptr->rsp_data.gba_rsp.b_tid.data_ptr,
               rsp_data_ptr->rsp_data.gba_rsp.b_tid.data_len);

        offset_ptr += rsp_data_ptr->rsp_data.gba_rsp.b_tid.data_len;
        out_ptr->qmi_rsp_data.rsp_data.gba_rsp.key_lifetime.data_ptr = offset_ptr;
        memcpy(out_ptr->qmi_rsp_data.rsp_data.gba_rsp.key_lifetime.data_ptr,
               rsp_data_ptr->rsp_data.gba_rsp.key_lifetime.data_ptr,
               rsp_data_ptr->rsp_data.gba_rsp.key_lifetime.data_len);
      }
      break;

    case QMI_UIM_SRVC_GBA_IMPI_RSP_MSG:
      out_ptr->qmi_rsp_data.rsp_data.gba_impi_rsp.impi.data_ptr =
        (uint8*)&out_ptr->payload;
      memcpy(out_ptr->qmi_rsp_data.rsp_data.gba_impi_rsp.impi.data_ptr,
             rsp_data_ptr->rsp_data.gba_impi_rsp.impi.data_ptr,
             rsp_data_ptr->rsp_data.gba_impi_rsp.impi.data_len);
      break;

    case QMI_UIM_SRVC_TERMINAL_CAPABILITY_RSP_MSG:
      {
        uint32_t     i          = 0;
        uint8_t    * offset_ptr = (uint8*)&out_ptr->payload;

        for(i = 0; i < rsp_data_ptr->rsp_data.terminal_capability_rsp.num_tlvs && i < QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX; i++)
        {
          if(rsp_data_ptr->rsp_data.terminal_capability_rsp.tlvs[i].value.data_len > 0 &&
             rsp_data_ptr->rsp_data.terminal_capability_rsp.tlvs[i].value.data_ptr)
          {
            out_ptr->qmi_rsp_data.rsp_data.terminal_capability_rsp.tlvs[i].value.data_ptr = offset_ptr;
            memcpy(out_ptr->qmi_rsp_data.rsp_data.terminal_capability_rsp.tlvs[i].value.data_ptr,
                   rsp_data_ptr->rsp_data.terminal_capability_rsp.tlvs[i].value.data_ptr,
                   rsp_data_ptr->rsp_data.terminal_capability_rsp.tlvs[i].value.data_len);
            offset_ptr += rsp_data_ptr->rsp_data.terminal_capability_rsp.tlvs[i].value.data_len;
          }
        }
      }
      break;

    case QMI_UIM_SRVC_REMOTE_UNLOCK_RSP_MSG:
      if (rsp_data_ptr->rsp_data.remote_unlock_rsp.operation !=
          QMI_UIM_REMOTE_UNLOCK_GET_MAX_SUPPORTED_VERSION)
      {
        out_ptr->qmi_rsp_data.rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_ptr =
          (uint8*)&out_ptr->payload;
        if (out_ptr->qmi_rsp_data.rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_ptr != NULL &&
            rsp_data_ptr->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_ptr != NULL &&
            rsp_data_ptr->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_len > 0)
        {
          memcpy(out_ptr->qmi_rsp_data.rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_ptr,
                 rsp_data_ptr->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_ptr,
                 rsp_data_ptr->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_len);
        }
      }
      break;

    case QMI_UIM_SRVC_GET_EID_RSP_MSG:
      if (rsp_data_ptr->rsp_data.eid_resp.eid_len > 0)
      {
        out_ptr->qmi_rsp_data.rsp_data.eid_resp.eid_ptr =
          (uint8*)&out_ptr->payload;
        memcpy(out_ptr->qmi_rsp_data.rsp_data.eid_resp.eid_ptr,
               rsp_data_ptr->rsp_data.eid_resp.eid_ptr,
               rsp_data_ptr->rsp_data.eid_resp.eid_len);
      }
      else
      {
        out_ptr->qmi_rsp_data.rsp_data.eid_resp.eid_ptr = nullptr;
      }
      break;

    case QMI_UIM_SRVC_GET_SET_SERVER_ADDRESS_RSP_MSG:
      {
        uint8_t    * offset_ptr = (uint8*)&out_ptr->payload;

        if (rsp_data_ptr->rsp_data.euicc_server_address_rsp.smds_address_ptr != nullptr)
        {
          out_ptr->qmi_rsp_data.rsp_data.euicc_server_address_rsp.smds_address_ptr = (char *)offset_ptr;
          strlcpy(out_ptr->qmi_rsp_data.rsp_data.euicc_server_address_rsp.smds_address_ptr,
                  rsp_data_ptr->rsp_data.euicc_server_address_rsp.smds_address_ptr,
                  (strlen(rsp_data_ptr->rsp_data.euicc_server_address_rsp.smds_address_ptr) + 1));

          offset_ptr += (strlen(rsp_data_ptr->rsp_data.euicc_server_address_rsp.smds_address_ptr) + 1);
        }
        else
        {
          out_ptr->qmi_rsp_data.rsp_data.euicc_server_address_rsp.smds_address_ptr = nullptr;
        }

        if (rsp_data_ptr->rsp_data.euicc_server_address_rsp.smdp_address_ptr != nullptr)
        {
          out_ptr->qmi_rsp_data.rsp_data.euicc_server_address_rsp.smdp_address_ptr = (char *)offset_ptr;
          strlcpy(out_ptr->qmi_rsp_data.rsp_data.euicc_server_address_rsp.smdp_address_ptr,
                  rsp_data_ptr->rsp_data.euicc_server_address_rsp.smdp_address_ptr,
                  (strlen(rsp_data_ptr->rsp_data.euicc_server_address_rsp.smdp_address_ptr) + 1));

          offset_ptr += (strlen(rsp_data_ptr->rsp_data.euicc_server_address_rsp.smdp_address_ptr) + 1);
        }
        else
        {
          out_ptr->qmi_rsp_data.rsp_data.euicc_server_address_rsp.smdp_address_ptr = nullptr;
        }
      }
      break;

    case QMI_UIM_SRVC_GET_PROFILE_INFO_RSP_MSG:
      {
        uint8_t    * offset_ptr = (uint8*)&out_ptr->payload;

        if (rsp_data_ptr->rsp_data.profile_info_rsp.iccid_len > 0)
        {
          out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.iccid_ptr =
            (uint8*)&out_ptr->payload;
          memcpy(out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.iccid_ptr,
                 rsp_data_ptr->rsp_data.profile_info_rsp.iccid_ptr,
                 rsp_data_ptr->rsp_data.profile_info_rsp.iccid_len);

          offset_ptr += rsp_data_ptr->rsp_data.profile_info_rsp.iccid_len;
        }
        else
        {
          out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.iccid_ptr = nullptr;
        }

        if (rsp_data_ptr->rsp_data.profile_info_rsp.nickname != nullptr)
        {
          out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.nickname = (char *)offset_ptr;
          strlcpy(out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.nickname,
                  rsp_data_ptr->rsp_data.profile_info_rsp.nickname,
                  (strlen(rsp_data_ptr->rsp_data.profile_info_rsp.nickname) + 1));

          offset_ptr += (strlen(rsp_data_ptr->rsp_data.profile_info_rsp.nickname) + 1);
        }
        else
        {
          out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.nickname = nullptr;
        }

        if (rsp_data_ptr->rsp_data.profile_info_rsp.spn != nullptr)
        {
          out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.spn = (char *)offset_ptr;
          strlcpy(out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.spn,
                  rsp_data_ptr->rsp_data.profile_info_rsp.spn,
                  (strlen(rsp_data_ptr->rsp_data.profile_info_rsp.spn) + 1));

          offset_ptr += (strlen(rsp_data_ptr->rsp_data.profile_info_rsp.spn) + 1);
        }
        else
        {
          out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.spn = nullptr;
        }

        if (rsp_data_ptr->rsp_data.profile_info_rsp.name != nullptr)
        {
          out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.name = (char *)offset_ptr;
          strlcpy(out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.name,
                  rsp_data_ptr->rsp_data.profile_info_rsp.name,
                  (strlen(rsp_data_ptr->rsp_data.profile_info_rsp.name) + 1));

          offset_ptr += (strlen(rsp_data_ptr->rsp_data.profile_info_rsp.name) + 1);
        }
        else
        {
          out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.name = nullptr;
        }

        if (rsp_data_ptr->rsp_data.profile_info_rsp.icon_len > 0)
        {
          out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.icon_data_ptr =
            (uint8*)&out_ptr->payload;
          memcpy(out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.icon_data_ptr,
                 rsp_data_ptr->rsp_data.profile_info_rsp.icon_data_ptr,
                 rsp_data_ptr->rsp_data.profile_info_rsp.icon_len);

          offset_ptr += rsp_data_ptr->rsp_data.profile_info_rsp.icon_len;
        }
        else
        {
          out_ptr->qmi_rsp_data.rsp_data.profile_info_rsp.icon_data_ptr = nullptr;
        }
      }
      break;

    default:
      break;
  }

  return out_ptr;
} /* qcril_uim_copy_callback */


/*---------------------------------------------------------------------------
  Class functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  UimQmiUimResponseMsg - UimQmiUimResponseMsg destructor
===========================================================================*/
UimQmiUimResponseMsg::~UimQmiUimResponseMsg()
{
  if (mRspData != nullptr)
  {
    qcril_free(mRspData);
    mRspData = nullptr;
  }
  mRspSize = 0;
} /* UimQmiUimResponseMsg::~UimQmiUimResponseMsg() */


/*===========================================================================
  FUNCTION  UimQmiUimResponseMsg - UimQmiUimResponseMsg constructor
===========================================================================*/
UimQmiUimResponseMsg::UimQmiUimResponseMsg
(
  qmi_uim_rsp_data_type * rsp_data,
  void                  * orig_ptr
) : SolicitedMessage<void>(get_class_message_id())
{
  mRspData = nullptr;
  mRspSize = 0;
  mName    = MESSAGE_NAME;
  mOrigPtr = orig_ptr;
  mRspId   = QMI_UIM_SRVC_NONE_RSP_MSG;
  mRspData = (void *)qcril_uim_copy_callback(rsp_data, orig_ptr, &mRspSize, &mRspId);
} /* UimQmiUimResponseMsg::UimQmiUimResponseMsg */

