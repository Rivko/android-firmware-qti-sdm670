/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_radio_config_dispatch_helper.c
  @brief   qcril qmi - radio config set/get function handlers

  DESCRIPTION
    Handles radio config set/get functions.
    Dispatches the config items by grouping them wrt
    their QMI messages, and calling the respective
    function handlers.

******************************************************************************/

#include "qcril_qmi_radio_config_dispatch_helper.h"
#include "qcril_qmi_radio_config_socket.h"
#include "modules/data/SetQualityMeasurementMessage.h"
#include <framework/GenericCallback.h>
#include <modules/android/RilRequestMessage.h>

#include "framework/Log.h"
extern "C" {
#include "data_system_determination_v01.h"
#include "qcril_qmi_radio_config_misc.h"
#include "qcril_qmi_radio_config_data.h"
#include "qcrili.h"
#include "qcril_qmi_radio_config_packing.h"
#include "radio_config_interface.pb.h"
#include "qcril_qmi_lte_direct_disc_dispatcher.h"
#include "common_v01.h"
}

#define TAG "QCRIL_RADIO_CONFIG"

class RadioConfigDataSetQualityMeasurementCallback : public GenericCallback<qmi_response_type_v01> {
private:
    legacy_request_payload params;
    com_qualcomm_qti_radioconfiginterface_ConfigMsg radio_config_resp;

public:
    inline RadioConfigDataSetQualityMeasurementCallback(string clientToken,
        const qcril_request_params_type &p, com_qualcomm_qti_radioconfiginterface_ConfigMsg &resp_data) :
                GenericCallback(clientToken), params(p)
    {
        radio_config_resp.settings_count = resp_data.settings_count;
        size_t settings_size = sizeof(com_qualcomm_qti_radioconfiginterface_ConfigItemMsg) * radio_config_resp.settings_count;
        radio_config_resp.settings = (com_qualcomm_qti_radioconfiginterface_ConfigItemMsg*)malloc(settings_size);
        if (radio_config_resp.settings != NULL && resp_data.settings != NULL)
        {
            memcpy(radio_config_resp.settings, resp_data.settings, settings_size);
        }
        else
        {
            radio_config_resp.settings = NULL;
        }
    }

    inline ~RadioConfigDataSetQualityMeasurementCallback()
    {
        if (radio_config_resp.settings != NULL)
        {
            free(radio_config_resp.settings);
        }
    }

    Message::Callback *clone() override;

    void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
        std::shared_ptr<qmi_response_type_v01> responseDataPtr) override;

    qcril_request_params_type &get_params() {
        return params.get_params();
    }
};

void RadioConfigDataSetQualityMeasurementCallback::onResponse(
    std::shared_ptr<Message> solicitedMsg, Status status,
    std::shared_ptr<qmi_response_type_v01> responseDataPtr) {

    boolean fill_send_err_resp = FALSE;
    size_t i = 0;
    if (status == Message::Callback::Status::SUCCESS && radio_config_resp.settings != NULL && responseDataPtr != NULL) {
        Log::getInstance().d("[RadioConfigDataSetQualityMeasurementCallback]: Callback[msg = " +
            solicitedMsg->dump() + "] executed. client data = " +
            mClientToken + " status = Message::Callback::Status::SUCCESS");
        if (responseDataPtr->result != QMI_RESULT_SUCCESS_V01)
        {
          fill_send_err_resp = TRUE;
        }
        else
        {
          qcril_qmi_radio_config_socket_send((RIL_Token)params.get_params().t,
              com_qualcomm_qti_radioconfiginterface_MessageType_RADIO_CONFIG_MSG_RESPONSE,
              com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_SET_CONFIG,
              0, com_qualcomm_qti_radioconfiginterface_Error_RADIO_CONFIG_ERR_SUCCESS,
              (void*)&radio_config_resp, sizeof(radio_config_resp));
        }
    }
    else if(radio_config_resp.settings != NULL){
      fill_send_err_resp = TRUE;
    }
    else {
        Log::getInstance().d("[RadioConfigDataSetQualityMeasurementCallback]: Callback[msg = " +
            solicitedMsg->dump() + "] executed. client data = " +
            mClientToken + " FAILURE !!");
        qcril_qmi_radio_config_socket_send((RIL_Token)params.get_params().t,
            com_qualcomm_qti_radioconfiginterface_MessageType_RADIO_CONFIG_MSG_RESPONSE,
            com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_SET_CONFIG,
            0, com_qualcomm_qti_radioconfiginterface_Error_RADIO_CONFIG_ERR_GENERIC_FAILURE,
            NULL,0);
   }
   if(fill_send_err_resp) {
     //if not success fill in the data structure with error
     for (i = 0; i < radio_config_resp.settings_count; i++)
     {
       if (radio_config_resp.settings[i].errorCause == com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_NO_ERR)
       {
         radio_config_resp.settings[i].errorCause = com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
       }
     }
     qcril_qmi_radio_config_socket_send((RIL_Token)params.get_params().t,
         com_qualcomm_qti_radioconfiginterface_MessageType_RADIO_CONFIG_MSG_RESPONSE,
         com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_SET_CONFIG,
         0, com_qualcomm_qti_radioconfiginterface_Error_RADIO_CONFIG_ERR_SUCCESS,
         (void*)&radio_config_resp, sizeof(radio_config_resp));

   }
}

Message::Callback *RadioConfigDataSetQualityMeasurementCallback::clone() {
    RadioConfigDataSetQualityMeasurementCallback *clone =
                     new RadioConfigDataSetQualityMeasurementCallback(mClientToken, params.get_params(), radio_config_resp);
    return clone;
}

/**
 * Analyzes and processes the set_config request data.
 */
void qcril_qmi_radio_config_dispatch_set_request
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  dsd_set_quality_measurement_info_req_msg_v01 data_set_quality_req;
  memset(&data_set_quality_req, 0, sizeof(data_set_quality_req));

  com_qualcomm_qti_radioconfiginterface_ConfigMsg resp_data;
  memset(&resp_data, 0, sizeof(com_qualcomm_qti_radioconfiginterface_ConfigMsg));

  uint8_t data_filled = FALSE;
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

    //response ptr
    resp_data.settings_count = config_msg_ptr->settings_count;
    resp_data.settings = (com_qualcomm_qti_radioconfiginterface_ConfigItemMsg*)qcril_malloc(sizeof(com_qualcomm_qti_radioconfiginterface_ConfigItemMsg) * config_msg_ptr->settings_count);
    if(resp_data.settings == NULL)
    {
      resp_data.settings_count = 0;
      QCRIL_LOG_ERROR("malloc failed");
      break;
    }
    msg_len = sizeof(resp_data);
    uint8_t is_completed = FALSE;

    if (qcril_qmi_lte_direct_disc_dispatcher_req_handler(config_msg_ptr, &resp_data))
    {
      QCRIL_LOG_INFO("Handled in qcril_qmi_lte_direct_disc_dispatcher; completed!!!");
      is_completed = TRUE;
    }

    for(i = 0; !is_completed && i < config_msg_ptr->settings_count; i++)
    {
      temp = config_msg_ptr->settings[i];
      item = qcril_qmi_radio_config_map_socket_item_to_config_item(temp.item);

      resp_data.settings[i].item = temp.item;
      resp_data.settings[i].has_errorCause = TRUE;
      resp_data.settings[i].errorCause =
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
              data_set_quality_req.in_call_lte_rsrp_low_valid = TRUE;
              data_set_quality_req.in_call_lte_rsrp_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_MID:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
              {
                data_filled = TRUE;
                data_set_quality_req.in_call_lte_rsrp_mid_valid = TRUE;
                data_set_quality_req.in_call_lte_rsrp_mid = (uint16_t)(temp.intValue);
              }
              else
              {
                QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
                resp_data.settings[i].errorCause =
                  com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
              }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.in_call_lte_rsrp_high_valid = TRUE;
              data_set_quality_req.in_call_lte_rsrp_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
              {
                data_filled = TRUE;
                data_set_quality_req.in_call_wifi_rssi_threshold_low_valid = TRUE;
                data_set_quality_req.in_call_wifi_rssi_threshold_low = (uint16_t)(temp.intValue);
              }
              else
              {
                QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
                resp_data.settings[i].errorCause =
                  com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
              }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.in_call_wifi_rssi_threshold_high_valid = TRUE;
              data_set_quality_req.in_call_wifi_rssi_threshold_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX ))
            {
              data_filled = TRUE;
              data_set_quality_req.in_call_wifi_sinr_threshold_low_valid = TRUE;
              data_set_quality_req.in_call_wifi_sinr_threshold_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.in_call_wifi_sinr_threshold_high_valid = TRUE;
              data_set_quality_req.in_call_wifi_sinr_threshold_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.idle_lte_rsrp_low_valid = TRUE;
              data_set_quality_req.idle_lte_rsrp_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_MID:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.idle_lte_rsrp_mid_valid = TRUE;
              data_set_quality_req.idle_lte_rsrp_mid = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.idle_lte_rsrp_high_valid = TRUE;
              data_set_quality_req.idle_lte_rsrp_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.idle_wifi_rssi_threshold_low_valid = TRUE;
              data_set_quality_req.idle_wifi_rssi_threshold_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.idle_wifi_rssi_threshold_high_valid = TRUE;
              data_set_quality_req.idle_wifi_rssi_threshold_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.idle_wifi_sinr_threshold_low_valid = TRUE;
              data_set_quality_req.idle_wifi_sinr_threshold_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.idle_wifi_sinr_threshold_high_valid = TRUE;
              data_set_quality_req.idle_wifi_sinr_threshold_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_LOW:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.ecio_1x_threshold_low_valid = TRUE;
              data_set_quality_req.ecio_1x_threshold_low = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
              com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
          }
          break;
        case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_HIGH:
          if(temp.has_intValue)
          {
            if( (SHRT_MIN <= (int)temp.intValue) && ((int)temp.intValue <= SHRT_MAX) )
            {
              data_filled = TRUE;
              data_set_quality_req.ecio_1x_threshold_high_valid = TRUE;
              data_set_quality_req.ecio_1x_threshold_high = (uint16_t)(temp.intValue);
            }
            else
            {
              QCRIL_LOG_ERROR("item value is greater than the expected maximum!");
              resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("item doesnt have int value");
            resp_data.settings[i].errorCause =
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
       std::shared_ptr<rildata::SetQualityMeasurementMessage> msg =
            std::make_shared<rildata::SetQualityMeasurementMessage>(data_set_quality_req);
       if(msg)
       {
           RadioConfigDataSetQualityMeasurementCallback cb("set-cb-token", *params_ptr, resp_data);
           msg->setCallback(&cb);
           msg->dispatch();
       }
#endif
    }
  }while(FALSE);

  if(!data_filled)
  {
    qcril_qmi_radio_config_socket_send( (RIL_Token)params_ptr->t,
          com_qualcomm_qti_radioconfiginterface_MessageType_RADIO_CONFIG_MSG_RESPONSE,
          com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_SET_CONFIG,
          0, com_qualcomm_qti_radioconfiginterface_Error_RADIO_CONFIG_ERR_SUCCESS,
          (void*)&resp_data, msg_len);
  }

  if (resp_data.settings != NULL)
  {
    qcril_free(resp_data.settings);
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
  com_qualcomm_qti_radioconfiginterface_ConfigMsg resp_data;
  memset(&resp_data, 0, sizeof(com_qualcomm_qti_radioconfiginterface_ConfigMsg));
  size_t msg_len = 0;

  QCRIL_NOTUSED(params_ptr);
  QCRIL_NOTUSED(ret_ptr);
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
    resp_data.settings_count = config_msg_ptr->settings_count;
    resp_data.settings = (com_qualcomm_qti_radioconfiginterface_ConfigItemMsg*)qcril_malloc(config_msg_ptr->settings_count *
        sizeof(com_qualcomm_qti_radioconfiginterface_ConfigItemMsg));
    if(resp_data.settings == NULL)
    {
      resp_data.settings_count = 0;
      QCRIL_LOG_ERROR("malloc failed");
      break;
    }
    msg_len = sizeof(resp_data);
    uint8_t is_completed = FALSE;

    if (qcril_qmi_lte_direct_disc_dispatcher_req_handler(config_msg_ptr, &resp_data))
    {
      QCRIL_LOG_INFO("Handled in qcril_qmi_lte_direct_disc_dispatcher; completed!!!");
      is_completed = TRUE;
    }

    for(i = 0; !is_completed && i < config_msg_ptr->settings_count; i++)
    {
      temp = config_msg_ptr->settings[i];
      item = qcril_qmi_radio_config_map_socket_item_to_config_item(temp.item);
      resp_data.settings[i].item = temp.item;
      resp_data.settings[i].has_errorCause = TRUE;
      resp_data.settings[i].errorCause = com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_NO_ERR;

      QCRIL_LOG_INFO("item id: %d to be set to: %d", item, temp.intValue);

      switch(item)
      {
        default:
          QCRIL_LOG_ERROR("Unknown item!!");
          resp_data.settings[i].errorCause =
                com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_ITEM_NOT_PRESENT;
          break;
      }
    }
  } while (FALSE);

  qcril_qmi_radio_config_socket_send( (RIL_Token)params_ptr->t,
      com_qualcomm_qti_radioconfiginterface_MessageType_RADIO_CONFIG_MSG_RESPONSE,
      com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_GET_CONFIG,
      0, com_qualcomm_qti_radioconfiginterface_Error_RADIO_CONFIG_ERR_SUCCESS,
      (void*)&resp_data, msg_len);

  if(resp_data.settings != NULL)
  {
    qcril_free(resp_data.settings);
  }
  QCRIL_LOG_FUNC_RETURN();
}

