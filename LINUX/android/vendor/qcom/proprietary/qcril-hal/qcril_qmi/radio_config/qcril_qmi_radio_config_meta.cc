/******************************************************************************
#  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_radio_config_meta.c
  @brief   qcril qmi - radio config meta info

  DESCRIPTION
    Handles radio config meta info related functions.

******************************************************************************/

#include "qcril_log.h"
#include "qcril_qmi_radio_config_meta.h"
#include "qcril_qmi_radio_config_imss.h"
#include "qcril_qmi_radio_config_data.h"
#include "qcril_qmi_radio_config_meta_table.h"
#include "qcril_qmi_radio_config_meta_table_v02.h"

static const std::vector<qcril_qmi_radio_config_meta_type> *qcril_qmi_radio_config_meta_data;

size_t meta_table_length;

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

//===========================================================================
// qcril_qmi_radio_config_find_get_config_req_handler
//===========================================================================
qcril_qmi_radio_config_handler_type* qcril_qmi_radio_config_find_get_config_req_handler
(
   qcril_qmi_radio_config_item_type config_item
)
{
  size_t i;
  for (i=0; i< meta_table_length; i++)
  {
    if ( (*qcril_qmi_radio_config_meta_data)[i].config_item == config_item )
    {
      return (*qcril_qmi_radio_config_meta_data)[i].get_config_req_handler;
    }
  }
  return NULL;
} // qcril_qmi_radio_config_find_get_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_validate_and_find_get_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_handler_type* qcril_qmi_radio_config_validate_and_find_get_config_resp_handler
(
   qcril_qmi_radio_config_item_type config_item,
   unsigned long qmi_msg_id
)
{
  size_t i;
  for (i=0; i< meta_table_length; i++)
  {
    if ( (*qcril_qmi_radio_config_meta_data)[i].config_item == config_item)
    {
      if ((*qcril_qmi_radio_config_meta_data)[i].qmi_get_resp_msg_id > 0 &&
          (*qcril_qmi_radio_config_meta_data)[i].qmi_get_resp_msg_id == (signed long)qmi_msg_id)
      {
        return (*qcril_qmi_radio_config_meta_data)[i].get_config_resp_handler;
      }
      else
      {
        QCRIL_LOG_ERROR("get qmi msgid of the config item does not match the value in the table!");
        break;
      }
    }
  }
  return NULL;
} // qcril_qmi_radio_config_validate_and_find_get_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_find_set_config_req_handler
//===========================================================================
qcril_qmi_radio_config_handler_type* qcril_qmi_radio_config_find_set_config_req_handler
(
   qcril_qmi_radio_config_item_type config_item
)
{
  size_t i;
  for (i=0; i< meta_table_length; i++)
  {
    if ( (*qcril_qmi_radio_config_meta_data)[i].config_item == config_item )
    {
      return (*qcril_qmi_radio_config_meta_data)[i].set_config_req_handler;
    }
  }
  return NULL;
} // qcril_qmi_radio_config_find_set_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_validate_and_find_set_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_handler_type* qcril_qmi_radio_config_validate_and_find_set_config_resp_handler
(
   qcril_qmi_radio_config_item_type config_item,
   unsigned long qmi_msg_id
)
{
  size_t i;
  for (i=0; i< meta_table_length; i++)
  {
    if ( (*qcril_qmi_radio_config_meta_data)[i].config_item == config_item )
    {
      if ((*qcril_qmi_radio_config_meta_data)[i].qmi_set_resp_msg_id > 0 &&
          (*qcril_qmi_radio_config_meta_data)[i].qmi_set_resp_msg_id == (signed long)qmi_msg_id)
      {
        return (*qcril_qmi_radio_config_meta_data)[i].set_config_resp_handler;
      }
      else
      {
        QCRIL_LOG_ERROR("get qmi msgid of the config item does not match the value in the table!");
        break;
      }
    }
  }
  return NULL;
} // qcril_qmi_radio_config_validate_and_find_set_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_get_item_log_str
//===========================================================================
const char* qcril_qmi_radio_config_get_item_log_str(qcril_qmi_radio_config_item_type config_item)
{
  size_t i;
  for (i=0; i< meta_table_length; i++)
  {
    if ( (*qcril_qmi_radio_config_meta_data)[i].config_item == config_item )
    {
      if ((*qcril_qmi_radio_config_meta_data)[i].log_str)
      {
        return (*qcril_qmi_radio_config_meta_data)[i].log_str;
      }
      else
      {
        break;
      }
    }
  }
  return "Unknown msg";
} // qcril_qmi_radio_config_get_item_log_str

//===========================================================================
// qcril_qmi_radio_config_get_item_value_type
//===========================================================================
qcril_qmi_radio_config_item_value_type qcril_qmi_radio_config_get_item_value_type
(
   qcril_qmi_radio_config_item_type config_item
)
{
  size_t i;
  for (i=0; i< meta_table_length; i++)
  {
    if ( (*qcril_qmi_radio_config_meta_data)[i].config_item == config_item )
    {
      return (*qcril_qmi_radio_config_meta_data)[i].config_item_type;
    }
  }
  return QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INVALID;
} // qcril_qmi_radio_config_get_item_value_type

//===========================================================================
// qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp
//===========================================================================
qcril_qmi_radio_config_settings_resp_type qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp
(
   ims_settings_rsp_enum_v01 qmi_ims_setting_failcause
)
{
  qcril_qmi_radio_config_settings_resp_type ret = QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_OTHER_INTERNAL_ERR;
  switch(qmi_ims_setting_failcause)
  {
    case IMS_SETTINGS_MSG_NO_ERR_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR;
      break;

    case IMS_SETTINGS_MSG_IMS_NOT_READY_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_IMS_NOT_READY;
      break;

    case IMS_SETTINGS_MSG_FILE_NOT_AVAILABLE_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_FILE_NOT_AVAILABLE;
      break;

    case IMS_SETTINGS_MSG_READ_FAILED_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_READ_FAILED;
      break;

    case IMS_SETTINGS_MSG_WRITE_FAILED_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_WRITE_FAILED;
      break;

    case IMS_SETTINGS_MSG_OTHER_INTERNAL_ERR_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_OTHER_INTERNAL_ERR;
      break;

    default:
      ret = QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_OTHER_INTERNAL_ERR;
      break;
  }
  QCRIL_LOG_INFO("ims settings resp error: %d mapped to radio config error %d",
                 qmi_ims_setting_failcause, ret);
  return ret;
}// qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp

//===========================================================================
// qcril_qmi_radio_config_map_qmi_error_to_radio_config_error
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_map_qmi_error_to_radio_config_error
(
   qmi_error_type_v01 qmi_error
)
{
  qcril_qmi_radio_config_error_type ret = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  switch(qmi_error)
  {
    case QMI_ERR_NONE_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      break;

    case QMI_ERR_NO_MEMORY_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_ERROR_NO_MEMORY;
      break;

    case QMI_ERR_CAUSE_CODE_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_ERROR_ERROR_CAUSE_CODE;
      break;

    case QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_ERROR_REQUEST_NOT_SUPPORTED;
      break;

    case QMI_ERR_MALFORMED_MSG_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_ERROR_MALFORMED_MSG;
      break;

    case QMI_ERR_INFO_UNAVAILABLE_V01:
      ret = QCRIL_QMI_RADIO_CONFIG_ERROR_INFO_UNAVAILABLE;
      break;

    default:
      ret = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
      break;
  }
  QCRIL_LOG_INFO("qmi error: %d mapped to radio config error %d",
                 qmi_error, ret);
  return ret;
}// qcril_qmi_radio_config_map_qmi_error_to_radio_config_error

//===========================================================================
// qcril_qmi_radio_config_map_internal_error_to_radio_config_error
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_map_internal_error_to_radio_config_error
(
  qmi_client_error_type error
)
{
  qcril_qmi_radio_config_error_type ret = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  switch(error)
  {
    case QMI_NO_ERR:
      ret = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      break;

    default:
      ret = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
      break;
  }
  QCRIL_LOG_INFO("Internal error: %d mapped to radio config error %d",
                 error, ret);
  return ret;
}// qcril_qmi_radio_config_map_internal_error_to_radio_config_error

void qcril_qmi_radio_config_update_meta_table
(
  int modem_version
)
{
  if(modem_version == 1)
  {
    qcril_qmi_radio_config_meta_data = get_meta_table_v02();
    meta_table_length = get_length_meta_table_v02();
  }
  else
  {
    qcril_qmi_radio_config_meta_data = get_meta_table_v01();
    meta_table_length = get_length_meta_table_v01();
  }
}
