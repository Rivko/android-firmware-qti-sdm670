/******************************************************************************
  @file    qcril_qmi_radio_config_dispatch_helper.c
  @brief   qcril qmi - radio config set/get function handlers

  DESCRIPTION
    Handles radio config set/get functions.
    Dispatches the config items by grouping them wrt
    their QMI messages, and calling the respective
    function handlers.

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/
#include "common_v01.h"
#include "qcril_qmi_radio_config_dispatch_helper.h"
#include "qcril_qmi_radio_config_misc.h"
#include "qcril_qmi_radio_config_data.h"
#include "qcril_qmi_radio_config_socket.h"
#include "qcrili.h"
#include "qcril_qmi_radio_config_packing.h"
#include "radio_config_interface.pb.h"
#include "qcril_qmi_lte_direct_disc_dispatcher.h"

/**
 * Analyzes and processes the set_config request data.
 */
void qcril_qmi_radio_config_dispatch_set_request
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  qcril_qmi_radio_config_data_quality_measurement_req_type *data_set_quality_req_ptr = NULL;
  com_qualcomm_qti_radioconfiginterface_ConfigMsg *resp_ptr = NULL;
  size_t msg_len = 0;

  QCRIL_LOG_FUNC_ENTRY();
  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("Invalid arguments, cannot process further..");
    return;
  }

  do {
    com_qualcomm_qti_radioconfiginterface_ConfigMsg* config_msg_ptr =
      (com_qualcomm_qti_radioconfiginterface_ConfigMsg*)(params_ptr->data);
    if(config_msg_ptr == NULL)
    {
      //To return NULL RESPONSE?? or to handle such case in Java layer
      //could be that anotgher client(not our java service) might send it this way.
      //HOW TO HANDLE THIS?
      QCRIL_LOG_ERROR("No config items in the message!");
      break; //or send empty response with error?
    }

    QCRIL_LOG_INFO("SET_RADIO_CONFIG_REQ with %d config items.", config_msg_ptr->settings_count);

    //HARDCODED, NEEDS TO CHANGE WHILE MERGING.
    //THIS IS FOR TEST PURPOSE, BEFORE THE FINAL CHANGE IS READY.
    size_t i;
    com_qualcomm_qti_radioconfiginterface_ConfigItemMsg temp;
    qcril_qmi_radio_config_item_type item;

    qcril_qmi_radio_config_data_quality_measurement_req_type* data_set_quality_req_ptr =
      qcril_malloc(sizeof(qcril_qmi_radio_config_data_quality_measurement_req_type));
    if(data_set_quality_req_ptr == NULL)
    {
      QCRIL_LOG_ERROR("malloc failed");
      break;
    }

    uint8_t data_filled = FALSE;

    //response ptr
    resp_ptr = qcril_malloc(sizeof(com_qualcomm_qti_radioconfiginterface_ConfigMsg));
    if(resp_ptr == NULL)
    {
      QCRIL_LOG_ERROR("malloc failed");
      break;
    }
    resp_ptr->settings_count = config_msg_ptr->settings_count;
    resp_ptr->settings = qcril_malloc(config_msg_ptr->settings_count *
        sizeof(com_qualcomm_qti_radioconfiginterface_ConfigItemMsg));
    if(resp_ptr->settings == NULL)
    {
      resp_ptr->settings_count = 0;
      QCRIL_LOG_ERROR("malloc failed");
      break;
    }

    msg_len = sizeof(resp_ptr);
    uint8_t is_completed = FALSE;

    if (qcril_qmi_lte_direct_disc_dispatcher_req_handler(config_msg_ptr, resp_ptr))
    {
      QCRIL_LOG_INFO("Handled in qcril_qmi_lte_direct_disc_dispatcher; completed!!!");
      is_completed = TRUE;
    }

    for(i = 0; !is_completed && i < config_msg_ptr->settings_count; i++)
    {
      temp = config_msg_ptr->settings[i];
      item = qcril_qmi_radio_config_map_socket_item_to_config_item(temp.item);

      resp_ptr->settings[i].item = temp.item;
      resp_ptr->settings[i].has_errorCause = TRUE;
      resp_ptr->settings[i].errorCause =
        com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_NO_ERR;

      QCRIL_LOG_INFO("item id: %d to be set to: %d", item, temp.intValue);

      switch(item)
      {
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->in_call_lte_rsrp_low_valid = TRUE;
              data_set_quality_req_ptr->in_call_lte_rsrp_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_MID:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->in_call_lte_rsrp_mid_valid = TRUE;
              data_set_quality_req_ptr->in_call_lte_rsrp_mid = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->in_call_lte_rsrp_high_valid = TRUE;
              data_set_quality_req_ptr->in_call_lte_rsrp_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->in_call_wifi_rssi_threshold_low_valid = TRUE;
              data_set_quality_req_ptr->in_call_wifi_rssi_threshold_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->in_call_wifi_rssi_threshold_high_valid = TRUE;
              data_set_quality_req_ptr->in_call_wifi_rssi_threshold_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX ))
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->in_call_wifi_sinr_threshold_low_valid = TRUE;
              data_set_quality_req_ptr->in_call_wifi_sinr_threshold_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->in_call_wifi_sinr_threshold_high_valid = TRUE;
              data_set_quality_req_ptr->in_call_wifi_sinr_threshold_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->idle_lte_rsrp_low_valid = TRUE;
              data_set_quality_req_ptr->idle_lte_rsrp_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_MID:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->idle_lte_rsrp_mid_valid = TRUE;
              data_set_quality_req_ptr->idle_lte_rsrp_mid = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->idle_lte_rsrp_high_valid = TRUE;
              data_set_quality_req_ptr->idle_lte_rsrp_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->idle_wifi_rssi_threshold_low_valid = TRUE;
              data_set_quality_req_ptr->idle_wifi_rssi_threshold_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->idle_wifi_rssi_threshold_high_valid = TRUE;
              data_set_quality_req_ptr->idle_wifi_rssi_threshold_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {

              data_filled = TRUE;
              data_set_quality_req_ptr->idle_wifi_sinr_threshold_low_valid = TRUE;
              data_set_quality_req_ptr->idle_wifi_sinr_threshold_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->idle_wifi_sinr_threshold_high_valid = TRUE;
              data_set_quality_req_ptr->idle_wifi_sinr_threshold_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->ecio_1x_threshold_low_valid = TRUE;
              data_set_quality_req_ptr->ecio_1x_threshold_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req_ptr->ecio_1x_threshold_high_valid = TRUE;
              data_set_quality_req_ptr->ecio_1x_threshold_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_ptr->settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        default:
          QCRIL_LOG_DEBUG("Unhandled item %d", item);
      }
    }
    //there are some valid config items! so send the info to data
    if(data_filled)
    {
      QCRIL_LOG_INFO("Data req ptr is filled, sending request to data!");

#ifndef QMI_RIL_UTF
      qcril_qmi_radio_config_data_resp_type data_resp =
        qcril_data_set_quality_measurement(data_set_quality_req_ptr);

      if(data_resp.resp.result != QMI_RESULT_SUCCESS_V01)
      {
        //if not success fill in the data structure with error
        for(i = 0; i < config_msg_ptr->settings_count; i++)
        {
          if(/*config_msg_ptr->settings[i] != NULL &&*/
              resp_ptr->settings[i].errorCause ==
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_NO_ERR)
          {
            resp_ptr->settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
        }
      }
#endif
    }

  } while (FALSE);

  qcril_qmi_radio_config_socket_send(params_ptr->t,
      com_qualcomm_qti_radioconfiginterface_MessageType_RADIO_CONFIG_MSG_RESPONSE,
      com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_SET_CONFIG,
      0, 0, (void*)resp_ptr, msg_len);

  qcril_free(data_set_quality_req_ptr);
  if (resp_ptr)
  {
    qcril_free(resp_ptr->settings);
    qcril_free(resp_ptr);
  }

  QCRIL_LOG_FUNC_RETURN();
}


/**
 * Analyzes and processes the get_config request data.
 */
void qcril_qmi_radio_config_dispatch_get_request
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  com_qualcomm_qti_radioconfiginterface_ConfigMsg* resp_ptr = NULL;
  size_t msg_len = 0;

  QCRIL_LOG_FUNC_ENTRY();
  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("Invalid arguments, cannot process further..");
    return;
  }

  do {
    com_qualcomm_qti_radioconfiginterface_ConfigMsg* config_msg_ptr =
      (com_qualcomm_qti_radioconfiginterface_ConfigMsg*)(params_ptr->data);
    if(config_msg_ptr == NULL)
    {
      //To return NULL RESPONSE?? or to handle such case in Java layer
      //could be that anotgher client(not our java service) might send it this way.
      //HOW TO HANDLE THIS?
      QCRIL_LOG_ERROR("No config items in the message!");
      break; //or send empty response with error?
    }

    QCRIL_LOG_INFO("GET_RADIO_CONFIG_REQ with %d config items.", config_msg_ptr->settings_count);

    //HARDCODED, NEEDS TO CHANGE WHILE MERGING.
    //THIS IS FOR TEST PURPOSE, BEFORE THE FINAL CHANGE IS READY.
    size_t i;
    com_qualcomm_qti_radioconfiginterface_ConfigItemMsg temp;
    qcril_qmi_radio_config_item_type item;

    //response ptr
    resp_ptr = qcril_malloc(sizeof(com_qualcomm_qti_radioconfiginterface_ConfigMsg));
    if(resp_ptr == NULL)
    {
      QCRIL_LOG_ERROR("malloc failed");
      break;
    }
    resp_ptr->settings_count = config_msg_ptr->settings_count;
    resp_ptr->settings = qcril_malloc(config_msg_ptr->settings_count *
        sizeof(com_qualcomm_qti_radioconfiginterface_ConfigItemMsg));
    if(resp_ptr->settings == NULL)
    {
      resp_ptr->settings_count = 0;
      QCRIL_LOG_ERROR("malloc failed");
      break;
    }
    msg_len = sizeof(resp_ptr);
    uint8_t is_completed = FALSE;

    if (qcril_qmi_lte_direct_disc_dispatcher_req_handler(config_msg_ptr, resp_ptr))
    {
      QCRIL_LOG_INFO("Handled in qcril_qmi_lte_direct_disc_dispatcher; completed!!!");
      is_completed = TRUE;
    }

    for(i = 0; !is_completed && i < config_msg_ptr->settings_count; i++)
    {
      temp = config_msg_ptr->settings[i];
      item = qcril_qmi_radio_config_map_socket_item_to_config_item(temp.item);

      resp_ptr->settings[i].item = temp.item;
      resp_ptr->settings[i].has_errorCause = TRUE;
      resp_ptr->settings[i].errorCause =
        com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_NO_ERR;

      QCRIL_LOG_INFO("item id: %d to be set to: %d", item, temp.intValue);

      switch(item)
      {
        default:
          QCRIL_LOG_ERROR("Unknown item!!");
          resp_ptr->settings[i].errorCause =
            com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_ITEM_NOT_PRESENT;
          break;
      }
    }
  } while (FALSE);

  qcril_qmi_radio_config_socket_send(params_ptr->t,
      com_qualcomm_qti_radioconfiginterface_MessageType_RADIO_CONFIG_MSG_RESPONSE,
      com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_GET_CONFIG,
      0, 0, (void*)resp_ptr, msg_len);

  if (resp_ptr)
  {
    qcril_free(resp_ptr->settings);
    qcril_free(resp_ptr);
  }

  QCRIL_LOG_FUNC_RETURN();
}
