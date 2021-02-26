/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_lte_direct_disc_dispatcher.c
  @brief   qcril qmi - Dispatcher for provisioning requests

  DESCRIPTION
    Dispatcher for LTE-D provisioning requests

******************************************************************************/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "qcril_qmi_lte_direct_disc_dispatcher.h"
#include "qcril_qmi_lte_direct_disc.h"
#include "qcril_qmi_radio_config_misc.h"
#include "qcril_qmi_radio_config_meta.h"
#include "qcril_qmi_radio_config_packing.h"


/*===========================================================================
                    INTERNAL DEFINITIONS AND TYPES
===========================================================================*/

/*===========================================================================
                                FUNCTIONS
===========================================================================*/

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_dispatcher_req_handler

===========================================================================*/
/*!
  @brief
  Lte direct discovery dispatch handler for provisioning requests

  @return
  RIL_Errno
 */
/*=========================================================================*/
uint8_t qcril_qmi_lte_direct_disc_dispatcher_req_handler
(
 com_qualcomm_qti_radioconfiginterface_ConfigMsg *config_msg_ptr,
 com_qualcomm_qti_radioconfiginterface_ConfigMsg *resp_ptr /* Out parameter */
)
{
  com_qualcomm_qti_radioconfiginterface_ConfigFailureCause error_cause;
  com_qualcomm_qti_radioconfiginterface_ConfigItemMsg temp;
  com_qualcomm_qti_radioconfiginterface_LtedConfig *config = NULL;
  pb_bytes_array_t *byte_array_value = NULL;
  int item;
  uint8_t ret = FALSE;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_FUNC_ENTRY();

  if (config_msg_ptr->settings_count == 1)
  {
    temp = config_msg_ptr->settings[0];
    item = qcril_qmi_radio_config_map_socket_item_to_config_item(temp.item);

    error_cause = com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_GENERIC_ERR;

    switch(item)
    {
      /* SET requests */
      case QCRIL_QMI_RADIO_CONFIG_SET_LTED_CONFIG:
        QCRIL_LOG_ERROR("QCRIL_QMI_RADIO_CONFIG_SET_LTED_CONFIG");
        if(temp.byteArrayValue != NULL)
        {
          QCRIL_LOG_ERROR("temp.byteArrayValue->size = %d",
                  temp.byteArrayValue->size);
          if(temp.byteArrayValue->size > 0)
          {
            config = qcril_qmi_radio_config_unpack_lted_config(
                    temp.byteArrayValue->bytes,
                    temp.byteArrayValue->size);

            QCRIL_LOG_ERROR("config = 0x%x", config);
            if (config != NULL)
            {
              QCRIL_LOG_ERROR("config->apn = %s", config->apn);
              QCRIL_LOG_ERROR("config->announcingPolicy_count = 0x%x",
                      config->announcingPolicy_count);
              QCRIL_LOG_ERROR("config->monitoringPolicy_count = 0x%x",
                      config->monitoringPolicy_count);
            }
            ril_err = qcril_qmi_lte_direct_disc_set_lted_config_sync(config);
            if (ril_err == RIL_E_SUCCESS)
            {
              error_cause = com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_NO_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("Invalid data: temp.byteArrayValue->size is 0");
          }
        }
        else
        {
          QCRIL_LOG_ERROR("Invalid data: temp.byteArrayValue is NULL");
        }
        ret = TRUE;
        break;

      case QCRIL_QMI_RADIO_CONFIG_SET_LTED_CATEGORY:
        QCRIL_LOG_ERROR("QCRIL_QMI_RADIO_CONFIG_SET_LTED_CATEGORY");
        if(temp.byteArrayValue != NULL)
        {
          QCRIL_LOG_ERROR("temp.byteArrayValue->size = %d",
                  temp.byteArrayValue->size);
          if(temp.byteArrayValue->size > 0)
          {
            com_qualcomm_qti_radioconfiginterface_LtedCategory *category = NULL;

            category = qcril_qmi_radio_config_unpack_lted_category(
                    temp.byteArrayValue->bytes,
                    temp.byteArrayValue->size);

            QCRIL_LOG_ERROR("category = 0x%x", category);
            if (category != NULL)
            {
              QCRIL_LOG_ERROR("category->osAppId = %s", category->osAppId);
              QCRIL_LOG_ERROR("category->has_category = %d, category->category = %d",
                      category->has_category, category->category);
            }
            ril_err = qcril_qmi_lte_direct_disc_set_lted_category_sync(category);
            if (ril_err == RIL_E_SUCCESS)
            {
              error_cause = com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_NO_ERR;
            }
          }
          else
          {
            QCRIL_LOG_ERROR("Invalid data: temp.byteArrayValue->size is 0");
          }
        }
        else
        {
          QCRIL_LOG_ERROR("Invalid data: temp.byteArrayValue is NULL");
        }
        ret = TRUE;
        break;

      /* GET Requests */
      case QCRIL_QMI_RADIO_CONFIG_GET_LTED_CONFIG:
        QCRIL_LOG_ERROR("QCRIL_QMI_RADIO_CONFIG_GET_LTED_CONFIG");

        config = qcril_malloc(sizeof(com_qualcomm_qti_radioconfiginterface_LtedConfig));

        if (config != NULL)
        {
          ril_err = qcril_qmi_lte_direct_disc_get_lted_config_sync(config);

          if (ril_err == RIL_E_SUCCESS)
          {
            byte_array_value = qcril_qmi_radio_config_pack_lted_config(config);
            if (byte_array_value != NULL)
            {
              error_cause = com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_NO_ERR;
            }
          }
        }
        else
        {
          QCRIL_LOG_ERROR("malloc failed");
          break;
        }
        ret = TRUE;
        break;
      case QCRIL_QMI_RADIO_CONFIG_GET_LTED_CATEGORY:
        QCRIL_LOG_ERROR("QCRIL_QMI_RADIO_CONFIG_GET_LTED_CATEGORY");
        if(temp.byteArrayValue != NULL)
        {
          QCRIL_LOG_ERROR("temp.byteArrayValue->size = %d",
                  temp.byteArrayValue->size);
          if(temp.byteArrayValue->size > 0)
          {
            com_qualcomm_qti_radioconfiginterface_LtedCategory *req_category  = NULL;
            com_qualcomm_qti_radioconfiginterface_LtedCategory *resp_category = NULL;
            uint32_t category;

            req_category = qcril_qmi_radio_config_unpack_lted_category(
                    temp.byteArrayValue->bytes,
                    temp.byteArrayValue->size);

            QCRIL_LOG_ERROR("req_category = 0x%x", req_category);
            if (req_category != NULL)
            {
              QCRIL_LOG_ERROR("req_category->osAppId = %s", req_category->osAppId);
              ril_err = qcril_qmi_lte_direct_disc_get_lted_category_sync(
                        req_category->osAppId,
                        &category);
            }
            QCRIL_LOG_ERROR("ril_err = %d", ril_err);

            if (ril_err == RIL_E_SUCCESS)
            {
              resp_category =
                  qcril_malloc(sizeof(com_qualcomm_qti_radioconfiginterface_LtedCategory));
              if (resp_category != NULL)
              {
                memset(resp_category, 0x0, sizeof(*resp_category));
                resp_category->osAppId = qmi_ril_util_str_clone(req_category->osAppId);
                resp_category->has_category = TRUE;
                resp_category->category = category;

                QCRIL_LOG_ERROR("pack the output LtedCategory");
                byte_array_value = qcril_qmi_radio_config_pack_lted_category(resp_category);
                if (byte_array_value != NULL)
                {
                  error_cause =
                      com_qualcomm_qti_radioconfiginterface_ConfigFailureCause_CONFIG_NO_ERR;
                }
              }
            }
          }
          else
          {
            QCRIL_LOG_ERROR("Invalid data: temp.byteArrayValue->size is 0");
          }
        }
        else
        {
          QCRIL_LOG_ERROR("Invalid data: temp.byteArrayValue is NULL");
        }
        ret = TRUE;
        break;

      default:
        break;
    }

    if (ret == TRUE)
    {
      memset(&(resp_ptr->settings[0]), 0,
              sizeof(com_qualcomm_qti_radioconfiginterface_ConfigItemMsg));
      resp_ptr->settings[0].item = temp.item;
      resp_ptr->settings[0].has_errorCause = TRUE;
      resp_ptr->settings[0].errorCause = error_cause;
      if (byte_array_value != NULL)
      {
        resp_ptr->settings[0].byteArrayValue = byte_array_value;
      }
    }
  }

  QCRIL_LOG_FUNC_RETURN();
  return ret;
}
