/*****************************************************************************
  @file    qcril_qmi_imsa.c
  @brief   qcril qmi - IMS Application

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI IMS Application.

  ---------------------------------------------------------------------------

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  ---------------------------------------------------------------------------
******************************************************************************/

#include "qcril_qmi_imsa.h"
#include "qcril_qmi_client.h"
#include "qcril_log.h"
#include "ip_multimedia_subsystem_application_v01.h"
#include "ip_multimedia_subsystem_settings_v01.h"
#include "qcril_qmi_ims_radio_agent.h"
#include "qcril_qmi_ims_misc.h"
#include "qcril_reqlist.h"
#include "qcril_am.h"
#include "qcril_qmi_nas.h"
#include "qcril_qmi_imss.h"
#include "qcril_arb.h"
#include "qcril_cm_ss.h"

typedef struct
{
   pthread_mutex_t       imsa_info_lock_mutex;
   boolean inited;
   boolean ims_status_change_registered;
   uint8_t ims_registered_valid;
   uint8_t ims_registered;
   uint8_t ims_registration_error_code_valid;
   uint16_t ims_registration_error_code;
   uint8_t ims_registration_error_string_valid;
   char *ims_registration_error_string;
   uint8_t ims_srv_status_valid;
   qcril_qmi_imsa_srv_status_type ims_srv_status;
   uint8_t registration_network_valid;
   uint8_t registration_network;
   uint8_t uri_list_valid;
   uint32_t uri_list_len;
   p_associated_uri_v01 URI_List[IMSA_MAX_NUM_URIS_V01];
   uint32_t ims_capability_len;
   dms_ims_capability_type_v01 ims_capability[QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01];
   uint8_t max_ims_instances_valid;
   uint8_t max_ims_instances;
   boolean ims_wlan_registration_status;
} qcril_qmi_imsa_info_type;

qcril_qmi_imsa_info_type qcril_qmi_imsa_info;

static void qcril_qmi_imsa_store_service_status_cl(
    uint8_t sms_service_status_valid,
    imsa_service_status_enum_v01 sms_service_status,
    uint8_t voip_service_status_valid,
    imsa_service_status_enum_v01 voip_service_status,
    uint8_t vt_service_status_valid,
    imsa_service_status_enum_v01 vt_service_status,
    uint8_t sms_service_rat_valid,
    imsa_service_rat_enum_v01 sms_service_rat,
    uint8_t voip_service_rat_valid,
    imsa_service_rat_enum_v01 voip_service_rat,
    uint8_t vt_service_rat_valid,
    imsa_service_rat_enum_v01 vt_service_rat,
    uint8_t ut_service_status_valid,
    imsa_service_status_enum_v01 ut_service_status,
    uint8_t ut_service_rat_valid,
    imsa_service_rat_enum_v01 ut_service_rat
);

static void qcril_qmi_imsa_store_registration_status(
    uint8_t ims_registered_valid,
    uint8_t ims_registered,
    uint8_t ims_registration_error_code_valid,
    uint16_t ims_registration_error_code,
    uint8_t ims_registration_error_string_valid,
    char *ims_registration_error_string,
    uint8_t registration_network_valid,
    uint8_t registration_network,
    uint8_t uri_list_valid,
    p_associated_uri_v01 *URI_List,
    uint32_t uri_list_len
);

static void qcril_qmi_imsa_get_ims_sub_configs(uint64_t* ims_sub_configs, uint8_t max_cap_len);

static void qcril_qmi_imsa_map_imsa_block_status_to_ims_block_status
(
    imsa_ims_failure_reason_type_v01* imsa_block_status,
    ims_BlockStatus* ims_block_status
);

boolean qcril_qmi_imsa_update_wlan_registration_status();
//===========================================================================
// qcril_qmi_imsa_pre_init
//===========================================================================
void qcril_qmi_imsa_pre_init(void)
{
   pthread_mutexattr_t mutex_attr;
   pthread_mutexattr_init( &mutex_attr );
   pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
   pthread_mutex_init( &qcril_qmi_imsa_info.imsa_info_lock_mutex, &mutex_attr);
   pthread_mutexattr_destroy(&mutex_attr);
} // qcril_qmi_imsa_pre_init

//===========================================================================
// qcril_qmi_imsa_info_lock
//===========================================================================
void qcril_qmi_imsa_info_lock()
{
  pthread_mutex_lock( &qcril_qmi_imsa_info.imsa_info_lock_mutex );
} // qcril_qmi_imsa_info_lock

//===========================================================================
// qcril_qmi_imsa_info_unlock
//===========================================================================
void qcril_qmi_imsa_info_unlock()
{
  pthread_mutex_unlock( &qcril_qmi_imsa_info.imsa_info_lock_mutex );
} // qcril_qmi_imsa_info_unlock

//===========================================================================
// qcril_qmi_imsa_set_init_state
//===========================================================================
void qcril_qmi_imsa_set_init_state(boolean inited)
{
   qcril_qmi_imsa_info_lock();
   qcril_qmi_imsa_info.inited = inited;
   qcril_qmi_imsa_info_unlock();
} // qcril_qmi_imsa_set_init_state

//===========================================================================
// qcril_qmi_imsa_init
//===========================================================================
void qcril_qmi_imsa_init()
{
   QCRIL_LOG_FUNC_ENTRY();

   qcril_qmi_imsa_info_lock();
   qcril_qmi_imsa_info.inited = TRUE;
   qcril_qmi_imsa_info.ims_registered_valid = FALSE;
   qcril_qmi_imsa_info.registration_network_valid = FALSE;
   qcril_qmi_imsa_info.ims_status_change_registered = FALSE;
   qcril_qmi_imsa_info.ims_srv_status_valid = FALSE;
   qcril_qmi_imsa_info.uri_list_valid = FALSE;
   qcril_qmi_imsa_info.ims_wlan_registration_status = FALSE;
   qcril_qmi_imsa_info_unlock();

   // register for indication
   qmi_client_error_type ret = QMI_NO_ERR;
   imsa_ind_reg_req_msg_v01 ind_reg_req;
   imsa_ind_reg_rsp_msg_v01 *ind_reg_resp_ptr = qcril_malloc(sizeof(imsa_ind_reg_rsp_msg_v01));

   if (ind_reg_resp_ptr)
   {
      memset(&ind_reg_req, 0, sizeof(ind_reg_req));
      ind_reg_req.reg_status_config_valid = TRUE;
      ind_reg_req.reg_status_config = 1;  // enable
      ind_reg_req.service_status_config_valid = TRUE;
      ind_reg_req.service_status_config = 1;  // enable
      ind_reg_req.rat_handover_config_valid = TRUE;
      ind_reg_req.rat_handover_config = 1;  // enable
      ind_reg_req.geo_location_status_config_valid = TRUE;
      ind_reg_req.geo_location_status_config = 1; // enable
      ind_reg_req.vowifiprofile_status_config_valid = TRUE;
      ind_reg_req.vowifiprofile_status_config = 1; // enable
      ind_reg_req.ims_block_status_config_valid = TRUE;
      ind_reg_req.ims_block_status_config = 1;

      ret = qcril_qmi_client_send_msg_async( QCRIL_QMI_CLIENT_IMSA,
                                             QMI_IMSA_IND_REG_REQ_V01,
                                             (void*) &ind_reg_req,
                                             sizeof(ind_reg_req),
                                             (void*) ind_reg_resp_ptr,
                                             sizeof(*ind_reg_resp_ptr),
                                             (void*)0
                                            );
      if (QMI_NO_ERR != ret)
      {
         QCRIL_LOG_ERROR("registration for indication failed %d", ret);
         qcril_free(ind_reg_resp_ptr);
      }
   }
   else
   {
      QCRIL_LOG_ERROR("ind_reg_resp_ptr malloc failed");
   }

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_init

//===========================================================================
// qcril_qmi_imsa_cleanup
//===========================================================================
void qcril_qmi_imsa_cleanup()
{
   QCRIL_LOG_FUNC_ENTRY();

   qcril_qmi_imsa_info_lock();

   qcril_qmi_imsa_store_registration_status(
       TRUE, IMSA_STATUS_NOT_REGISTERED_V01,        // ims_registered
       FALSE, 0,                                    // ims_registration_error_code
       FALSE, NULL,                                 // ims_registration_error_string
       FALSE, ims_RadioTechType_RADIO_TECH_UNKNOWN, // registration_network
       FALSE, NULL, 0);                             // URI_List


   qcril_qmi_imsa_store_service_status_cl(
       TRUE, IMSA_NO_SERVICE_V01,  // SMS SERVICE_STATUS
       TRUE, IMSA_NO_SERVICE_V01,  // VOIP SERVICE_STATUS
       TRUE, IMSA_NO_SERVICE_V01,  // VT SERVICE_STATUS
       FALSE, IMSA_WWAN_V01,       // SMS SERVICE_RAT
       FALSE, IMSA_WWAN_V01,       // VOIP SERVICE_RAT
       FALSE, IMSA_WWAN_V01,       // VT SERVICE_RAT
       TRUE, IMSA_NO_SERVICE_V01,  // UT SERVICE_STATUS
       FALSE, IMSA_WWAN_V01);      // UT SERVICE_RAT

   qcril_qmi_imsa_info_unlock();

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_cleanup

//===========================================================================
// qcril_qmi_imsa_store_registration_status
//===========================================================================
void qcril_qmi_imsa_store_registration_status(
      uint8_t ims_registered_valid,
      uint8_t ims_registered,
      uint8_t ims_registration_error_code_valid,
      uint16_t ims_registration_error_code,
      uint8_t ims_registration_error_string_valid,
      char *ims_registration_error_string,
      uint8_t registration_network_valid,
      uint8_t registration_network,
      uint8_t uri_list_valid,
      p_associated_uri_v01 *URI_List,
      uint32_t uri_list_len
      )
{
      size_t len;
      qcril_qmi_imsa_info.ims_registered_valid = ims_registered_valid;
      qcril_qmi_imsa_info.ims_registered = ims_registered;

      qcril_qmi_imsa_info.registration_network_valid = registration_network_valid;
      qcril_qmi_imsa_info.registration_network = registration_network;

      qcril_qmi_imsa_info.ims_registration_error_code_valid = ims_registration_error_code_valid;
      qcril_qmi_imsa_info.ims_registration_error_code = ims_registration_error_code;

      if(qcril_qmi_imsa_info.ims_registration_error_string != NULL)
      {
        qcril_free(qcril_qmi_imsa_info.ims_registration_error_string);
        qcril_qmi_imsa_info.ims_registration_error_string = NULL;
      }
      qcril_qmi_imsa_info.ims_registration_error_string_valid = ims_registration_error_string_valid;
      if (ims_registration_error_string_valid && ims_registration_error_string)
      {
        len = strlen(ims_registration_error_string) + 1;
        qcril_qmi_imsa_info.ims_registration_error_string = qcril_malloc(len);
        if(qcril_qmi_imsa_info.ims_registration_error_string != NULL)
        {
          strlcpy(qcril_qmi_imsa_info.ims_registration_error_string, ims_registration_error_string,
              len);
        }
      }

      qcril_qmi_imsa_info.uri_list_valid = uri_list_valid;
      if(uri_list_valid == FALSE)
      {
        qcril_qmi_imsa_info.uri_list_len = 0;
      }
      else
      {
        memset(qcril_qmi_imsa_info.URI_List, 0,
              sizeof(p_associated_uri_v01) * IMSA_MAX_NUM_URIS_V01);
        qcril_qmi_imsa_info.uri_list_len = uri_list_len;
        if(uri_list_len > 0)
        {
          memcpy(qcril_qmi_imsa_info.URI_List, URI_List,
                  uri_list_len * sizeof(p_associated_uri_v01));
        }
      }
}
//===========================================================================
// qcril_qmi_imsa_get_ims_registration_info
//===========================================================================
ims_Registration* qcril_qmi_imsa_get_ims_registration_info()
{
    ims_Registration* reg_ptr = NULL;
    reg_ptr = qcril_malloc(sizeof(*reg_ptr));
    boolean failure = FALSE;
    if(reg_ptr != NULL)
    {
        reg_ptr->has_state = FALSE;
        if(qcril_qmi_imsa_info.ims_registered_valid)
        {
            reg_ptr->has_state = TRUE;
            reg_ptr->state = qcril_qmi_ims_map_qmi_ims_reg_state_to_ims_reg_state(qcril_qmi_imsa_info.ims_registered);
            QCRIL_LOG_INFO("ims_registered: %d", qcril_qmi_imsa_info.ims_registered);
        }

        if(qcril_qmi_imsa_info.registration_network_valid)
        {
            reg_ptr->has_radioTech = TRUE;
            reg_ptr->radioTech = qcril_qmi_imsa_info.registration_network;
            QCRIL_LOG_INFO("ims_registration_network: %d",qcril_qmi_imsa_info.registration_network);
        }

        reg_ptr->has_errorCode = FALSE;
        if(qcril_qmi_imsa_info.ims_registration_error_code_valid)
        {
            reg_ptr->has_errorCode = TRUE;
            reg_ptr->errorCode = qcril_qmi_imsa_info.ims_registration_error_code;
            QCRIL_LOG_INFO("ims registration error code: %d", qcril_qmi_imsa_info.ims_registration_error_code);
        }
        if(qcril_qmi_imsa_info.ims_registration_error_string_valid && strlen(qcril_qmi_imsa_info.ims_registration_error_string) > 0)
        {
            reg_ptr->errorMessage.arg = qcril_malloc(strlen(qcril_qmi_imsa_info.ims_registration_error_string) + 1);
            if(reg_ptr->errorMessage.arg != NULL)
            {
                strlcpy(reg_ptr->errorMessage.arg, qcril_qmi_imsa_info.ims_registration_error_string,
                        strlen(qcril_qmi_imsa_info.ims_registration_error_string) + 1);
                QCRIL_LOG_INFO("ims registration error message: %s", qcril_qmi_imsa_info.ims_registration_error_string);
            }
            else
            {
                failure = TRUE;
            }
        }

        if(qcril_qmi_imsa_info.uri_list_valid && qcril_qmi_imsa_info.uri_list_len > 0)
        {
          // The list of p-Associated URIs is to be sent as a single string to the upper
          // layers. The URIs are to be separated by a vertical bar ('|').
          uint32_t i;
          uint32_t total_uri_string_length = 0;
          // Get the sum of lengths of all the URIs.
          for(i = 0; i < qcril_qmi_imsa_info.uri_list_len; i++)
          {
            total_uri_string_length += strlen(qcril_qmi_imsa_info.URI_List[i].uri);
          }
          // Calculate non-URI size required for '|' separators.
          if(qcril_qmi_imsa_info.uri_list_len > 1)
          {
            total_uri_string_length += qcril_qmi_imsa_info.uri_list_len - 1;
          }
          reg_ptr->pAssociatedUris.arg = qcril_malloc(total_uri_string_length + 1);
          if(reg_ptr->pAssociatedUris.arg)
          {
            for(i = 0; i < qcril_qmi_imsa_info.uri_list_len; i++)
            {
              strlcat(reg_ptr->pAssociatedUris.arg, qcril_qmi_imsa_info.URI_List[i].uri,
                      total_uri_string_length + 1);
              if(i != qcril_qmi_imsa_info.uri_list_len - 1)
              {
                strlcat(reg_ptr->pAssociatedUris.arg, "|", total_uri_string_length + 1);
              }
            }
          }
        }
    }
    if(failure)
    {
        qcril_qmi_ims_free_ims_registration(reg_ptr);
        reg_ptr = NULL;
    }
  return reg_ptr;
}
//===========================================================================
// qcril_qmi_imsa_reg_status_ind_hdlr
//===========================================================================
void qcril_qmi_imsa_reg_status_ind_hdlr(void *ind_data_ptr)
{
   imsa_registration_status_ind_msg_v01 *reg_ind_msg_ptr = (imsa_registration_status_ind_msg_v01*) ind_data_ptr;
   uint8_t unsol_nw_status = FALSE;
   boolean nw_status_reported = FALSE;

   QCRIL_LOG_FUNC_ENTRY();

   if (NULL != reg_ind_msg_ptr)
   {
      qcril_qmi_imsa_info_lock();
      uint8_t ims_reg_status = reg_ind_msg_ptr->ims_reg_status;
      ims_RadioTechType ims_reg_rat = ims_RadioTechType_RADIO_TECH_UNKNOWN;

      if(reg_ind_msg_ptr->registration_network_valid)
      {
        qcril_qmi_ims_map_imsa_rat_to_ims_rat(reg_ind_msg_ptr->registration_network, &ims_reg_rat);
      }

      if (!reg_ind_msg_ptr->ims_reg_status_valid)
      {
          ims_reg_status = (reg_ind_msg_ptr->ims_registered ?
                            IMSA_STATUS_REGISTERED_V01 :
                            IMSA_STATUS_NOT_REGISTERED_V01);
      }

      qcril_qmi_imsa_store_registration_status(TRUE,
                                               ims_reg_status,
                                               reg_ind_msg_ptr->ims_registration_failure_error_code_valid,
                                               reg_ind_msg_ptr->ims_registration_failure_error_code,
                                               reg_ind_msg_ptr->registration_error_string_valid,
                                               reg_ind_msg_ptr->registration_error_string,
                                               reg_ind_msg_ptr->registration_network_valid,
                                               ims_reg_rat,
                                               reg_ind_msg_ptr->uri_list_valid,
                                               reg_ind_msg_ptr->uri_list,
                                               reg_ind_msg_ptr->uri_list_len);

      QCRIL_LOG_INFO("here: string gotten: %s", reg_ind_msg_ptr->registration_error_string);
      QCRIL_LOG_INFO("ims_registered: %d", qcril_qmi_imsa_info.ims_registered);

      ims_Registration *reg_ptr = qcril_qmi_imsa_get_ims_registration_info();
      qcril_qmi_imsa_info_unlock();
      if(reg_ptr != NULL)
      {
          imsRadioSendMessage(0, ims_MsgType_UNSOL_RESPONSE,
                                    ims_MsgId_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED,
                                    ims_Error_E_SUCCESS, reg_ptr, sizeof(*reg_ptr));
          qcril_qmi_ims_free_ims_registration(reg_ptr);
      }

      if(qcril_qmi_imsa_update_wlan_registration_status())
      {
          /* We need to report change in wlan reg irrespective of whether old data
             rat reported was WLAN or not - for telephony to fetch operator name */
          QCRIL_LOG_INFO(".. IMS wlan reg status changed notify UNSOL_NW_STATE_CHANGED");
          qcril_qmi_nas_wave_voice_data_status();
          nw_status_reported = TRUE;
      }
      if(qmi_ril_is_feature_supported(QMI_RIL_FEATURE_RIL_DATA_REG_STATE_CONDITIONAL_REPORT))
      {
        unsol_nw_status = qcril_qmi_imsa_compare_ims_registered_report_unsol();
        if(unsol_nw_status && !nw_status_reported)
        {
            QCRIL_LOG_INFO(".. IMS status changed notify UNSOL_NW_STATE_CHANGED");
            qcril_qmi_nas_wave_voice_data_status();
        }
      }
   }
   else
   {
      QCRIL_LOG_ERROR("ind_data_ptr is NULL");
   }
   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_reg_status_ind_hdlr

//===========================================================================
// qcril_qmi_imsa_vowifi_status_ind_hdlr
//===========================================================================
void qcril_qmi_imsa_vowifi_status_ind_hdlr(void *ind_data_ptr)
{
   imsa_vowifiprofile_status_ind_msg_v01 *ind_msg_ptr = NULL;
   ims_VoWiFiCallQuality *ims_vowifi_call_quality;

   QCRIL_LOG_FUNC_ENTRY();

   ind_msg_ptr = (imsa_vowifiprofile_status_ind_msg_v01 *) ind_data_ptr;
   if (NULL != ind_msg_ptr)
   {
     QCRIL_LOG_INFO("vowifiprofile_status: %d ",
                    ind_msg_ptr->vowifiprofile_status);

     ims_vowifi_call_quality = qcril_malloc(sizeof(*ims_vowifi_call_quality));
     if(NULL != ims_vowifi_call_quality)
     {
       memset((void *)ims_vowifi_call_quality, 0, sizeof(*ims_vowifi_call_quality));
       ims_vowifi_call_quality->has_quality =
               qcril_qmi_ims_map_qmi_wifi_quality_to_ims_wifi_quality(
                                         ind_msg_ptr->vowifiprofile_status,
                                         (int *)&ims_vowifi_call_quality->quality);

       imsRadioSendMessage(0, ims_MsgType_UNSOL_RESPONSE,
                                 ims_MsgId_UNSOL_VOWIFI_CALL_QUALITY,
                                 ims_Error_E_SUCCESS,
                                 ims_vowifi_call_quality,
                                 sizeof(*ims_vowifi_call_quality));

       qcril_free(ims_vowifi_call_quality);
     }
   }
   else
   {
     QCRIL_LOG_ERROR("ind_data_ptr is NULL");
   }

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_vowifi_status_ind_hdlr

//===========================================================================
// qcril_qmi_imsa_block_status_ind_hdlr
//===========================================================================
void qcril_qmi_imsa_block_status_ind_hdlr(void *ind_data_ptr)
{
   imsa_ims_block_status_ind_msg_v01 *block_ind_msg_ptr =
                        (imsa_ims_block_status_ind_msg_v01*) ind_data_ptr;
   ims_RegistrationBlockStatus reg_block_status_ptr;

   QCRIL_LOG_FUNC_ENTRY();
   if (copy_qmi_blk_status_to_ims_registration_block_status(block_ind_msg_ptr,
       &reg_block_status_ptr))
   {
       imsRadioSendMessage(0, ims_MsgType_UNSOL_RESPONSE,
                                 ims_MsgId_UNSOL_RESPONSE_REGISTRATION_BLOCK_STATUS,
                                 ims_Error_E_SUCCESS, &reg_block_status_ptr,
                                 sizeof(reg_block_status_ptr));
   }
   else
   {
      QCRIL_LOG_ERROR("Block status ind message is NULL");
   }
   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_block_status_ind_hdlr

//===========================================================================
// qcril_qmi_imsa_geo_loc_status_ind_hdlr
//===========================================================================
void qcril_qmi_imsa_geo_loc_status_ind_hdlr(void *ind_data_ptr)
{
   imsa_geo_loc_status_ind_msg_v01 *ind_msg_ptr = (imsa_geo_loc_status_ind_msg_v01 *) ind_data_ptr;
   imsa_set_geo_loc_req_msg_v01 set_geo;
   imsa_set_geo_loc_rsp_msg_v01 set_geo_rsp;
   RIL_Errno err = RIL_E_SUCCESS;
   ims_GeoLocationInfo ims_rsp;

   QCRIL_LOG_FUNC_ENTRY();

   if (NULL != ind_msg_ptr)
   {
       QCRIL_LOG_INFO("lat_v: %c lat: %g long_v: %c long: %g",
                      ind_msg_ptr->latitude_valid ? 'Y' : 'N',
                      ind_msg_ptr->latitude,
                      ind_msg_ptr->longitude_valid ? 'Y' : 'N',
                      ind_msg_ptr->longitude);

       memset(&ims_rsp, 0, sizeof(ims_rsp));
       ims_rsp.has_lat = ind_msg_ptr->latitude_valid;
       ims_rsp.lat = ind_msg_ptr->latitude;

       ims_rsp.has_lon = ind_msg_ptr->longitude_valid;
       ims_rsp.lon = ind_msg_ptr->longitude;

       err = (RIL_Errno)imsRadioSendMessage(0, ims_MsgType_UNSOL_RESPONSE,
                                 ims_MsgId_UNSOL_REQUEST_GEOLOCATION,
                                 ims_Error_E_SUCCESS, &ims_rsp, sizeof(ims_rsp));
       if (err)
       {
           memset(&set_geo, 0, sizeof(set_geo));
           memset(&set_geo_rsp, 0, sizeof(set_geo_rsp));
           set_geo.latitude_valid = ind_msg_ptr->latitude_valid;
           set_geo.latitude = ind_msg_ptr->latitude;
           set_geo.longitude_valid = ind_msg_ptr->longitude_valid;
           set_geo.longitude = ind_msg_ptr->longitude;
           err = qcril_qmi_client_send_msg_sync_ex(
                   QCRIL_QMI_CLIENT_IMSA,
                   QMI_IMSA_SET_GEO_LOC_REQ_V01,
                   &set_geo, sizeof(set_geo),
                   &set_geo_rsp, sizeof(set_geo_rsp),
                   QCRIL_QMI_SYNC_REQ_DEF_TIMEOUT
           );
       }

   }
   else
   {
      QCRIL_LOG_ERROR("ind_data_ptr is NULL");
   }
   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_geo_loc_status_ind_hdlr

// qcril_qmi_imsa_set_geo_loc
void qcril_qmi_imsa_set_geo_loc
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
    qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;
    uint32 user_data;
    qcril_reqlist_public_type reqlist_entry;
    imsa_set_geo_loc_req_msg_v01 req_msg;
    imsa_set_geo_loc_rsp_msg_v01 *response_msg = NULL;
    ims_GeoLocationInfo *ims_req;
    RIL_Errno err = RIL_E_GENERIC_FAILURE;
    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();

    if ( params_ptr )
    {
        if ( params_ptr->data )
        {
            do
            {
                ims_req = (ims_GeoLocationInfo *) params_ptr->data;
                memset(&req_msg, 0, sizeof(req_msg));
                req_msg.latitude_valid = ims_req->has_lat;
                req_msg.latitude = ims_req->lat;
                req_msg.longitude_valid = ims_req->has_lon;
                req_msg.longitude = ims_req->lon;
                if (ims_req->has_addressInfo)
                {
                    if (ims_req->addressInfo.city.arg)
                    {
                        req_msg.city_valid = TRUE;
                        strlcpy(req_msg.city, ims_req->addressInfo.city.arg, sizeof(req_msg.city));
                        QCRIL_LOG_DEBUG("City: %s", ims_req->addressInfo.city.arg);
                    }

                    if (ims_req->addressInfo.postalCode.arg)
                    {
                        req_msg.zipCode_valid = TRUE;
                        strlcpy(req_msg.zipCode, ims_req->addressInfo.postalCode.arg, sizeof(req_msg.zipCode));
                        QCRIL_LOG_DEBUG("Postal Code: %s", ims_req->addressInfo.postalCode.arg);
                    }

                    if (ims_req->addressInfo.state.arg)
                    {
                        req_msg.state_valid = TRUE;
                        strlcpy(req_msg.state, ims_req->addressInfo.state.arg, sizeof(req_msg.state));
                        QCRIL_LOG_DEBUG("State: %s", ims_req->addressInfo.state.arg);
                    }

                    if (ims_req->addressInfo.country.arg)
                    {
                        req_msg.countryName_valid = TRUE;
                        strlcpy(req_msg.countryName, ims_req->addressInfo.country.arg, sizeof(req_msg.countryName));
                        QCRIL_LOG_DEBUG("Country: %s", ims_req->addressInfo.country.arg);
                    }

                    if (ims_req->addressInfo.countryCode.arg)
                    {
                        req_msg.countryCode_valid = TRUE;
                        strlcpy(req_msg.countryCode, ims_req->addressInfo.countryCode.arg, sizeof(req_msg.countryCode));
                        QCRIL_LOG_DEBUG("Country Code: %s", ims_req->addressInfo.countryCode.arg);
                    }
                }
                response_msg = qcril_malloc(sizeof(imsa_set_geo_loc_rsp_msg_v01));
                if (!response_msg)
                {
                    QCRIL_LOG_ERROR("Unable to allocate memory for imsa_set_geo_loc_rsp_msg_v01");
                    break;
                }

                qcril_reqlist_default_entry( params_ptr->t, params_ptr->event_id,
                        QCRIL_DEFAULT_MODEM_ID,
                        QCRIL_REQ_AWAITING_CALLBACK,
                        QCRIL_EVT_NONE, NULL, &reqlist_entry );

                if (qcril_reqlist_new(instance_id, &reqlist_entry) != E_SUCCESS)
                {
                    QCRIL_LOG_ERROR("qcril_reqlist_new failed");
                    break;
                }

                user_data = QCRIL_COMPOSE_USER_DATA(instance_id, QCRIL_DEFAULT_MODEM_ID,
                        reqlist_entry.req_id);

                err = qcril_qmi_client_send_msg_async_ex(
                           QCRIL_QMI_CLIENT_IMSA,
                           QMI_IMSA_SET_GEO_LOC_REQ_V01,
                           &req_msg, sizeof(req_msg),
                           response_msg, sizeof(*response_msg),
                           (void *)(uintptr_t) user_data);
                QCRIL_LOG_INFO("qmi send async ex : err %d", (int) err);
            } while(FALSE);
        }

        if (err != RIL_E_SUCCESS)
        {
           qcril_send_empty_payload_request_response(
                   instance_id,
                   params_ptr->t,
                   params_ptr->event_id,
                   err );
        }
    }
} // qcril_qmi_imsa_set_geo_loc

//===========================================================================
// qcril_qmi_imsa_store_service_status_cl
//===========================================================================
void qcril_qmi_imsa_store_service_status_cl(
    uint8_t sms_service_status_valid,
    imsa_service_status_enum_v01 sms_service_status,
    uint8_t voip_service_status_valid,
    imsa_service_status_enum_v01 voip_service_status,
    uint8_t vt_service_status_valid,
    imsa_service_status_enum_v01 vt_service_status,
    uint8_t sms_service_rat_valid,
    imsa_service_rat_enum_v01 sms_service_rat,
    uint8_t voip_service_rat_valid,
    imsa_service_rat_enum_v01 voip_service_rat,
    uint8_t vt_service_rat_valid,
    imsa_service_rat_enum_v01 vt_service_rat,
    uint8_t ut_service_status_valid,
    imsa_service_status_enum_v01 ut_service_status,
    uint8_t ut_service_rat_valid,
    imsa_service_rat_enum_v01 ut_service_rat
)
{
    if(sms_service_status_valid)
    {
      qcril_qmi_imsa_info.ims_srv_status_valid = TRUE;

      qcril_qmi_imsa_info.ims_srv_status.sms_service_status_valid = sms_service_status_valid;
      qcril_qmi_imsa_info.ims_srv_status.sms_service_status = sms_service_status;
    }

    if (sms_service_rat_valid)
    {
      qcril_qmi_imsa_info.ims_srv_status.sms_service_rat_valid = sms_service_rat_valid;
      qcril_qmi_imsa_info.ims_srv_status.sms_service_rat = sms_service_rat;
    }

    if(voip_service_status_valid)
    {
      qcril_qmi_imsa_info.ims_srv_status_valid = TRUE;

      qcril_qmi_imsa_info.ims_srv_status.voip_service_status_valid = voip_service_status_valid;
      qcril_qmi_imsa_info.ims_srv_status.voip_service_status = voip_service_status;
    }

    if (voip_service_rat_valid)
    {
      qcril_qmi_imsa_info.ims_srv_status.voip_service_rat_valid = voip_service_rat_valid;
      qcril_qmi_imsa_info.ims_srv_status.voip_service_rat = voip_service_rat;
    }

    if(vt_service_status_valid)
    {
      qcril_qmi_imsa_info.ims_srv_status_valid = TRUE;

      qcril_qmi_imsa_info.ims_srv_status.vt_service_status_valid = vt_service_status_valid;
      qcril_qmi_imsa_info.ims_srv_status.vt_service_status = vt_service_status;
    }

    if (vt_service_rat_valid)
    {
      qcril_qmi_imsa_info.ims_srv_status.vt_service_rat_valid = vt_service_rat_valid;
      qcril_qmi_imsa_info.ims_srv_status.vt_service_rat = vt_service_rat;
    }

    if(ut_service_status_valid)
    {
      qcril_qmi_imsa_info.ims_srv_status_valid = TRUE;

      qcril_qmi_imsa_info.ims_srv_status.ut_service_status_valid = ut_service_status_valid;
      qcril_qmi_imsa_info.ims_srv_status.ut_service_status = ut_service_status;
    }

    if (ut_service_rat_valid)
    {
      qcril_qmi_imsa_info.ims_srv_status.ut_service_rat_valid = ut_service_rat_valid;
      qcril_qmi_imsa_info.ims_srv_status.ut_service_rat = ut_service_rat;
    }
} // qcril_qmi_imsa_store_service_status_cl

//===========================================================================
// qcril_qmi_imsa_service_status_ind_hdlr
//===========================================================================
void qcril_qmi_imsa_service_status_ind_hdlr(void *ind_data_ptr)
{
    imsa_service_status_ind_msg_v01 *service_ind_msg_ptr = (imsa_service_status_ind_msg_v01*) ind_data_ptr;
    uint8_t unsol_nw_status = FALSE;
    boolean nw_status_reported = FALSE;

    QCRIL_LOG_FUNC_ENTRY();
    if (NULL != service_ind_msg_ptr)
    {
        QCRIL_LOG_ESSENTIAL("VOIP: service_status(%s): %d, rat(%s): %d",
            service_ind_msg_ptr->voip_service_status_valid ? "valid": "not valid",
            service_ind_msg_ptr->voip_service_status,
            service_ind_msg_ptr->voip_service_rat_valid ? "valid": "not valid",
            service_ind_msg_ptr->voip_service_rat);
        QCRIL_LOG_ESSENTIAL("VT: service_status(%s): %d, rat(%s): %d",
            service_ind_msg_ptr->vt_service_status_valid ? "valid": "not valid",
            service_ind_msg_ptr->vt_service_status,
            service_ind_msg_ptr->vt_service_rat_valid ? "valid": "not valid",
            service_ind_msg_ptr->vt_service_rat);
        QCRIL_LOG_ESSENTIAL("UT: service_status(%s): %d, rat(%s): %d",
            service_ind_msg_ptr->ut_service_status_valid ? "valid": "not valid",
            service_ind_msg_ptr->ut_service_status,
            service_ind_msg_ptr->ut_service_rat_valid ? "valid": "not valid",
            service_ind_msg_ptr->ut_service_rat);

        qcril_qmi_imsa_info_lock();

        qcril_qmi_imsa_store_service_status_cl(
            service_ind_msg_ptr->sms_service_status_valid,
            service_ind_msg_ptr->sms_service_status,
            service_ind_msg_ptr->voip_service_status_valid,
            service_ind_msg_ptr->voip_service_status,
            service_ind_msg_ptr->vt_service_status_valid,
            service_ind_msg_ptr->vt_service_status,
            service_ind_msg_ptr->sms_service_rat_valid,
            service_ind_msg_ptr->sms_service_rat,
            service_ind_msg_ptr->voip_service_rat_valid,
            service_ind_msg_ptr->voip_service_rat,
            service_ind_msg_ptr->vt_service_rat_valid,
            service_ind_msg_ptr->vt_service_rat,
            service_ind_msg_ptr->ut_service_status_valid,
            service_ind_msg_ptr->ut_service_status,
            service_ind_msg_ptr->ut_service_rat_valid,
            service_ind_msg_ptr->ut_service_rat );

        ims_SrvStatusList *ims_srv_status_list_ptr = qcril_qmi_ims_create_ims_srvstatusinfo(
                                                          &qcril_qmi_imsa_info.ims_srv_status );

        qcril_qmi_imsa_info_unlock();

        if (ims_srv_status_list_ptr)
        {
            imsRadioSendMessage(
                0,
                ims_MsgType_UNSOL_RESPONSE,
                ims_MsgId_UNSOL_SRV_STATUS_UPDATE,
                ims_Error_E_SUCCESS,
                ims_srv_status_list_ptr,
                sizeof(*ims_srv_status_list_ptr) );
            qcril_qmi_ims_free_srvstatuslist(ims_srv_status_list_ptr);

#ifndef QMI_RIL_UTF
            // Trigger QCRIL_AM_EVENT_IMS_SRV_CHANGED only if the VoIP/VT RAT is valid
            if (service_ind_msg_ptr->voip_service_rat_valid ||
                     service_ind_msg_ptr->vt_service_rat_valid)
            {
                qcril_am_handle_event(QCRIL_AM_EVENT_IMS_SRV_CHANGED, NULL);
            }
#endif
        }
        if(qcril_qmi_imsa_update_wlan_registration_status())
        {
            /* We need to report change in wlan reg irrespective of whether old data
               rat reported was WLAN or not - for telephony to fetch operator name */
            QCRIL_LOG_INFO(".. IMS wlan reg status changed notify UNSOL_NW_STATE_CHANGED");
            qcril_qmi_nas_wave_voice_data_status();
            nw_status_reported = TRUE;
        }
        if(qmi_ril_is_feature_supported(QMI_RIL_FEATURE_RIL_DATA_REG_STATE_CONDITIONAL_REPORT))
        {
            unsol_nw_status = qcril_qmi_imsa_compare_ims_registered_report_unsol();
            if(unsol_nw_status && !nw_status_reported)
            {
                QCRIL_LOG_INFO(".. IMS status changed notify UNSOL_NW_STATE_CHANGED");
                qcril_qmi_nas_wave_voice_data_status();
            }
        }
    }
    else
    {
        QCRIL_LOG_ERROR("ind_data_ptr is NULL");
    }
    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_service_status_ind_hdlr

//===========================================================================
// qcril_qmi_imsa_rat_handover_status_ind_hdlr
//===========================================================================
static void qcril_qmi_imsa_rat_handover_status_ind_hdlr(void *ind_data_ptr)
{
    QCRIL_LOG_FUNC_ENTRY();
    imsa_rat_handover_status_ind_msg_v01 *rat_ho_ind_msg_ptr = (imsa_rat_handover_status_ind_msg_v01*) ind_data_ptr;
    if (NULL != rat_ho_ind_msg_ptr)
    {
        if (rat_ho_ind_msg_ptr->rat_ho_status_info_valid)
        {
            ims_Handover *ims_handover_ptr = qcril_qmi_ims_create_ims_handover_from_imsa_rat_info(&rat_ho_ind_msg_ptr->rat_ho_status_info);

            if (ims_handover_ptr)
            {
                imsRadioSendMessage(
                    0,
                    ims_MsgType_UNSOL_RESPONSE,
                    ims_MsgId_UNSOL_RESPONSE_HANDOVER,
                    ims_Error_E_SUCCESS,
                    ims_handover_ptr,
                    sizeof(*ims_handover_ptr) );
                qcril_qmi_ims_free_ims_handover(ims_handover_ptr);

                QCRIL_LOG_DEBUG("rat_ho_status = %d, source_rat = %d, target_rat = %d\n",
                        rat_ho_ind_msg_ptr->rat_ho_status_info.rat_ho_status,
                        rat_ho_ind_msg_ptr->rat_ho_status_info.source_rat,
                        rat_ho_ind_msg_ptr->rat_ho_status_info.target_rat);

                if (rat_ho_ind_msg_ptr->rat_ho_status_info.rat_ho_status ==
                        IMSA_STATUS_RAT_HO_SUCCESS_V01)
                {
#ifndef QMI_RIL_UTF
                    qcril_am_handle_event(QCRIL_AM_EVENT_IMS_HANDOVER,
                            &rat_ho_ind_msg_ptr->rat_ho_status_info.target_rat);
#endif
                }
            }
            else
            {
                QCRIL_LOG_DEBUG("ims_handover_ptr creation failed");
            }
        }
    }
    else
    {
        QCRIL_LOG_ERROR("ind_data_ptr is NULL");
    }
    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_rat_handover_status_ind_hdlr

//===========================================================================
// qcril_qmi_imsa_rtp_statistics_ind_hdlr
//===========================================================================
static void qcril_qmi_imsa_rtp_statistics_ind_hdlr(void *ind_data_ptr)
{
    IxErrnoType                    result = E_SUCCESS;
    qcril_reqlist_public_type      req_info;

    QCRIL_LOG_FUNC_ENTRY();
    imsa_rtp_statistics_ind_msg_v01 *rtp_statistics_ind_msg_ptr = (imsa_rtp_statistics_ind_msg_v01*) ind_data_ptr;
    if (NULL != rtp_statistics_ind_msg_ptr)
    {
        ims_RtpStatisticsData* ims_rtp_stats_ptr = qcril_malloc(sizeof(ims_RtpStatisticsData));

        if (ims_rtp_stats_ptr)
        {
            QCRIL_LOG_DEBUG("rx_expected_rtp_pkt = %d, rx_rtp_pkt_loss = %d\n",
                    rtp_statistics_ind_msg_ptr->total_rx_expected_rtp_pkt_count,
                    rtp_statistics_ind_msg_ptr->total_rx_rtp_pkt_loss_count);

            /* check for RTP stats pending request */
            result  = qcril_reqlist_query_by_request( QCRIL_DEFAULT_INSTANCE_ID,
                                                    QCRIL_EVT_IMS_SOCKET_REQ_GET_RTP_STATISTICS,
                                                    &req_info );

            /* now check for RTP error stats pending request as both of them wait for same indication */
            if( result == E_SUCCESS )
            {
                ims_rtp_stats_ptr->has_count =
                    rtp_statistics_ind_msg_ptr->total_rx_expected_rtp_pkt_count_valid;
                ims_rtp_stats_ptr->count =
                    rtp_statistics_ind_msg_ptr->total_rx_expected_rtp_pkt_count;
            }
            else
            {
               result  = qcril_reqlist_query_by_request( QCRIL_DEFAULT_INSTANCE_ID,
                                                       QCRIL_EVT_IMS_SOCKET_REQ_GET_RTP_ERROR_STATISTICS,
                                                       &req_info );
                if( result == E_SUCCESS )
                {
                    ims_rtp_stats_ptr->has_count =
                        rtp_statistics_ind_msg_ptr->total_rx_rtp_pkt_loss_count_valid;
                    ims_rtp_stats_ptr->count =
                        rtp_statistics_ind_msg_ptr->total_rx_rtp_pkt_loss_count;
                }
            }

            if( result == E_SUCCESS )
            {
               /* Send the RTP stats response */
                imsRadioSendMessage(
                    req_info.t,
                    ims_MsgType_RESPONSE,
                    qcril_qmi_ims_map_event_to_request(req_info.request),
                    ims_Error_E_SUCCESS,
                    ims_rtp_stats_ptr,
                    sizeof(*ims_rtp_stats_ptr) );
            }
            else
            {
                QCRIL_LOG_DEBUG("No RTP stats req found\n");
            }

            qcril_free(ims_rtp_stats_ptr);
        }
        else
        {
            QCRIL_LOG_DEBUG("ims_rtp_stats_ptr creation failed");
        }
    }
    else
    {
        QCRIL_LOG_ERROR("ind_data_ptr is NULL");
    }
    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_rtp_statistics_ind_hdlr

//===========================================================================
// qcril_qmi_imsa_unsol_ind_cb_helper
//===========================================================================
void qcril_qmi_imsa_unsol_ind_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
)
{
  qmi_ind_callback_type * qmi_callback = NULL;

  uint32_t decoded_payload_len = 0;
  qmi_client_error_type qmi_err = QMI_NO_ERR;
  void* decoded_payload = NULL;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  if( NULL != params_ptr && NULL != params_ptr->data )
  {
    qmi_callback = (qmi_ind_callback_type*) params_ptr->data;
    qmi_idl_get_message_c_struct_len(
                     qcril_qmi_client_get_service_object(QCRIL_QMI_CLIENT_IMSA),
                     QMI_IDL_INDICATION,
                     qmi_callback->msg_id,
                     &decoded_payload_len);

    if(decoded_payload_len)
    {
      decoded_payload = qcril_malloc(decoded_payload_len);
    }

    if ( decoded_payload || !decoded_payload_len )
    {
        if( decoded_payload_len )
        {
          qmi_err = qmi_client_message_decode(qcril_qmi_client_get_user_handle(QCRIL_QMI_CLIENT_IMSA),
                                              QMI_IDL_INDICATION,
                                              qmi_callback->msg_id,
                                              qmi_callback->data_buf,
                                              qmi_callback->data_buf_len,
                                              decoded_payload,
                                              (int)decoded_payload_len);
        }

        if ( QMI_NO_ERR == qmi_err )
        {
          switch(qmi_callback->msg_id)
          {
              case QMI_IMSA_REGISTRATION_STATUS_IND_V01:
                qcril_qmi_imsa_reg_status_ind_hdlr(decoded_payload);
                break;

              case QMI_IMSA_SERVICE_STATUS_IND_V01:
                qcril_qmi_imsa_service_status_ind_hdlr(decoded_payload);
                break;

              case QMI_IMSA_RAT_HANDOVER_STATUS_IND_V01:
                qcril_qmi_imsa_rat_handover_status_ind_hdlr(decoded_payload);
                break;

              case QMI_IMSA_RTP_STATISTICS_IND_V01:
                qcril_qmi_imsa_rtp_statistics_ind_hdlr(decoded_payload);
                break;

              case QMI_IMSA_GEO_LOC_STATUS_IND_V01:
                qcril_qmi_imsa_geo_loc_status_ind_hdlr(decoded_payload);
                break;

              case QMI_IMSA_VoWIFI_STATUS_IND_V01:
                qcril_qmi_imsa_vowifi_status_ind_hdlr(decoded_payload);
                break;

              case QMI_IMSA_IMS_BLOCK_STATUS_IND_V01:
                qcril_qmi_imsa_block_status_ind_hdlr(decoded_payload);
                break;

              default:
                QCRIL_LOG_INFO("Unknown QMI IMSA indication %d", qmi_callback->msg_id);
                break;
            }
          }
          else
          {
              QCRIL_LOG_INFO("Indication decode failed for msg %d of svc %d with error %d", qmi_callback->msg_id, QCRIL_QMI_CLIENT_IMSA, qmi_err );
          }

          if( decoded_payload_len )
          {
            qcril_free(decoded_payload);
          }
    }

    if( qmi_callback->data_buf )
    {
      qcril_free(qmi_callback->data_buf);
    }
  }
  else
  {
    QCRIL_LOG_ERROR("params_ptr or params_ptr->data is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();

} // qcril_qmi_imsa_unsol_ind_cb_helper

//===========================================================================
// qcril_qmi_imsa_unsol_ind_cb
//===========================================================================
void qcril_qmi_imsa_unsol_ind_cb
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void              *ind_buf,
  unsigned int       ind_buf_len,
  void              *ind_cb_data
)
{
  qmi_ind_callback_type qmi_callback;

  QCRIL_LOG_FUNC_ENTRY();

  qmi_callback.data_buf = qcril_malloc(ind_buf_len);

  if( qmi_callback.data_buf )
  {
    qmi_callback.user_handle = user_handle;
    qmi_callback.msg_id = msg_id;
    memcpy(qmi_callback.data_buf,ind_buf,ind_buf_len);
    qmi_callback.data_buf_len = ind_buf_len;
    qmi_callback.cb_data = ind_cb_data;

    qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                   QCRIL_DEFAULT_MODEM_ID,
                   QCRIL_DATA_ON_STACK,
                   QCRIL_EVT_QMI_IMSA_HANDLE_INDICATIONS,
                   (void*) &qmi_callback,
                   sizeof(qmi_callback),
                   (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
  }
  else
  {
    QCRIL_LOG_FATAL("malloc failed");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_unsol_ind_cb

//===========================================================================
// qcril_qmi_imsa_get_reg_status_resp_hdlr
//===========================================================================
void qcril_qmi_imsa_get_reg_status_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
   imsa_get_registration_status_resp_msg_v01 *resp_msg_ptr = NULL;

   QCRIL_LOG_FUNC_ENTRY();
   if( NULL != params_ptr && NULL != params_ptr->data )
   {
      resp_msg_ptr = (imsa_get_registration_status_resp_msg_v01 *)params_ptr->data;

      if (QMI_RESULT_SUCCESS_V01 == resp_msg_ptr->resp.result &&
          (resp_msg_ptr->ims_reg_status_valid || resp_msg_ptr->ims_registered_valid))
      {
         qcril_qmi_imsa_info_lock();
         uint8_t ims_reg_status = resp_msg_ptr->ims_reg_status;
         ims_RadioTechType ims_reg_rat = ims_RadioTechType_RADIO_TECH_UNKNOWN;

         if (!resp_msg_ptr->ims_reg_status_valid)
         {
             ims_reg_status = (resp_msg_ptr->ims_registered ?
                               IMSA_STATUS_REGISTERED_V01 :
                               IMSA_STATUS_NOT_REGISTERED_V01);
         }

         if(resp_msg_ptr->registration_network_valid)
         {
           qcril_qmi_ims_map_imsa_rat_to_ims_rat(resp_msg_ptr->registration_network, &ims_reg_rat);
         }

         qcril_qmi_imsa_store_registration_status(TRUE,
                                                  ims_reg_status,
                                                  resp_msg_ptr->ims_registration_failure_error_code_valid,
                                                  resp_msg_ptr->ims_registration_failure_error_code,
                                                  resp_msg_ptr->registration_error_string_valid,
                                                  resp_msg_ptr->registration_error_string,
                                                  resp_msg_ptr->registration_network_valid,
                                                  ims_reg_rat,
                                                  resp_msg_ptr->uri_list_valid,
                                                  resp_msg_ptr->uri_list,
                                                  resp_msg_ptr->uri_list_len);

         ims_Registration *reg_ptr = qcril_qmi_imsa_get_ims_registration_info();
         qcril_qmi_imsa_info_unlock();
         if(reg_ptr != NULL)
         {
             imsRadioSendMessage(params_ptr->t, ims_MsgType_RESPONSE,
                                       ims_MsgId_REQUEST_IMS_REGISTRATION_STATE,
                                       ims_Error_E_SUCCESS, reg_ptr, sizeof(*reg_ptr));
             qcril_qmi_ims_free_ims_registration(reg_ptr);
         }
      }
      else
      {
          QCRIL_LOG_INFO("error: %d", resp_msg_ptr->resp.error);
          qcril_qmi_imsa_info_lock();
          qcril_qmi_imsa_info.ims_registered_valid = FALSE;
          qcril_qmi_imsa_info.registration_network_valid = FALSE;
          qcril_qmi_imsa_info.ims_registration_error_code_valid = FALSE;
          qcril_qmi_imsa_info.ims_registration_error_string_valid = FALSE;
          qcril_qmi_imsa_info.uri_list_valid = FALSE;
          imsRadioSendMessage(params_ptr->t, ims_MsgType_RESPONSE, ims_MsgId_REQUEST_IMS_REGISTRATION_STATE, ims_Error_E_GENERIC_FAILURE, NULL, 0);
          qcril_qmi_imsa_info_unlock();
      }
   }
   else
   {
      QCRIL_LOG_ERROR("params_ptr or params_ptr->data is NULL");
   }
   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_get_reg_status_resp_hdlr

//===========================================================================
// qcril_qmi_imsa_ind_reg_resp_hdlr
//===========================================================================
void qcril_qmi_imsa_ind_reg_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
   imsa_ind_reg_rsp_msg_v01 *resp_msg_ptr = NULL;
   QCRIL_LOG_FUNC_ENTRY();
   if( NULL != params_ptr && NULL != params_ptr->data )
   {
      resp_msg_ptr = (imsa_ind_reg_rsp_msg_v01 *)params_ptr->data;
      QCRIL_LOG_INFO("result: %d, error: %d", resp_msg_ptr->resp.result, resp_msg_ptr->resp.error);
      if (QMI_RESULT_SUCCESS_V01 == resp_msg_ptr->resp.result)
      {
         qcril_qmi_imsa_info_lock();
         qcril_qmi_imsa_info.ims_status_change_registered = TRUE;
         qcril_qmi_imsa_info_unlock();
      }
   }
   else
   {
      QCRIL_LOG_ERROR("params_ptr or params_ptr->data is NULL");
   }
   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_ind_reg_resp_hdlr

//===========================================================================
// qcril_qmi_imsa_service_status_resp_hdlr
//===========================================================================
void qcril_qmi_imsa_get_service_status_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
    imsa_get_service_status_resp_msg_v01 *resp_msg_ptr = NULL;

    QCRIL_LOG_FUNC_ENTRY();
    if( NULL != params_ptr && NULL != params_ptr->data )
    {
        resp_msg_ptr = (imsa_get_service_status_resp_msg_v01 *)params_ptr->data;
        if (QMI_RESULT_SUCCESS_V01 == resp_msg_ptr->resp.result)
        {
            QCRIL_LOG_ESSENTIAL("VOIP: service_status(%s): %d, rat(%s): %d",
                resp_msg_ptr->voip_service_status_valid ? "valid": "not valid",
                resp_msg_ptr->voip_service_status,
                resp_msg_ptr->voip_service_rat_valid ? "valid": "not valid",
                resp_msg_ptr->voip_service_rat);
            QCRIL_LOG_ESSENTIAL("VT: service_status(%s): %d, rat(%s): %d",
                resp_msg_ptr->vt_service_status_valid ? "valid": "not valid",
                resp_msg_ptr->vt_service_status,
                resp_msg_ptr->vt_service_rat_valid ? "valid": "not valid",
                resp_msg_ptr->vt_service_rat);
            QCRIL_LOG_ESSENTIAL("UT: service_status(%s): %d, rat(%s): %d",
                resp_msg_ptr->ut_service_status_valid ? "valid": "not valid",
                resp_msg_ptr->ut_service_status,
                resp_msg_ptr->ut_service_rat_valid ? "valid": "not valid",
                resp_msg_ptr->ut_service_rat);

            qcril_qmi_imsa_info_lock();
            qcril_qmi_imsa_store_service_status_cl(
                resp_msg_ptr->sms_service_status_valid,
                resp_msg_ptr->sms_service_status,
                resp_msg_ptr->voip_service_status_valid,
                resp_msg_ptr->voip_service_status,
                resp_msg_ptr->vt_service_status_valid,
                resp_msg_ptr->vt_service_status,
                resp_msg_ptr->sms_service_rat_valid,
                resp_msg_ptr->sms_service_rat,
                resp_msg_ptr->voip_service_rat_valid,
                resp_msg_ptr->voip_service_rat,
                resp_msg_ptr->vt_service_rat_valid,
                resp_msg_ptr->vt_service_rat,
                resp_msg_ptr->ut_service_status_valid,
                resp_msg_ptr->ut_service_status,
                resp_msg_ptr->ut_service_rat_valid,
                resp_msg_ptr->ut_service_rat );

            ims_SrvStatusList *ims_srv_status_list_ptr = qcril_qmi_ims_create_ims_srvstatusinfo(
                                                              &qcril_qmi_imsa_info.ims_srv_status );

            qcril_qmi_imsa_info_unlock();

            if (ims_srv_status_list_ptr)
            {
                imsRadioSendMessage(
                    params_ptr->t,
                    ims_MsgType_RESPONSE,
                    ims_MsgId_REQUEST_QUERY_SERVICE_STATUS,
                    ims_Error_E_SUCCESS,
                    ims_srv_status_list_ptr,
                    sizeof(*ims_srv_status_list_ptr) );
                qcril_qmi_ims_free_srvstatuslist(ims_srv_status_list_ptr);

#ifndef QMI_RIL_UTF
                // Trigger QCRIL_AM_EVENT_IMS_SRV_CHANGED only if the VoIP/VT RAT is valid
                if (resp_msg_ptr->voip_service_rat_valid ||
                        resp_msg_ptr->vt_service_rat_valid)
                {
                    qcril_am_handle_event(QCRIL_AM_EVENT_IMS_SRV_CHANGED, NULL);
                }
#endif
            }
            else
            {
                imsRadioSendMessage(
                    params_ptr->t,
                    ims_MsgType_RESPONSE,
                    ims_MsgId_REQUEST_QUERY_SERVICE_STATUS,
                    ims_Error_E_GENERIC_FAILURE,
                    NULL,
                    0 );
            }
        }
        else
        {
            QCRIL_LOG_INFO("error: %d", resp_msg_ptr->resp.error);
            qcril_qmi_imsa_info_lock();
            qcril_qmi_imsa_info.ims_srv_status_valid = FALSE;
            qcril_qmi_imsa_info_unlock();
            imsRadioSendMessage(
                params_ptr->t,
                ims_MsgType_RESPONSE,
                ims_MsgId_REQUEST_QUERY_SERVICE_STATUS,
                ims_Error_E_GENERIC_FAILURE,
                NULL,
                0 );
        }
    }
    else
    {
        QCRIL_LOG_ERROR("params_ptr or params_ptr->data is NULL");
    }
    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_service_status_resp_hdlr

//===========================================================================
// qcril_qmi_imsa_get_rtp_statistics_resp_hdlr
//===========================================================================
void qcril_qmi_imsa_get_rtp_statistics_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
    imsa_get_rtp_statistics_resp_msg_v01 *resp_msg_ptr = NULL;

    QCRIL_LOG_FUNC_ENTRY();
    if( NULL != params_ptr && NULL != params_ptr->data )
    {
        resp_msg_ptr = (imsa_get_rtp_statistics_resp_msg_v01 *)params_ptr->data;
        if (QMI_RESULT_SUCCESS_V01 != resp_msg_ptr->resp.result)
        {
            QCRIL_LOG_INFO("error: %d", resp_msg_ptr->resp.error);
            imsRadioSendMessage(
                params_ptr->t,
                ims_MsgType_RESPONSE,
                qcril_qmi_ims_map_event_to_request(params_ptr->event_id),
                ims_Error_E_GENERIC_FAILURE,
                NULL,
                0 );
        }
    }
    else
    {
        QCRIL_LOG_ERROR("params_ptr or params_ptr->data is NULL");
    }
    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_service_status_resp_hdlr

//===========================================================================
// qcril_qmi_imsa_set_geo_loc_resp_hdlr
//===========================================================================
void qcril_qmi_imsa_set_geo_loc_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
    imsa_set_geo_loc_rsp_msg_v01 *resp_msg_ptr = NULL;
    ims_Error ims_error = ims_Error_E_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();
    if( NULL != params_ptr && NULL != params_ptr->data )
    {
        resp_msg_ptr = (imsa_set_geo_loc_rsp_msg_v01 *)params_ptr->data;
        if (QMI_RESULT_SUCCESS_V01 != resp_msg_ptr->resp.result)
        {
            QCRIL_LOG_INFO("error: %d", resp_msg_ptr->resp.error);
            ims_error = ims_Error_E_GENERIC_FAILURE;
        }
        QCRIL_LOG_INFO("ims_error: %d", ims_error);
        imsRadioSendMessage(
                params_ptr->t,
                ims_MsgType_RESPONSE,
                qcril_qmi_ims_map_event_to_request(params_ptr->event_id),
                ims_error,
                NULL,
                0 );
    }
    else
    {
        QCRIL_LOG_ERROR("params_ptr or params_ptr->data is NULL");
    }
    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_set_geo_loc_resp_hdlr

//===========================================================================
// qcril_qmi_imsa_get_vowifi_status_resp_hdlr
//===========================================================================
void qcril_qmi_imsa_get_vowifi_status_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
  imsa_get_vowifi_status_resp_msg_v01 *resp_msg_ptr = NULL;
  ims_VoWiFiCallQuality *ims_vowifi_call_quality = NULL;

  QCRIL_LOG_FUNC_ENTRY();
  if( NULL != params_ptr && NULL != params_ptr->data )
  {
    resp_msg_ptr = (imsa_get_vowifi_status_resp_msg_v01 *)params_ptr->data;
    if (QMI_RESULT_SUCCESS_V01 == resp_msg_ptr->resp.result)
    {
      ims_vowifi_call_quality = qcril_malloc(sizeof(*ims_vowifi_call_quality));
      if(NULL != ims_vowifi_call_quality)
      {
        memset((void *)ims_vowifi_call_quality, 0, sizeof(*ims_vowifi_call_quality));
        if(resp_msg_ptr->vowifiprofile_status_valid)
        {
          ims_vowifi_call_quality->has_quality =
                  qcril_qmi_ims_map_qmi_wifi_quality_to_ims_wifi_quality(
                                            resp_msg_ptr->vowifiprofile_status,
                                            (int *)&ims_vowifi_call_quality->quality);
          QCRIL_LOG_INFO("has_quality: %d quality: %d",
                           ims_vowifi_call_quality->has_quality,
                           ims_vowifi_call_quality->quality);
        }

        imsRadioSendMessage(params_ptr->t, ims_MsgType_RESPONSE,
              qcril_qmi_ims_map_event_to_request(params_ptr->event_id),
              ims_Error_E_SUCCESS,
              ims_vowifi_call_quality, sizeof(*ims_vowifi_call_quality));

        qcril_free(ims_vowifi_call_quality);
      }
    }
    else
    {
      QCRIL_LOG_INFO("error: %d", resp_msg_ptr->resp.error);

      imsRadioSendMessage(params_ptr->t, ims_MsgType_RESPONSE,
            qcril_qmi_ims_map_event_to_request(params_ptr->event_id),
            ims_Error_E_GENERIC_FAILURE, NULL, 0 );
    }
  }
  else
  {
    QCRIL_LOG_ERROR("params_ptr or params_ptr->data is NULL");
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_get_vowifi_status_resp_hdlr

//===========================================================================
// qcril_qmi_imsa_command_cb_helper
//===========================================================================
void qcril_qmi_imsa_command_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
)
{
  qcril_instance_id_e_type instance_id;
  uint32 user_data;
  uint16 req_id;
  qcril_reqlist_public_type req_info;
  qcril_request_params_type req_data;
  qmi_resp_callback_type * qmi_resp_callback = NULL;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  if( NULL != params_ptr && NULL != params_ptr->data )
  {
    qmi_resp_callback = (qmi_resp_callback_type *) params_ptr->data;
    if ( qmi_resp_callback->data_buf != NULL )
    {
       memset(&req_data, 0, sizeof(req_data));
       req_data.datalen = qmi_resp_callback->data_buf_len;
       req_data.data = qmi_resp_callback->data_buf;

       QCRIL_LOG_INFO("msg_id 0x%.2x", qmi_resp_callback->msg_id);

       if (QMI_IMSA_IND_REG_RSP_V01 == qmi_resp_callback->msg_id)
       {
          if( qmi_resp_callback->transp_err != QMI_NO_ERR )
          {
             QCRIL_LOG_INFO("Transp error (%d) recieved", qmi_resp_callback->transp_err);
          }
          else
          {
             qcril_qmi_imsa_ind_reg_resp_hdlr(&req_data);
          }
       }
       else
       {
          user_data = ( uint32 )(uintptr_t) qmi_resp_callback->cb_data;
          instance_id = QCRIL_EXTRACT_INSTANCE_ID_FROM_USER_DATA( user_data );
          req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA( user_data );
          req_data.instance_id = instance_id;
          req_data.modem_id = QCRIL_DEFAULT_MODEM_ID;
          /* Lookup the Token ID */
          if ( qcril_reqlist_query_by_req_id( req_id, &instance_id, &req_info ) == E_SUCCESS )
          {
            if( qmi_resp_callback->transp_err != QMI_NO_ERR )
            {
              QCRIL_LOG_INFO("Transp error (%d) recieved from QMI for RIL request %d", qmi_resp_callback->transp_err, req_info.request);
              qcril_send_empty_payload_request_response( instance_id, req_info.t, req_info.request, RIL_E_GENERIC_FAILURE ); // Send GENERIC_FAILURE response
            }
            else
            {
              req_data.t = req_info.t;
              req_data.event_id = req_info.request;
              switch(qmi_resp_callback->msg_id)
              {
              case QMI_IMSA_GET_REGISTRATION_STATUS_RSP_V01:
                qcril_qmi_imsa_get_reg_status_resp_hdlr(&req_data);
                break;

              case QMI_IMSA_GET_SERVICE_STATUS_RSP_V01:
                qcril_qmi_imsa_get_service_status_resp_hdlr(&req_data);
                break;

              case QMI_IMSA_GET_RTP_STATISTICS_RSP_V01:
                qcril_qmi_imsa_get_rtp_statistics_resp_hdlr(&req_data);
                break;

              case QMI_IMSA_SET_GEO_LOC_RSP_V01:
                qcril_qmi_imsa_set_geo_loc_resp_hdlr(&req_data);
                break;

              case QMI_IMSA_GET_VoWIFI_STATUSRSP_V01:
                qcril_qmi_imsa_get_vowifi_status_resp_hdlr(&req_data);
                break;

              default:
                  QCRIL_LOG_INFO("Unsupported QMI IMSA message %d", qmi_resp_callback->msg_id);
                  break;
              }
            }
          }
          else
          {
            QCRIL_LOG_ERROR( "Req ID: %d not found", req_id );
          }
       }
       qcril_free( qmi_resp_callback->data_buf );
    }
    else
    {
       QCRIL_LOG_ERROR("qmi_resp_callback->data_buf is NULL");
    }
  }
  else
  {
    QCRIL_LOG_ERROR("params_ptr or params_ptr->data is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_command_cb_helper

//===========================================================================
// qcril_qmi_imsa_command_cb
//===========================================================================
void qcril_qmi_imsa_command_cb
(
   qmi_client_type              user_handle,
   unsigned int                 msg_id,
   void                        *resp_c_struct,
   unsigned int                 resp_c_struct_len,
   void                        *resp_cb_data,
   qmi_client_error_type        transp_err
)
{
  qmi_resp_callback_type qmi_resp_callback;
  memset(&qmi_resp_callback, 0, sizeof(qmi_resp_callback));

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_LOG_INFO(".. msg id %d", (int) msg_id );

  qmi_resp_callback.user_handle = user_handle;
  qmi_resp_callback.msg_id = msg_id;
  qmi_resp_callback.data_buf = (void*) resp_c_struct;
  qmi_resp_callback.data_buf_len = resp_c_struct_len;
  qmi_resp_callback.cb_data = resp_cb_data;
  qmi_resp_callback.transp_err = transp_err;

  qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                 QCRIL_DEFAULT_MODEM_ID,
                 QCRIL_DATA_ON_STACK,
                 QCRIL_EVT_QMI_IMSA_HANDLE_COMM_CALLBACKS,
                 (void*) &qmi_resp_callback,
                 sizeof(qmi_resp_callback),
                 (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_command_cb

//===========================================================================
// qcril_qmi_imsa_request_ims_registration_state
//===========================================================================
void qcril_qmi_imsa_request_ims_registration_state
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;
  uint32 user_data;
  qcril_reqlist_public_type reqlist_entry;
  imsa_get_registration_status_resp_msg_v01* response_msg = NULL;

  QCRIL_NOTUSED( ret_ptr );

  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_imsa_info_lock();

  if( NULL != params_ptr )
  {
    if (qcril_qmi_imsa_info.inited)
    {
        if(qcril_qmi_imsa_info.ims_registered_valid || qcril_qmi_imsa_info.ims_registration_error_code_valid ||
           qcril_qmi_imsa_info.ims_registration_error_string_valid)
        {
            ims_Registration *reg_ptr = qcril_qmi_imsa_get_ims_registration_info();
            if(reg_ptr != NULL)
            {
                imsRadioSendMessage(params_ptr->t, ims_MsgType_RESPONSE,
                                          ims_MsgId_REQUEST_IMS_REGISTRATION_STATE,
                                          (ims_Error)RIL_E_SUCCESS, reg_ptr, sizeof(*reg_ptr));
                qcril_qmi_ims_free_ims_registration(reg_ptr);
            }
       }
       else
       {
          do
          {
            /* Add entry to ReqList */
            qcril_reqlist_default_entry( params_ptr->t, params_ptr->event_id,
                                         QCRIL_DEFAULT_MODEM_ID,
                                         QCRIL_REQ_AWAITING_CALLBACK,
                                         QCRIL_EVT_NONE, NULL, &reqlist_entry );

            if ( qcril_reqlist_new( instance_id, &reqlist_entry ) != E_SUCCESS )
            {
              /* Fail to add to ReqList */
              qcril_send_empty_payload_request_response( instance_id, params_ptr->t,
                                                         params_ptr->event_id, RIL_E_GENERIC_FAILURE );
              break;
            }

            user_data = QCRIL_COMPOSE_USER_DATA( instance_id, QCRIL_DEFAULT_MODEM_ID,
                                                 reqlist_entry.req_id );

            response_msg = qcril_malloc( sizeof(*response_msg) );
            if( response_msg == NULL )
            {
              qcril_send_empty_payload_request_response( instance_id, params_ptr->t,
                                  params_ptr->event_id, RIL_E_GENERIC_FAILURE );
              break;
            }

            if (qcril_qmi_client_send_msg_async(
                                          QCRIL_QMI_CLIENT_IMSA,
                                          QMI_IMSA_GET_REGISTRATION_STATUS_REQ_V01,
                                          NULL,
                                          0,
                                          response_msg,
                                          sizeof(*response_msg),
                                          (void *)(uintptr_t) user_data) != QMI_NO_ERR)
            {
              qcril_send_empty_payload_request_response( instance_id, params_ptr->t,
                                  params_ptr->event_id, RIL_E_GENERIC_FAILURE );
              if( response_msg != NULL )
              {
                  qcril_free( response_msg );
              }
            }

          } while(0);
       }
    }
    else
    {
       qcril_send_empty_payload_request_response( instance_id, params_ptr->t, params_ptr->event_id, RIL_E_GENERIC_FAILURE );
    }
  }
  else
  {
    QCRIL_LOG_ERROR("params_ptr is NULL");
  }

  qcril_qmi_imsa_info_unlock();
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_request_ims_registration_state

//===========================================================================
// qcril_qmi_imsa_request_query_ims_srv_status
//===========================================================================
void qcril_qmi_imsa_request_query_ims_srv_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
    qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;
    uint32 user_data;
    qcril_reqlist_public_type reqlist_entry;
    imsa_get_service_status_resp_msg_v01* response_msg = NULL;
    boolean failed = FALSE;

    QCRIL_NOTUSED( ret_ptr );

    QCRIL_LOG_FUNC_ENTRY();

    if( NULL != params_ptr )
    {
        qcril_qmi_imsa_info_lock();
        if (qcril_qmi_imsa_info.inited)
        {
            if (qcril_qmi_imsa_info.ims_srv_status_valid && qcril_qmi_imsa_info.ims_status_change_registered)
            { // Use cached info to reply back
                ims_SrvStatusList *ims_srv_status_list_ptr =
                    qcril_qmi_ims_create_ims_srvstatusinfo(&qcril_qmi_imsa_info.ims_srv_status);
                imsRadioSendMessage(
                    params_ptr->t,
                    ims_MsgType_RESPONSE,
                    ims_MsgId_REQUEST_QUERY_SERVICE_STATUS,
                    ims_Error_E_SUCCESS,
                    ims_srv_status_list_ptr,
                    sizeof(*ims_srv_status_list_ptr) );
                qcril_qmi_ims_free_srvstatuslist(ims_srv_status_list_ptr);
            }
            else
            {
                do
                {
                    /* Add entry to ReqList */
                    qcril_reqlist_default_entry( params_ptr->t, params_ptr->event_id,
                                                 QCRIL_DEFAULT_MODEM_ID,
                                                 QCRIL_REQ_AWAITING_CALLBACK,
                                                 QCRIL_EVT_NONE,
                                                 NULL,
                                                 &reqlist_entry );

                    if ( qcril_reqlist_new( instance_id, &reqlist_entry ) != E_SUCCESS )
                    {
                        /* Fail to add to ReqList */
                        failed = TRUE;
                        break;
                    }
                    user_data = QCRIL_COMPOSE_USER_DATA( instance_id,
                                                         QCRIL_DEFAULT_MODEM_ID,
                                                         reqlist_entry.req_id );

                    response_msg = qcril_malloc( sizeof(*response_msg) );
                    if( response_msg == NULL )
                    {
                        failed = TRUE;
                        break;
                    }

                    if ( qcril_qmi_client_send_msg_async(
                             QCRIL_QMI_CLIENT_IMSA,
                             QMI_IMSA_GET_SERVICE_STATUS_REQ_V01,
                             NULL, 0,
                             response_msg, sizeof(*response_msg),
                             (void *)(uintptr_t) user_data) != QMI_NO_ERR )
                    {
                        failed = TRUE;
                        break;
                    }
                } while(0);
            }
        }
        else
        {
            failed = TRUE;
        }
        qcril_qmi_imsa_info_unlock();

        if (failed)
        {
            qcril_send_empty_payload_request_response(
                instance_id,
                params_ptr->t,
                params_ptr->event_id,
                RIL_E_GENERIC_FAILURE );

            if( response_msg != NULL )
            {
                qcril_free( response_msg );
            }
        }
    }
    else
    {
        QCRIL_LOG_ERROR("params_ptr is NULL");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_request_query_ims_srv_status

//===========================================================================
// qcril_qmi_imsa_request_get_rtp_statistics
//===========================================================================
void qcril_qmi_imsa_request_get_rtp_statistics
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
    qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;
    uint32 user_data;
    qcril_reqlist_public_type reqlist_entry;
    imsa_get_rtp_statistics_resp_msg_v01* response_msg = NULL;
    boolean failed = FALSE;

    QCRIL_NOTUSED( ret_ptr );

    QCRIL_LOG_FUNC_ENTRY();

    if( NULL != params_ptr )
    {
        qcril_qmi_imsa_info_lock();
        if (qcril_qmi_imsa_info.inited)
        {
            do
            {
                /* Add entry to ReqList */
                qcril_reqlist_default_entry( params_ptr->t,
                                             params_ptr->event_id,
                                             QCRIL_DEFAULT_MODEM_ID,
                                             QCRIL_REQ_AWAITING_CALLBACK,
                                             QCRIL_EVT_NONE,
                                             NULL,
                                             &reqlist_entry );

                if ( qcril_reqlist_new( instance_id, &reqlist_entry ) != E_SUCCESS )
                {
                    /* Fail to add to ReqList */
                    failed = TRUE;
                    break;
                }
                user_data = QCRIL_COMPOSE_USER_DATA( instance_id,
                                                     QCRIL_DEFAULT_MODEM_ID,
                                                     reqlist_entry.req_id );

                response_msg = qcril_malloc( sizeof(*response_msg) );
                if( response_msg == NULL )
                {
                    failed = TRUE;
                    break;
                }

                if ( qcril_qmi_client_send_msg_async(
                         QCRIL_QMI_CLIENT_IMSA,
                         QMI_IMSA_GET_RTP_STATISTICS_REQ_V01,
                         NULL, 0,
                         response_msg, sizeof(*response_msg),
                         (void *)(uintptr_t) user_data) != QMI_NO_ERR )
                {
                    failed = TRUE;
                    break;
                }
            } while(0);

            if (failed)
            {
                qcril_send_empty_payload_request_response(
                    instance_id,
                    params_ptr->t,
                    params_ptr->event_id,
                    RIL_E_GENERIC_FAILURE );

                if( response_msg != NULL )
                {
                    qcril_free( response_msg );
                }
            }
        }
        qcril_qmi_imsa_info_unlock();
    }
    else
    {
        QCRIL_LOG_ERROR("params_ptr is NULL");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_request_get_rtp_statistics

//===========================================================================
// qcril_qmi_imsa_request_get_vowifi_call_quality
//===========================================================================
void qcril_qmi_imsa_request_get_vowifi_call_quality
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
    qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;
    uint32 user_data;
    qcril_reqlist_public_type reqlist_entry;
    imsa_get_vowifi_status_resp_msg_v01* response_msg = NULL;
    boolean failed = FALSE;

    QCRIL_NOTUSED( ret_ptr );

    QCRIL_LOG_FUNC_ENTRY();

    if( NULL != params_ptr )
    {
        qcril_qmi_imsa_info_lock();
        if (qcril_qmi_imsa_info.inited)
        {
            do
            {
                /* Add entry to ReqList */
                qcril_reqlist_default_entry( params_ptr->t,
                                             params_ptr->event_id,
                                             QCRIL_DEFAULT_MODEM_ID,
                                             QCRIL_REQ_AWAITING_CALLBACK,
                                             QCRIL_EVT_NONE,
                                             NULL,
                                             &reqlist_entry );

                if ( qcril_reqlist_new( instance_id, &reqlist_entry ) != E_SUCCESS )
                {
                    /* Fail to add to ReqList */
                    failed = TRUE;
                    break;
                }
                user_data = QCRIL_COMPOSE_USER_DATA( instance_id,
                                                     QCRIL_DEFAULT_MODEM_ID,
                                                     reqlist_entry.req_id );

                response_msg = qcril_malloc( sizeof(*response_msg) );
                if( response_msg == NULL )
                {
                    failed = TRUE;
                    break;
                }

                if ( qcril_qmi_client_send_msg_async(
                         QCRIL_QMI_CLIENT_IMSA,
                         QMI_IMSA_GET_VoWIFI_STATUS_REQ_V01,
                         NULL, 0,
                         response_msg, sizeof(*response_msg),
                         (void *)(uintptr_t) user_data) != QMI_NO_ERR )
                {
                    failed = TRUE;
                    break;
                }
            } while(0);

            if (failed)
            {
                QCRIL_LOG_ERROR("Failed to process... ");
                qcril_send_empty_payload_request_response(
                    instance_id,
                    params_ptr->t,
                    params_ptr->event_id,
                    RIL_E_GENERIC_FAILURE );

                if( response_msg != NULL )
                {
                    qcril_free( response_msg );
                }
            }
        }
        qcril_qmi_imsa_info_unlock();
    }
    else
    {
        QCRIL_LOG_ERROR("params_ptr is NULL");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imsa_request_get_vowifi_call_quality

//===========================================================================
// qcril_qmi_imsa_is_ims_voip_registered
//===========================================================================
boolean qcril_qmi_imsa_is_ims_voip_in_service()
{
    boolean ret = FALSE;
    qcril_qmi_imsa_info_lock();
    if (qcril_qmi_imsa_info.ims_registered_valid &&
        (ims_Registration_RegState_REGISTERED ==
         qcril_qmi_ims_map_qmi_ims_reg_state_to_ims_reg_state(qcril_qmi_imsa_info.ims_registered))&&
        qcril_qmi_imsa_info.ims_srv_status_valid &&
        qcril_qmi_imsa_info.ims_srv_status.voip_service_status_valid &&
        (qcril_qmi_imsa_info.ims_srv_status.voip_service_status != IMSA_NO_SERVICE_V01))
    {
        ret = TRUE;
    }
    qcril_qmi_imsa_info_unlock();
    return ret;
} // qcril_qmi_imsa_is_ims_voip_in_service

//===========================================================================
// qcril_qmi_imsa_get_srv_status
//===========================================================================
qcril_qmi_imsa_srv_status_type qcril_qmi_imsa_get_srv_status()
{
    qcril_qmi_imsa_srv_status_type srv_status;
    qcril_qmi_imsa_info_lock();
    if (qcril_qmi_imsa_info.ims_srv_status_valid)
    {
        memcpy(&srv_status, &qcril_qmi_imsa_info.ims_srv_status, sizeof(srv_status));
    }
    else
    {
        memset(&srv_status, 0, sizeof(srv_status));
    }
    qcril_qmi_imsa_info_unlock();
    return srv_status;
} // qcril_qmi_imsa_get_srv_status

/*=========================================================================
  FUNCTION:  qcril_qmi_imsa_ims_registered_wlan_status

===========================================================================*/
/*!
    @brief
    Checking whether IMS registered over WLAN RAT through

    @return
    If registered over WLAN, return TRUE
    Else return FALSE
*/
/*=========================================================================*/
uint8_t qcril_qmi_imsa_ims_registered_wlan_status()
{
    uint8_t status = FALSE;
    uint8_t wifi_call_valid = FALSE;
    qcril_ims_setting_wfc_status_type wifi_call;

    qcril_qmi_imss_fetch_wifi_call_info(&wifi_call_valid, &wifi_call);

    QCRIL_LOG_DEBUG( "WiFi Call valid %d, WiFi Call %d", wifi_call_valid, wifi_call);

    qcril_qmi_imsa_info_lock();

    QCRIL_LOG_DEBUG( "IMS registered valid %d, Status %d", qcril_qmi_imsa_info.ims_registered_valid, qcril_qmi_imsa_info.ims_registered);
    QCRIL_LOG_DEBUG( "IMS service status valid %d", qcril_qmi_imsa_info.ims_srv_status_valid);
    QCRIL_LOG_DEBUG( "VOIP service STATUS valid %d, VOIP service STATUS %d", qcril_qmi_imsa_info.ims_srv_status.voip_service_status_valid,
       qcril_qmi_imsa_info.ims_srv_status.voip_service_status);
    QCRIL_LOG_DEBUG( "VOIP service RAT valid %d, VOIP service RAT %d", qcril_qmi_imsa_info.ims_srv_status.voip_service_rat_valid,
       qcril_qmi_imsa_info.ims_srv_status.voip_service_rat);
    QCRIL_LOG_DEBUG( "VT service STATUS valid %d, VT service STATUS %d", qcril_qmi_imsa_info.ims_srv_status.vt_service_status_valid,
       qcril_qmi_imsa_info.ims_srv_status.vt_service_status);
    QCRIL_LOG_DEBUG( "VT service RAT valid %d, VT service RAT %d", qcril_qmi_imsa_info.ims_srv_status.vt_service_rat_valid,
       qcril_qmi_imsa_info.ims_srv_status.vt_service_rat);

    if(!wifi_call_valid || (QCRIL_IMS_SETTING_WFC_ON != wifi_call) )
    {
        QCRIL_LOG_DEBUG( "WFC is NOT ON");
    }
    else if(qcril_qmi_imsa_info.ims_registered_valid &&
       (IMSA_STATUS_REGISTERED_V01 == qcril_qmi_imsa_info.ims_registered) )
    {
        if(qcril_qmi_imsa_info.ims_srv_status_valid)
        {
            if( (qcril_qmi_imsa_info.ims_srv_status.voip_service_status_valid &&
                (IMSA_FULL_SERVICE_V01 == qcril_qmi_imsa_info.ims_srv_status.voip_service_status) ) &&
                (qcril_qmi_imsa_info.ims_srv_status.voip_service_rat_valid &&
                (IMSA_WLAN_V01 == qcril_qmi_imsa_info.ims_srv_status.voip_service_rat) )
              )
            {
                status = TRUE;
            }
            else if( (qcril_qmi_imsa_info.ims_srv_status.vt_service_status_valid &&
                     (IMSA_FULL_SERVICE_V01 == qcril_qmi_imsa_info.ims_srv_status.vt_service_status) ) &&
                     (qcril_qmi_imsa_info.ims_srv_status.vt_service_rat_valid &&
                     (IMSA_WLAN_V01 == qcril_qmi_imsa_info.ims_srv_status.vt_service_rat) )
                   )
            {
                status = TRUE;
            }
        }
    }
    qcril_qmi_imsa_info_unlock();

    QCRIL_LOG_DEBUG( "IMS registered over WLAN STATUS and also WFC is ON %d", status);
    return status;
}

boolean qcril_qmi_imsa_update_wlan_registration_status()
{
  boolean changed = FALSE;
  QCRIL_LOG_FUNC_ENTRY();
  qcril_qmi_imsa_info_lock();
  boolean wlan_status = qcril_qmi_imsa_ims_registered_wlan_status();
  if(qcril_qmi_imsa_info.ims_wlan_registration_status != wlan_status)
  {
     QCRIL_LOG_DEBUG("updating wlan reg status from %d to %d", qcril_qmi_imsa_info.ims_wlan_registration_status, wlan_status);
     qcril_qmi_imsa_info.ims_wlan_registration_status = wlan_status;
     changed = TRUE;
  }
  qcril_qmi_imsa_info_unlock();
  QCRIL_LOG_FUNC_RETURN_WITH_RET(changed);
  return changed;
}

uint8_t qcril_qmi_imsa_compare_ims_registered_report_unsol()
{
    uint8_t ret = FALSE;
    uint8_t old_data_rat_status = FALSE;
    uint8_t imsa_wlan_status = FALSE;
    uint8_t imss_wlan_status = FALSE;
    uint8_t dsd_wlan_status = FALSE;
    uint32_t dsd_wlan_index = QMI_RIL_ZERO;
    uint8_t reported_tech_valid = FALSE;
    int reported_tech;

    QCRIL_LOG_FUNC_ENTRY();
    dsd_wlan_status = qcril_arb_check_wlan_rat_dsd_reported_helper(&dsd_wlan_index);
    old_data_rat_status = qcril_qmi_nas_fetch_data_reg_rat(&reported_tech_valid, &reported_tech);

    if(!dsd_wlan_status)
    {
        return ret;
    }

    imsa_wlan_status = qcril_qmi_imsa_ims_registered_wlan_status();
    QCRIL_LOG_INFO("IMS registered over WLAN status %d", imsa_wlan_status);

    if(imsa_wlan_status)
    {
        if(!old_data_rat_status || (RADIO_TECH_IWLAN != reported_tech))
        {
            QCRIL_LOG_INFO("IMS registered over WLAN: notify ATeL as needed");
            ret = TRUE;
        }
    }
    else
    {
        imss_wlan_status = qcril_qmi_imss_client_provisioning_wlan_status();
        QCRIL_LOG_INFO("WFC available over WLAN status %d", imss_wlan_status);

        if(!imss_wlan_status && old_data_rat_status && (RADIO_TECH_IWLAN == reported_tech))
        {
            QCRIL_LOG_INFO("IMS de-registered over WLAN: notify ATeL as needed");
            ret = TRUE;
        }
    }

    return ret;
}

void qcril_qmi_imsa_fetch_ims_reg_status(uint8_t *ims_registered_valid, uint8_t *ims_registered)
{
    qcril_qmi_imsa_info_lock();
    *ims_registered_valid = qcril_qmi_imsa_info.ims_registered_valid;
    *ims_registered = qcril_qmi_imsa_info.ims_registered;
    qcril_qmi_imsa_info_unlock();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_imsa_is_ims_registered_for_voip_vt_service

===========================================================================*/
/*!
    @brief
    Checking whether IMS registered for VOIP/VT service or not.

    @return
    If registered, return TRUE
    Else return FALSE
*/
/*=========================================================================*/
uint8_t qcril_qmi_imsa_is_ims_registered_for_voip_vt_service()
{
    uint8_t status = FALSE;

    qcril_qmi_imsa_info_lock();

    QCRIL_LOG_DEBUG( "IMS registered valid %d, Status %d", qcril_qmi_imsa_info.ims_registered_valid, qcril_qmi_imsa_info.ims_registered);
    QCRIL_LOG_DEBUG( "IMS service status valid %d", qcril_qmi_imsa_info.ims_srv_status_valid);
    QCRIL_LOG_DEBUG( "VOIP service STATUS valid %d, VOIP service STATUS %d", qcril_qmi_imsa_info.ims_srv_status.voip_service_status_valid,
       qcril_qmi_imsa_info.ims_srv_status.voip_service_status);
    QCRIL_LOG_DEBUG( "VT service STATUS valid %d, VT service STATUS %d", qcril_qmi_imsa_info.ims_srv_status.vt_service_status_valid,
       qcril_qmi_imsa_info.ims_srv_status.vt_service_status);

    if(qcril_qmi_imsa_info.ims_registered_valid &&
       (IMSA_STATUS_REGISTERED_V01 == qcril_qmi_imsa_info.ims_registered) )
    {
        if(qcril_qmi_imsa_info.ims_srv_status_valid)
        {
            if( qcril_qmi_imsa_info.ims_srv_status.voip_service_status_valid &&
                (IMSA_FULL_SERVICE_V01 == qcril_qmi_imsa_info.ims_srv_status.voip_service_status)
              )
            {
                status = TRUE;
            }
            else if( qcril_qmi_imsa_info.ims_srv_status.vt_service_status_valid &&
                     (IMSA_FULL_SERVICE_V01 == qcril_qmi_imsa_info.ims_srv_status.vt_service_status)
                   )
            {
                status = TRUE;
            }
        }
    }
    qcril_qmi_imsa_info_unlock();

    QCRIL_LOG_DEBUG( "IMS registered for VOIP or VT service %d", status);
    return status;
}

//===========================================================================
//@function qcril_qmi_imsa_ims_capability_ind_hdlr
//
//@brief Handler for ims capability indication
//===========================================================================
void qcril_qmi_imsa_ims_capability_ind_hdlr
(
    dms_ims_capability_ind_msg_v01 * payload
)
{
    ims_ImsSubConfig ims_sub_config_resp;
    uint64_t ims_sub_config[QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01];
    uint64_t* ims_sub_config_arr[QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01 + 1];
    int i;

    QCRIL_LOG_FUNC_ENTRY();
    if(NULL != payload)
    {
        qcril_qmi_imsa_info_lock();
        memset(&ims_sub_config_resp, 0, sizeof(ims_sub_config_resp));
        qcril_qmi_imsa_info.ims_capability_len = payload->ims_capability_len;
        memcpy(qcril_qmi_imsa_info.ims_capability, payload->ims_capability,
               sizeof(qcril_qmi_imsa_info.ims_capability));

        if(qcril_qmi_imsa_info.max_ims_instances_valid)
        {
            ims_sub_config_resp.has_simultStackCount = TRUE;
            ims_sub_config_resp.simultStackCount = qcril_qmi_imsa_info.max_ims_instances;
        }

        memset(&ims_sub_config, 0, sizeof(ims_sub_config));
        qcril_qmi_imsa_get_ims_sub_configs(ims_sub_config, QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01);
        memset(&ims_sub_config_arr, 0, sizeof(ims_sub_config_arr));

        for(i = 0; i < QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01; ++i)
        {
            ims_sub_config_arr[i] = &ims_sub_config[i];
        }
        ims_sub_config_resp.imsStackEnabled.arg = &ims_sub_config_arr;
        qcril_qmi_imsa_info_unlock();

        imsRadioSendMessage(0, ims_MsgType_UNSOL_RESPONSE,
                    ims_MsgId_UNSOL_IMS_SUB_CONFIG_CHANGED, ims_Error_E_SUCCESS,
                    &ims_sub_config_resp, sizeof(ims_sub_config_resp));
    }
    else
    {
        QCRIL_LOG_ERROR("Indication payload is null.");
    }

    QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
//@function qcril_qmi_imsa_request_get_ims_sub_config
//
//@brief Handle event QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_SUB_CONFIG
//===========================================================================
void qcril_qmi_imsa_request_get_ims_sub_config
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr // Output parameter
)
{
    RIL_Errno res = RIL_E_GENERIC_FAILURE;
    ims_ImsSubConfig ims_sub_config_resp;
    uint64_t ims_sub_config[QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01];
    uint64_t* ims_sub_config_arr[QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01 + 1];
    int i;
    QCRIL_NOTUSED(ret_ptr); // Output parameter

    QCRIL_LOG_FUNC_ENTRY();

    res = qcril_qmi_nas_get_device_capability(FALSE, FALSE, FALSE, TRUE);

    if(NULL != params_ptr)
    {
        if(res == RIL_E_SUCCESS)
        {
            qcril_qmi_imsa_info_lock();
            memset(&ims_sub_config_resp, 0, sizeof(ims_sub_config_resp));
            if(qcril_qmi_imsa_info.max_ims_instances_valid)
            {
                ims_sub_config_resp.has_simultStackCount = TRUE;
                ims_sub_config_resp.simultStackCount = qcril_qmi_imsa_info.max_ims_instances;
            }
            else
            {
                qcril_qmi_imsa_info_unlock();
                imsRadioSendMessage(params_ptr->t, ims_MsgType_RESPONSE,
                        ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG, ims_Error_E_REQUEST_NOT_SUPPORTED,
                        NULL, 0);
                QCRIL_LOG_INFO("max_ims_instances is not valid, sending unsupported error.");
                QCRIL_LOG_FUNC_RETURN();
                return;
            }
            if(qcril_qmi_imsa_info.ims_capability_len > 0)
            {
                memset(&ims_sub_config, 0, sizeof(ims_sub_config));
                qcril_qmi_imsa_get_ims_sub_configs(ims_sub_config,
                        QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01);
                memset(&ims_sub_config_arr, 0, sizeof(ims_sub_config_arr));

                for(i = 0; i < QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01; ++i)
                {
                    ims_sub_config_arr[i] = &ims_sub_config[i];
                }
                ims_sub_config_resp.imsStackEnabled.arg = &ims_sub_config_arr;
            }
            qcril_qmi_imsa_info_unlock();
            imsRadioSendMessage(params_ptr->t, ims_MsgType_RESPONSE,
                    ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG, ims_Error_E_SUCCESS,
                    &ims_sub_config_resp, sizeof(ims_sub_config_resp));
        }
        else
        {
            imsRadioSendMessage(params_ptr->t, ims_MsgType_RESPONSE,
                    ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG, ims_Error_E_GENERIC_FAILURE, NULL, 0);
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
//@function qcril_qmi_imsa_store_ims_capability
//
//@brief Utility function to store the ims capability in the cache.
//===========================================================================
void qcril_qmi_imsa_store_ims_capability
(
    uint8_t max_ims_instances_valid,
    uint8_t max_ims_instances,
    uint8_t ims_capability_valid,
    uint32_t ims_capability_len,
    dms_ims_capability_type_v01* ims_capability
)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_qmi_imsa_info_lock();
    qcril_qmi_imsa_info.max_ims_instances_valid = max_ims_instances_valid;
    qcril_qmi_imsa_info.max_ims_instances = max_ims_instances;

    memset(&qcril_qmi_imsa_info.ims_capability, 0,
           sizeof(qcril_qmi_imsa_info.ims_capability));

    if( ims_capability_valid )
    {
        qcril_qmi_imsa_info.ims_capability_len = ims_capability_len;
        memcpy(qcril_qmi_imsa_info.ims_capability, ims_capability,
               sizeof(qcril_qmi_imsa_info.ims_capability));
    }
    else
    {
        qcril_qmi_imsa_info.ims_capability_len = 0;
    }

    qcril_qmi_imsa_info_unlock();
    QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
//@function qcril_qmi_imsa_get_ims_sub_configs
//
//@brief Utility function to convert qmi response to telephony ims response.
//===========================================================================
void qcril_qmi_imsa_get_ims_sub_configs(uint64_t* ims_sub_configs, uint8_t max_cap_len)
{
    uint32_t i;
    uint32_t ims_cap_len;
    dms_ims_capability_type_v01 ims_cap[QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01];

    QCRIL_LOG_FUNC_ENTRY();
    ims_cap_len = qcril_qmi_imsa_info.ims_capability_len;
    memset(&ims_cap, 0, sizeof(ims_cap));
    memcpy(ims_cap, qcril_qmi_imsa_info.ims_capability, sizeof(ims_cap));

    for(i = 0; i < ims_cap_len && ims_cap_len <= max_cap_len; ++i)
    {
        if(ims_cap[i].subscription == DMS_PRIMARY_SUBS_V01)
        {
            ims_sub_configs[RIL_SUBSCRIPTION_1] = ims_cap[i].enabled;
            QCRIL_LOG_INFO("Primary sub ims_cap:%d", ims_cap[i].enabled);
        }
        if(ims_cap[i].subscription == DMS_SECONDARY_SUBS_V01)
        {
            ims_sub_configs[RIL_SUBSCRIPTION_2] = ims_cap[i].enabled;
            QCRIL_LOG_INFO("Secondary sub ims_cap:%d", ims_cap[i].enabled);
        }
        if(ims_cap[i].subscription == DMS_TERTIARY_SUBS_V01)
        {
            ims_sub_configs[RIL_SUBSCRIPTION_3] = ims_cap[i].enabled;
            QCRIL_LOG_INFO("Tertiary sub ims_cap:%d", ims_cap[i].enabled);
        }
    }
    QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:   copy_qmi_blk_status_to_ims_registration_block_status

===========================================================================*/
/*!
    @brief
    Copy Block Status Indication information.

    @return
    If copied information, return TRUE
    Else return FALSE
*/
/*=========================================================================*/
uint8_t copy_qmi_blk_status_to_ims_registration_block_status
(
    imsa_ims_block_status_ind_msg_v01 *src,
    ims_RegistrationBlockStatus *dest
)
{
  if (NULL != src)
  {
    //Wlan Failures
    if(src->ims_failure_on_wlan_valid)
    {
      dest->has_imsBlockStatusOnWlan = TRUE;
      qcril_qmi_imsa_map_imsa_block_status_to_ims_block_status(
              &src->ims_failure_on_wlan, &dest->imsBlockStatusOnWlan);
    }

    //Wwan Failures
    if(src->ims_failure_on_wwan_valid)
    {
      dest->has_imsBlockStatusOnWwan = TRUE;
      qcril_qmi_imsa_map_imsa_block_status_to_ims_block_status(
              &src->ims_failure_on_wwan, &dest->imsBlockStatusOnWwan);
    }
    return TRUE;
  }
  else
  {
    QCRIL_LOG_ERROR("ims_error: Empty indication for block status!");
    return FALSE;
  }
}

//=========================================================================
//  FUNCTION:   qcril_qmi_imsa_map_imsa_block_status_to_ims_block_status
//=========================================================================
void qcril_qmi_imsa_map_imsa_block_status_to_ims_block_status
(
    imsa_ims_failure_reason_type_v01* imsa_block_status,
    ims_BlockStatus* ims_block_status
)
{
  ims_block_status->has_blockReason =  TRUE;
  ims_block_status->blockReason = qcril_qmi_ims_map_qmi_block_reason_to_ims_block_reason(
          imsa_block_status->block_reason_mask);

  switch(imsa_block_status->block_reason_mask)
  {
    case IMSA_BLOCK_REASON_PDP_FAILURE_BIT_MASK_V01:
      //PDP failures
      ims_block_status->has_blockReasonDetails = TRUE;
      ims_block_status->blockReasonDetails.has_regFailureReasonType = TRUE;
      ims_block_status->blockReasonDetails.regFailureReasonType =
      qcril_qmi_call_end_type_to_ims_reg_failure(
              imsa_block_status->pdp_failure_reason.call_end_reason_type);
      ims_block_status->blockReasonDetails.has_RegFailureReason = TRUE;
      ims_block_status->blockReasonDetails.RegFailureReason =
              imsa_block_status->pdp_failure_reason.call_end_reason;
    break;
    case IMSA_BLOCK_REASON_REGISTRATION_FAILURE_BIT_MASK_V01:
      //Registration failures
      ims_block_status->has_blockReasonDetails = TRUE;
      ims_block_status->blockReasonDetails.has_regFailureReasonType = FALSE;
      ims_block_status->blockReasonDetails.has_RegFailureReason = TRUE;
      ims_block_status->blockReasonDetails.RegFailureReason =
              imsa_block_status->registration_failure_reason;
    break;
    case IMSA_BLOCK_REASON_HANDOVER_FAILURE_BIT_MASK_V01:
      //HandOver failures
      ims_block_status->has_blockReasonDetails = TRUE;
      ims_block_status->blockReasonDetails.has_regFailureReasonType = TRUE;
      ims_block_status->blockReasonDetails.regFailureReasonType =
              qcril_qmi_call_end_type_to_ims_reg_failure(
              imsa_block_status->handover_failure_reason.call_end_reason_type);
      ims_block_status->blockReasonDetails.has_RegFailureReason = TRUE;
      ims_block_status->blockReasonDetails.RegFailureReason =
              imsa_block_status->handover_failure_reason.call_end_reason;
    break;
    default:
      QCRIL_LOG_INFO("ims_error: Unknwon  registration block status mask: %d",
              imsa_block_status->block_reason_mask);
    break;
  }
}
