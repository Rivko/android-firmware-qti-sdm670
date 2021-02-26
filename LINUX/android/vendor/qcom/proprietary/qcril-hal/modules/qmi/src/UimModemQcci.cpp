/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/


#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_idl_lib_internal.h"
#include "user_identity_module_v01.h"
#include "framework/Log.h"
#include "qmi_cci_target_ext.h"
#include "UimModemQcci.h"
#include "framework/legacy.h"
#include "UimModemEndPointModule.h"
#include <arpa/inet.h>

#include <string.h>

#define qcril_malloc(size) qcril_malloc_adv(size, __func__, __LINE__)

#define QCRIL_ASSERT( xx_exp ) QCRIL_LOG_ASSERT( xx_exp )

/* Synchronous message default timeout (in milli-seconds) */
#define QMI_UIM_DEFAULT_TIMEOUT 5000

/* structure to hold cb pointer and cb data */
#define QMI_UIM_MAKE_CB_STRUCT(cb_struct, user_cb_ptr, user_data_ptr)  \
  cb_struct->user_cb = user_cb_ptr;                                    \
  cb_struct->user_data = user_data_ptr;

/* Used to validate QMI_UIM client service pointer */
#define QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle)  \
        if (client_handle == NULL)                                    \
        {                                                                  \
          QCRIL_LOG_ERROR("%s", "client_handle is not intialized");   \
          return QMI_SERVICE_ERR;                                          \
        }

extern void *qcril_malloc_adv ( size_t size, const char* func_name, int line_num);


/*---------------------------------------------------------------------------
  structures
---------------------------------------------------------------------------*/
/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_CB_STRUCT_TYPE

   DESCRIPTION:   Structure used for holding information regarding callback
-------------------------------------------------------------------------------*/
typedef struct
{
  qmi_uim_user_async_cb_type      user_cb;
  void                          * user_data;
} qmi_uim_cb_struct_type;


/*---------------------------------------------------------------------------
  helper functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  qcril_uim_qmi_flip_data
===========================================================================*/
/*!
@brief
  Similar to a memcpy, this function copies 2 8-bit data per recursion
  from one location to another but swaps the order of the 2 bytes.
*/
/*=========================================================================*/
static int qcril_uim_qmi_flip_data
(
  uint8_t                      * des,
  const uint8_t                * src,
  uint32_t                       data_len
)
{
  unsigned int     i             =   0;
  unsigned short   temp_path     =   0;

  /* Return if path length is ODD */
  if(des == NULL || src == NULL || (data_len & 0x1) != 0)
  {
    QCRIL_LOG_ERROR("Invalid Input, cannot proceed: src = 0x%x, des = 0x%x, path_length = 0x%x",
                    src, des, data_len);
    QCRIL_ASSERT(0);
    return -1;
  }

  for(i = 0; (i + 1) < data_len; i += 2)
  {
    temp_path = (*(src + i) << 8) |
                (*(src + i + 1));

    memcpy(des, (unsigned char*)&temp_path, sizeof(unsigned short));
    des += sizeof(unsigned short);
  }

  return 0;
} /* qcril_uim_qmi_flip_data */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_set_file_attribute_pin
===========================================================================*/
/*!
@brief
  This function checks each bit of the section mask setting the
  pins accordingly.
*/
/*=========================================================================*/
static void qcril_uim_qmi_set_file_attribute_pin
(
  uim_sec_condition_enum_v01       sec_value,
  const uint16_t                   sec_mask,
  qmi_uim_security_type          * rsp_data
)
{
  if(rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  rsp_data->security_value = (qmi_uim_security_value_type)sec_value;
  if(sec_mask & 1)
  {
    rsp_data->pin1 = QMI_UIM_TRUE;
  }
  else
  {
    rsp_data->pin1 = QMI_UIM_FALSE;
  }
  if(sec_mask & 2)
  {
    rsp_data->pin2 = QMI_UIM_TRUE;
  }
  else
  {
    rsp_data->pin2 = QMI_UIM_FALSE;
  }
  if(sec_mask & 4)
  {
    rsp_data->upin = QMI_UIM_TRUE;
  }
  else
  {
    rsp_data->upin = QMI_UIM_FALSE;
  }
  if(sec_mask & 8)
  {
    rsp_data->adm = QMI_UIM_TRUE;
  }
  else
  {
    rsp_data->adm = QMI_UIM_FALSE;
  }
} /* qcril_uim_qmi_set_file_attribute_pin */


/*---------------------------------------------------------------------------
  response conversion functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_read_transparent_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_read_transparent_resp
(
  const uim_read_transparent_resp_msg_v01         * qmi_response,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_READ_TRANSPARENT_RSP_MSG;
  if(qmi_response->card_result_valid)
  {
    rsp_data->rsp_data.read_transparent_rsp.sw1 = (unsigned char)qmi_response->card_result.sw1;
    rsp_data->rsp_data.read_transparent_rsp.sw2 = (unsigned char)qmi_response->card_result.sw2;
  }
  else
  {
    rsp_data->rsp_data.read_transparent_rsp.sw1 = 0;
    rsp_data->rsp_data.read_transparent_rsp.sw2 = 0;
  }
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->read_result_valid)
    {
      if((qmi_response->read_result.content_len) > QMI_UIM_CONTENT_TRANSPARENT_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* in sync calls this is freed by client */
      rsp_data->rsp_data.read_transparent_rsp.content.data_ptr = (unsigned char *)qcril_malloc(qmi_response->read_result.content_len);
      if(rsp_data->rsp_data.read_transparent_rsp.content.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.read_transparent_rsp.content.data_ptr,
             qmi_response->read_result.content,
             qmi_response->read_result.content_len);
      rsp_data->rsp_data.read_transparent_rsp.content.data_len = (unsigned short)qmi_response->read_result.content_len;

      /* Also update the encrypted flag */
      if(qmi_response->encryption_valid)
      {
        rsp_data->rsp_data.read_transparent_rsp.is_content_encrypted =
          (qmi_response->encryption == 1) ? QMI_UIM_TRUE : QMI_UIM_FALSE;
      }
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_read_transparent_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_read_record_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_read_record_resp
(
  const uim_read_record_resp_msg_v01         * qmi_response,
  qmi_uim_rsp_data_type                      * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_READ_RECORD_RSP_MSG;
  if(qmi_response->card_result_valid)
  {
    rsp_data->rsp_data.read_record_rsp.sw1 = (unsigned char)qmi_response->card_result.sw1;
    rsp_data->rsp_data.read_record_rsp.sw2 = (unsigned char)qmi_response->card_result.sw2;
  }
  else
  {
    rsp_data->rsp_data.read_record_rsp.sw1 = 0;
    rsp_data->rsp_data.read_record_rsp.sw2 = 0;
  }
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->read_result_valid)
    {
      if((qmi_response->read_result.content_len) > QMI_UIM_CONTENT_RECORD_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      rsp_data->rsp_data.read_record_rsp.content.data_ptr = (unsigned char *)qcril_malloc(qmi_response->read_result.content_len);
      if(rsp_data->rsp_data.read_record_rsp.content.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.read_record_rsp.content.data_ptr,
             (unsigned char*)qmi_response->read_result.content,
             (unsigned short)qmi_response->read_result.content_len);
      rsp_data->rsp_data.read_record_rsp.content.data_len = (unsigned short)qmi_response->read_result.content_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_read_record_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_write_transparent_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_write_transparent_resp
(
  const uim_write_transparent_resp_msg_v01       * qmi_response,
  qmi_uim_rsp_data_type                          * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_WRITE_TRANSPARENT_RSP_MSG;
  if(qmi_response->card_result_valid)
  {
    rsp_data->rsp_data.write_transparent_rsp.sw1 = (unsigned char)qmi_response->card_result.sw1;
    rsp_data->rsp_data.write_transparent_rsp.sw2 = (unsigned char)qmi_response->card_result.sw2;
  }
  else
  {
    rsp_data->rsp_data.write_transparent_rsp.sw1 = 0;
    rsp_data->rsp_data.write_transparent_rsp.sw2 = 0;
  }
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_write_transparent_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_write_record_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_write_record_resp
(
  const uim_write_record_resp_msg_v01          * qmi_response,
  qmi_uim_rsp_data_type                        * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_WRITE_RECORD_RSP_MSG;
  if(qmi_response->card_result_valid)
  {
    rsp_data->rsp_data.write_record_rsp.sw1 = (unsigned char)qmi_response->card_result.sw1;
    rsp_data->rsp_data.write_record_rsp.sw2 = (unsigned char)qmi_response->card_result.sw2;
  }
  else
  {
    rsp_data->rsp_data.write_record_rsp.sw1 = 0;
    rsp_data->rsp_data.write_record_rsp.sw2 = 0;
  }
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_write_record_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_file_attributes_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_file_attributes_resp
(
  const uim_get_file_attributes_resp_msg_v01    * qmi_response,
  qmi_uim_rsp_data_type                         * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GET_FILE_ATTRIBUTES_RSP_MSG;
  if(qmi_response->card_result_valid)
  {
    rsp_data->rsp_data.get_file_attributes_rsp.sw1 = (unsigned char)qmi_response->card_result.sw1;
    rsp_data->rsp_data.get_file_attributes_rsp.sw2 = (unsigned char)qmi_response->card_result.sw2;
  }
  else
  {
    rsp_data->rsp_data.get_file_attributes_rsp.sw1 = 0;
    rsp_data->rsp_data.get_file_attributes_rsp.sw2 = 0;
  }
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->file_attributes_valid)
    {
      rsp_data->rsp_data.get_file_attributes_rsp.file_size = (unsigned short)qmi_response->file_attributes.file_size;
      rsp_data->rsp_data.get_file_attributes_rsp.file_id = (unsigned short)qmi_response->file_attributes.file_id;
      rsp_data->rsp_data.get_file_attributes_rsp.file_type = (qmi_uim_file_type)qmi_response->file_attributes.file_type;
      rsp_data->rsp_data.get_file_attributes_rsp.record_size = (unsigned short)qmi_response->file_attributes.rec_size;
      rsp_data->rsp_data.get_file_attributes_rsp.record_count = (unsigned short)qmi_response->file_attributes.rec_count;

      /*set the security values*/
      qcril_uim_qmi_set_file_attribute_pin(qmi_response->file_attributes.sec_read,
                                     qmi_response->file_attributes.sec_read_mask,
                                     &(rsp_data->rsp_data.get_file_attributes_rsp.read_security));
      qcril_uim_qmi_set_file_attribute_pin(qmi_response->file_attributes.sec_write,
                                     qmi_response->file_attributes.sec_write_mask,
                                     &(rsp_data->rsp_data.get_file_attributes_rsp.write_security));
      qcril_uim_qmi_set_file_attribute_pin(qmi_response->file_attributes.sec_increase,
                                     qmi_response->file_attributes.sec_increase_mask,
                                     &(rsp_data->rsp_data.get_file_attributes_rsp.increase_security));
      qcril_uim_qmi_set_file_attribute_pin(qmi_response->file_attributes.sec_deactivate,
                                     qmi_response->file_attributes.sec_deactivate_mask,
                                     &(rsp_data->rsp_data.get_file_attributes_rsp.deactivate_security));
      qcril_uim_qmi_set_file_attribute_pin(qmi_response->file_attributes.sec_activate,
                                     qmi_response->file_attributes.sec_activate_mask,
                                     &(rsp_data->rsp_data.get_file_attributes_rsp.activate_security));

      if((qmi_response->file_attributes.raw_value_len) > QMI_UIM_FILE_ATTR_RAW_VALUE_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* in sync calls this is freed by client */
      rsp_data->rsp_data.get_file_attributes_rsp.raw_value.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->file_attributes.raw_value_len);
      if(rsp_data->rsp_data.get_file_attributes_rsp.raw_value.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.get_file_attributes_rsp.raw_value.data_ptr,
             qmi_response->file_attributes.raw_value,
             qmi_response->file_attributes.raw_value_len);
      rsp_data->rsp_data.get_file_attributes_rsp.raw_value.data_len =
        (unsigned short)qmi_response->file_attributes.raw_value_len;
    }
  }
  else
  {
    QCRIL_LOG_INFO( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_file_attributes_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_refresh_register_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_refresh_register_resp
(
  const uim_refresh_register_resp_msg_v01    * qmi_response,
  qmi_uim_rsp_data_type                      * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_REFRESH_REGISTER_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_refresh_register_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_set_pin_protection_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_set_pin_protection_resp
(
  const uim_set_pin_protection_resp_msg_v01      * qmi_response,
  qmi_uim_rsp_data_type                          * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_SET_PIN_PROTECTION_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->encrypted_pin1_valid)
    {
      if((qmi_response->encrypted_pin1.enc_pin1_value_len) > QMI_UIM_ENCRYPTED_PIN_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* in sync calls this is freed by client */
      rsp_data->rsp_data.set_pin_protection_rsp.encr_pin_data.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->encrypted_pin1.enc_pin1_value_len);
      if(rsp_data->rsp_data.set_pin_protection_rsp.encr_pin_data.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.set_pin_protection_rsp.encr_pin_data.data_ptr,
             (unsigned char*)qmi_response->encrypted_pin1.enc_pin1_value,
             (unsigned short)qmi_response->encrypted_pin1.enc_pin1_value_len);
      rsp_data->rsp_data.set_pin_protection_rsp.encr_pin_data.data_len =
        (unsigned short)qmi_response->encrypted_pin1.enc_pin1_value_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
    if(qmi_response->retries_left_valid)
    {
      rsp_data->rsp_data.set_pin_protection_rsp.num_retries = (unsigned char)qmi_response->retries_left.verify_left;
      rsp_data->rsp_data.set_pin_protection_rsp.num_unblock_retries = (unsigned char)qmi_response->retries_left.unblock_left;
    }
  }
} /* qcril_uim_qmi_conv_set_pin_protection_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_verify_pin_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_verify_pin_resp
(
  const uim_verify_pin_resp_msg_v01           * qmi_response,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_VERIFY_PIN_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->encrypted_pin1_valid)
    {
      if((qmi_response->encrypted_pin1.enc_pin1_value_len) > QMI_UIM_ENCRYPTED_PIN_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* in sync calls this is freed by client */
      rsp_data->rsp_data.verify_pin_rsp.encr_pin_data.data_ptr = (unsigned char *)qcril_malloc(qmi_response->encrypted_pin1.enc_pin1_value_len);
      if(rsp_data->rsp_data.verify_pin_rsp.encr_pin_data.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.verify_pin_rsp.encr_pin_data.data_ptr,
             (unsigned char*)qmi_response->encrypted_pin1.enc_pin1_value,
             (unsigned short)qmi_response->encrypted_pin1.enc_pin1_value_len);
      rsp_data->rsp_data.verify_pin_rsp.encr_pin_data.data_len =
        (unsigned short)qmi_response->encrypted_pin1.enc_pin1_value_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
    /* rentries are sent only on failure */
    if(qmi_response->retries_left_valid)
    {
      rsp_data->rsp_data.verify_pin_rsp.num_retries = (unsigned char)qmi_response->retries_left.verify_left;
      rsp_data->rsp_data.verify_pin_rsp.num_unblock_retries = (unsigned char)qmi_response->retries_left.unblock_left;
    }
  }
} /* qcril_uim_qmi_conv_verify_pin_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_unblock_pin_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_unblock_pin_resp
(
  const uim_unblock_pin_resp_msg_v01         * qmi_response,
  qmi_uim_rsp_data_type                      * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_UNBLOCK_PIN_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->encrypted_pin1_valid)
    {
      if((qmi_response->encrypted_pin1.enc_pin1_value_len) > QMI_UIM_ENCRYPTED_PIN_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* in sync calls this is freed by client */
      rsp_data->rsp_data.unblock_pin_rsp.encr_pin_data.data_ptr = (unsigned char *)qcril_malloc(qmi_response->encrypted_pin1.enc_pin1_value_len);
      if(rsp_data->rsp_data.unblock_pin_rsp.encr_pin_data.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.unblock_pin_rsp.encr_pin_data.data_ptr,
             (unsigned char*)qmi_response->encrypted_pin1.enc_pin1_value,
             (unsigned short)qmi_response->encrypted_pin1.enc_pin1_value_len);
      rsp_data->rsp_data.unblock_pin_rsp.encr_pin_data.data_len =
        (unsigned short)qmi_response->encrypted_pin1.enc_pin1_value_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
    if(qmi_response->retries_left_valid)
    {
      rsp_data->rsp_data.unblock_pin_rsp.num_retries = (unsigned char)qmi_response->retries_left.verify_left;
      rsp_data->rsp_data.unblock_pin_rsp.num_unblock_retries = (unsigned char)qmi_response->retries_left.unblock_left;
    }
  }
} /* qcril_uim_qmi_conv_unblock_pin_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_change_pin_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_change_pin_resp
(
  const uim_change_pin_resp_msg_v01        * qmi_response,
  qmi_uim_rsp_data_type                    * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_CHANGE_PIN_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->encrypted_pin1_valid)
    {
      if((qmi_response->encrypted_pin1.enc_pin1_value_len) > QMI_UIM_ENCRYPTED_PIN_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* in sync calls this is freed by client */
      rsp_data->rsp_data.change_pin_rsp.encr_pin_data.data_ptr = (unsigned char *)qcril_malloc(qmi_response->encrypted_pin1.enc_pin1_value_len);
      if(rsp_data->rsp_data.change_pin_rsp.encr_pin_data.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.change_pin_rsp.encr_pin_data.data_ptr,
             (unsigned char*)(qmi_response->encrypted_pin1.enc_pin1_value),
             (unsigned short)(qmi_response->encrypted_pin1.enc_pin1_value_len));
      rsp_data->rsp_data.change_pin_rsp.encr_pin_data.data_len =
        (unsigned short)qmi_response->encrypted_pin1.enc_pin1_value_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
    if(qmi_response->retries_left_valid)
    {
      rsp_data->rsp_data.change_pin_rsp.num_retries = (unsigned char)qmi_response->retries_left.verify_left;
      rsp_data->rsp_data.change_pin_rsp.num_unblock_retries = (unsigned char)qmi_response->retries_left.unblock_left;
    }
  }
} /* qcril_uim_qmi_conv_change_pin_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_depersonalization_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_depersonalization_resp
(
  const uim_depersonalization_resp_msg_v01      * qmi_response,
  qmi_uim_rsp_data_type                         * rsp_data
)
{
  if((qmi_response == NULL) || (rsp_data == NULL))
  {
    QCRIL_LOG_ERROR("%s","Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_DEPERSONALIZATION_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
    if(qmi_response->retries_left_valid)
    {
      rsp_data->rsp_data.depersonalization_rsp.num_retries = (unsigned char)qmi_response->retries_left.verify_left;
      rsp_data->rsp_data.depersonalization_rsp.num_unblock_retries = (unsigned char)qmi_response->retries_left.unblock_left;
    }
  }
} /* qcril_uim_qmi_conv_depersonalization_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_power_down_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_power_down_resp
(
  const uim_power_down_resp_msg_v01           * qmi_response,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_POWER_DOWN_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_power_down_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_power_up_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_power_up_resp
(
  const uim_power_up_resp_msg_v01          * qmi_response,
  qmi_uim_rsp_data_type                    * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_POWER_UP_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_power_up_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_copy_app_info
===========================================================================*/
/*!
@brief
  copies application info from response/indication data to response data
  struct compatible to the card.
*/
/*=========================================================================*/
static int qcril_uim_qmi_copy_app_info
(
  const app_info_type_v01                       * app_info,
  uint32_t                                        app_info_len,
  qmi_uim_card_status_type                      * card_status_ptr,
  uint32_t                                        slot_index
)
{
  uint32_t i = 0;

  if(app_info == NULL || card_status_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    return -1;
  }

  if(slot_index >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("wrong input slot_index: 0x%x app_info_len: 0x%x",
                    slot_index, app_info_len);
    return -1;
  }

  for(i = 0; i < app_info_len && i < QMI_UIM_MAX_APP_PER_CARD_COUNT; i++)
  {
    if(app_info[i].aid_value_len > QMI_UIM_MAX_AID_LEN)
    {
      QCRIL_LOG_ERROR("%s", "data length too long");
      return -1;
    }
    ((card_status_ptr->card[slot_index]).application[i]).app_type =
      (qmi_uim_app_type)app_info[i].app_type;
    ((card_status_ptr->card[slot_index]).application[i]).app_state =
      (qmi_uim_app_state_type)app_info[i].app_state;
    ((card_status_ptr->card[slot_index]).application[i]).perso_state =
      (qmi_uim_perso_state_type)app_info[i].perso_state;
    ((card_status_ptr->card[slot_index]).application[i]).perso_feature =
      (qmi_uim_perso_feature_status_type)app_info[i].perso_feature;
    ((card_status_ptr->card[slot_index]).application[i]).perso_retries =
      (unsigned char)app_info[i].perso_retries;
    ((card_status_ptr->card[slot_index]).application[i]).perso_unblock_retries =
      (unsigned char)app_info[i].perso_unblock_retries;
    ((card_status_ptr->card[slot_index]).application[i]).aid_len =
      (unsigned char)app_info[i].aid_value_len;
    memcpy (((card_status_ptr->card[slot_index]).application[i]).aid_value,
             (char*)app_info[i].aid_value,
             app_info[i].aid_value_len);

    ((card_status_ptr->card[slot_index]).application[i]).univ_pin =
      (unsigned char)app_info[i].univ_pin;
    ((card_status_ptr->card[slot_index]).application[i]).pin1_state =
      (qmi_uim_pin_status_type)app_info[i].pin1.pin_state;
    ((card_status_ptr->card[slot_index]).application[i]).pin1_num_retries =
      (unsigned char)app_info[i].pin1.pin_retries;
    ((card_status_ptr->card[slot_index]).application[i]).puk1_num_retries =
      (unsigned char)app_info[i].pin1.puk_retries;

    ((card_status_ptr->card[slot_index]).application[i]).pin2_state =
      (qmi_uim_pin_status_type)app_info[i].pin2.pin_state;
    ((card_status_ptr->card[slot_index]).application[i]).pin2_num_retries =
      (unsigned char)app_info[i].pin2.pin_retries;
    ((card_status_ptr->card[slot_index]).application[i]).puk2_num_retries =
      (unsigned char)app_info[i].pin2.puk_retries;
  }

  return 0;
} /* qcril_uim_qmi_copy_app_info */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_copy_extended_card_status
===========================================================================*/
/*!
@brief
  copies extended card status response/indication data to response data
  struct compatible to the card.
*/
/*=========================================================================*/
static int qcril_uim_qmi_copy_extended_card_status
(
  const uim_extended_card_status_type_v01       * extended_card_status_ptr,
  qmi_uim_card_status_type                      * card_status_ptr,
  qmi_uim_bool_type                             * card_status_invalid_ptr
)
{
  uint32_t i = 0;

  if(card_status_ptr == NULL ||
     extended_card_status_ptr == NULL ||
     card_status_invalid_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    return -1;
  }

  card_status_ptr->card_status_type = QMI_UIM_EXTENDED_CARD_STATUS_TYPE;
  if(extended_card_status_ptr->index_gw_len > 0)
  {
    card_status_ptr->index_gw_pri_prov = (unsigned short)extended_card_status_ptr->index_gw[0];
  }
  else
  {
    card_status_ptr->index_gw_pri_prov = 0xFFFF;
  }
  if(extended_card_status_ptr->index_1x_len > 0)
  {
    card_status_ptr->index_1x_pri_prov = (unsigned short)extended_card_status_ptr->index_1x[0];
  }
  else
  {
    card_status_ptr->index_1x_pri_prov = 0xFFFF;
  }
  if(extended_card_status_ptr->index_gw_len > 1)
  {
    card_status_ptr->index_gw_sec_prov = (unsigned short)extended_card_status_ptr->index_gw[1];
  }
  else
  {
    card_status_ptr->index_gw_sec_prov = 0xFFFF;
  }
  if(extended_card_status_ptr->index_1x_len > 1)
  {
    card_status_ptr->index_1x_sec_prov = (unsigned short)extended_card_status_ptr->index_1x[1];
  }
  else
  {
    card_status_ptr->index_1x_sec_prov = 0xFFFF;
  }
  if(extended_card_status_ptr->index_gw_len > 2)
  {
    card_status_ptr->index_gw_ter_prov = (unsigned short)extended_card_status_ptr->index_gw[2];
  }
  else
  {
    card_status_ptr->index_gw_ter_prov = 0xFFFF;
  }
  if(extended_card_status_ptr->index_1x_len > 2)
  {
    card_status_ptr->index_1x_ter_prov = (unsigned short)extended_card_status_ptr->index_1x[2];
  }
  else
  {
    card_status_ptr->index_1x_ter_prov = 0xFFFF;
  }
  card_status_ptr->num_slots = (unsigned char)extended_card_status_ptr->card_info_len;

  for(i = 0; i < card_status_ptr->num_slots && i < QMI_UIM_MAX_CARD_COUNT; i++)
  {
    if((extended_card_status_ptr->card_info[i]).app_info_len > QMI_UIM_EXTENDED_APPS_MAX_V01)
    {
      QCRIL_LOG_ERROR("%s", "data length too long");
      return -1;
    }

    if(extended_card_status_ptr->card_info[i].card_state == UIM_EXTENDED_CARD_STATE_UNKNOWN_V01)
    {
      card_status_ptr->card[i].card_state = QMI_UIM_CARD_STATE_ABSENT;
      card_status_invalid_ptr[i] = QMI_UIM_TRUE;
    }
    else
    {
      (card_status_ptr->card[i]).card_state =
        (qmi_uim_card_state_type)(extended_card_status_ptr->card_info[i]).card_state;
      card_status_invalid_ptr[i] = QMI_UIM_FALSE;
    }
    (card_status_ptr->card[i]).upin_state =
      (qmi_uim_pin_status_type)(extended_card_status_ptr->card_info[i]).upin.pin_state;
    (card_status_ptr->card[i]).upin_num_retries =
      (unsigned char)(extended_card_status_ptr->card_info[i]).upin.pin_retries;
    (card_status_ptr->card[i]).upuk_num_retries =
      (unsigned char)(extended_card_status_ptr->card_info[i]).upin.puk_retries;
    (card_status_ptr->card[i]).card_error =
      (qmi_uim_card_error_type)(extended_card_status_ptr->card_info[i]).error_code;
    (card_status_ptr->card[i]).num_app =
      (unsigned char)(extended_card_status_ptr->card_info[i]).app_info_len;

    if(qcril_uim_qmi_copy_app_info((extended_card_status_ptr->card_info[i]).app_info,
                                   (extended_card_status_ptr->card_info[i]).app_info_len,
                                   card_status_ptr,
                                   i) < 0)
    {
      return -1;
    }
  }
  return 0;
}/* qcril_uim_qmi_copy_extended_card_status */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_copy_legacy_card_status
===========================================================================*/
/*!
@brief
  copies legacy card status response/indication data to response data
  struct compatible to the card.
*/
/*=========================================================================*/
static int qcril_uim_qmi_copy_legacy_card_status
(
  const uim_card_status_type_v01                * legacy_card_status,
  qmi_uim_card_status_type                      * card_status_ptr
)
{
  uint32_t i = 0;

  if(card_status_ptr == NULL || legacy_card_status == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    return -1;
  }

  if(legacy_card_status->card_info_len > QMI_UIM_CARDS_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return -1;
  }

  card_status_ptr->card_status_type = QMI_UIM_LEGACY_CARD_STATUS_TYPE;
  card_status_ptr->index_gw_pri_prov = (unsigned short)legacy_card_status->index_gw_pri;
  card_status_ptr->index_1x_pri_prov = (unsigned short)legacy_card_status->index_1x_pri;
  card_status_ptr->index_gw_sec_prov = (unsigned short)legacy_card_status->index_gw_sec;
  card_status_ptr->index_1x_sec_prov = (unsigned short)legacy_card_status->index_1x_sec;
  card_status_ptr->num_slots = (unsigned char)legacy_card_status->card_info_len;

  for(i = 0; i < card_status_ptr->num_slots; i++)
  {
    if((legacy_card_status->card_info[i]).app_info_len > QMI_UIM_APPS_MAX_V01)
    {
      QCRIL_LOG_ERROR("%s", "data length too long");
      return -1;
    }
    (card_status_ptr->card[i]).card_state =
      (qmi_uim_card_state_type)(legacy_card_status->card_info[i]).card_state;
    (card_status_ptr->card[i]).upin_state =
      (qmi_uim_pin_status_type)(legacy_card_status->card_info[i]).upin.pin_state;
    (card_status_ptr->card[i]).upin_num_retries =
      (unsigned char)(legacy_card_status->card_info[i]).upin.pin_retries;
    (card_status_ptr->card[i]).upuk_num_retries =
      (unsigned char)(legacy_card_status->card_info[i]).upin.puk_retries;
    (card_status_ptr->card[i]).card_error =
      (qmi_uim_card_error_type)(legacy_card_status->card_info[i]).error_code;
    (card_status_ptr->card[i]).num_app =
      (unsigned char)(legacy_card_status->card_info[i]).app_info_len;

    if(qcril_uim_qmi_copy_app_info((legacy_card_status->card_info[i]).app_info,
                                   (legacy_card_status->card_info[i]).app_info_len,
                                   card_status_ptr,
                                   i) < 0)
    {
      QCRIL_LOG_ERROR("%s", "unable to copy app info");
      return -1;
    }
  }
  return 0;
}/* qcril_uim_qmi_copy_legacy_card_status */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_copy_card_status_validity
===========================================================================*/
/*!
@brief
  copies legacy card status validity data to response data
  struct compatible to the card.
*/
/*=========================================================================*/
static int qcril_uim_qmi_copy_card_status_validity
(
  uint32_t                        card_status_validity_len,
  const uint8_t                 * card_status_validity_ptr,
  uint8_t                         card_status_validity_valid,
  qmi_uim_bool_type             * card_status_invalid_ptr
)
{
  uint8_t i = 0;

  if(card_status_validity_valid)
  {
    if(card_status_validity_len > QMI_UIM_CARDS_MAX_V01)
    {
      QCRIL_LOG_ERROR("card status validity length too long: 0x%x",
                      card_status_validity_len);
      return -1;
    }
    for(i = 0; i < card_status_validity_len && i < QMI_UIM_MAX_CARD_COUNT; i++)
    {
      card_status_invalid_ptr[i] =
        (card_status_validity_ptr[i] == 0) ? QMI_UIM_TRUE : QMI_UIM_FALSE;
    }
  }
  else
  {
    memset((void *)card_status_invalid_ptr,
           (uint8_t)QMI_UIM_FALSE,
           QMI_UIM_MAX_CARD_COUNT);
  }

  return 0;
} /* qcril_uim_qmi_copy_card_status_validity */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_get_card_status_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_get_card_status_resp
(
  const uim_get_card_status_resp_msg_v01       * qmi_response,
  qmi_uim_rsp_data_type                        * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GET_CARD_STATUS_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->extended_card_status_valid)
    {
      if(qcril_uim_qmi_copy_extended_card_status(&qmi_response->extended_card_status,
                                                 &rsp_data->rsp_data.get_card_status_rsp.card_status,
                                                 rsp_data->rsp_data.get_card_status_rsp.card_status_validity.card_status_invalid) < 0)
      {
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
    }
    else if(qmi_response->card_status_valid)
    {
      if(qcril_uim_qmi_copy_legacy_card_status(&qmi_response->card_status,
                                               &rsp_data->rsp_data.get_card_status_rsp.card_status) < 0)
      {
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }

      if(qcril_uim_qmi_copy_card_status_validity(
           qmi_response->card_status_validity_len,
           qmi_response->card_status_validity,
           qmi_response->card_status_validity_valid,
           rsp_data->rsp_data.get_card_status_rsp.card_status_validity.card_status_invalid) < 0)
      {
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("%s", "card status data not present");
      rsp_data->qmi_err_code = QMI_SERVICE_ERR;
      return;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_get_card_status_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_event_reg_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_event_reg_resp
(
  const uim_event_reg_resp_msg_v01        * qmi_response,
  qmi_uim_rsp_data_type                   * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_EVENT_REG_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->event_mask_valid)
    {
      rsp_data->rsp_data.event_reg_rsp.event_mask = (unsigned long int)qmi_response->event_mask;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_event_reg_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_authenticate_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_authenticate_resp
(
  const uim_authenticate_resp_msg_v01      * qmi_response,
  qmi_uim_rsp_data_type                    * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_AUTHENTICATE_RSP_MSG;
  if(qmi_response->card_result_valid)
  {
    rsp_data->rsp_data.authenticate_rsp.sw1 = (unsigned char)qmi_response->card_result.sw1;
    rsp_data->rsp_data.authenticate_rsp.sw2 = (unsigned char)qmi_response->card_result.sw2;
  }
  else
  {
    rsp_data->rsp_data.authenticate_rsp.sw1 = 0;
    rsp_data->rsp_data.authenticate_rsp.sw2 = 0;
  }

  /* Since we may have data even in case error response, update the data
     irrespective of the result */
  if(qmi_response->content_valid)
  {
    if((qmi_response->content_len) > QMI_UIM_AUTHENTICATE_RESULT_MAX_V01)
    {
      QCRIL_LOG_ERROR("%s", "data length too long");
      rsp_data->qmi_err_code = QMI_SERVICE_ERR;
      return;
    }
    /* in sync calls this is freed by client */
    rsp_data->rsp_data.authenticate_rsp.auth_response.data_ptr = (unsigned char *)qcril_malloc(qmi_response->content_len);
    if(rsp_data->rsp_data.authenticate_rsp.auth_response.data_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "data pointer NULL");
      rsp_data->qmi_err_code = QMI_SERVICE_ERR;
      return;
    }
    memcpy((rsp_data->rsp_data.authenticate_rsp.auth_response.data_ptr),
           (unsigned char*)(qmi_response->content),
           (unsigned short)(qmi_response->content_len));
    rsp_data->rsp_data.authenticate_rsp.auth_response.data_len = (unsigned short)qmi_response->content_len;
  }

  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_authenticate_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_get_service_status_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_get_service_status_resp
(
  const uim_get_service_status_resp_msg_v01      * qmi_response,
  qmi_uim_rsp_data_type                          * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GET_SERVICE_STATUS_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->fdn_status_valid)
    {
      rsp_data->rsp_data.get_service_status_rsp.fdn_status = (unsigned char)qmi_response->fdn_status;
      rsp_data->rsp_data.get_service_status_rsp.fdn_status_valid = QMI_UIM_TRUE;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_get_service_status_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_set_service_status_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_set_service_status_resp
(
  const uim_set_service_status_resp_msg_v01      * qmi_response,
  qmi_uim_rsp_data_type                          * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_SET_SERVICE_STATUS_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_set_service_status_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_change_provisioning_session_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_change_provisioning_session_resp
(
  const uim_change_provisioning_session_resp_msg_v01     * qmi_response,
  qmi_uim_rsp_data_type                                  * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_CHANGE_PROV_SESSION_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01 ||
     qmi_response->resp.error == QMI_ERR_NO_EFFECT_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_change_provisioning_session_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_get_label_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_get_label_resp
(
  const uim_get_label_resp_msg_v01        * qmi_response,
  qmi_uim_rsp_data_type                   * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GET_LABEL_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->label_valid)
    {
      if(qmi_response->label_len <= QMI_UIM_MAX_LABEL_LEN)
      {
        memcpy (rsp_data->rsp_data.get_label_rsp.label_value,
                (char*)(qmi_response->label),
                rsp_data->rsp_data.get_label_rsp.label_len);
        rsp_data->rsp_data.get_label_rsp.label_len = (unsigned char)qmi_response->label_len;
      }
      else
      {
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
    }
  }
  else
  {
    QCRIL_LOG_INFO( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_get_label_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_close_session_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_close_session_resp
(
  const uim_close_session_resp_msg_v01          * qmi_response,
  qmi_uim_rsp_data_type                         * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_CLOSE_SESSION_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_close_session_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_send_apdu_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_send_apdu_resp
(
  const uim_send_apdu_resp_msg_v01            * qmi_response,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_SEND_APDU_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->apdu_valid)
    {
      if(qmi_response->apdu_len > QMI_UIM_APDU_DATA_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* in sync calls this is freed by client */
      rsp_data->rsp_data.send_apdu_rsp.apdu_response.data_ptr = (unsigned char *)qcril_malloc(qmi_response->apdu_len);
      if(rsp_data->rsp_data.send_apdu_rsp.apdu_response.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy((rsp_data->rsp_data.send_apdu_rsp.apdu_response.data_ptr),
             (unsigned char*)qmi_response->apdu,
             (unsigned short)qmi_response->apdu_len);
      rsp_data->rsp_data.send_apdu_rsp.apdu_response.data_len = (unsigned short)qmi_response->apdu_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
    /* Upon ERR_INSUFFICIENT_RESOURCES, we get the Long APDU response TLV */
    if (qmi_response->long_response_valid)
    {
      rsp_data->rsp_data.send_apdu_rsp.token     = qmi_response->long_response.token;
      rsp_data->rsp_data.send_apdu_rsp.total_len = qmi_response->long_response.total_length;
    }
  }
} /* qcril_uim_qmi_conv_send_apdu_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_logical_channel_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_logical_channel_resp
(
  const uim_logical_channel_resp_msg_v01         * qmi_response,
  qmi_uim_rsp_data_type                          * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_LOGICAL_CHANNEL_RSP_MSG;
  if(qmi_response->card_result_valid)
  {
    rsp_data->rsp_data.logical_channel_rsp.sw1 = (unsigned char)qmi_response->card_result.sw1;
    rsp_data->rsp_data.logical_channel_rsp.sw2 = (unsigned char)qmi_response->card_result.sw2;
  }
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->channel_id_valid)
    {
      rsp_data->rsp_data.logical_channel_rsp.channel_id = (uint8_t)qmi_response->channel_id;
    }
    if(qmi_response->select_response_valid)
    {
      if(qmi_response->select_response_len > QMI_UIM_SELECT_RESPONSE_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* For sync calls this is freed by client */
      rsp_data->rsp_data.logical_channel_rsp.select_response.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->select_response_len);
      if(rsp_data->rsp_data.logical_channel_rsp.select_response.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy((rsp_data->rsp_data.logical_channel_rsp.select_response.data_ptr),
             (unsigned char*)qmi_response->select_response,
             (unsigned short)qmi_response->select_response_len);
      rsp_data->rsp_data.logical_channel_rsp.select_response.data_len =
        (unsigned short)qmi_response->select_response_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_logical_channel_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_open_logical_channel_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_open_logical_channel_resp
(
  const uim_open_logical_channel_resp_msg_v01         * qmi_response,
  qmi_uim_rsp_data_type                               * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_OPEN_LOGICAL_CHANNEL_RSP_MSG;
  if(qmi_response->card_result_valid)
  {
    rsp_data->rsp_data.logical_channel_rsp.sw1 = (unsigned char)qmi_response->card_result.sw1;
    rsp_data->rsp_data.logical_channel_rsp.sw2 = (unsigned char)qmi_response->card_result.sw2;
  }
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->channel_id_valid)
    {
      rsp_data->rsp_data.logical_channel_rsp.channel_id = (uint8_t)qmi_response->channel_id;
    }
    if(qmi_response->select_response_valid)
    {
      if(qmi_response->select_response_len > QMI_UIM_SELECT_RESPONSE_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* For sync calls this is freed by client */
      rsp_data->rsp_data.logical_channel_rsp.select_response.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->select_response_len);
      if(rsp_data->rsp_data.logical_channel_rsp.select_response.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy((rsp_data->rsp_data.logical_channel_rsp.select_response.data_ptr),
             (unsigned char*)qmi_response->select_response,
             (unsigned short)qmi_response->select_response_len);
      rsp_data->rsp_data.logical_channel_rsp.select_response.data_len =
        (unsigned short)qmi_response->select_response_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_open_logical_channel_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_get_atr_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_get_atr_resp
(
  const uim_get_atr_resp_msg_v01            * qmi_response,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GET_ATR_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->atr_value_valid)
    {
      if(qmi_response->atr_value_len > QMI_UIM_ATR_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* in sync calls this is freed by client */
      rsp_data->rsp_data.get_atr_rsp.atr_response.data_ptr = (unsigned char *)qcril_malloc(qmi_response->atr_value_len);
      if(rsp_data->rsp_data.get_atr_rsp.atr_response.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy((rsp_data->rsp_data.get_atr_rsp.atr_response.data_ptr),
             (unsigned char*)qmi_response->atr_value,
             (unsigned short)qmi_response->atr_value_len);
      rsp_data->rsp_data.get_atr_rsp.atr_response.data_len = (unsigned short)qmi_response->atr_value_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_get_atr_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_send_status_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_send_status_resp
(
  const uim_send_status_cmd_resp_msg_v01     * qmi_response,
  qmi_uim_rsp_data_type                      * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_SEND_STATUS_RSP_MSG;

  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->status_response_valid)
    {
      if((qmi_response->status_response_len) > QMI_UIM_SELECT_RESPONSE_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }

      rsp_data->rsp_data.send_status_rsp.status_response.data_ptr = (unsigned char *)qcril_malloc(qmi_response->status_response_len);
      if(rsp_data->rsp_data.send_status_rsp.status_response.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.send_status_rsp.status_response.data_ptr,
             (unsigned char*)qmi_response->status_response,
             (unsigned short)qmi_response->status_response_len);
      rsp_data->rsp_data.send_status_rsp.status_response.data_len = (unsigned short)qmi_response->status_response_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_send_status_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_reselect_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_reselect_resp
(
  const uim_reselect_resp_msg_v01                     * qmi_response,
  qmi_uim_rsp_data_type                               * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_RESELECT_RSP_MSG;
  if(qmi_response->card_result_valid)
  {
    rsp_data->rsp_data.reselect_rsp.sw1 = (unsigned char)qmi_response->card_result.sw1;
    rsp_data->rsp_data.reselect_rsp.sw2 = (unsigned char)qmi_response->card_result.sw2;
  }
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->select_response_valid)
    {
      if(qmi_response->select_response_len > QMI_UIM_SELECT_RESPONSE_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* For sync calls this is freed by client */
      rsp_data->rsp_data.logical_channel_rsp.select_response.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->select_response_len);
      if(rsp_data->rsp_data.reselect_rsp.select_response.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy((rsp_data->rsp_data.reselect_rsp.select_response.data_ptr),
             (unsigned char*)qmi_response->select_response,
             (unsigned short)qmi_response->select_response_len);
      rsp_data->rsp_data.reselect_rsp.select_response.data_len =
        (unsigned short)qmi_response->select_response_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_reselect_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_supply_voltage_resp
===========================================================================*/
/*!
@brief
  Translates response data coming from QCCI to QCRIL UIM type. If there is
  invalid data, report in rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_supply_voltage_resp
(
  const uim_supply_voltage_resp_msg_v01       * qmi_response,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_SUPPLY_VOLTAGE_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_supply_voltage_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_set_apdu_behavior_resp
===========================================================================*/
/*!
@brief
  Translates response data coming from QCCI to QCRIL UIM type. If there is
  invalid data, report in rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_set_apdu_behavior_resp
(
  const uim_set_apdu_behavior_resp_msg_v01       * qmi_response,
  qmi_uim_rsp_data_type                          * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_SET_APDU_BEHAVIOR_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_supply_voltage_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_sap_connection_resp
===========================================================================*/
/*!
@brief
  Translates response data coming from QCCI into QMI response datatype.
  If there is invalid data, report error in rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_sap_connection_resp
(
  const uim_sap_connection_resp_msg_v01       * qmi_response,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_SAP_CONNECTION_RSP_MSG;

  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->sap_state_valid)
    {
      rsp_data->rsp_data.sap_connection_rsp.connection_status_valid = QMI_UIM_TRUE;
      rsp_data->rsp_data.sap_connection_rsp.connection_status =
        (qmi_uim_sap_connection_state_type)qmi_response->sap_state;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }

} /* qcril_uim_qmi_conv_sap_connection_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_sap_request_resp
===========================================================================*/
/*!
@brief
  Translates response data coming from QCCI into QMI response datatype.
  If there is invalid data, report error in rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_sap_request_resp
(
  const uim_sap_request_resp_msg_v01          * qmi_response,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_SAP_REQUEST_RSP_MSG;

  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    /* Updated response based on the respective TLVs */
    if(qmi_response->ATR_value_valid)
    {
      if(qmi_response->ATR_value_len > QMI_UIM_ATR_DATA_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* For sync calls this is freed by client, for async calls it is freed
         by the main callback using qcril_uim_qmi_free_data */
      rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->ATR_value_len);
      if(rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy((rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr),
             (unsigned char*)qmi_response->ATR_value,
             (unsigned short)qmi_response->ATR_value_len);
      rsp_data->rsp_data.sap_response_rsp.sap_response.data_len =
        (unsigned short)qmi_response->ATR_value_len;
    }
    else if(qmi_response->apdu_valid)
    {
      if(qmi_response->apdu_len > QMI_UIM_APDU_DATA_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* For sync calls this is freed by client, for async calls it is freed
         by the main callback using qcril_uim_qmi_free_data */
      rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->apdu_len);
      if(rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy((rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr),
             (unsigned char*)qmi_response->apdu,
             (unsigned short)qmi_response->apdu_len);
      rsp_data->rsp_data.sap_response_rsp.sap_response.data_len =
        (unsigned short)qmi_response->apdu_len;
    }
    else if(qmi_response->status_valid)
    {
      if(qmi_response->status_len > QMI_UIM_CARD_READER_DATA_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* For sync calls this is freed by client, for async calls it is freed
         by the main callback using qcril_uim_qmi_free_data */
      rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->status_len);
      if(rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy((rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr),
             (unsigned char*)qmi_response->status,
             (unsigned short)qmi_response->status_len);
      rsp_data->rsp_data.sap_response_rsp.sap_response.data_len =
        (unsigned short)qmi_response->status_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_sap_request_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_gba_request_resp
===========================================================================*/
/*!
@brief
  Translates response data coming from QCCI to QCRIL UIM type. If there is
  invalid data, report in rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_gba_request_resp
(
  const uim_gba_resp_msg_v01                  * qmi_response,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GBA_RSP_MSG;

  /* Copy & update the response struct */
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;

    /* Copy KS NAF */
    if(qmi_response->ks_naf_valid)
    {
      /* For sync calls this is freed by client */
      rsp_data->rsp_data.gba_rsp.ks_naf.data_ptr =
        (unsigned char *)qcril_malloc(sizeof(qmi_response->ks_naf));
      if(rsp_data->rsp_data.gba_rsp.ks_naf.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.gba_rsp.ks_naf.data_ptr,
             (unsigned char*)qmi_response->ks_naf,
             (unsigned short)(sizeof(qmi_response->ks_naf)));
      rsp_data->rsp_data.gba_rsp.ks_naf.data_len =
        (unsigned short)sizeof(qmi_response->ks_naf);
    }
    /* Copy encrypted KS NAF */
    else if(qmi_response->encrypted_ks_naf_valid)
    {
      if(qmi_response->encrypted_ks_naf_len > QMI_UIM_ENCRYPTED_KS_NAF_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* For sync calls this is freed by client */
      rsp_data->rsp_data.gba_rsp.ks_naf.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->encrypted_ks_naf_len);
      if(rsp_data->rsp_data.gba_rsp.ks_naf.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.gba_rsp.ks_naf.data_ptr,
             (unsigned char*)qmi_response->encrypted_ks_naf,
             (unsigned short)qmi_response->encrypted_ks_naf_len);
      rsp_data->rsp_data.gba_rsp.ks_naf.data_len =
        (unsigned short)qmi_response->encrypted_ks_naf_len;
      rsp_data->rsp_data.gba_rsp.is_ks_naf_encrypted = QMI_UIM_TRUE;
    }

    /* Copy bootstrapping transaction ID */
    if(qmi_response->btid_valid)
    {
      if(qmi_response->btid_len > QMI_UIM_BTID_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* For sync calls this is freed by client */
      rsp_data->rsp_data.gba_rsp.b_tid.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->btid_len);
      if(rsp_data->rsp_data.gba_rsp.b_tid.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.gba_rsp.b_tid.data_ptr,
             (unsigned char*)qmi_response->btid,
             (unsigned short)qmi_response->btid_len);
      rsp_data->rsp_data.gba_rsp.b_tid.data_len =
        (unsigned short)qmi_response->btid_len;
    }

    /* Copy lifetime */
    if(qmi_response->lifetime_valid)
    {
      if(qmi_response->lifetime_len > QMI_UIM_KS_LIFETIME_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* For sync calls this is freed by client */
      rsp_data->rsp_data.gba_rsp.key_lifetime.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->lifetime_len);
      if(rsp_data->rsp_data.gba_rsp.key_lifetime.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.gba_rsp.key_lifetime.data_ptr,
             (unsigned char*)qmi_response->lifetime,
             (unsigned short)qmi_response->lifetime_len);
      rsp_data->rsp_data.gba_rsp.key_lifetime.data_len =
        (unsigned short)qmi_response->lifetime_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_gba_request_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_gba_imsi_resp
===========================================================================*/
/*!
@brief
  Translates response data coming from QCCI to QCRIL UIM type. If there is
  invalid data, report in rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_gba_imsi_resp
(
  const uim_get_gba_impi_resp_msg_v01         * qmi_response,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GBA_IMPI_RSP_MSG;

  /* Copy & update the response struct */
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;

    /* Copy encrypted IMPI if sent */
    if(qmi_response->encrypted_impi_valid)
    {
      if(qmi_response->encrypted_impi_len > QMI_UIM_ENCRYPTED_IMPI_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* For sync calls this is freed by client */
      rsp_data->rsp_data.gba_impi_rsp.impi.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->encrypted_impi_len);
      if(rsp_data->rsp_data.gba_impi_rsp.impi.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.gba_impi_rsp.impi.data_ptr,
             (unsigned char*)qmi_response->encrypted_impi,
             (unsigned short)qmi_response->encrypted_impi_len);
      rsp_data->rsp_data.gba_impi_rsp.impi.data_len =
        (unsigned short)qmi_response->encrypted_impi_len;

      rsp_data->rsp_data.gba_impi_rsp.is_encrypted = QMI_UIM_TRUE;
    }
    /* otherwise copy plaintext IMPI, if available */
    else if(qmi_response->impi_valid)
    {
      if(qmi_response->impi_len > QMI_UIM_IMPI_MAX_V01)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      /* For sync calls this is freed by client */
      rsp_data->rsp_data.gba_impi_rsp.impi.data_ptr =
        (unsigned char *)qcril_malloc(qmi_response->impi_len);
      if(rsp_data->rsp_data.gba_impi_rsp.impi.data_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }
      memcpy(rsp_data->rsp_data.gba_impi_rsp.impi.data_ptr,
             (unsigned char*)qmi_response->impi,
             (unsigned short)qmi_response->impi_len);
      rsp_data->rsp_data.gba_impi_rsp.impi.data_len =
        (unsigned short)qmi_response->impi_len;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_gba_imsi_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_get_configuration_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_get_configuration_resp
(
  const uim_get_configuration_resp_msg_v01       * qmi_response,
  qmi_uim_rsp_data_type                          * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GET_CONFIGURATION_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->personalization_status_valid)
    {
      rsp_data->rsp_data.get_configuration_rsp.perso_status_valid = QMI_UIM_TRUE;
      rsp_data->rsp_data.get_configuration_rsp.perso_status_len =
        (unsigned char)qmi_response->personalization_status_len;
      if(qmi_response->personalization_status_len > 0)
      {
        uint8 i = 0;
        uint8 j = 0;

        for(i = 0; i < qmi_response->personalization_status_len && i < QMI_UIM_MAX_PERSO_FEATURES; i++)
        {
          rsp_data->rsp_data.get_configuration_rsp.perso_status[i].feature =
            (qmi_uim_perso_feature_id_type)qmi_response->personalization_status[i].feature;
          rsp_data->rsp_data.get_configuration_rsp.perso_status[i].verify_left =
            qmi_response->personalization_status[i].verify_left;
          rsp_data->rsp_data.get_configuration_rsp.perso_status[i].unblock_left =
            qmi_response->personalization_status[i].unblock_left;

          if(qmi_response->perso_control_key_present_valid)
          {
            /* Since QMI UIM provides slot0 data, update the control key presence for slot0 only */
            for(j = 0;
                j < qmi_response->perso_control_key_present[0].feature_control_key_present_len && j < QMI_UIM_MAX_PERSO_FEATURES;
                j++)
            {
              if(qmi_response->personalization_status[i].feature ==
                   qmi_response->perso_control_key_present[0].feature_control_key_present[j].feature &&
                 qmi_response->perso_control_key_present[0].feature_control_key_present[j].ck_present != 0)
              {
                rsp_data->rsp_data.get_configuration_rsp.perso_status[i].ck_present = QMI_UIM_TRUE;
                break;
              }
            }
          }
          else
          {
            /* If perso feature is present and control key present TLV is not present then mark
               CK present as TRUE. This is done to support older modem with new apps builds where
               control key present TLV might not be provided */
            rsp_data->rsp_data.get_configuration_rsp.perso_status[i].ck_present = QMI_UIM_TRUE;
          }
        }
      }
    }
    if(qmi_response->temporary_depersonalization_status_valid)
    {
      uint8 i = 0;

      for(; i < qmi_response->temporary_depersonalization_status[0].temporary_unlock_status_len &&
                 i < QMI_UIM_MAX_PERSO_FEATURES; i++)
      {
        rsp_data->rsp_data.get_configuration_rsp.temp_perso_status[i].feature =
          (qmi_uim_perso_feature_id_type)qmi_response->temporary_depersonalization_status[0].temporary_unlock_status[i].feature;
        rsp_data->rsp_data.get_configuration_rsp.temp_perso_status[i].unlock_time_left =
          qmi_response->temporary_depersonalization_status[0].temporary_unlock_status[i].temporary_unlock_time_left;
      }
      rsp_data->rsp_data.get_configuration_rsp.temp_perso_status_len =
        qmi_response->temporary_depersonalization_status[0].temporary_unlock_status_len;

      for( i = 0; i < qmi_response->temporary_depersonalization_status_len && i < QMI_UIM_MAX_CARD_COUNT; i++)
      {
        if(qmi_response->temporary_depersonalization_status[i].temporary_unlock_status_len > 0)
        {
          rsp_data->rsp_data.get_configuration_rsp.temp_unlock_status[i] = QMI_UIM_TRUE;
        }
      }
    }
    if(qmi_response->automatic_selection_valid)
    {
      rsp_data->rsp_data.get_configuration_rsp.auto_selection_valid = QMI_UIM_TRUE;
      rsp_data->rsp_data.get_configuration_rsp.auto_selection =
        (unsigned char) qmi_response->automatic_selection;
    }
    if(qmi_response->halt_subscription_valid)
    {
      rsp_data->rsp_data.get_configuration_rsp.halt_subscription_valid = QMI_UIM_TRUE;
      rsp_data->rsp_data.get_configuration_rsp.halt_subscription =
        (unsigned char) qmi_response->halt_subscription;
    }
    if(qmi_response->extended_apdu_valid)
    {
      rsp_data->rsp_data.get_configuration_rsp.extended_apdu_valid = QMI_UIM_TRUE;
      rsp_data->rsp_data.get_configuration_rsp.extended_apdu =
        (unsigned char) qmi_response->extended_apdu;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_conv_get_configuration_resp */


/*===========================================================================
  FUNCTION  qmi_uim_add_profile_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qmi_uim_add_profile_resp
(
  const uim_add_profile_resp_msg_v01       * qmi_response,
  qmi_uim_rsp_data_type                    * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_ADD_PROFILE_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qmi_uim_add_profile_resp */


/*===========================================================================
  FUNCTION  qmi_uim_get_profiles_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qmi_uim_get_profiles_resp
(
  const uim_get_sim_profile_resp_msg_v01   * qmi_response,
  qmi_uim_rsp_data_type                    * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GET_SIM_PROFILES_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;

    if (qmi_response->profile_info_valid &&
        qmi_response->profile_info_len > 0)
    {
      uint32_t  len = 0;

      for (len = 0; len < qmi_response->profile_info_len &&
                    len < QMI_UIM_PROFILES_MAX &&
                    len < QMI_UIM_PROFILES_MAX_V01; len++)
      {
        rsp_data->rsp_data.get_profiles_resp.profiles[len].profile_id =
             (qmi_uim_profile_id_type)qmi_response->profile_info[len].profile_id;
        rsp_data->rsp_data.get_profiles_resp.profiles[len].profile_type =
             (qmi_uim_profile_type)qmi_response->profile_info[len].profile_type;
      }
      rsp_data->rsp_data.get_profiles_resp.profiles_len = len;
    }
    rsp_data->rsp_data.get_profiles_resp.active_profile_valid =
         qmi_response->active_profile_info_valid;
    rsp_data->rsp_data.get_profiles_resp.active_profile.profile_id =
         (qmi_uim_profile_id_type)qmi_response->active_profile_info.profile_id;
    rsp_data->rsp_data.get_profiles_resp.active_profile.profile_type =
         (qmi_uim_profile_type)qmi_response->active_profile_info.profile_type;
  }
  else
  {
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qmi_uim_get_profiles_resp */


/*===========================================================================
  FUNCTION  qmi_uim_delete_profile_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qmi_uim_delete_profile_resp
(
  const uim_delete_profile_resp_msg_v01    * qmi_response,
  qmi_uim_rsp_data_type                    * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_DELETE_PROFILE_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qmi_uim_delete_profile_resp */


/*===========================================================================
  FUNCTION  qmi_uim_update_profile_nickname_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qmi_uim_update_profile_nickname_resp
(
  const uim_update_profile_nickname_resp_msg_v01   * qmi_response,
  qmi_uim_rsp_data_type                            * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_UPDATE_PROFILE_NICKNAME_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qmi_uim_update_profile_nickname_resp */


/*===========================================================================
  FUNCTION  qmi_uim_set_sim_profile_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qmi_uim_set_sim_profile_resp
(
  const uim_set_sim_profile_resp_msg_v01   * qmi_response,
  qmi_uim_rsp_data_type                    * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_SET_SIM_PROFILE_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qmi_uim_set_sim_profile_resp */


/*===========================================================================
  FUNCTION  qmi_uim_euicc_memory_reset_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qmi_uim_euicc_memory_reset_resp
(
  const uim_euicc_memory_reset_resp_msg_v01   * qmi_response,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_EUICC_MEMORY_RESET_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qmi_uim_euicc_memory_reset_resp */


/*===========================================================================
  FUNCTION  qmi_uim_user_consent_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qmi_uim_user_consent_resp
(
  const uim_profile_user_consent_resp_msg_v01   * qmi_response,
  qmi_uim_rsp_data_type                         * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_USER_CONSENT_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qmi_uim_user_consent_resp */


/*===========================================================================
  FUNCTION  qmi_uim_get_eid_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qmi_uim_get_eid_resp
(
  const uim_get_eid_resp_msg_v01   * qmi_response,
  qmi_uim_rsp_data_type            * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GET_EID_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if (qmi_response->eid_value_valid &&
        qmi_response->eid_value_len > 0 &&
        qmi_response->eid_value_len <= QMI_UIM_EID_LEN_V01)
    {
      rsp_data->rsp_data.eid_resp.eid_len = qmi_response->eid_value_len;
      rsp_data->rsp_data.eid_resp.eid_ptr = (uint8_t *)&qmi_response->eid_value[0];
    }
  }
  else
  {
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qmi_uim_get_eid_resp */


/*===========================================================================
  FUNCTION  qmi_uim_get_set_server_address_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qmi_uim_get_set_server_address_resp
(
  const uim_euicc_default_server_address_resp_msg_v01  * qmi_response,
  qmi_uim_rsp_data_type                                * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GET_SET_SERVER_ADDRESS_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if (qmi_response->default_smds_address_valid &&
        qmi_response->default_smds_address_len > 0 &&
        qmi_response->default_smds_address_len < QMI_UIM_FQDN_MAX_V01)
    {
      rsp_data->rsp_data.euicc_server_address_rsp.smds_address_ptr =
           (char *)&qmi_response->default_smds_address[0];
    }
    if (qmi_response->default_smdp_address_valid &&
        qmi_response->default_smdp_address_len > 0 &&
        qmi_response->default_smdp_address_len < QMI_UIM_FQDN_MAX_V01)
    {
      rsp_data->rsp_data.euicc_server_address_rsp.smdp_address_ptr =
           (char *)&qmi_response->default_smdp_address[0];
    }
  }
  else
  {
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qmi_uim_get_set_server_address_resp */


/*===========================================================================
  FUNCTION  qmi_uim_get_profile_info_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qmi_uim_get_profile_info_resp
(
  const uim_get_profile_info_resp_msg_v01   * qmi_response,
  qmi_uim_rsp_data_type                     * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GET_PROFILE_INFO_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;

    if (qmi_response->profile_iccid_valid &&
        qmi_response->profile_iccid_len > 0 &&
        qmi_response->profile_iccid_len <= QMI_UIM_ICCID_LEN_MAX_V01)
    {
      rsp_data->rsp_data.profile_info_rsp.iccid_len = qmi_response->profile_iccid_len;
      rsp_data->rsp_data.profile_info_rsp.iccid_ptr = (uint8_t *)&qmi_response->profile_iccid[0];
    }

    if (qmi_response->profile_state_valid)
    {
      rsp_data->rsp_data.profile_info_rsp.state =
        (qmi_uim_profile_state_type)qmi_response->profile_state;
    }

    if (qmi_response->profile_nickname_valid &&
        qmi_response->profile_nickname_len > 0 &&
        qmi_response->profile_nickname_len <= QMI_UIM_EUICC_NICKNAME_MAX_V01)
    {
      rsp_data->rsp_data.profile_info_rsp.nickname = (char *)&qmi_response->profile_nickname[0];
    }

    if (qmi_response->profile_spn_valid &&
        qmi_response->profile_spn_len > 0 &&
        qmi_response->profile_spn_len <= QMI_UIM_EUICC_SPN_MAX_V01)
    {
      rsp_data->rsp_data.profile_info_rsp.spn = (char *)&qmi_response->profile_spn[0];
    }

    if (qmi_response->profile_name_valid &&
        qmi_response->profile_name_len > 0 &&
        qmi_response->profile_name_len <= QMI_UIM_EUICC_PROFILE_NAME_MAX_V01)
    {
      rsp_data->rsp_data.profile_info_rsp.name = (char *)&qmi_response->profile_name[0];
    }

    if (qmi_response->profile_icon_type_valid)
    {
      rsp_data->rsp_data.profile_info_rsp.icon_type =
        (qmi_uim_profile_icon_type)qmi_response->profile_icon_type;
    }

    if (qmi_response->profile_icon_data_valid &&
        qmi_response->profile_icon_data_len > 0 &&
        qmi_response->profile_icon_data_len <= QMI_UIM_EUICC_ICON_MAX_V01)
    {
      rsp_data->rsp_data.profile_info_rsp.icon_len = qmi_response->profile_icon_data_len;
      rsp_data->rsp_data.profile_info_rsp.icon_data_ptr = (uint8_t *)&qmi_response->profile_icon_data[0];
    }

    if (qmi_response->profile_class_valid)
    {
      rsp_data->rsp_data.profile_info_rsp.pclass =
        (qmi_uim_profile_class_type)qmi_response->profile_class;
    }

    if (qmi_response->profile_policy_rules_valid)
    {
      rsp_data->rsp_data.profile_info_rsp.policy_rules =
        (uint64_t)qmi_response->profile_policy_rules;
    }
  }
  else
  {
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qmi_uim_get_profile_info_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_conv_personalization_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_conv_personalization_resp
(
  const uim_personalization_resp_msg_v01        * qmi_response,
  qmi_uim_rsp_data_type                         * rsp_data
)
{
  if((qmi_response == NULL) || (rsp_data == NULL))
  {
    QCRIL_LOG_ERROR("%s","Invalid input, cannot proceed");
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_PERSONALIZATION_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }

  /* Include retries if TLV is present */
  if(qmi_response->retries_left_valid)
  {
    rsp_data->rsp_data.personalization_rsp.num_retries_valid = QMI_UIM_TRUE;
    rsp_data->rsp_data.personalization_rsp.num_retries = (unsigned char)qmi_response->retries_left.verify_left;
  }
} /* qcril_uim_qmi_conv_personalization_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_get_physical_slot_mapping_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_get_physical_slot_mapping_resp
(
  const uim_get_physical_slot_mapping_resp_msg_v01  * qmi_response,
  qmi_uim_rsp_data_type                             * rsp_data
)
{
  uint8  slot_id = 0;
  if((qmi_response == NULL) || (rsp_data == NULL))
  {
    QCRIL_LOG_ERROR("%s","Invalid input, cannot proceed");
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_GET_PHYSICAL_SLOT_MAPPING_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }

  /* Copy slot mapping if TLV is present */
  if(qmi_response->logical_slot_valid)
  {
    rsp_data->rsp_data.phy_slot_mapping_rsp.num_slots = MIN(qmi_response->logical_slot_len,
                                                            QMI_UIM_MAX_CARD_COUNT);
    for(slot_id = 0; slot_id < qmi_response->logical_slot_len &&
                     slot_id < QMI_UIM_MAX_CARD_COUNT &&
                     slot_id < QMI_UIM_EXTENDED_CARDS_MAX_V01 ; slot_id++)
    {
      rsp_data->rsp_data.phy_slot_mapping_rsp.logical_slot[slot_id] =
                                      (qmi_uim_phy_slot_type)qmi_response->logical_slot[slot_id];
    }
  }
} /* qcril_uim_qmi_get_physical_slot_mapping_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_get_slots_status_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_get_slots_status_resp
(
  const uim_get_slots_status_resp_msg_v01  * qmi_response,
  qmi_uim_rsp_data_type                    * rsp_data
)
{
  uint8  slot_id = 0;

  if((qmi_response == NULL) || (rsp_data == NULL))
  {
    QCRIL_LOG_ERROR("%s","Invalid input, cannot proceed");
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));
  rsp_data->rsp_id = QMI_UIM_SRVC_GET_SLOTS_STATUS_RSP_MSG;

  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }

  /* Copy slot mapping if TLV is present */
  if(qmi_response->physical_slot_status_valid)
  {
    rsp_data->rsp_data.slots_status_rsp.no_of_slots = MIN(qmi_response->physical_slot_status_len,
                                                          QMI_UIM_MAX_SLOT_COUNT);

    for(slot_id = 0; slot_id < qmi_response->physical_slot_status_len; slot_id++)
    {
      rsp_data->rsp_data.slots_status_rsp.slot_status[slot_id].card_state =
          (qmi_uim_slot_card_state_type)qmi_response->physical_slot_status[slot_id].physical_card_status;
      rsp_data->rsp_data.slots_status_rsp.slot_status[slot_id].slot_state =
          (qmi_uim_slot_state_type)qmi_response->physical_slot_status[slot_id].physical_slot_state;
      rsp_data->rsp_data.slots_status_rsp.slot_status[slot_id].logical_slot=
          (qmi_uim_slot_type)qmi_response->physical_slot_status[slot_id].logical_slot;
      rsp_data->rsp_data.slots_status_rsp.slot_status[slot_id].iccid_len =
                                      qmi_response->physical_slot_status[slot_id].iccid_len;
      memcpy(rsp_data->rsp_data.slots_status_rsp.slot_status[slot_id].iccid,
             qmi_response->physical_slot_status[slot_id].iccid,
             QMI_UIM_MAX_ICCID_LEN);
    }
  }
} /* qcril_uim_qmi_get_slots_status_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_switch_slot_ext_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_switch_slot_ext_resp
(
  const uim_switch_slot_ext_resp_msg_v01   * qmi_response,
  qmi_uim_rsp_data_type                    * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_SWITCH_SLOT_EXT_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_switch_slot_ext_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_remote_unlock_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_remote_unlock_resp
(
  const uim_remote_unlock_resp_msg_v01       * qmi_response,
  qmi_uim_rsp_data_type                      * rsp_data
)
{
  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));
  rsp_data->rsp_id = QMI_UIM_SRVC_REMOTE_UNLOCK_RSP_MSG;
  rsp_data->rsp_data.remote_unlock_rsp.operation = QMI_UIM_REMOTE_UNLOCK_INVALID_OPERATION;

  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    const uint8_t  * resp_ptr  = NULL;
    uint32_t         resp_len  = 0;

    rsp_data->qmi_err_code = QMI_NO_ERR;

    if (qmi_response->encrypted_key_valid &&
        qmi_response->encrypted_key_len > 0)
    {
      rsp_data->rsp_data.remote_unlock_rsp.operation =
        QMI_UIM_REMOTE_UNLOCK_RETRIEVE_ENCRYPTED_KEY;
      resp_ptr = qmi_response->encrypted_key;
      resp_len = qmi_response->encrypted_key_len;
    }
    else if (qmi_response->simlock_data_valid &&
        qmi_response->simlock_data_len > 0)
    {
      rsp_data->rsp_data.remote_unlock_rsp.operation =
        QMI_UIM_REMOTE_UNLOCK_PROCESS_SIMLOCK_DATA;
      resp_ptr = qmi_response->simlock_data;
      resp_len = qmi_response->simlock_data_len;
    }
    else if (qmi_response->hmac_data_valid &&
        qmi_response->hmac_data_len > 0)
    {
      rsp_data->rsp_data.remote_unlock_rsp.operation =
        QMI_UIM_REMOTE_UNLOCK_GENERATE_MAC;
      resp_ptr = qmi_response->hmac_data;
      resp_len = qmi_response->hmac_data_len;
    }
    else if (qmi_response->maximum_version_valid)
    {
      rsp_data->rsp_data.remote_unlock_rsp.operation =
        QMI_UIM_REMOTE_UNLOCK_GET_MAX_SUPPORTED_VERSION;
      rsp_data->rsp_data.remote_unlock_rsp.remote_simlock_version.major_version =
        qmi_response->maximum_version.major_version;
      rsp_data->rsp_data.remote_unlock_rsp.remote_simlock_version.minor_version =
        qmi_response->maximum_version.minor_version;
    }
    else if (qmi_response->blob_request_valid &&
        qmi_response->blob_request_len > 0)
    {
      rsp_data->rsp_data.remote_unlock_rsp.operation =
        QMI_UIM_REMOTE_UNLOCK_GENERATE_BLOB;
      resp_ptr = qmi_response->blob_request;
      resp_len = qmi_response->blob_request_len;
    }
    else if (qmi_response->delay_timer_value_valid)
    {
      rsp_data->rsp_data.remote_unlock_rsp.timer_value = qmi_response->delay_timer_value;
    }

    if (resp_ptr != NULL && resp_len != 0)
    {
      rsp_data->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_ptr =
            (uint8_t *)qcril_malloc(sizeof(uint8_t) * resp_len);
      if (rsp_data->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_ptr != NULL)
      {
        memcpy(rsp_data->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_ptr,
               resp_ptr,
               resp_len);
        rsp_data->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_len = resp_len;
      }
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_remote_unlock_resp */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_terminal_capability_resp
===========================================================================*/
/*!
@brief
  translates response data coming from QCCI into response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.
*/
/*=========================================================================*/
static void qcril_uim_qmi_terminal_capability_resp
(
  const uim_terminal_capability_resp_msg_v01       * qmi_response,
  qmi_uim_rsp_data_type                            * rsp_data
)
{
  uint8_t                i = 0;

  if(qmi_response == NULL || rsp_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  rsp_data->rsp_id = QMI_UIM_SRVC_TERMINAL_CAPABILITY_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    rsp_data->rsp_data.terminal_capability_rsp.num_tlvs = 0;

    if(qmi_response->terminal_capability_tlv_valid)
    {
      /* Terminal capability TLV is valid... this happens only incase of
         GET request, and not incase of SET request */
      if(qmi_response->terminal_capability_tlv_len > QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX_V01 ||
         qmi_response->terminal_capability_tlv_len > QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        rsp_data->qmi_err_code = QMI_SERVICE_ERR;
        return;
      }

      for (i = 0; i < qmi_response->terminal_capability_tlv_len; i++)
      {
        /* For sync calls this is freed by client */
        if (qmi_response->terminal_capability_tlv[i].value_len > 0)
        {
          /* A TLV might have a zero-length value */
          rsp_data->rsp_data.terminal_capability_rsp.tlvs[i].value.data_ptr =
            (uint8_t *)qcril_malloc(qmi_response->terminal_capability_tlv[i].value_len);
          if(rsp_data->rsp_data.terminal_capability_rsp.tlvs[i].value.data_ptr == NULL)
          {
            QCRIL_LOG_ERROR("%s", "data pointer NULL");
            rsp_data->qmi_err_code = QMI_SERVICE_ERR;
            return;
          }
          memcpy(rsp_data->rsp_data.terminal_capability_rsp.tlvs[i].value.data_ptr,
                 qmi_response->terminal_capability_tlv[i].value,
                 qmi_response->terminal_capability_tlv[i].value_len);
        }
        rsp_data->rsp_data.terminal_capability_rsp.tlvs[i].value.data_len =
          qmi_response->terminal_capability_tlv[i].value_len;
        rsp_data->rsp_data.terminal_capability_rsp.tlvs[i].tag =
          (uint8_t) qmi_response->terminal_capability_tlv[i].tag;
        rsp_data->rsp_data.terminal_capability_rsp.tlvs[i].tlv_present = QMI_UIM_TRUE;

        rsp_data->rsp_data.terminal_capability_rsp.num_tlvs ++;
      }
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
} /* qcril_uim_qmi_terminal_capability_resp */


/*---------------------------------------------------------------------------
  Callback functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_card_status_ind_hdlr
===========================================================================*/
/*!
@brief
  converts decoded indication response data coming from QCCI to response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.

@return
  If return code < 0, then the operation has failed
*/
/*=========================================================================*/
static int qcril_qmi_uim_get_card_status_ind_hdlr
(
  uim_status_change_ind_msg_v01         * ind_data_ptr,
  qmi_uim_indication_data_type          * ind_data
)
{
  if(ind_data_ptr != NULL)
  {
    if(ind_data_ptr->extended_card_status_valid)
    {
      if(qcril_uim_qmi_copy_extended_card_status(&ind_data_ptr->extended_card_status,
                                                 &ind_data->status_change_ind.card_status,
                                                 ind_data->status_change_ind.card_status_validity.card_status_invalid) < 0)
      {
        return QMI_SERVICE_ERR;
      }
    }
    else if(ind_data_ptr->card_status_valid)
    {
      /* Legacy card status is processed only if extended card status is missing. */
      if(qcril_uim_qmi_copy_legacy_card_status(&ind_data_ptr->card_status,
                                               &ind_data->status_change_ind.card_status) < 0)
      {
        return QMI_SERVICE_ERR;
      }

      if(qcril_uim_qmi_copy_card_status_validity(
           ind_data_ptr->card_status_validity_len,
           ind_data_ptr->card_status_validity,
           ind_data_ptr->card_status_validity_valid,
           ind_data->status_change_ind.card_status_validity.card_status_invalid) < 0)
      {
        return QMI_SERVICE_ERR;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("%s", "card status not present in indication");
      return QMI_SERVICE_ERR;
    }
  }
  else
  {
    return QMI_SERVICE_ERR;
  }
  return 0;
} /* qcril_qmi_uim_get_card_status_ind_hdlr */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_refresh_ind_hdlr
===========================================================================*/
/*!
@brief
  converts decoded indication response data coming from QCCI to response data
  compatible to the card. If there is invalid data, report in
  rsp_data->qmi_err_code.

@return
  If return code < 0, then the operation has failed
*/
/*=========================================================================*/
static int qcril_qmi_uim_refresh_ind_hdlr
(
  uim_refresh_ind_msg_v01          * ind_data_ptr,
  qmi_uim_indication_data_type     * ind_data
)
{
  unsigned int  i = 0;

  if(ind_data_ptr == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  if(ind_data_ptr->refresh_event_valid)
  {
    if((ind_data_ptr->refresh_event.aid_len > QMI_UIM_AID_MAX_V01) ||
       (ind_data_ptr->refresh_event.files_len > QMI_UIM_REFRESH_FILES_MAX_V01))
    {
      QCRIL_LOG_ERROR("%s", "data length too long");
      return QMI_SERVICE_ERR;
    }
    ind_data->refresh_ind.refresh_event.refresh_stage = (qmi_uim_refresh_stage)ind_data_ptr->refresh_event.stage;
    ind_data->refresh_ind.refresh_event.refresh_mode = (qmi_uim_refresh_mode)ind_data_ptr->refresh_event.mode;
    ind_data->refresh_ind.refresh_event.session_type = (qmi_uim_session_type)ind_data_ptr->refresh_event.session_type;
    ind_data->refresh_ind.refresh_event.aid_len = (unsigned char)ind_data_ptr->refresh_event.aid_len;
    memcpy((ind_data->refresh_ind.refresh_event.aid_value),
           ind_data_ptr->refresh_event.aid,
           ind_data_ptr->refresh_event.aid_len);
    ind_data->refresh_ind.refresh_event.num_files = (unsigned short)(ind_data_ptr->refresh_event.files_len);
    ind_data->refresh_ind.refresh_event.files_ptr =
      (qmi_uim_refresh_file_id_type*)qcril_malloc(ind_data_ptr->refresh_event.files_len * sizeof(qmi_uim_refresh_file_id_type));
    if(ind_data->refresh_ind.refresh_event.files_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "data pointer NULL");
      return QMI_SERVICE_ERR;
    }
    memset(ind_data->refresh_ind.refresh_event.files_ptr,
           0,
           ind_data_ptr->refresh_event.files_len * sizeof(qmi_uim_refresh_file_id_type));
    for(i = 0; i < ind_data_ptr->refresh_event.files_len; i++)
    {
      if((ind_data_ptr->refresh_event.files[i].path_len) > QMI_UIM_MAX_FILE_PATH)
      {
        QCRIL_LOG_ERROR("%s", "data length too long");
        return QMI_SERVICE_ERR;
      }
      ind_data->refresh_ind.refresh_event.files_ptr[i].file_id =
        (unsigned short)(ind_data_ptr->refresh_event.files[i].file_id);
      ind_data->refresh_ind.refresh_event.files_ptr[i].path_len =
        (unsigned char)(ind_data_ptr->refresh_event.files[i].path_len);
      memcpy(ind_data->refresh_ind.refresh_event.files_ptr[i].path_value,
             ind_data_ptr->refresh_event.files[i].path,
             ind_data_ptr->refresh_event.files[i].path_len);
    }
  }

  return 0;
} /* qcril_qmi_uim_refresh_ind_hdlr */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_send_apdu_ind_hdlr
===========================================================================*/
/*!
@brief
  Converts decoded indication data coming from QCCI to indication data in
  internla QMI format. If there is invalid data, report in
  rsp_data->qmi_err_code.

@return
  If return code < 0, then the operation has failed
*/
/*=========================================================================*/
static int qcril_qmi_uim_send_apdu_ind_hdlr
(
  uim_send_apdu_ind_msg_v01          * qcci_data_ptr,
  qmi_uim_indication_data_type       * qmi_data_ptr
)
{
  if ((qcci_data_ptr == NULL) || (qmi_data_ptr == NULL))
  {
    return QMI_SERVICE_ERR;
  }

  /* Sanity check */
  if (qcci_data_ptr->long_response_chunk.apdu_len > QMI_UIM_APDU_DATA_MAX_V01)
  {
    QCRIL_LOG_ERROR("Data length too long: 0x%x", qcci_data_ptr->long_response_chunk.apdu_len);
    return QMI_SERVICE_ERR;
  }

  /* Allocate memory for APDU data & copy individual fields */
  qmi_data_ptr->send_apdu_ind.apdu.data_len = qcci_data_ptr->long_response_chunk.apdu_len;
  qmi_data_ptr->send_apdu_ind.apdu.data_ptr =
    (unsigned char *)qcril_malloc(qmi_data_ptr->send_apdu_ind.apdu.data_len);
  if(qmi_data_ptr->send_apdu_ind.apdu.data_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Couldnt allocate memory for APDU data !");
    qmi_data_ptr->send_apdu_ind.apdu.data_len = 0;
    return QMI_SERVICE_ERR;
  }
  memcpy(qmi_data_ptr->send_apdu_ind.apdu.data_ptr,
         qcci_data_ptr->long_response_chunk.apdu,
         qmi_data_ptr->send_apdu_ind.apdu.data_len);

  qmi_data_ptr->send_apdu_ind.token     = qcci_data_ptr->long_response_chunk.token;
  qmi_data_ptr->send_apdu_ind.total_len = qcci_data_ptr->long_response_chunk.total_length;
  qmi_data_ptr->send_apdu_ind.offset    = qcci_data_ptr->long_response_chunk.offset;

  return 0;
} /* qcril_qmi_uim_send_apdu_ind_hdlr */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_recovery_ind_hdlr
===========================================================================*/
/*!
@brief
  Converts decoded indication data coming from QCCI to indication data in
  internla QMI format. If there is invalid data, report in
  rsp_data->qmi_err_code.

@return
  If return code < 0, then the operation has failed
*/
/*=========================================================================*/
static int qcril_qmi_uim_recovery_ind_hdlr
(
  uim_recovery_ind_msg_v01           * qcci_data_ptr,
  qmi_uim_indication_data_type       * qmi_data_ptr
)
{
  if ((qcci_data_ptr == NULL) || (qmi_data_ptr == NULL))
  {
    return QMI_SERVICE_ERR;
  }


  /* Allocate memory for APDU data & copy individual fields */
  qmi_data_ptr->recovery_ind.slot = (qmi_uim_slot_type)qcci_data_ptr->slot;

  return 0;
} /* qcril_qmi_uim_recovery_ind_hdlr */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_supply_voltage_ind_hdlr
===========================================================================*/
/*!
@brief
  Converts decoded indication data coming from QCCI to indication data in
  internal QMI format. If there is invalid data, report in
  rsp_data->qmi_err_code.

@return
  If return code < 0, then the operation has failed
*/
/*=========================================================================*/
static int qcril_qmi_uim_supply_voltage_ind_hdlr
(
  uim_supply_voltage_ind_msg_v01       * qcci_data_ptr,
  qmi_uim_indication_data_type         * qmi_data_ptr
)
{
  if ((qcci_data_ptr == NULL) || (qmi_data_ptr == NULL))
  {
    return QMI_SERVICE_ERR;
  }

  /* Copy individual fields */
  qmi_data_ptr->supply_voltage_ind.slot        = (qmi_uim_slot_type)qcci_data_ptr->slot;
  qmi_data_ptr->supply_voltage_ind.vcc_command = (qmi_uim_vcc_command_type)qcci_data_ptr->vcc_command;

  return 0;
} /* qcril_qmi_uim_supply_voltage_ind_hdlr */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_simlock_temp_unlock_ind_hdlr
===========================================================================*/
/*!
@brief
  Converts decoded indication data coming from QCCI to indication data in
  internal QMI format. If there is invalid data, report in
  rsp_data->qmi_err_code.

@return
  If return code < 0, then the operation has failed
*/
/*=========================================================================*/
static int qcril_qmi_uim_simlock_temp_unlock_ind_hdlr
(
  uim_temporary_unlock_ind_msg_v01          * qcci_data_ptr,
  qmi_uim_indication_data_type              * qmi_data_ptr
)
{
  uint32_t i = 0;

  if ((qcci_data_ptr == NULL) || (qmi_data_ptr == NULL))
  {
    return QMI_SERVICE_ERR;
  }

  for (i = 0; i < qcci_data_ptr->temporary_unlock_status_len; i++)
  {
    uint32_t index = (uint32_t)(qcci_data_ptr->temporary_unlock_status[i].slot_id - UIM_SLOT_1_V01);

    if (index >= QMI_UIM_MAX_CARD_COUNT)
    {
      continue;
    }

    if(qcci_data_ptr->temporary_unlock_status[i].feature_info_len > 0)
    {
      qmi_data_ptr->simlock_temp_unlock_ind.temporay_unlock_status[index] = QMI_UIM_TRUE;
    }
  }

  return 0;
} /* qcril_qmi_uim_simlock_temp_unlock_ind_hdlr */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_slot_status_change_ind_hdlr
===========================================================================*/
static int qcril_qmi_uim_slot_status_change_ind_hdlr
(
  uim_slot_status_change_ind_msg_v01    * ind_data_ptr,
  qmi_uim_slots_status_type   * ind_data
)
{
  uint32_t  i  = 0;

  if(ind_data_ptr != NULL && ind_data != NULL)
  {
    if(ind_data_ptr->physical_slot_status_valid &&
       ind_data_ptr->physical_slot_status_len > 0 &&
       ind_data_ptr->physical_slot_status_len <= 5)
    {
      memset(ind_data, 0x00, sizeof(qmi_uim_slots_status_type));
      ind_data->no_of_slots = ind_data_ptr->physical_slot_status_len;

      for (i = 0; i < ind_data->no_of_slots && i < QMI_UIM_MAX_SLOT_COUNT; i++)
      {
        ind_data->slot_status[i].card_state   =
           (qmi_uim_slot_card_state_type)ind_data_ptr->physical_slot_status[i].physical_card_status;
        ind_data->slot_status[i].slot_state   =
           (qmi_uim_slot_state_type)ind_data_ptr->physical_slot_status[i].physical_slot_state;
        ind_data->slot_status[i].logical_slot =
           (qmi_uim_slot_type)ind_data_ptr->physical_slot_status[i].logical_slot;
        ind_data->slot_status[i].iccid_len    = ind_data_ptr->physical_slot_status[i].iccid_len;
        memcpy(ind_data->slot_status[i].iccid,
               ind_data_ptr->physical_slot_status[i].iccid,
               QMI_UIM_MAX_ICCID_LEN);
      }
    }
    else
    {
      QCRIL_LOG_ERROR("%s", "card status not present in indication");
      return QMI_SERVICE_ERR;
    }
  }
  else
  {
    return QMI_SERVICE_ERR;
  }

  return 0;
} /* qcril_qmi_uim_slot_status_change_ind_hdlr */


/*===========================================================================

  FUNCTION:  qcril_qmi_sap_ind_hdlr

===========================================================================*/
/*!
    @brief
    This is the SAP indication callback implementation for the QMI interface.

    @return
    None.

*/
/*=========================================================================*/
static int qcril_qmi_sap_ind_hdlr
(
  uim_sap_connection_ind_msg_v01   * ind_data_ptr,
  qmi_uim_indication_data_type     * qmi_data_ptr
)
{
  if (ind_data_ptr == NULL ||
      ind_data_ptr->sap_connection_event_valid == FALSE)
  {
    QCRIL_LOG_ERROR("%s", "NULL ind_data_ptr");
    return QMI_SERVICE_ERR;
  }

  qmi_data_ptr->sap_ind.sap_state = (qmi_uim_sap_status_type)ind_data_ptr->sap_connection_event.sap_state;
  qmi_data_ptr->sap_ind.slot      = (qmi_uim_slot_type)ind_data_ptr->sap_connection_event.slot;

  return 0;
} /* qcril_qmi_sap_ind_hdlr */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_lpa_add_profile_ind_hdlr
===========================================================================*/
static int qcril_qmi_uim_lpa_add_profile_ind_hdlr
(
  uim_add_profile_ind_msg_v01               * qcci_data_ptr,
  qmi_uim_indication_data_type              * qmi_data_ptr
)
{
  if ((qcci_data_ptr == NULL) || (qmi_data_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process indication");
    return QMI_SERVICE_ERR;
  }

  qmi_data_ptr->lpa_add_profile_ind.slot = (qmi_uim_slot_type)qcci_data_ptr->slot;
  qmi_data_ptr->lpa_add_profile_ind.profile_download_status =
         (qmi_uim_lpa_download_status_type)qcci_data_ptr->profile_download_status;
  qmi_data_ptr->lpa_add_profile_ind.error_cause_valid       =
                                        (qmi_uim_bool_type)qcci_data_ptr->error_cause_valid;
  qmi_data_ptr->lpa_add_profile_ind.error_cause             =
                (qmi_uim_lpa_download_error_cause_type)qcci_data_ptr->error_cause;
  qmi_data_ptr->lpa_add_profile_ind.percentage_valid        =
                                         (qmi_uim_bool_type)qcci_data_ptr->percentage_valid;
  qmi_data_ptr->lpa_add_profile_ind.percentage = qcci_data_ptr->percentage;
  qmi_data_ptr->lpa_add_profile_ind.profile_policy_rules_valid =
                               (qmi_uim_bool_type)qcci_data_ptr->profile_policy_rules_valid;
  qmi_data_ptr->lpa_add_profile_ind.profile_policy_mask     =
         (qmi_uim_lpa_ppr_mask_type)qcci_data_ptr->profile_policy_rules.profile_policy_mask;
  qmi_data_ptr->lpa_add_profile_ind.user_consent_needed     =
                 (qmi_uim_bool_type)qcci_data_ptr->profile_policy_rules.user_consent_needed;

  return 0;
} /* qcril_qmi_uim_lpa_add_profile_ind_hdlr */


/*===========================================================================
  FUNCTION  qmi_client_indication_cb
===========================================================================*/
/*!
@brief
  This is the callback function that will be called by the generic
  services layer to report asynchronous indications.  This function will
  process the indication TLV's and then call the user registered
  functions with the indication data.

@return
  None.

@note

  - Dependencies

  - Side Effects
*/
/*=========================================================================*/
void qmi_client_indication_cb
(
  qmi_client_type                client_handle,
  unsigned int                   msg_id,
  void                         * ind_buf,
  unsigned int                   ind_buf_len,
  void                         * ind_cb_data
)
{
  uint32_t                               decoded_payload_len  =   0;
  qmi_client_error_type                  qmi_err              =   QMI_NO_ERR;
  void                                 * decoded_payload      =   NULL;
  qmi_uim_indication_id_type             ind_id;
  qmi_uim_indication_data_type         * ind_data             =   NULL;

  QCRIL_LOG_INFO("qmi_client_indication_cb, msg_id: 0x%x", msg_id);

  if(client_handle == NULL)
  {
    QCRIL_LOG_ERROR("%s", "client_handle is not intialized, discarding");
    return;
  }

  if(ind_buf == NULL)
  {
    return;
  }

  ind_data = (qmi_uim_indication_data_type*)qcril_malloc(sizeof(qmi_uim_indication_data_type));

  if(ind_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "ind_data failed to allocate memory");
    return;
  }

  /* Initialize data */
  memset(ind_data, 0x0, sizeof(qmi_uim_indication_data_type));

  qmi_idl_get_message_c_struct_len(  uim_get_service_object_v01(),
                                     QMI_IDL_INDICATION,
                                     msg_id,
                                     &decoded_payload_len);

  if(decoded_payload_len)
  {
    decoded_payload = qcril_malloc(decoded_payload_len);
    if (decoded_payload == NULL)
    {
      QCRIL_LOG_ERROR("Failed to allocate buffer for QMI UIM Indication: %d\n", msg_id);
      qcril_free(ind_data);
      return;
    }
  }
  else
  {
    QCRIL_LOG_ERROR("%s: Failed to find decoded_payload_len", __FUNCTION__);
    qcril_free(ind_data);
    return;
  }

  if (decoded_payload != NULL)
  {
    qmi_err = qmi_client_message_decode( client_handle,
                                         QMI_IDL_INDICATION,
                                         msg_id,
                                         ind_buf,
                                         ind_buf_len,
                                         decoded_payload,
                                         decoded_payload_len);

    /* Determine the indication ID and process appropriately */
    if (QMI_NO_ERR == qmi_err)/* what is going on here */
    {
      switch(msg_id)
      {
        case QMI_UIM_STATUS_CHANGE_IND_V01:
          ind_id = QMI_UIM_SRVC_STATUS_CHANGE_IND_MSG;
          if(qcril_qmi_uim_get_card_status_ind_hdlr((uim_status_change_ind_msg_v01*)decoded_payload, ind_data) != 0)
          {
            qcril_free(decoded_payload);
            qcril_free(ind_data);
            return;
          }
          break;
        case QMI_UIM_REFRESH_IND_V01:
          ind_id = QMI_UIM_SRVC_REFRESH_IND_MSG;
          if(qcril_qmi_uim_refresh_ind_hdlr((uim_refresh_ind_msg_v01*)decoded_payload, ind_data) != 0)
          {
            qcril_free(decoded_payload);
            qcril_free(ind_data);
            return;
          }
          break;
        case QMI_UIM_SEND_APDU_IND_V01:
          ind_id = QMI_UIM_SRVC_SEND_APDU_IND_MSG;
          if(qcril_qmi_uim_send_apdu_ind_hdlr((uim_send_apdu_ind_msg_v01*)decoded_payload, ind_data) != 0)
          {
            qcril_free(decoded_payload);
            qcril_free(ind_data);
            return;
          }
          break;
        case QMI_UIM_RECOVERY_IND_V01:
          ind_id = QMI_UIM_SRVC_RECOVERY_IND_MSG;
          if(qcril_qmi_uim_recovery_ind_hdlr((uim_recovery_ind_msg_v01*)decoded_payload, ind_data) != 0)
          {
            qcril_free(decoded_payload);
            qcril_free(ind_data);
            return;
          }
          break;
        case QMI_UIM_SUPPLY_VOLTAGE_IND_V01:
          ind_id = QMI_UIM_SRVC_SUPPLY_VOLTAGE_IND_MSG;
          if(qcril_qmi_uim_supply_voltage_ind_hdlr((uim_supply_voltage_ind_msg_v01*)decoded_payload, ind_data) != 0)
          {
            qcril_free(decoded_payload);
            qcril_free(ind_data);
            return;
          }
          break;
        case QMI_UIM_SAP_CONNECTION_IND_V01:
          ind_id = QMI_UIM_SRVC_SAP_IND_MSG;
          if (qcril_qmi_sap_ind_hdlr((uim_sap_connection_ind_msg_v01*)decoded_payload, ind_data) != 0)
          {
            qcril_free(decoded_payload);
            qcril_free(ind_data);
            return;
          }
          break;
        case QMI_UIM_TEMPORARY_UNLOCK_STATUS_IND_V01:
          ind_id = QMI_UIM_SRVC_SIMLOCK_TEMP_UNLOCK_IND_MSG;
          if(qcril_qmi_uim_simlock_temp_unlock_ind_hdlr((uim_temporary_unlock_ind_msg_v01*)decoded_payload, ind_data) != 0)
          {
            qcril_free(decoded_payload);
            qcril_free(ind_data);
            return;
          }
          break;
        case QMI_UIM_ADD_PROFILE_IND_V01:
          ind_id = QMI_UIM_SRVC_LPA_APP_PROFILE_IND_MSG;
          if(qcril_qmi_uim_lpa_add_profile_ind_hdlr((uim_add_profile_ind_msg_v01*)decoded_payload, ind_data) != 0)
          {
            qcril_free(decoded_payload);
            qcril_free(ind_data);
            return;
          }
          break;
        case QMI_UIM_SLOT_STATUS_CHANGE_IND_V01:
          ind_id = QMI_UIM_SRVC_SLOT_STATUS_CHANGE_IND_MSG;
          if (qcril_qmi_uim_slot_status_change_ind_hdlr((uim_slot_status_change_ind_msg_v01*)decoded_payload,
                                                        &ind_data->slot_status_change_ind) != 0)
          {
            qcril_free(decoded_payload);
            qcril_free(ind_data);
            return;
          }
          break;
        default:
          QCRIL_LOG_INFO("Unknown QMI UIM indication %d", msg_id);
          qcril_free(decoded_payload);
          qcril_free(ind_data);
          return;
      }
    }
    else
    {
      QCRIL_LOG_INFO("Indication decode failed for msg %d with error %d", msg_id, qmi_err );
      qcril_free(decoded_payload);
      qcril_free(ind_data);
      return;
    }
    qcril_free(decoded_payload);

    if (ind_cb_data != nullptr)
    {
      UimModemEndPointModule *me = (UimModemEndPointModule *)ind_cb_data;
      me->onQmiUnsolIndCb(ind_cb_data, ind_id, ind_data);
    }
  }

  /* Free data allocated in these indications */
  switch (msg_id)
  {
    case QMI_UIM_REFRESH_IND_V01:
      if(ind_data->refresh_ind.refresh_event.files_ptr)
      {
        qcril_free(ind_data->refresh_ind.refresh_event.files_ptr);
        ind_data->refresh_ind.refresh_event.files_ptr = NULL;
      }
      break;
    case QMI_UIM_SEND_APDU_IND_V01:
      if(ind_data->send_apdu_ind.apdu.data_ptr)
      {
        qcril_free(ind_data->send_apdu_ind.apdu.data_ptr);
        ind_data->send_apdu_ind.apdu.data_ptr = NULL;
      }
      break;
    default:
      /* Nothing to do */
      break;
  }

  qcril_free(ind_data);
} /* qmi_client_indication_cb */


/*===========================================================================
  FUNCTION  qcril_uim_qmi_free_data
===========================================================================*/
static void qcril_uim_qmi_free_data
(
  unsigned long                 msg_id,
  qmi_uim_rsp_data_type       * rsp_data
)
{
  /* Free allocated data based on id */
  switch (msg_id)
  {
    case QMI_UIM_READ_TRANSPARENT_RESP_V01:
      if (rsp_data->rsp_data.read_transparent_rsp.content.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.read_transparent_rsp.content.data_ptr);
        rsp_data->rsp_data.read_transparent_rsp.content.data_ptr = NULL;
      }
      break;
    case QMI_UIM_READ_RECORD_RESP_V01:
      if (rsp_data->rsp_data.read_record_rsp.content.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.read_record_rsp.content.data_ptr);
        rsp_data->rsp_data.read_record_rsp.content.data_ptr = NULL;
      }
      break;
    case QMI_UIM_GET_FILE_ATTRIBUTES_RESP_V01:
      if (rsp_data->rsp_data.get_file_attributes_rsp.raw_value.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.get_file_attributes_rsp.raw_value.data_ptr);
        rsp_data->rsp_data.get_file_attributes_rsp.raw_value.data_ptr = NULL;
      }
      break;
    case QMI_UIM_SET_PIN_PROTECTION_RESP_V01:
      if (rsp_data->rsp_data.set_pin_protection_rsp.encr_pin_data.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.set_pin_protection_rsp.encr_pin_data.data_ptr);
        rsp_data->rsp_data.set_pin_protection_rsp.encr_pin_data.data_ptr = NULL;
      }
      break;
    case QMI_UIM_VERIFY_PIN_RESP_V01:
      if (rsp_data->rsp_data.verify_pin_rsp.encr_pin_data.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.verify_pin_rsp.encr_pin_data.data_ptr);
        rsp_data->rsp_data.verify_pin_rsp.encr_pin_data.data_ptr = NULL;
      }
      break;
    case QMI_UIM_UNBLOCK_PIN_RESP_V01:
      if (rsp_data->rsp_data.unblock_pin_rsp.encr_pin_data.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.unblock_pin_rsp.encr_pin_data.data_ptr);
        rsp_data->rsp_data.unblock_pin_rsp.encr_pin_data.data_ptr = NULL;
      }
      break;
    case QMI_UIM_CHANGE_PIN_RESP_V01:
      if (rsp_data->rsp_data.change_pin_rsp.encr_pin_data.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.change_pin_rsp.encr_pin_data.data_ptr);
        rsp_data->rsp_data.change_pin_rsp.encr_pin_data.data_ptr = NULL;
      }
      break;
    case QMI_UIM_AUTHENTICATE_RESP_V01:
      if (rsp_data->rsp_data.authenticate_rsp.auth_response.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.authenticate_rsp.auth_response.data_ptr);
        rsp_data->rsp_data.authenticate_rsp.auth_response.data_ptr = NULL;
      }
      break;
    case QMI_UIM_SEND_APDU_RESP_V01:
      if (rsp_data->rsp_data.send_apdu_rsp.apdu_response.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.send_apdu_rsp.apdu_response.data_ptr);
        rsp_data->rsp_data.send_apdu_rsp.apdu_response.data_ptr = NULL;
      }
      break;
    case QMI_UIM_GET_ATR_RESP_V01:
      if (rsp_data->rsp_data.get_atr_rsp.atr_response.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.get_atr_rsp.atr_response.data_ptr);
        rsp_data->rsp_data.get_atr_rsp.atr_response.data_ptr = NULL;
      }
      break;
    case QMI_UIM_LOGICAL_CHANNEL_RESP_V01:
    case QMI_UIM_OPEN_LOGICAL_CHANNEL_RESP_V01:
      if (rsp_data->rsp_data.logical_channel_rsp.select_response.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.logical_channel_rsp.select_response.data_ptr);
        rsp_data->rsp_data.logical_channel_rsp.select_response.data_ptr = NULL;
      }
      break;
    case QMI_UIM_SEND_STATUS_RESP_V01:
      if (rsp_data->rsp_data.send_status_rsp.status_response.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.send_status_rsp.status_response.data_ptr);
        rsp_data->rsp_data.send_status_rsp.status_response.data_ptr = NULL;
      }
      break;
    case QMI_UIM_RESELECT_RESP_V01:
      if (rsp_data->rsp_data.reselect_rsp.select_response.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.reselect_rsp.select_response.data_ptr);
        rsp_data->rsp_data.reselect_rsp.select_response.data_ptr = NULL;
      }
      break;
    case QMI_UIM_SAP_REQUEST_RESP_V01:
      if (rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr);
        rsp_data->rsp_data.sap_response_rsp.sap_response.data_ptr = NULL;
      }
      break;
    case QMI_UIM_GBA_RESP_V01:
      if (rsp_data->rsp_data.gba_rsp.ks_naf.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.gba_rsp.ks_naf.data_ptr);
        rsp_data->rsp_data.gba_rsp.ks_naf.data_ptr = NULL;
      }
      if (rsp_data->rsp_data.gba_rsp.b_tid.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.gba_rsp.b_tid.data_ptr);
        rsp_data->rsp_data.gba_rsp.b_tid.data_ptr = NULL;
      }
      if (rsp_data->rsp_data.gba_rsp.key_lifetime.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.gba_rsp.key_lifetime.data_ptr);
        rsp_data->rsp_data.gba_rsp.key_lifetime.data_ptr = NULL;
      }
      break;
    case QMI_UIM_GET_GBA_IMPI_RESP_V01:
      if (rsp_data->rsp_data.gba_impi_rsp.impi.data_ptr)
      {
        qcril_free(rsp_data->rsp_data.gba_impi_rsp.impi.data_ptr);
        rsp_data->rsp_data.gba_impi_rsp.impi.data_ptr = NULL;
      }
      break;
    case QMI_UIM_TERMINAL_CAPABILITY_RESP_V01:
      {
        uint8_t        i = 0;

        for (i = 0; i < rsp_data->rsp_data.terminal_capability_rsp.num_tlvs && i < QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX; i++)
        {
          qcril_free(rsp_data->rsp_data.terminal_capability_rsp.tlvs[i].value.data_ptr);
          rsp_data->rsp_data.terminal_capability_rsp.tlvs[i].value.data_ptr = NULL;
        }
      }
      break;
    case QMI_UIM_REMOTE_UNLOCK_RESP_V01:
      if (rsp_data->rsp_data.remote_unlock_rsp.operation !=
          QMI_UIM_REMOTE_UNLOCK_GET_MAX_SUPPORTED_VERSION &&
          rsp_data->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_ptr != NULL)
      {
        qcril_free(rsp_data->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_ptr);
        rsp_data->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_ptr = NULL;
        rsp_data->rsp_data.remote_unlock_rsp.remote_unlock_rsp.data_len = 0;
      }
      break;
    default:
      QCRIL_LOG_INFO( "%s: requires no data to be freed", __FUNCTION__);
      break;
  }
} /* qcril_uim_qmi_free_data */


/*===========================================================================
  FUNCTION  qcril_uim_convert_msg_id
===========================================================================*/
/*!
@brief
  Converts message id to the response id.

@return
  Error codes.

@note

  - Dependencies
    - None.

  - Side Effects
    - None.
*/
/*=========================================================================*/
static int qcril_uim_convert_msg_id
(
  unsigned long               msg_id,
  qmi_uim_rsp_id_type       * rsp_id
)
{
  switch (msg_id)
  {
    case QMI_UIM_READ_TRANSPARENT_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_READ_TRANSPARENT_RSP_MSG;
      break;

    case QMI_UIM_READ_RECORD_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_READ_RECORD_RSP_MSG;
      break;

    case QMI_UIM_WRITE_TRANSPARENT_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_WRITE_TRANSPARENT_RSP_MSG;
      break;

    case QMI_UIM_WRITE_RECORD_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_WRITE_RECORD_RSP_MSG;
      break;

    case QMI_UIM_GET_FILE_ATTRIBUTES_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_GET_FILE_ATTRIBUTES_RSP_MSG;
      break;

    case QMI_UIM_REFRESH_REGISTER_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_REFRESH_REGISTER_RSP_MSG;
      break;

    case QMI_UIM_SET_PIN_PROTECTION_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_SET_PIN_PROTECTION_RSP_MSG;
      break;

    case QMI_UIM_VERIFY_PIN_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_VERIFY_PIN_RSP_MSG;
      break;

    case QMI_UIM_UNBLOCK_PIN_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_UNBLOCK_PIN_RSP_MSG;
      break;

    case QMI_UIM_CHANGE_PIN_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_CHANGE_PIN_RSP_MSG;
      break;

    case QMI_UIM_DEPERSONALIZATION_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_DEPERSONALIZATION_RSP_MSG;
      break;

    case QMI_UIM_POWER_DOWN_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_POWER_DOWN_RSP_MSG;
      break;

    case QMI_UIM_POWER_UP_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_POWER_UP_RSP_MSG;
      break;

    case QMI_UIM_GET_CARD_STATUS_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_GET_CARD_STATUS_RSP_MSG;
      break;

    case QMI_UIM_AUTHENTICATE_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_AUTHENTICATE_RSP_MSG;
      break;

    case QMI_UIM_CLOSE_SESSION_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_CLOSE_SESSION_RSP_MSG;
      break;

    case QMI_UIM_GET_SERVICE_STATUS_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_GET_SERVICE_STATUS_RSP_MSG;
      break;

    case QMI_UIM_SET_SERVICE_STATUS_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_SET_SERVICE_STATUS_RSP_MSG;
      break;

    case QMI_UIM_CHANGE_PROVISIONING_SESSION_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_CHANGE_PROV_SESSION_RSP_MSG;
      break;

    case QMI_UIM_GET_LABEL_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_GET_LABEL_RSP_MSG;
      break;

    case QMI_UIM_SEND_APDU_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_SEND_APDU_RSP_MSG;
      break;

    case QMI_UIM_SAP_CONNECTION_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_SAP_CONNECTION_RSP_MSG;
      break;

    case QMI_UIM_SAP_REQUEST_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_SAP_REQUEST_RSP_MSG;
      break;

    case QMI_UIM_LOGICAL_CHANNEL_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_LOGICAL_CHANNEL_RSP_MSG;
      break;

    case QMI_UIM_OPEN_LOGICAL_CHANNEL_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_OPEN_LOGICAL_CHANNEL_RSP_MSG;
      break;

    case QMI_UIM_GET_ATR_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_GET_ATR_RSP_MSG;
      break;

    case QMI_UIM_SEND_STATUS_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_SEND_STATUS_RSP_MSG;
      break;

    case QMI_UIM_RESELECT_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_RESELECT_RSP_MSG;
      break;

    case QMI_UIM_SUPPLY_VOLTAGE_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_SUPPLY_VOLTAGE_RSP_MSG;
      break;

    case QMI_UIM_GET_CONFIGURATION_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_GET_CONFIGURATION_RSP_MSG;
      break;

    case QMI_UIM_GBA_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_GBA_RSP_MSG;
      break;

    case QMI_UIM_GET_GBA_IMPI_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_GBA_IMPI_RSP_MSG;
      break;

    case QMI_UIM_PERSONALIZATION_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_PERSONALIZATION_RSP_MSG;
      break;

    case QMI_UIM_GET_PHYSICAL_SLOT_MAPPING_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_GET_PHYSICAL_SLOT_MAPPING_RSP_MSG;
      break;

    case QMI_UIM_SWITCH_SLOT_EXT_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_SWITCH_SLOT_EXT_RSP_MSG;
      break;

    case QMI_UIM_SET_APDU_BEHAVIOR_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_SET_APDU_BEHAVIOR_RSP_MSG;
      break;

    case QMI_UIM_TERMINAL_CAPABILITY_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_TERMINAL_CAPABILITY_RSP_MSG;
      break;

    case QMI_UIM_REMOTE_UNLOCK_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_REMOTE_UNLOCK_RSP_MSG;
      break;

    case QMI_UIM_GET_SLOTS_STATUS_RESP_V01:
      *rsp_id = QMI_UIM_SRVC_GET_SLOTS_STATUS_RSP_MSG;
      break;

    default:
      return QMI_INTERNAL_ERR;
  }
  return QMI_NO_ERR;
} /* qcril_uim_convert_msg_id */


/*===========================================================================
  FUNCTION  qmi_uim_client_async_cb
===========================================================================*/
static void qmi_uim_client_async_cb
(
  qmi_client_type                client_handle,
  unsigned int                   msg_id,
  void                         * qmi_response,
  unsigned int                   resp_c_struct_len,
  void                         * resp_cb_data,
  qmi_client_error_type          transp_err
)
{
  qmi_uim_user_async_cb_type     user_cb;
  qmi_uim_rsp_data_type          rsp_data;

  QCRIL_NOTUSED(resp_c_struct_len);

  if(qmi_response == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  /* initialize qmi response data */
  memset((void*)&rsp_data, 0x0, sizeof(qmi_uim_rsp_data_type));

  QCRIL_LOG_INFO( "qmi_uim_client_async_cb: msg_id = 0x%x ", msg_id);

  if(resp_cb_data == NULL)
  {
    QCRIL_LOG_ERROR("%s: resp_cb_data was not valid", __FUNCTION__);
    return;
  }

  /* Update response ID */
  if(qcril_uim_convert_msg_id(msg_id, &rsp_data.rsp_id) < 0)
  {
    QCRIL_LOG_ERROR("Unknown async reply msg, msg_id=0x%x",msg_id);
  }

  if(transp_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI for RIL request", transp_err);
    rsp_data.sys_err_code = (int)transp_err;
    rsp_data.qmi_err_code = QMI_ERR_MESSAGE_DELIVERY_FAILURE_V01;
  }
  else
  {
    /* call the response conversion function based on message id */
    switch (msg_id)
    {
      case QMI_UIM_READ_TRANSPARENT_RESP_V01:
        qcril_uim_qmi_conv_read_transparent_resp((uim_read_transparent_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_READ_RECORD_RESP_V01:
        qcril_uim_qmi_conv_read_record_resp((uim_read_record_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_WRITE_TRANSPARENT_RESP_V01:
        qcril_uim_qmi_conv_write_transparent_resp((uim_write_transparent_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_WRITE_RECORD_RESP_V01:
        qcril_uim_qmi_conv_write_record_resp((uim_write_record_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_FILE_ATTRIBUTES_RESP_V01:
        qcril_uim_qmi_conv_file_attributes_resp((uim_get_file_attributes_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_REFRESH_REGISTER_RESP_V01:
        qcril_uim_qmi_conv_refresh_register_resp((uim_refresh_register_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_SET_PIN_PROTECTION_RESP_V01:
        qcril_uim_qmi_conv_set_pin_protection_resp((uim_set_pin_protection_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_VERIFY_PIN_RESP_V01:
        qcril_uim_qmi_conv_verify_pin_resp((uim_verify_pin_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_UNBLOCK_PIN_RESP_V01:
        qcril_uim_qmi_conv_unblock_pin_resp((uim_unblock_pin_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_CHANGE_PIN_RESP_V01:
        qcril_uim_qmi_conv_change_pin_resp((uim_change_pin_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_DEPERSONALIZATION_RESP_V01:
        qcril_uim_qmi_conv_depersonalization_resp((uim_depersonalization_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_CARD_STATUS_RESP_V01:
        qcril_uim_qmi_conv_get_card_status_resp((uim_get_card_status_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_POWER_DOWN_RESP_V01:
        qcril_uim_qmi_conv_power_down_resp((uim_power_down_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_POWER_UP_RESP_V01:
        qcril_uim_qmi_conv_power_up_resp((uim_power_up_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_CLOSE_SESSION_RESP_V01:
        qcril_uim_qmi_conv_close_session_resp((uim_close_session_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_AUTHENTICATE_RESP_V01:
        qcril_uim_qmi_conv_authenticate_resp((uim_authenticate_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_SERVICE_STATUS_RESP_V01:
        qcril_uim_qmi_conv_get_service_status_resp((uim_get_service_status_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_SET_SERVICE_STATUS_RESP_V01:
        qcril_uim_qmi_conv_set_service_status_resp((uim_set_service_status_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_CHANGE_PROVISIONING_SESSION_RESP_V01:
        qcril_uim_qmi_conv_change_provisioning_session_resp((uim_change_provisioning_session_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_LABEL_RESP_V01:
        qcril_uim_qmi_conv_get_label_resp((uim_get_label_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_SEND_APDU_RESP_V01:
        qcril_uim_qmi_conv_send_apdu_resp((uim_send_apdu_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_LOGICAL_CHANNEL_RESP_V01:
        qcril_uim_qmi_conv_logical_channel_resp((uim_logical_channel_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_OPEN_LOGICAL_CHANNEL_RESP_V01:
        qcril_uim_qmi_conv_open_logical_channel_resp((uim_open_logical_channel_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_ATR_RESP_V01:
        qcril_uim_qmi_conv_get_atr_resp((uim_get_atr_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_SEND_STATUS_RESP_V01:
        qcril_uim_qmi_conv_send_status_resp((uim_send_status_cmd_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_RESELECT_RESP_V01:
        qcril_uim_qmi_conv_reselect_resp((uim_reselect_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_SUPPLY_VOLTAGE_RESP_V01:
        qcril_uim_qmi_conv_supply_voltage_resp((uim_supply_voltage_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_CONFIGURATION_RESP_V01:
        qcril_uim_qmi_conv_get_configuration_resp((uim_get_configuration_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_SAP_CONNECTION_RESP_V01:
        qcril_uim_qmi_conv_sap_connection_resp((uim_sap_connection_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_SAP_REQUEST_RESP_V01:
        qcril_uim_qmi_conv_sap_request_resp((uim_sap_request_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GBA_RESP_V01:
        qcril_uim_qmi_conv_gba_request_resp((uim_gba_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_GBA_IMPI_RESP_V01:
        qcril_uim_qmi_conv_gba_imsi_resp((uim_get_gba_impi_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_PERSONALIZATION_RESP_V01:
        qcril_uim_qmi_conv_personalization_resp((uim_personalization_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_PHYSICAL_SLOT_MAPPING_RESP_V01:
        qcril_uim_qmi_get_physical_slot_mapping_resp((uim_get_physical_slot_mapping_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_SWITCH_SLOT_EXT_RESP_V01:
        qcril_uim_qmi_switch_slot_ext_resp((uim_switch_slot_ext_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_SLOTS_STATUS_RESP_V01:
        qcril_uim_qmi_get_slots_status_resp((uim_get_slots_status_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_TERMINAL_CAPABILITY_RESP_V01:
        qcril_uim_qmi_terminal_capability_resp((uim_terminal_capability_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_REMOTE_UNLOCK_RESP_V01:
        qcril_uim_qmi_remote_unlock_resp((uim_remote_unlock_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_ADD_PROFILE_RESP_V01:
        qmi_uim_add_profile_resp((uim_add_profile_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_SIM_PROFILE_RESP_V01:
        qmi_uim_get_profiles_resp((uim_get_sim_profile_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_SET_SIM_PROFILE_RESP_V01:
        qmi_uim_set_sim_profile_resp((uim_set_sim_profile_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_DELETE_PROFILE_RESP_V01:
        qmi_uim_delete_profile_resp((uim_delete_profile_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_EUICC_MEMORY_RESET_RESP_V01:
        qmi_uim_euicc_memory_reset_resp((uim_euicc_memory_reset_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_UPDATE_PROFILE_NICKNAME_RESP_V01:
        qmi_uim_update_profile_nickname_resp((uim_update_profile_nickname_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_EID_RESP_V01:
        qmi_uim_get_eid_resp((uim_get_eid_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_PROFILE_USER_CONSENT_RESP_V01:
        qmi_uim_user_consent_resp((uim_profile_user_consent_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_GET_PROFILE_INFO_RESP_V01:
        qmi_uim_get_profile_info_resp((uim_get_profile_info_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      case QMI_UIM_EUICC_DEFAULT_SERVER_ADDRESS_RESP_V01:
        qmi_uim_get_set_server_address_resp((uim_euicc_default_server_address_resp_msg_v01*)qmi_response, &rsp_data);
        break;
      default:
        QCRIL_LOG_ERROR("qmi_uim_srvc_async_cb: unknown rsp ID = %x", (unsigned int)rsp_data.rsp_id);
        break;
    }
  }

  user_cb = ((qmi_uim_cb_struct_type*)resp_cb_data)->user_cb;

  if(user_cb != NULL && client_handle != NULL)
  {
    /* Call the user callback */
    user_cb ( &rsp_data,
              ((qmi_uim_cb_struct_type*)resp_cb_data)->user_data);
  }

  /* Free up any memory allocated */
  qcril_free(resp_cb_data);
  resp_cb_data = NULL;
  qcril_uim_qmi_free_data(msg_id, &rsp_data);
  qcril_free(qmi_response);
} /* qmi_uim_client_async_cb */


/*---------------------------------------------------------------------------
   Command handling functions from the Client
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  qcril_qmi_uim_reset
===========================================================================*/
/*!
@brief
  Resets UIM service.  If the user_cb function pointer is set to NULL,
  then this function will be invoked synchronously, otherwise it will
  be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value in rsp_data will give you the QMI error reason.
  Otherwise, qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - Resets UIM service
*/
/*=========================================================================*/
int qcril_qmi_uim_reset
(
  qmi_client_type               client_handle,
  qmi_uim_user_async_cb_type    user_cb,
  void                        * user_data,
  int                         * qmi_err_code
)
{
  int                             rc;

  qmi_txn_handle                  txn_handle;
  uim_reset_resp_msg_v01        * qmi_response  = NULL;
  qmi_uim_cb_struct_type        * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if(qmi_err_code == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  /* Check before reinitializing client_handle with new handle */
  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_response = (uim_reset_resp_msg_v01*)qcril_malloc(sizeof(uim_reset_resp_msg_v01));
  if(qmi_response == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  memset(qmi_response, 0, sizeof(uim_reset_resp_msg_v01));

  if (user_cb)
  {
    *qmi_err_code = QMI_SERVICE_ERR_NONE;
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_RESET_REQ_V01,
                                    NULL,
                                    0,
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free(qmi_response);
        qcril_free(cb_data);
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_RESET_REQ_V01,
                                   NULL,
                                   0,
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    qcril_free(qmi_response);
  }
  return rc;
} /* qcril_qmi_uim_reset */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_read_transparent
===========================================================================*/
/*!
@brief
  Reads a transparent file from the card.  If the user_cb function
  pointer is set to NULL, this function will be invoked synchronously.
  Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value in rsp_data will give you the QMI error reason.
  Otherwise, qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_read_transparent
(
  qmi_client_type                               client_handle,
  const qmi_uim_read_transparent_params_type  * params,
  qmi_uim_user_async_cb_type                    user_cb,
  void                                        * user_data,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  int                                   rc;

  qmi_txn_handle                        txn_handle;
  uim_read_transparent_req_msg_v01    * qmi_request   = NULL;
  uim_read_transparent_resp_msg_v01   * qmi_response  = NULL;
  qmi_uim_cb_struct_type              * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  /* Check before reinitializing client_handle with new handle */
  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if((params->file_id.path.data_len > QMI_UIM_PATH_MAX_V01) || (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_read_transparent_req_msg_v01*)qcril_malloc(sizeof(uim_read_transparent_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_read_transparent_resp_msg_v01*)qcril_malloc(sizeof(uim_read_transparent_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_read_transparent_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_read_transparent_resp_msg_v01));

  /*set read transparent request params*/
  qmi_request->read_transparent.length = (uint16_t)params->length;
  qmi_request->read_transparent.offset = (uint16_t)params->offset;

  /*set file information*/
  qmi_request->file_id.file_id = (uint16_t)params->file_id.file_id;
  qmi_request->file_id.path_len = (uint32_t)(params->file_id.path.data_len);
  if(qcril_uim_qmi_flip_data(qmi_request->file_id.path,
                             (uint8_t*)params->file_id.path.data_ptr,
                             params->file_id.path.data_len) < 0)
  {
    qcril_free(qmi_request);
    qcril_free(qmi_response);
    return QMI_SERVICE_ERR;
  }

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  /* Set encryption TLV if needed */
  if(params->encrypt_data)
  {
    qmi_request->encryption_valid = 1;
    qmi_request->encryption = 1;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_READ_TRANSPARENT_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_READ_TRANSPARENT_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_read_transparent_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);
  return rc;
} /* qcril_qmi_uim_read_transparent */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_read_record
===========================================================================*/
/*!
@brief
  Read a record from a linear/cyclic file from the card.
  If the user_cb function pointer is set to NULL, this function will
  be invoked synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value in rsp_data will give you the QMI error reason.
  Otherwise, qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_read_record
(
  qmi_client_type                               client_handle,
  const qmi_uim_read_record_params_type       * params,
  qmi_uim_user_async_cb_type                    user_cb,
  void                                        * user_data,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  int                             rc;

  qmi_txn_handle                  txn_handle;
  uim_read_record_req_msg_v01   * qmi_request   = NULL;
  uim_read_record_resp_msg_v01  * qmi_response  = NULL;
  qmi_uim_cb_struct_type        * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if((params->file_id.path.data_len > QMI_UIM_PATH_MAX_V01) || (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_read_record_req_msg_v01*)qcril_malloc(sizeof(uim_read_record_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_read_record_resp_msg_v01*)qcril_malloc(sizeof(uim_read_record_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_read_record_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_read_record_resp_msg_v01));

  /*set read record request parameters*/
  qmi_request->read_record.length = (uint16_t)(params->length);
  qmi_request->read_record.record = (uint16_t)(params->record);

  /*set file information*/
  qmi_request->file_id.file_id = (uint16_t)(params->file_id.file_id);
  qmi_request->file_id.path_len = (uint32_t)(params->file_id.path.data_len);
  if(qcril_uim_qmi_flip_data(qmi_request->file_id.path,
                             params->file_id.path.data_ptr,
                             params->file_id.path.data_len) < 0)
  {
    qcril_free(qmi_request);
    qcril_free(qmi_response);
    return QMI_SERVICE_ERR;
  }

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_READ_RECORD_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_READ_RECORD_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_read_record_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);
  return rc;
} /* qcril_qmi_uim_read_record */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_write_transparent
===========================================================================*/
/*!
@brief
  Writes a transparent file to the card.  If the user_cb function
  pointer is set to NULL, this function will be invoked synchronously.
  Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_write_transparent
(
  qmi_client_type                               client_handle,
  const qmi_uim_write_transparent_params_type * params,
  qmi_uim_user_async_cb_type                    user_cb,
  void                                        * user_data,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  int                                   rc;

  qmi_txn_handle                        txn_handle;
  uim_write_transparent_req_msg_v01   * qmi_request   = NULL;
  uim_write_transparent_resp_msg_v01  * qmi_response  = NULL;
  qmi_uim_cb_struct_type              * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if((params->data.data_len > QMI_UIM_CONTENT_TRANSPARENT_MAX_V01) ||
     (params->file_id.path.data_len > QMI_UIM_PATH_MAX_V01) ||
     (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_write_transparent_req_msg_v01*)qcril_malloc(sizeof(uim_write_transparent_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_write_transparent_resp_msg_v01*)qcril_malloc(sizeof(uim_write_transparent_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_write_transparent_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_write_transparent_resp_msg_v01));

  /*set write transparent request params*/
  qmi_request->write_transparent.offset = (uint16_t)params->offset;
  qmi_request->write_transparent.data_len = (uint32_t)params->data.data_len;
  memcpy(qmi_request->write_transparent.data, params->data.data_ptr, params->data.data_len);

  /*set file information*/
  qmi_request->file_id.file_id = (uint16_t)params->file_id.file_id;
  qmi_request->file_id.path_len = (uint32_t)(params->file_id.path.data_len);
  if(qcril_uim_qmi_flip_data(qmi_request->file_id.path,
                             (uint8_t*)(params->file_id.path.data_ptr),
                             (uint32_t)(params->file_id.path.data_len)) < 0)
  {
    qcril_free(qmi_request);
    qcril_free(qmi_response);
    return QMI_SERVICE_ERR;
  }

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_WRITE_TRANSPARENT_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_WRITE_TRANSPARENT_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_write_transparent_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);
  return rc;
} /* qcril_qmi_uim_write_transparent */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_write_record
===========================================================================*/
/*!
@brief
  Writes a record to a linear/cyclic file from the card.  If the user_cb
  function pointer is set to NULL, this function will be invoked synchronously.
  Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_write_record
(
  qmi_client_type                               client_handle,
  const qmi_uim_write_record_params_type      * params,
  qmi_uim_user_async_cb_type                    user_cb,
  void                                        * user_data,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  int                                rc;

  qmi_txn_handle                     txn_handle;
  uim_write_record_req_msg_v01     * qmi_request   = NULL;
  uim_write_record_resp_msg_v01    * qmi_response  = NULL;
  qmi_uim_cb_struct_type           * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if((params->data.data_len > QMI_UIM_CONTENT_RECORD_MAX_V01) ||
     (params->file_id.path.data_len > QMI_UIM_PATH_MAX_V01) ||
     (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_write_record_req_msg_v01*)qcril_malloc(sizeof(uim_write_record_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_write_record_resp_msg_v01*)qcril_malloc(sizeof(uim_write_record_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_write_record_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_write_record_resp_msg_v01));

  /*set write record request params*/
  qmi_request->write_record.record = (uint16_t)params->record;
  qmi_request->write_record.data_len = (uint32_t)params->data.data_len;
  memcpy(qmi_request->write_record.data, params->data.data_ptr, params->data.data_len);

  /*set file information*/
  qmi_request->file_id.file_id = (uint16_t)params->file_id.file_id;
  qmi_request->file_id.path_len = (uint32_t)(params->file_id.path.data_len);
  if(qcril_uim_qmi_flip_data(qmi_request->file_id.path,
                             (uint8_t*)(params->file_id.path.data_ptr),
                             (uint32_t)(params->file_id.path.data_len)) < 0)
  {
    qcril_free(qmi_request);
    qcril_free(qmi_response);
    return QMI_SERVICE_ERR;
  }

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_WRITE_RECORD_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_WRITE_RECORD_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_write_record_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);
  return rc;
} /* qcril_qmi_uim_write_record */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_file_attributes
===========================================================================*/
/*!
@brief
  Gets the file Attributes for a file on the card.  If the user_cb
  function pointer is set to NULL, this function will be invoked synchronously.
  Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_get_file_attributes
(
  qmi_client_type                                     client_handle,
  const qmi_uim_get_file_attributes_params_type     * params,
  qmi_uim_user_async_cb_type                          user_cb,
  void                                              * user_data,
  qmi_uim_rsp_data_type                             * rsp_data
)
{
  int                                      rc;

  qmi_txn_handle                           txn_handle;
  uim_get_file_attributes_req_msg_v01    * qmi_request    = NULL;
  uim_get_file_attributes_resp_msg_v01   * qmi_response   = NULL;
  qmi_uim_cb_struct_type                 * cb_data        = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if((params->file_id.path.data_len > QMI_UIM_PATH_MAX_V01) || (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_get_file_attributes_req_msg_v01*)qcril_malloc(sizeof(uim_get_file_attributes_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_get_file_attributes_resp_msg_v01*)qcril_malloc(sizeof(uim_get_file_attributes_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_get_file_attributes_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_get_file_attributes_resp_msg_v01));

  /*set file attributes request params*/
  /*set file information*/
  qmi_request->file_id.file_id = (uint16_t)(params->file_id.file_id);
  qmi_request->file_id.path_len = (uint32_t)(params->file_id.path.data_len);
  if(qcril_uim_qmi_flip_data(qmi_request->file_id.path,
                             (uint8_t*)(params->file_id.path.data_ptr),
                             (uint32_t)(params->file_id.path.data_len)) < 0)
  {
    qcril_free(qmi_request);
    qcril_free(qmi_response);
    return QMI_SERVICE_ERR;
  }

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)(params->session_info.session_type);
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_GET_FILE_ATTRIBUTES_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_GET_FILE_ATTRIBUTES_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_file_attributes_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);
  return rc;
} /* qcril_qmi_uim_get_file_attributes */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_refresh_register
===========================================================================*/
/*!
@brief
  Registers for file change notifications triggered by the card.
  This function is supported only synchronously.

@return
  If return code < 0, the operation failed. In the failure case, if the
  return code is QMI_SERVICE_ERR, then the qmi_err_code value will give you
  the QMI error reason.  Otherwise, qmi_err_code will have meaningless data.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_refresh_register
(
  qmi_client_type                                   client_handle,
  const qmi_uim_refresh_register_params_type      * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                   rc;
  int                                   i = 0;
  qmi_txn_handle                        txn_handle;
  uim_refresh_register_req_msg_v01    * qmi_request    = NULL;
  uim_refresh_register_resp_msg_v01   * qmi_response   = NULL;
  qmi_uim_cb_struct_type              * cb_data        = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if((params->num_files > QMI_UIM_REFRESH_FILES_MAX_V01) || (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_refresh_register_req_msg_v01*)qcril_malloc(sizeof(uim_refresh_register_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_refresh_register_resp_msg_v01*)qcril_malloc(sizeof(uim_refresh_register_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_refresh_register_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_refresh_register_resp_msg_v01));

  /*set refresh register, vote initialization and file information*/
  qmi_request->register_refresh.register_flag = (uint8_t)(params->reg_for_refresh);
  qmi_request->register_refresh.vote_for_init = (uint8_t)(params->vote_for_init);
  qmi_request->register_refresh.files_len = (uint32_t)(params->num_files);

  /*set file information*/
  for(i = 0; i < params->num_files; i++)
  {
    qmi_request->register_refresh.files[i].file_id = (uint16_t)(params->files_ptr[i].file_id);
    qmi_request->register_refresh.files[i].path_len = (uint32_t)(params->files_ptr[i].path.data_len);
    if((params->files_ptr[i].path.data_len) > QMI_UIM_PATH_MAX_V01)
    {
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_INTERNAL_ERR;
    }
    if(qcril_uim_qmi_flip_data(qmi_request->register_refresh.files[i].path,
                               (uint8_t*)(params->files_ptr[i].path.data_ptr),
                               (uint32_t)(params->files_ptr[i].path.data_len)) < 0)
    {
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_INTERNAL_ERR;
    }
  }

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)(params->session_info.session_type);
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_REFRESH_REGISTER_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_REFRESH_REGISTER_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
  if(!rc)
    {
      qcril_uim_qmi_conv_refresh_register_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);
  return rc;
} /* qcril_qmi_uim_refresh_register */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_refresh_ok
===========================================================================*/
/*!
@brief
  Enables the client to indicate if it is OK to start the refresh procedure.
  This function is supported only synchronously.

@return
  If return code < 0, the operation failed. In the failure case, if the
  return code is QMI_SERVICE_ERR, then the qmi_err_code value will give you
  the QMI error reason.  Otherwise, qmi_err_code will have meaningless data.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_refresh_ok
(
  qmi_client_type                                   client_handle,
  const qmi_uim_refresh_ok_params_type            * params,
  qmi_uim_rsp_data_type                           * rsp_data
)
{

  int                                   rc;

  uim_refresh_ok_req_msg_v01          * qmi_request   = NULL;
  uim_refresh_ok_resp_msg_v01         * qmi_response  = NULL;

  QCRIL_NOTUSED(client_handle);

  if (params == nullptr || rsp_data == nullptr)
  {
    return QMI_SERVICE_ERR;
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_refresh_ok_req_msg_v01*)qcril_malloc(sizeof(uim_refresh_ok_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_refresh_ok_resp_msg_v01*)qcril_malloc(sizeof(uim_refresh_ok_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_refresh_ok_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_refresh_ok_resp_msg_v01));

  /*set if ok to refresh*/
  qmi_request->ok_to_refresh = (uint8_t)params->ok_to_refresh;

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  rc = qmi_client_send_msg_sync( client_handle,
                                 QMI_UIM_REFRESH_OK_REQ_V01,
                                 (void*) qmi_request,
                                 sizeof(*qmi_request),
                                 (void*) qmi_response,
                                 sizeof(*qmi_response),
                                 QMI_UIM_DEFAULT_TIMEOUT);

  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
  qcril_free(qmi_request);
  qcril_free(qmi_response);
  return rc;
} /* qcril_qmi_uim_refresh_ok */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_refresh_complete
===========================================================================*/
/*!
@brief
  Indicates to the modem that the client has finished the refresh procedure
  after re-reading all the cached files. This function is supported only
  synchronously.

@return
  If return code < 0, the operation failed. In the failure case, if the
  return code is QMI_SERVICE_ERR, then the qmi_err_code value will give you
  the QMI error reason.  Otherwise, qmi_err_code will have meaningless data.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_refresh_complete
(
  qmi_client_type                                   client_handle,
  const qmi_uim_refresh_complete_params_type      * params,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                       rc;

  uim_refresh_complete_req_msg_v01        * qmi_request   = NULL;
  uim_refresh_complete_resp_msg_v01       * qmi_response  = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params || !rsp_data)
  {
    return QMI_SERVICE_ERR;
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_refresh_complete_req_msg_v01*)qcril_malloc(sizeof(uim_refresh_complete_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_refresh_complete_resp_msg_v01*)qcril_malloc(sizeof(uim_refresh_complete_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_refresh_complete_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_refresh_complete_resp_msg_v01));

  /*set if refresh is successful*/
  qmi_request->refresh_success = (uint8_t)params->refresh_success;

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  rc = qmi_client_send_msg_sync( client_handle,
                                 QMI_UIM_REFRESH_COMPLETE_REQ_V01,
                                 (void*) qmi_request,
                                 sizeof(*qmi_request),
                                 (void*) qmi_response,
                                 sizeof(*qmi_response),
                                 QMI_UIM_DEFAULT_TIMEOUT);

  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
  qcril_free(qmi_request);
  qcril_free(qmi_response);

  return rc;
} /* qcril_qmi_uim_refresh_complete */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_refresh_get_last_event
===========================================================================*/
/*!
@brief
  Retreives the latest refresh event. This function is supported only
  synchronously.

@return
  If return code < 0, the operation failed. In the failure case, if the
  return code is QMI_SERVICE_ERR, then the qmi_err_code value will give you
  the QMI error reason.  Otherwise, qmi_err_code will have meaningless data.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_refresh_get_last_event
(
  qmi_client_type                                     client_handle,
  const qmi_uim_refresh_get_last_event_params_type  * params,
  qmi_uim_rsp_data_type                             * rsp_data
)
{
  int                                             rc;

  unsigned int                                    i             = 0;
  uim_refresh_get_last_event_req_msg_v01        * qmi_request   = NULL;
  uim_refresh_get_last_event_resp_msg_v01       * qmi_response  = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->session_info.aid.data_len > QMI_UIM_MAX_AID_LEN)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_refresh_get_last_event_req_msg_v01*)qcril_malloc(sizeof(uim_refresh_get_last_event_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_refresh_get_last_event_resp_msg_v01*)qcril_malloc(sizeof(uim_refresh_get_last_event_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_refresh_get_last_event_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_refresh_get_last_event_resp_msg_v01));

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = 0;
  memset(qmi_request->session_information.aid, 0, 32);
  if((params->session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1) ||
     (params->session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2) ||
     (params->session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_3))
  {
    qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
    memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);
  }

  rc = qmi_client_send_msg_sync( client_handle,
                                 QMI_UIM_REFRESH_GET_LAST_EVENT_REQ_V01,
                                 (void*) qmi_request,
                                 sizeof(*qmi_request),
                                 (void*) qmi_response,
                                 sizeof(*qmi_response),
                                 QMI_UIM_DEFAULT_TIMEOUT);

  qcril_free(qmi_request);
  if((qmi_response->refresh_event.aid_len > QMI_UIM_AID_MAX_V01) ||
      (qmi_response->refresh_event.files_len > QMI_UIM_REFRESH_FILES_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    qcril_free(qmi_response);
    return QMI_INTERNAL_ERR;
  }
  rsp_data->rsp_id = QMI_UIM_SRVC_REFRESH_GET_LAST_EVENT_RSP_MSG;
  if(qmi_response->resp.result == QMI_RESULT_SUCCESS_V01)
  {
    rsp_data->qmi_err_code = QMI_NO_ERR;
    if(qmi_response->refresh_event_valid == 1)
    {
      rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.refresh_stage =
        (qmi_uim_refresh_stage)qmi_response->refresh_event.stage;
      rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.refresh_mode =
        (qmi_uim_refresh_mode)qmi_response->refresh_event.mode;
      rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.session_type =
        (qmi_uim_session_type)qmi_response->refresh_event.session_type;
      rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.aid_len = 0;
      memset(rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.aid_value, 0, 32);
      rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.aid_len =
        (unsigned char)qmi_response->refresh_event.aid_len;
      memcpy(rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.aid_value,
             qmi_response->refresh_event.aid,
             qmi_response->refresh_event.aid_len);

      rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.num_files = (unsigned short)(qmi_response->refresh_event.files_len);
      rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.files_ptr =
                                        (qmi_uim_refresh_file_id_type *)qcril_malloc(qmi_response->refresh_event.files_len);
      if(rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.files_ptr == NULL)
      {
        QCRIL_LOG_ERROR("%s", "data pointer NULL");
        qcril_free(qmi_response);
        return QMI_INTERNAL_ERR;
      }
      /* in sync calls this is freed by client */
      memcpy(rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.files_ptr,
             (qmi_uim_refresh_file_id_type*)(qmi_response->refresh_event.files),
             qmi_response->refresh_event.files_len);
      for(i = 0; (i < qmi_response->refresh_event.files_len); i++)
      {
        if(qmi_response->refresh_event.files[i].path_len > QMI_UIM_PATH_MAX_V01)
        {
          QCRIL_LOG_ERROR("%s", "data length too long");
          qcril_free(qmi_response);
          return QMI_INTERNAL_ERR;
        }
        rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.files_ptr->file_id =
          (unsigned short)(qmi_response->refresh_event.files[i].file_id);
        rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.files_ptr->path_len =
          (unsigned char)(qmi_response->refresh_event.files[i].path_len);
        memcpy(rsp_data->rsp_data.refresh_get_last_event_rsp.refresh_event.files_ptr->path_value,
               qmi_response->refresh_event.files[i].path,
               qmi_response->refresh_event.files[i].path_len);
      }
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "response error: 0x%x", qmi_response->resp.error);
    rsp_data->qmi_err_code = (int)qmi_response->resp.error;
  }
  qcril_free(qmi_response);

  return rc;
} /* qcril_qmi_uim_refresh_get_last_event */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_set_pin_protection
===========================================================================*/
/*!
@brief
  Enables or disables specified pin on the card.  If the user_cb function
  pointer is set to NULL, this function will be invoked synchronously.
  Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_set_pin_protection
(
  qmi_client_type                                   client_handle,
  const qmi_uim_set_pin_protection_params_type    * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                       rc;
  qmi_txn_handle                            txn_handle;
  uim_set_pin_protection_req_msg_v01      * qmi_request   = NULL;
  uim_set_pin_protection_resp_msg_v01     * qmi_response  = NULL;
  qmi_uim_cb_struct_type                  * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if((params->pin_data.data_len > QMI_UIM_PIN_MAX_V01) || (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_set_pin_protection_req_msg_v01*)qcril_malloc(sizeof(uim_set_pin_protection_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_set_pin_protection_resp_msg_v01*)qcril_malloc(sizeof(uim_set_pin_protection_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_set_pin_protection_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_set_pin_protection_resp_msg_v01));

  /*set pin request params*/
  qmi_request->set_pin_protection.pin_id = (uim_pin_id_enum_v01)params->pin_id;
  qmi_request->set_pin_protection.pin_operation = (uim_pin_operation_enum_v01)params->pin_operation;
  qmi_request->set_pin_protection.pin_value_len = (uint32_t)params->pin_data.data_len;
  memcpy(qmi_request->set_pin_protection.pin_value, params->pin_data.data_ptr, params->pin_data.data_len);

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_SET_PIN_PROTECTION_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_SET_PIN_PROTECTION_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_set_pin_protection_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_set_pin_protection */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_verify_pin
===========================================================================*/
/*!
@brief
  Verifies the specified pin on the card.  If the user_cb function
  pointer is set to NULL, this function will be invoked synchronously.
  Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_verify_pin
(
  qmi_client_type                                     client_handle,
  const qmi_uim_verify_pin_params_type              * params,
  qmi_uim_user_async_cb_type                          user_cb,
  void                                              * user_data,
  qmi_uim_rsp_data_type                             * rsp_data
)
{
  int                                   rc;
  qmi_txn_handle                        txn_handle;
  uim_verify_pin_req_msg_v01          * qmi_request   = NULL;
  uim_verify_pin_resp_msg_v01         * qmi_response  = NULL;
  qmi_uim_cb_struct_type              * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(((!params->is_pin1_encrypted) && (params->pin_data.data_len > QMI_UIM_PIN_MAX_V01)) ||
     (params->is_pin1_encrypted && (params->pin_data.data_len > QMI_UIM_ENCRYPTED_PIN_MAX_V01)) ||
     (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_verify_pin_req_msg_v01*)qcril_malloc(sizeof(uim_verify_pin_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_verify_pin_resp_msg_v01*)qcril_malloc(sizeof(uim_verify_pin_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_verify_pin_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_verify_pin_resp_msg_v01));

  /*set encrypted pin request params*/
  if(params->is_pin1_encrypted)
  {
    qmi_request->encrypted_pin1_valid = 1;
    qmi_request->verify_pin.pin_id = (uim_pin_id_enum_v01)params->pin_id;
    qmi_request->encrypted_pin1.enc_pin1_value_len = (uint32_t)params->pin_data.data_len;
    qmi_request->verify_pin.pin_value_len = 0;
    memcpy(qmi_request->encrypted_pin1.enc_pin1_value, params->pin_data.data_ptr, params->pin_data.data_len);
  }
  /*set verify pin request params*/
  else
  {
    qmi_request->encrypted_pin1_valid = 0;
    qmi_request->verify_pin.pin_id = (uim_pin_id_enum_v01)params->pin_id;
    qmi_request->encrypted_pin1.enc_pin1_value_len = 0;
    qmi_request->verify_pin.pin_value_len = (uint32_t)params->pin_data.data_len;
    memcpy(qmi_request->verify_pin.pin_value, params->pin_data.data_ptr, params->pin_data.data_len);
  }

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_VERIFY_PIN_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_VERIFY_PIN_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_verify_pin_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_verify_pin */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_unblock_pin
===========================================================================*/
/*!
@brief
  Unblocks the specified pin on the card.  If the user_cb function
  pointer is set to NULL, this function will be invoked synchronously.
  Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_unblock_pin
(
  qmi_client_type                                     client_handle,
  const qmi_uim_unblock_pin_params_type             * params,
  qmi_uim_user_async_cb_type                          user_cb,
  void                                              * user_data,
  qmi_uim_rsp_data_type                             * rsp_data
)
{
  int                                   rc;
  qmi_txn_handle                        txn_handle;
  uim_unblock_pin_req_msg_v01         * qmi_request   = NULL;
  uim_unblock_pin_resp_msg_v01        * qmi_response  = NULL;
  qmi_uim_cb_struct_type              * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if((params->puk_data.data_len > QMI_UIM_PIN_MAX_V01) ||
     (params->new_pin_data.data_len > QMI_UIM_PIN_MAX_V01) ||
     (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_unblock_pin_req_msg_v01*)qcril_malloc(sizeof(uim_unblock_pin_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_unblock_pin_resp_msg_v01*)qcril_malloc(sizeof(uim_unblock_pin_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_unblock_pin_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_unblock_pin_resp_msg_v01));

  /*set unblock puk request params*/
  qmi_request->unblock_pin.pin_id = (uim_pin_id_enum_v01)params->pin_id;
  qmi_request->unblock_pin.puk_value_len = (uint32_t)params->puk_data.data_len;
  memcpy(qmi_request->unblock_pin.puk_value, params->puk_data.data_ptr, params->puk_data.data_len);

  /*set unblock new pin request params*/
  qmi_request->unblock_pin.new_pin_value_len = (uint32_t)params->new_pin_data.data_len;
  memcpy(qmi_request->unblock_pin.new_pin_value, params->new_pin_data.data_ptr, params->new_pin_data.data_len);

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)(params->session_info.session_type);
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_UNBLOCK_PIN_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_UNBLOCK_PIN_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_unblock_pin_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_unblock_pin */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_change_pin
===========================================================================*/
/*!
@brief
  Changes the specified pin on the card.  If the user_cb function
  pointer is set to NULL, this function will be invoked synchronously.
  Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_change_pin
(
  qmi_client_type                                   client_handle,
  const qmi_uim_change_pin_params_type            * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                 rc;
  qmi_txn_handle                      txn_handle;
  uim_change_pin_req_msg_v01        * qmi_request   =  NULL;
  uim_change_pin_resp_msg_v01       * qmi_response  =  NULL;
  qmi_uim_cb_struct_type            * cb_data       =  NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if((params->old_pin_data.data_len > QMI_UIM_PIN_MAX_V01) ||
     (params->new_pin_data.data_len > QMI_UIM_PIN_MAX_V01) ||
     (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_change_pin_req_msg_v01*)qcril_malloc(sizeof(uim_change_pin_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_change_pin_resp_msg_v01*)qcril_malloc(sizeof(uim_change_pin_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_change_pin_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_change_pin_resp_msg_v01));

  /*set change old pin request params*/
  qmi_request->change_pin.pin_id = (uim_pin_id_enum_v01)params->pin_id;
  qmi_request->change_pin.old_pin_value_len = (uint32_t)params->old_pin_data.data_len;
  memcpy(qmi_request->change_pin.old_pin_value, params->old_pin_data.data_ptr, params->old_pin_data.data_len);

  /*set change new pin request params*/
  qmi_request->change_pin.new_pin_value_len = (uint32_t)params->new_pin_data.data_len;
  memcpy(qmi_request->change_pin.new_pin_value, params->new_pin_data.data_ptr, params->new_pin_data.data_len);

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_CHANGE_PIN_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_CHANGE_PIN_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
   if(!rc)
    {
      qcril_uim_qmi_conv_change_pin_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_change_pin */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_depersonalization
===========================================================================*/
/*!
@brief
  Deactivates or unblocks specified personalization on the phone.  If the
  user_cb function pointer is set to NULL, this function will be invoked
  synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_depersonalization
(
  qmi_client_type                                     client_handle,
  const qmi_uim_depersonalization_params_type       * params,
  qmi_uim_user_async_cb_type                          user_cb,
  void                                              * user_data,
  qmi_uim_rsp_data_type                             * rsp_data
)
{
  int                                       rc;
  qmi_txn_handle                            txn_handle;
  uim_depersonalization_req_msg_v01       * qmi_request   = NULL;
  uim_depersonalization_resp_msg_v01      * qmi_response  = NULL;
  qmi_uim_cb_struct_type                  * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->ck_data.data_len > QMI_UIM_CK_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_depersonalization_req_msg_v01*)qcril_malloc(sizeof(uim_depersonalization_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_depersonalization_resp_msg_v01*)qcril_malloc(sizeof(uim_depersonalization_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_depersonalization_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_depersonalization_resp_msg_v01));

  /*set mandatory depersonalization params*/
  qmi_request->depersonalization.feature = (uim_perso_feature_enum_v01)params->perso_feature;
  qmi_request->depersonalization.operation = (uim_perso_operation_enum_v01)params->perso_operation;

  qmi_request->depersonalization.ck_value_len = (uint32_t)params->ck_data.data_len;
  memcpy(qmi_request->depersonalization.ck_value, params->ck_data.data_ptr, params->ck_data.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_DEPERSONALIZATION_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_DEPERSONALIZATION_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_depersonalization_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_depersonalization */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_power_down
===========================================================================*/
/*!
@brief
  Powers down the card.  If the user_cb function pointer is set to NULL,
  this function will be invoked synchronously. Otherwise it will be invoked
  asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_power_down
(
  qmi_client_type                                   client_handle,
  const qmi_uim_power_down_params_type            * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                rc;
  qmi_txn_handle                     txn_handle;
  uim_power_down_req_msg_v01       * qmi_request   = NULL;
  uim_power_down_resp_msg_v01      * qmi_response  = NULL;
  qmi_uim_cb_struct_type           * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_request = (uim_power_down_req_msg_v01*)qcril_malloc(sizeof(uim_power_down_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_power_down_resp_msg_v01*)qcril_malloc(sizeof(uim_power_down_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_power_down_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_power_down_resp_msg_v01));

  /*power down request params*/
  qmi_request->slot = (uim_slot_enum_v01)params->slot;

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_POWER_DOWN_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_POWER_DOWN_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_power_down_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_power_down */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_power_up
===========================================================================*/
/*!
@brief
  Powers up the card.  If the user_cb function pointer is set to NULL,
  this function will be invoked synchronously. Otherwise it will be invoked
  asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_power_up
(
  qmi_client_type                                     client_handle,
  const qmi_uim_power_up_params_type                * params,
  qmi_uim_user_async_cb_type                          user_cb,
  void                                              * user_data,
  qmi_uim_rsp_data_type                             * rsp_data
)
{
  int                              rc;
  qmi_txn_handle                   txn_handle;
  uim_power_up_req_msg_v01       * qmi_request   = NULL;
  uim_power_up_resp_msg_v01      * qmi_response  = NULL;
  qmi_uim_cb_struct_type         * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_request = (uim_power_up_req_msg_v01*)qcril_malloc(sizeof(uim_power_up_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_power_up_resp_msg_v01*)qcril_malloc(sizeof(uim_power_up_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_power_up_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_power_up_resp_msg_v01));

  /*power up request params*/
  qmi_request->slot = (uim_slot_enum_v01)params->slot;

  /* Card power up mode */
  qmi_request->card_mode_valid = 1;
  qmi_request->card_mode = (uim_card_mode_enum_v01)params->mode;

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_POWER_UP_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_POWER_UP_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_power_up_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_power_up */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_card_status
===========================================================================*/
/*!
@brief
  Retreives the Card status along with status of all the available applications
  on the card. If the user_cb function pointer is set to NULL,
  this function will be invoked synchronously. Otherwise it will be invoked
  asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_get_card_status
(
  qmi_client_type                           client_handle,
  qmi_uim_user_async_cb_type                user_cb,
  void                                    * user_data,
  qmi_uim_rsp_data_type                   * rsp_data
)
{
  int                                  rc = 0;
  qmi_txn_handle                       txn_handle;
  uim_get_card_status_req_msg_v01    * qmi_request   = NULL;
  uim_get_card_status_resp_msg_v01   * qmi_response  = NULL;
  qmi_uim_cb_struct_type             * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_request = (uim_get_card_status_req_msg_v01*)qcril_malloc(sizeof(uim_get_card_status_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_get_card_status_req_msg_v01));
  qmi_request->extended_card_status_valid = 1;
  qmi_request->extended_card_status = 1;

  qmi_response = (uim_get_card_status_resp_msg_v01*)qcril_malloc(sizeof(uim_get_card_status_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_response, 0, sizeof(uim_get_card_status_resp_msg_v01));

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    memset(cb_data, 0, sizeof(qmi_uim_cb_struct_type));
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_GET_CARD_STATUS_REQ_V01,
                                    (void *)qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_GET_CARD_STATUS_REQ_V01,
                                   (void *)qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_get_card_status_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_get_card_status */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_event_reg
===========================================================================*/
/*!
@brief
  Issues the Event registration command for the client. Note that this is a
  synchronous-only function call.

@return
  If return code < 0, the operation failed.  In the failure case, if the
  return code is QMI_SERVICE_ERR, then the qmi_err_code value will give the
  QMI error reason.  Otherwise, qmi_err_code will have meaningless data.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_event_reg
(
  qmi_client_type                                 client_handle,
  const qmi_uim_event_reg_params_type           * params,
  qmi_uim_rsp_data_type                         * rsp_data
)
{
  int                             rc;
  uim_event_reg_req_msg_v01     * qmi_request   = NULL;
  uim_event_reg_resp_msg_v01    * qmi_response  = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  QCRIL_LOG_INFO( "%s", __FUNCTION__);

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_request = (uim_event_reg_req_msg_v01*)qcril_malloc(sizeof(uim_event_reg_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_event_reg_resp_msg_v01*)qcril_malloc(sizeof(uim_event_reg_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_event_reg_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_event_reg_resp_msg_v01));

  qmi_request->event_mask = 0;
  if(params->card_status == QMI_UIM_TRUE)
  {
    qmi_request->event_mask |= QMI_UIM_EVENT_MASK_CARD_STATUS;
  }
  if(params->extended_card_status == QMI_UIM_TRUE)
  {
    qmi_request->event_mask |= QMI_UIM_EVENT_MASK_EXTENDED_CARD_STATUS;
  }
  if(params->recovery == QMI_UIM_TRUE)
  {
    qmi_request->event_mask |= QMI_UIM_EVENT_MASK_RECOVERY;
  }
  if(params->supply_voltage_status == QMI_UIM_TRUE)
  {
    qmi_request->event_mask |= QMI_UIM_EVENT_MASK_SUPPLY_VOLTAGE_STATUS;
  }
  if(params->sap_connection == QMI_UIM_TRUE)
  {
    qmi_request->event_mask |= QMI_UIM_EVENT_MASK_SAP_CONNECTION;
  }
  if(params->simlock_temp_unlock_status == QMI_UIM_TRUE)
  {
    qmi_request->event_mask |= QMI_UIM_EVENT_MASK_SIMLOCK_TEMP_UNLOCK_STATUS;
  }
  if(params->slot_status == QMI_UIM_TRUE)
  {
    qmi_request->event_mask |= QMI_UIM_EVENT_MASK_SLOT_STATUS;
  }

  rc = qmi_client_send_msg_sync( client_handle,
                                 QMI_UIM_EVENT_REG_REQ_V01,
                                 (void*) qmi_request,
                                 sizeof(*qmi_request),
                                 (void*) qmi_response,
                                 sizeof(*qmi_response),
                                 QMI_UIM_DEFAULT_TIMEOUT);
  if(!rc)
  {
    qcril_uim_qmi_conv_event_reg_resp(qmi_response, rsp_data);
  }
  qcril_free(qmi_request);
  qcril_free(qmi_response);

  return rc;
} /* qcril_qmi_uim_event_reg */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_authenticate
===========================================================================*/
/*!
@brief
  Issues the authenticate request on the card. If the user_cb function
  pointer is set to NULL, this function will be invoked synchronously.
  Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_authenticate
(
  qmi_client_type                                   client_handle,
  const qmi_uim_authenticate_params_type          * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                rc;
  qmi_txn_handle                     txn_handle;
  uim_authenticate_req_msg_v01     * qmi_request   = NULL;
  uim_authenticate_resp_msg_v01    * qmi_response  = NULL;
  qmi_uim_cb_struct_type           * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if((params->auth_data.data_len > QMI_UIM_AUTHENTICATE_DATA_MAX_V01) ||
     (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_authenticate_req_msg_v01*)qcril_malloc(sizeof(uim_authenticate_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_authenticate_resp_msg_v01*)qcril_malloc(sizeof(uim_authenticate_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_authenticate_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_authenticate_resp_msg_v01));

  /*authentication request params*/
  qmi_request->authentication_data.context = (uim_auth_context_enum_v01)(params->auth_context);
  qmi_request->authentication_data.data_len = (uint32_t)(params->auth_data.data_len);
  memcpy(qmi_request->authentication_data.data, params->auth_data.data_ptr, params->auth_data.data_len);

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_AUTHENTICATE_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_AUTHENTICATE_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_authenticate_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_authenticate */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_service_status
===========================================================================*/
/*!
@brief
  Issues the Service status command that queries for the status of particular
  services in the card e.g FDN, BDN etc. Note that currently only FDN query
  is supported. If the user_cb function pointer is set to NULL,
  this function will be invoked synchronously. Otherwise it will be invoked
  asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_get_service_status
(
  qmi_client_type                                   client_handle,
  const qmi_uim_get_service_status_params_type    * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                       rc;
  qmi_txn_handle                            txn_handle;
  uim_get_service_status_req_msg_v01      * qmi_request   = NULL;
  uim_get_service_status_resp_msg_v01     * qmi_response  = NULL;
  qmi_uim_cb_struct_type                  * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_get_service_status_req_msg_v01*)qcril_malloc(sizeof(uim_get_service_status_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_get_service_status_resp_msg_v01*)qcril_malloc(sizeof(uim_get_service_status_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_get_service_status_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_get_service_status_resp_msg_v01));

  /*set mask request params*/
  qmi_request->mask = (uint32_t)params->mask;

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_GET_SERVICE_STATUS_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_GET_SERVICE_STATUS_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_get_service_status_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_get_service_status */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_set_service_status
===========================================================================*/
/*!
@brief
  Issues the Service status command that sets the status of particular
  services in the card e.g FDN, BDN etc. Note that currently only FDN
  service is supported. If the user_cb function pointer is set to NULL,
  this function will be invoked synchronously. Otherwise it will be invoked
  asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_set_service_status
(
  qmi_client_type                                   client_handle,
  const qmi_uim_set_service_status_params_type    * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                      rc;
  qmi_txn_handle                           txn_handle;
  uim_set_service_status_req_msg_v01     * qmi_request   = NULL;
  uim_set_service_status_resp_msg_v01    * qmi_response  = NULL;
  qmi_uim_cb_struct_type                 * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_set_service_status_req_msg_v01*)qcril_malloc(sizeof(uim_set_service_status_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_set_service_status_resp_msg_v01*)qcril_malloc(sizeof(uim_set_service_status_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_set_service_status_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_set_service_status_resp_msg_v01));

  /*set fdn_status request params*/
  qmi_request->fdn_status_valid = 1;
  qmi_request->fdn_status = (uint8_t)params->fdn_status;

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_SET_SERVICE_STATUS_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_SET_SERVICE_STATUS_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_set_service_status_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_set_service_status */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_change_provisioning_session
===========================================================================*/
/*!
@brief
  Issues the change provisioning session command that is used to activate,
  deactivate or switch the provisioning sessions. If the user_cb function
  pointer is set to NULL, this function will be invoked synchronously.
  Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_change_provisioning_session
(
  qmi_client_type                                     client_handle,
  const qmi_uim_change_prov_session_params_type     * params,
  qmi_uim_user_async_cb_type                          user_cb,
  void                                              * user_data,
  qmi_uim_rsp_data_type                             * rsp_data
)
{
  int                                                rc;
  qmi_txn_handle                                     txn_handle;
  uim_change_provisioning_session_req_msg_v01      * qmi_request   = NULL;
  uim_change_provisioning_session_resp_msg_v01     * qmi_response  = NULL;
  qmi_uim_cb_struct_type                           * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->app_info.aid.data_len > QMI_UIM_AID_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_change_provisioning_session_req_msg_v01*)qcril_malloc(sizeof(uim_change_provisioning_session_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_change_provisioning_session_resp_msg_v01*)qcril_malloc(sizeof(uim_change_provisioning_session_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_change_provisioning_session_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_change_provisioning_session_resp_msg_v01));

  /*set session request params */
  qmi_request->session_change.session_type = (uim_session_type_enum_v01)params->session_type;
  qmi_request->session_change.activate = (uint8_t)params->activation_type;

  /*set application information request params onlt for activation */
  if (qmi_request->session_change.activate == 1)
  {
    qmi_request->application_information_valid = 1;
    qmi_request->application_information.slot = (uim_slot_enum_v01)params->app_info.slot;
    qmi_request->application_information.aid_len = (uint32_t)params->app_info.aid.data_len;
    memcpy(qmi_request->application_information.aid, params->app_info.aid.data_ptr, params->app_info.aid.data_len);
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_CHANGE_PROVISIONING_SESSION_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_CHANGE_PROVISIONING_SESSION_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_change_provisioning_session_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_change_provisioning_session */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_label
===========================================================================*/
/*!
@brief
  Issues the get label command that retrieves the label of an application
  from EF-DIR on the UICC card. Note that it will return an error if used on
  an ICC card. If the user_cb function pointer is set to NULL, this function
  will be invoked synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_get_label
(
  qmi_client_type                                   client_handle,
  const qmi_uim_get_label_params_type             * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                rc;
  qmi_txn_handle                     txn_handle;
  uim_get_label_req_msg_v01        * qmi_request   = NULL;
  uim_get_label_resp_msg_v01       * qmi_response  = NULL;
  qmi_uim_cb_struct_type           * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->app_info.aid.data_len > QMI_UIM_AID_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_get_label_req_msg_v01*)qcril_malloc(sizeof(uim_get_label_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_get_label_resp_msg_v01*)qcril_malloc(sizeof(uim_get_label_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_get_label_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_get_label_resp_msg_v01));

  /*set application information request params */
  qmi_request->application_information.slot = (uim_slot_enum_v01)params->app_info.slot;
  qmi_request->application_information.aid_len = (uint32_t)params->app_info.aid.data_len;
  memcpy(qmi_request->application_information.aid, params->app_info.aid.data_ptr, params->app_info.aid.data_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_GET_LABEL_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_GET_LABEL_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_get_label_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_get_label */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_close_session
===========================================================================*/
/*!
@brief
  Issues the close session command for a non-provisioning session that may
  have been opened before by the client. This function is supported only
  synchronously.

@return
  If return code < 0, the operation failed. In the failure case, if the
  return code is QMI_SERVICE_ERR, then the qmi_err_code value will give you
  the QMI error reason.  Otherwise, qmi_err_code will have meaningless data.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_close_session
(
  qmi_client_type                                   client_handle,
  const qmi_uim_close_session_params_type         * params,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                   rc;
  uim_close_session_req_msg_v01       * qmi_request   = NULL;
  uim_close_session_resp_msg_v01      * qmi_response  = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_close_session_req_msg_v01*)qcril_malloc(sizeof(uim_close_session_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_close_session_resp_msg_v01*)qcril_malloc(sizeof(uim_close_session_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_close_session_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_close_session_resp_msg_v01));

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  rc = qmi_client_send_msg_sync( client_handle,
                                 QMI_UIM_CLOSE_SESSION_REQ_V01,
                                 (void*) qmi_request,
                                 sizeof(*qmi_request),
                                 (void*) qmi_response,
                                 sizeof(*qmi_response),
                                 QMI_UIM_DEFAULT_TIMEOUT);
  if(!rc)
  {
    qcril_uim_qmi_conv_close_session_resp(qmi_response, rsp_data);
  }
  qcril_free(qmi_request);
  qcril_free(qmi_response);

  return rc;
} /* qcril_qmi_uim_close_session */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_send_apdu
===========================================================================*/
/*!
@brief
  Issues the request for sending raw APDUs to the card. An optional channel
  id params can be used when a logical channel is already opened previously
  using the qcril_qmi_uim_logical_channel command. If the user_cb function pointer
  is set to NULL, this function will be invoked synchronously. Otherwise it
  will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_send_apdu
(
  qmi_client_type                                   client_handle,
  const qmi_uim_send_apdu_params_type             * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                               rc;
  qmi_txn_handle                    txn_handle;
  uim_send_apdu_req_msg_v01       * qmi_request   = NULL;
  uim_send_apdu_resp_msg_v01      * qmi_response  = NULL;
  qmi_uim_cb_struct_type          * cb_data       = NULL;
  uint16_t                          offset        = 0;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_request = (uim_send_apdu_req_msg_v01*)qcril_malloc(sizeof(uim_send_apdu_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_send_apdu_resp_msg_v01*)qcril_malloc(sizeof(uim_send_apdu_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_send_apdu_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_send_apdu_resp_msg_v01));

  /*set slots and APDU request params */
  qmi_request->slot = (uim_slot_enum_v01)params->slot;

  /*set optional logical channel request params */
  if(params->channel_id_present == QMI_UIM_TRUE)
  {
    qmi_request->channel_id_valid = 1;
    qmi_request->channel_id = (uint8_t)params->channel_id;
  }
  else
  {
    qmi_request->channel_id_valid = 0;
  }

  qmi_request->long_request_valid = 0;
  /* If APDU is more than QMI_UIM_APDU_DATA_MAX_V01 than send APDU data in chunks */
  if(params->apdu.data_len > QMI_UIM_APDU_DATA_MAX_V01)
  {
    static boolean                            is_extended_apdu_supported    = FALSE;
    static boolean                            is_get_config_done            = FALSE;

    if(!is_get_config_done)
    {
      qmi_uim_rsp_data_type                     get_data_rsp;
      qmi_uim_get_configuration_params_type     get_config_params;

      memset(&get_data_rsp, 0x00, sizeof(qmi_uim_rsp_data_type));
      memset(&get_config_params, 0x00, sizeof(qmi_uim_get_configuration_params_type));
      get_config_params.extended_apdu = QMI_UIM_TRUE;

      rc = qcril_qmi_uim_get_configuration(client_handle,
                                           &get_config_params,
                                           NULL,
                                           NULL,
                                           &get_data_rsp) ;
      if(rc != 0)
      {
        return QMI_INTERNAL_ERR;
      }

      is_get_config_done = TRUE;
      if (get_data_rsp.rsp_data.get_configuration_rsp.extended_apdu_valid)
      {
        is_extended_apdu_supported = get_data_rsp.rsp_data.get_configuration_rsp.extended_apdu ? TRUE : FALSE;
      }
    }

    if(!is_extended_apdu_supported)
    {
      return QMI_INTERNAL_ERR;
    }

    qmi_request->long_request_valid = 1;
    qmi_request->long_request.total_length = params->apdu.data_len;
    qmi_request->long_request.token = 0;

    while(qmi_request->long_request.total_length > offset)
    {
      qmi_request->long_request.offset = offset;
      if((qmi_request->long_request.total_length - offset) > QMI_UIM_APDU_DATA_MAX_V01)
      {
        memcpy(qmi_request->apdu,
               params->apdu.data_ptr + offset,
               QMI_UIM_APDU_DATA_MAX_V01);
        qmi_request->apdu_len = QMI_UIM_APDU_DATA_MAX_V01;
      }
      else
      {
        break;
      }
      /* update offset */
      offset += qmi_request->apdu_len;
      /* Proceed with send APDU request */
      rc = qmi_client_send_msg_sync( client_handle,
                                     QMI_UIM_SEND_APDU_REQ_V01,
                                     (void*) qmi_request,
                                     sizeof(*qmi_request),
                                     (void*) qmi_response,
                                     sizeof(*qmi_response),
                                     QMI_UIM_DEFAULT_TIMEOUT);
      if(rc != QMI_NO_ERR)
      {
        qcril_uim_qmi_conv_send_apdu_resp(qmi_response, rsp_data);
        qcril_free(qmi_response);
        qcril_free(qmi_request);
        return rc;
      }
    }
  }
  qmi_request->apdu_len = (params->apdu.data_len - offset);
  memcpy(qmi_request->apdu,
         params->apdu.data_ptr + offset,
         qmi_request->apdu_len);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_SEND_APDU_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_SEND_APDU_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_send_apdu_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_send_apdu */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_logical_channel
===========================================================================*/
/*!
@brief
  Issues the request for open or close logical channel on a particlar
  application on the UICC card. Note that it will return an error if used on
  an ICC card. If the user_cb function pointer is set to NULL, this function
  will be invoked synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_logical_channel
(
  qmi_client_type                                   client_handle,
  const qmi_uim_logical_channel_params_type       * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                     rc;
  qmi_txn_handle                          txn_handle;
  uim_logical_channel_req_msg_v01       * qmi_request   = NULL;
  uim_logical_channel_resp_msg_v01      * qmi_response  = NULL;
  qmi_uim_cb_struct_type                * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->channel_data.aid.data_len > QMI_UIM_AID_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_logical_channel_req_msg_v01*)qcril_malloc(sizeof(uim_logical_channel_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_logical_channel_resp_msg_v01*)qcril_malloc(sizeof(uim_logical_channel_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_logical_channel_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_logical_channel_resp_msg_v01));

  /*set logical_channel request params */
  qmi_request->slot = (uim_slot_enum_v01)params->slot;

  if(params->operation_type == QMI_UIM_LOGICAL_CHANNEL_CLOSE)
  {
    qmi_request->channel_id_valid = 1;
    qmi_request->aid_valid = 0;
    qmi_request->channel_id = params->channel_data.close_channel_info.channel_id;
    qmi_request->terminate_application_valid = 1;
    qmi_request->terminate_application = params->channel_data.close_channel_info.terminate_app;
  }
  else
  {
    qmi_request->channel_id_valid = 0;
    qmi_request->aid_valid = 1;
    qmi_request->aid_len = (uint32_t)params->channel_data.aid.data_len;
    memcpy(qmi_request->aid, params->channel_data.aid.data_ptr, params->channel_data.aid.data_len);

    /* Update FCI info if it was sent */
    if (params->file_control_information.is_valid)
    {
      qmi_request->file_control_information_valid = 1;
      qmi_request->file_control_information =
        (uim_file_control_information_enum_v01)params->file_control_information.fci_value;
    }
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_LOGICAL_CHANNEL_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_LOGICAL_CHANNEL_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_logical_channel_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_logical_channel */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_open_logical_channel
===========================================================================*/
/*!
@brief
  Issues the request for open logical channel on the specified UICC card.
  If the user_cb function pointer is set to NULL, this function
  will be invoked synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_open_logical_channel
(
  qmi_client_type                                   client_handle,
  const qmi_uim_open_logical_channel_params_type  * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                     rc;
  qmi_txn_handle                          txn_handle;
  uim_open_logical_channel_req_msg_v01  * qmi_request   = NULL;
  uim_open_logical_channel_resp_msg_v01 * qmi_response  = NULL;
  qmi_uim_cb_struct_type                * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  if ( params->aid_present && (params->aid.data_len > QMI_UIM_AID_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "AID length too long");
    return QMI_INTERNAL_ERR;
  }

  /* Allocate request & reponse pointers */
  qmi_request = (uim_open_logical_channel_req_msg_v01*)qcril_malloc(
                   sizeof(uim_open_logical_channel_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_open_logical_channel_resp_msg_v01*)qcril_malloc(
                   sizeof(uim_open_logical_channel_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_open_logical_channel_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_open_logical_channel_resp_msg_v01));

  /* Set logical_channel request params */
  if (params->aid_present)
  {
    qmi_request->aid_valid = 1;
    qmi_request->aid_len = (uint32_t)params->aid.data_len;
    memcpy(qmi_request->aid, params->aid.data_ptr, params->aid.data_len);
  }

  qmi_request->slot = (uim_slot_enum_v01) params->slot;

  /* Update FCI info if it was sent */
  if (params->file_control_information.is_valid)
  {
    qmi_request->file_control_information_valid = 1;
    qmi_request->file_control_information =
      (uim_file_control_information_enum_v01)params->file_control_information.fci_value;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_OPEN_LOGICAL_CHANNEL_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_OPEN_LOGICAL_CHANNEL_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_open_logical_channel_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);
  return rc;
} /* qcril_qmi_uim_open_logical_channel */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_atr
===========================================================================*/
/*!
@brief
  Issues the request to retrieve the ATR of the specified card. If the
  user_cb function pointer is set to NULL, this function will be invoked
  synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_get_atr
(
  qmi_client_type                                   client_handle,
  const qmi_uim_get_atr_params_type               * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                               rc;
  qmi_txn_handle                    txn_handle;
  uim_get_atr_req_msg_v01         * qmi_request   = NULL;
  uim_get_atr_resp_msg_v01        * qmi_response  = NULL;
  qmi_uim_cb_struct_type          * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_request = (uim_get_atr_req_msg_v01*)qcril_malloc(sizeof(uim_get_atr_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_get_atr_resp_msg_v01*)qcril_malloc(sizeof(uim_get_atr_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_get_atr_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_get_atr_resp_msg_v01));

  /*set slots and APDU request params */
  qmi_request->slot = (uim_slot_enum_v01)params->slot;

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_GET_ATR_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_GET_ATR_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_get_atr_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_get_atr */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_send_status
===========================================================================*/
/*!
@brief
  Send STATUS command to the card for a DF or ADF.
  If the user_cb function pointer is set to NULL, this function will
  be invoked synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value in rsp_data will give you the QMI error reason.
  Otherwise, qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_send_status
(
  qmi_client_type                               client_handle,
  const qmi_uim_status_cmd_params_type        * params,
  qmi_uim_user_async_cb_type                    user_cb,
  void                                        * user_data,
  qmi_uim_rsp_data_type                       * rsp_data
)
{
  int                                 rc;
  qmi_txn_handle                      txn_handle;
  uim_send_status_cmd_req_msg_v01   * qmi_request   = NULL;
  uim_send_status_cmd_resp_msg_v01  * qmi_response  = NULL;
  qmi_uim_cb_struct_type            * cb_data       = NULL;

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_send_status_cmd_req_msg_v01*)qcril_malloc(sizeof(uim_send_status_cmd_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_send_status_cmd_resp_msg_v01*)qcril_malloc(sizeof(uim_send_status_cmd_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_send_status_cmd_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_send_status_cmd_resp_msg_v01));

  /*set session information*/
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  if(params->session_info.aid.data_len > 0 && params->session_info.aid.data_ptr != NULL)
  {
    memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);
  }

  qmi_request->status_cmd_mode_valid = 1;
  qmi_request->status_cmd_mode       = (uim_status_cmd_mode_enum_v01)params->mode;

  qmi_request->status_cmd_resp_valid = 1;
  qmi_request->status_cmd_resp       = (uim_status_cmd_resp_enum_v01)params->resp_type;

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_SEND_STATUS_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_SEND_STATUS_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_send_status_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);


  QCRIL_NOTUSED(client_handle);

  return rc;
} /* qcril_qmi_uim_send_status */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_reselect
===========================================================================*/
/*!
@brief
  Issues the reselection request of an app on the logical channel.

@return

  If the return code < 0, the operation failed and you will not get an
  asynchronous response.  If the operation doesn't fail (return code >=0),
  the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.
    - QMI request parameter must be allocated & filled

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_reselect
(
  qmi_client_type                                  client_handle,
  const qmi_uim_reselect_params_type             * params,
  qmi_uim_user_async_cb_type                       user_cb,
  void                                           * user_data,
  qmi_uim_rsp_data_type                          * rsp_data
)
{
  int                              rc;
  qmi_txn_handle                   txn_handle;
  qmi_uim_cb_struct_type         * cb_data      = NULL;
  uim_reselect_req_msg_v01       * qmi_request  = NULL;
  uim_reselect_resp_msg_v01      * qmi_response = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  /* Allocate request response QMI parameters */
  qmi_request = (uim_reselect_req_msg_v01 *)qcril_malloc(sizeof(uim_reselect_req_msg_v01));
  if (qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_reselect_resp_msg_v01*)qcril_malloc(sizeof(uim_reselect_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  /* Update QMI parameters from protobuf request & dispatch it to modem */
  memset(qmi_request, 0, sizeof(uim_reselect_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_reselect_resp_msg_v01));

  qmi_request->slot = (uim_slot_enum_v01)params->slot;
  qmi_request->channel_id = params->channel_id;
  qmi_request->select_mode = (uim_select_mode_enum_v01)params->select_mode;

  if (user_cb)
  {
    /* Create callback params */
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }

    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_RESELECT_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_RESELECT_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_reselect_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }

  /* Free request pointer in any case */
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_reselect */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_supply_voltage
===========================================================================*/
/*!
@brief
  Issues the request to inform the modem about the client's acknowledgement
  to deactivate the supply voltage Vcc line of the specified card. If the
  user_cb function pointer is set to NULL, this function will be invoked
  synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_supply_voltage
(
  qmi_client_type                                   client_handle,
  const qmi_uim_supply_voltage_params_type        * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                 rc;
  qmi_txn_handle                      txn_handle;
  uim_supply_voltage_req_msg_v01    * qmi_request   = NULL;
  uim_supply_voltage_resp_msg_v01   * qmi_response  = NULL;
  qmi_uim_cb_struct_type            * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_request = (uim_supply_voltage_req_msg_v01*)qcril_malloc(sizeof(uim_supply_voltage_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_supply_voltage_resp_msg_v01*)qcril_malloc(
                   sizeof(uim_supply_voltage_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_supply_voltage_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_supply_voltage_resp_msg_v01));

  /*set slots and APDU request params */
  qmi_request->slot = (uim_slot_enum_v01)params->slot;

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_SUPPLY_VOLTAGE_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_SUPPLY_VOLTAGE_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_supply_voltage_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_supply_voltage */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_sap_connection
===========================================================================*/
/*!
@brief
  Issues the request for SAP connection establishment or disconnection. If
  the user_cb function pointer is set to NULL, this function will be invoked
  synchronously. Otherwise it will be invoked asynchronously.

@return

  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_sap_connection
(
  qmi_client_type                                     client_handle,
  const qmi_uim_sap_connection_params_type          * params,
  qmi_uim_user_async_cb_type                          user_cb,
  void                                              * user_data,
  qmi_uim_rsp_data_type                             * rsp_data
)
{
  int                                rc;
  qmi_txn_handle                     txn_handle;
  qmi_uim_cb_struct_type           * cb_data      = NULL;
  uim_sap_connection_req_msg_v01   * qmi_request  = NULL;
  uim_sap_connection_resp_msg_v01  * qmi_response = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  /* Allocate request response QMI parameters */
  qmi_request = (uim_sap_connection_req_msg_v01 *)qcril_malloc(sizeof(uim_sap_connection_req_msg_v01));
  if (qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_sap_connection_resp_msg_v01*)qcril_malloc(sizeof(uim_sap_connection_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  /* Update QMI parameters from request */
  memset(qmi_request, 0, sizeof(uim_sap_connection_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_sap_connection_resp_msg_v01));

  qmi_request->sap_connect.slot = (uim_slot_enum_v01)params->slot;
  qmi_request->sap_connect.connect = (uim_sap_connect_op_enum_v01)params->operation_type;

  if (qmi_request->sap_connect.connect == UIM_SAP_OP_CONNECT_V01)
  {
    qmi_request->connection_condition_valid = 1;
    qmi_request->connection_condition =
      (uim_sap_connection_condition_enum_v01)params->conn_condition;
  }
  else if (qmi_request->sap_connect.connect == UIM_SAP_OP_DISCONNECT_V01)
  {
    /* Note - for disconnect req, there is no mode passed from client */
    qmi_request->disconnect_mode_valid = 1;
    qmi_request->disconnect_mode = (uim_sap_disconnect_mode_enum_v01)params->disconnect_mode;
  }

  if (user_cb)
  {
    /* Create callback params */
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }

    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_SAP_CONNECTION_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_SAP_CONNECTION_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_sap_connection_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }

  /* Free request pointer in any case */
  qcril_free(qmi_request);
  return rc;
} /* qcril_qmi_uim_sap_connection */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_sap_request
===========================================================================*/
/*!
@brief
  Issues the request for SAP various SAP requests. If the user_cb function
  pointer is set to NULL, this function will be invoked synchronously.
  Otherwise it will be invoked asynchronously.

@return

  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_sap_request
(
  qmi_client_type                                  client_handle,
  const qmi_uim_sap_request_params_type          * params,
  qmi_uim_user_async_cb_type                       user_cb,
  void                                           * user_data,
  qmi_uim_rsp_data_type                          * rsp_data
)
{
  int                              rc;
  qmi_txn_handle                   txn_handle;
  qmi_uim_cb_struct_type         * cb_data      = NULL;
  uim_sap_request_req_msg_v01    * qmi_request  = NULL;
  uim_sap_request_resp_msg_v01   * qmi_response = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  /* Allocate request response QMI parameters */
  qmi_request = (uim_sap_request_req_msg_v01 *)qcril_malloc(sizeof(uim_sap_request_req_msg_v01));
  if (qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_sap_request_resp_msg_v01*)qcril_malloc(sizeof(uim_sap_request_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  /* Update QMI parameters from request */
  memset(qmi_request, 0, sizeof(uim_sap_request_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_sap_request_resp_msg_v01));

  qmi_request->sap_request.slot = (uim_slot_enum_v01)params->slot;
  qmi_request->sap_request.sap_request = (uim_sap_request_enum_v01)params->request_type;

  if (qmi_request->sap_request.sap_request == UIM_SAP_REQUEST_SEND_APDU_V01)
  {
    if ((params->apdu.data_len >  0) &&
        (params->apdu.data_len <= QMI_UIM_APDU_DATA_MAX_V01))
    {
      qmi_request->apdu_valid = 1;
      qmi_request->apdu_len = (uint32_t)params->apdu.data_len;
      memcpy(qmi_request->apdu, params->apdu.data_ptr, params->apdu.data_len);
    }
    else
    {
      QCRIL_LOG_ERROR("Incorrect length for APDU request: 0x%x", params->apdu.data_len);
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
  }

  if (user_cb)
  {
    /* Create callback params */
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }

    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_SAP_REQUEST_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_SAP_REQUEST_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_sap_request_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }

  /* Free request pointer in any case */
  qcril_free(qmi_request);
  return rc;
} /* qcril_qmi_uim_sap_request */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_gba_request
===========================================================================*/
/*!
@brief
  Issues the modem request to perform GBA initialization procedure and
  generate a shared key with a NAF. If the user_cb function pointer is set
  to NULL, this function will be invoked synchronously. Otherwise it will be
  invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_gba_request
(
  qmi_client_type                                   client_handle,
  const qmi_uim_gba_params_type                   * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                 rc;
  qmi_txn_handle                      txn_handle;
  uim_gba_req_msg_v01               * qmi_request   = NULL;
  uim_gba_resp_msg_v01              * qmi_response  = NULL;
  qmi_uim_cb_struct_type            * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if ((params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01) ||
      (params->naf_id.fqdn.data_len      > QMI_UIM_FQDN_MAX_V01))
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_gba_req_msg_v01*)qcril_malloc(sizeof(uim_gba_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_gba_resp_msg_v01*)qcril_malloc(sizeof(uim_gba_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_gba_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_gba_resp_msg_v01));

  /* update session information */
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  /* update NAF id */
  qmi_request->naf_id.fqdn_len = (uint32_t)(params->naf_id.fqdn.data_len);
  memcpy(qmi_request->naf_id.fqdn, params->naf_id.fqdn.data_ptr, params->naf_id.fqdn.data_len);
  memcpy(qmi_request->naf_id.security_protocol_id, params->naf_id.protocol_id, sizeof(params->naf_id.protocol_id));

  /* update bootstrapping info */
  qmi_request->force_bootstrapping_valid = 1;
  qmi_request->force_bootstrapping = (uint8_t)params->force_bootstrapping;

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_GBA_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_GBA_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_gba_request_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_gba_request */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_gba_impi
===========================================================================*/
/*!
@brief
  Issues the modem request to fetch IMPI for GBA operations. If the user_cb
  function pointer is set to NULL, this function will be invoked
  synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_gba_impi
(
  qmi_client_type                                   client_handle,
  const qmi_uim_gba_impi_params_type              * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                 rc;
  qmi_txn_handle                      txn_handle;
  uim_get_gba_impi_req_msg_v01      * qmi_request   = NULL;
  uim_get_gba_impi_resp_msg_v01     * qmi_response  = NULL;
  qmi_uim_cb_struct_type            * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if (params->session_info.aid.data_len > QMI_UIM_AID_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_get_gba_impi_req_msg_v01*)qcril_malloc(sizeof(uim_get_gba_impi_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_get_gba_impi_resp_msg_v01*)qcril_malloc(sizeof(uim_get_gba_impi_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_get_gba_impi_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_get_gba_impi_resp_msg_v01));

  /* update session information */
  qmi_request->session_information.session_type = (uim_session_type_enum_v01)params->session_info.session_type;
  qmi_request->session_information.aid_len = (uint32_t)(params->session_info.aid.data_len);
  memcpy(qmi_request->session_information.aid, params->session_info.aid.data_ptr, params->session_info.aid.data_len);

  /* update encryption info */
  if (params->encrypt_data)
  {
    qmi_request->encrypt_data_valid = 1;
    qmi_request->encrypt_data = 1;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_GET_GBA_IMPI_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_GET_GBA_IMPI_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_gba_imsi_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_gba_impi */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_configuration
===========================================================================*/
/*!
@brief
  Gets the modem configuration.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_get_configuration
(
  qmi_client_type                                   client_handle,
  const qmi_uim_get_configuration_params_type     * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                     rc;
  qmi_txn_handle                          txn_handle;
  uim_get_configuration_req_msg_v01     * qmi_request   = NULL;
  uim_get_configuration_resp_msg_v01    * qmi_response  = NULL;
  qmi_uim_cb_struct_type                * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  if((params->auto_selection | params->perso_status | params->halt_subscription | params->extended_apdu) ==
     QMI_UIM_FALSE)
  {
    return QMI_SERVICE_ERR;
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_request = (uim_get_configuration_req_msg_v01*)qcril_malloc(sizeof(uim_get_configuration_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_get_configuration_resp_msg_v01*)qcril_malloc(sizeof(uim_get_configuration_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_get_configuration_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_get_configuration_resp_msg_v01));

  qmi_request->configuration_mask_valid = 1;
  if(params->auto_selection)
  {
    qmi_request->configuration_mask |= QMI_UIM_GET_CONFIG_MASK_AUTO_SELECTION;
  }
  if(params->perso_status)
  {
    qmi_request->configuration_mask |= QMI_UIM_GET_CONFIG_MASK_PERSO_STATUS;
  }
  if(params->halt_subscription)
  {
    qmi_request->configuration_mask |= QMI_UIM_GET_CONFIG_MASK_HALT_SUBSCRIPTION;
  }
  if(params->extended_apdu)
  {
    qmi_request->configuration_mask |= QMI_UIM_GET_CONFIG_MASK_EXTENDED_APDU;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_GET_CONFIGURATION_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_GET_CONFIGURATION_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_get_configuration_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }

  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_get_configuration */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_personalization
===========================================================================*/
/*!
@brief
  Activates specified personalization on the phone. Note that currently only
  the reactivate personalization is enabled via this interface. If the
  user_cb function pointer is set to NULL, this function will be invoked
  synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_personalization
(
  qmi_client_type                                   client_handle,
  const qmi_uim_personalization_params_type       * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                       rc;
  qmi_txn_handle                            txn_handle;
  uim_personalization_req_msg_v01         * qmi_request   = NULL;
  uim_personalization_resp_msg_v01        * qmi_response  = NULL;
  qmi_uim_cb_struct_type                  * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  if(params->ck_data.data_len > QMI_UIM_CK_MAX_V01)
  {
    QCRIL_LOG_ERROR("%s", "data length too long");
    return QMI_INTERNAL_ERR;
  }

  qmi_request = (uim_personalization_req_msg_v01*)qcril_malloc(sizeof(uim_personalization_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_personalization_resp_msg_v01*)qcril_malloc(sizeof(uim_personalization_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_personalization_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_personalization_resp_msg_v01));

  /* Set mandatory CK */
  qmi_request->ck_value_len = (uint32_t)params->ck_data.data_len;
  memcpy(qmi_request->ck_value, params->ck_data.data_ptr, params->ck_data.data_len);

  /* Set reactivate TLV */
  qmi_request->reactivate_feature_valid = 0x01;
  qmi_request->reactivate_feature = (uim_perso_feature_enum_v01)params->reactivate_feature;

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                             QMI_UIM_PERSONALIZATION_REQ_V01,
                                             (void*) qmi_request,
                                             sizeof(*qmi_request),
                                             (void*) qmi_response,
                                             sizeof(*qmi_response),
                                             qmi_uim_client_async_cb,
                                             cb_data,
                                             &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                            QMI_UIM_PERSONALIZATION_REQ_V01,
                                            (void*) qmi_request,
                                            sizeof(*qmi_request),
                                            (void*) qmi_response,
                                            sizeof(*qmi_response),
                                            QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_personalization_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_personalization */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_physical_slot_mapping
===========================================================================*/
/*!
@brief
  Gets the logical to physical slot mapping.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_get_physical_slot_mapping
(
  qmi_client_type                                   client_handle,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                           rc;
  qmi_txn_handle                                txn_handle;
  uim_get_physical_slot_mapping_resp_msg_v01  * qmi_response  = NULL;
  qmi_uim_cb_struct_type                      * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_response = (uim_get_physical_slot_mapping_resp_msg_v01*)qcril_malloc(sizeof(uim_get_physical_slot_mapping_resp_msg_v01));
  if(qmi_response == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  memset(qmi_response, 0, sizeof(uim_get_physical_slot_mapping_resp_msg_v01));

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                             QMI_UIM_GET_PHYSICAL_SLOT_MAPPING_REQ_V01,
                                             NULL,
                                             0,
                                             (void*) qmi_response,
                                             sizeof(*qmi_response),
                                             qmi_uim_client_async_cb,
                                             cb_data,
                                             &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                            QMI_UIM_GET_PHYSICAL_SLOT_MAPPING_REQ_V01,
                                            NULL,
                                            0,
                                            (void*) qmi_response,
                                            sizeof(*qmi_response),
                                            QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_get_physical_slot_mapping_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }

  return rc;
} /* qcril_qmi_uim_get_physical_slot_mapping */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_slot_status
===========================================================================*/
/*!
@brief
  Gets the physical slot status.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.
  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_get_slots_status
(
  qmi_client_type                                   client_handle,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                           rc;
  qmi_txn_handle                                txn_handle;
  uim_get_slots_status_resp_msg_v01           * qmi_response  = NULL;
  qmi_uim_cb_struct_type                      * cb_data       = NULL;

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_response = (uim_get_slots_status_resp_msg_v01*)qcril_malloc(sizeof(uim_get_slots_status_resp_msg_v01));

  if(qmi_response == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  memset(qmi_response, 0, sizeof(uim_get_slots_status_resp_msg_v01));

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }

    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async(client_handle,
                                   QMI_UIM_GET_SLOTS_STATUS_REQ_V01,
                                   NULL,
                                   0,
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   qmi_uim_client_async_cb,
                                   cb_data,
                                   &txn_handle);
    if (rc != QMI_NO_ERR)
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync(client_handle,
                                  QMI_UIM_GET_SLOTS_STATUS_REQ_V01,
                                  NULL,
                                  0,
                                  (void*) qmi_response,
                                  sizeof(*qmi_response),
                                  QMI_UIM_DEFAULT_TIMEOUT);
    if (rc != QMI_NO_ERR)
    {
      qcril_uim_qmi_get_slots_status_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }

  return rc;
} /* qcril_qmi_uim_get_slots_status */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_switch_slot_ext
===========================================================================*/
/*!
@brief
  Changes the logical to physical slot mapping.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_switch_slot_ext
(
  qmi_client_type                                   client_handle,
  const qmi_uim_physical_slot_mapping_type        * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                     rc;
  qmi_txn_handle                          txn_handle;
  uim_switch_slot_ext_req_msg_v01       * qmi_request   = NULL;
  uim_switch_slot_ext_resp_msg_v01      * qmi_response  = NULL;
  qmi_uim_cb_struct_type                * cb_data       = NULL;
  uint8                                   slot_id       = 0;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_request = (uim_switch_slot_ext_req_msg_v01*)qcril_malloc(sizeof(uim_switch_slot_ext_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_switch_slot_ext_resp_msg_v01*)qcril_malloc(sizeof(uim_switch_slot_ext_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_switch_slot_ext_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_switch_slot_ext_resp_msg_v01));

  /*switch slot request params*/
  for(slot_id = 0; slot_id < params->num_slots &&
                   slot_id < QMI_UIM_MAX_CARD_COUNT &&
                   slot_id < QMI_UIM_EXTENDED_CARDS_MAX_V01; slot_id++)
  {
    qmi_request->logical_slot[slot_id] = (uim_physical_slot_enum_v01)params->logical_slot[slot_id];
  }

  qmi_request->logical_slot_len = MIN(params->num_slots, QMI_UIM_EXTENDED_CARDS_MAX_V01);

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_SWITCH_SLOT_EXT_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
        qcril_free( qmi_response );
        qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_SWITCH_SLOT_EXT_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_switch_slot_ext_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);
  return rc;
} /* qcril_qmi_uim_switch_slot_ext */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_set_apdu_behavior
===========================================================================*/
/*!
@brief
  Issues the request to set the modem behavior for APDU transaction. If the
  user_cb function pointer is set to NULL, this function will be invoked
  synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_set_apdu_behavior
(
  qmi_client_type                                   client_handle,
  const qmi_uim_set_apdu_behavior_params_type     * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                     rc;
  qmi_txn_handle                          txn_handle;
  uim_set_apdu_behavior_req_msg_v01     * qmi_request   = NULL;
  uim_set_apdu_behavior_resp_msg_v01    * qmi_response  = NULL;
  qmi_uim_cb_struct_type                * cb_data       = NULL;

  QCRIL_NOTUSED(client_handle);

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  QCRIL_UIM_VALIDATE_SVC_PTR_RETURN_ERR_IF_NULL(client_handle);

  qmi_request = (uim_set_apdu_behavior_req_msg_v01*)qcril_malloc(sizeof(uim_set_apdu_behavior_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_set_apdu_behavior_resp_msg_v01*)qcril_malloc(
                   sizeof(uim_set_apdu_behavior_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_set_apdu_behavior_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_set_apdu_behavior_resp_msg_v01));

  /*set slots and APDU request params */
  qmi_request->slot = (uim_slot_enum_v01)params->slot;

  qmi_request->channel_id = params->channel_id;

  switch (params->apdu_behavior)
  {
    case QMI_UIM_APDU_BEHAVIOR_NO_GET_RESPONSE_FOR_WARNING:
      qmi_request->apdu_behavior = UIM_APDU_BEHAVIOR_NO_GET_RESPONSE_FOR_WARNING_V01;
      qmi_request->apdu_behavior_valid = 1;
      break;

    case QMI_UIM_APDU_BEHAVIOR_AUTOMATIC_GET_RESPONSE_FOR_WARNING:
      qmi_request->apdu_behavior = UIM_APDU_BEHAVIOR_AUTOMATIC_GET_RESPONSE_FOR_WARNING_V01;
      qmi_request->apdu_behavior_valid = 1;
      break;

    default:
      break;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_SET_APDU_BEHAVIOR_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_SET_APDU_BEHAVIOR_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_conv_set_apdu_behavior_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_set_apdu_behavior */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_remote_unlock
===========================================================================*/
/*!
@brief
  Issues the request for SIM remote lock and unlock.
  If the user_cb function pointer is set to NULL, this function will be invoked
  synchronously. Otherwise it will be invoked asynchronously.
@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.
  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.
@note
  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.
  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_remote_unlock
(
  qmi_client_type                            client_handle,
  const qmi_uim_remote_unlock_params_type  * params,
  qmi_uim_user_async_cb_type                 user_cb,
  void                                     * user_data,
  qmi_uim_rsp_data_type                    * rsp_data
)
{
  int                                 rc;
  qmi_txn_handle                      txn_handle;
  uim_remote_unlock_req_msg_v01     * qmi_request   = NULL;
  uim_remote_unlock_resp_msg_v01    * qmi_response  = NULL;
  qmi_uim_cb_struct_type            * cb_data       = NULL;

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  qmi_request = (uim_remote_unlock_req_msg_v01*)qcril_malloc(sizeof(uim_remote_unlock_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_remote_unlock_resp_msg_v01*)qcril_malloc(
                   sizeof(uim_remote_unlock_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_remote_unlock_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_remote_unlock_resp_msg_v01));

  qmi_request->operation_valid = TRUE;
  qmi_request->operation = (uim_remote_unlock_operation_type_enum_v01)params->operation;

  if (params->simlock_data.data_len > 0 &&
      params->simlock_data.data_len <= QMI_UIM_SIMLOCK_DATA_MAX &&
      params->simlock_data.data_ptr != NULL)
  {
    qmi_request->simlock_data_valid = TRUE;
    memcpy(qmi_request->simlock_data,
           params->simlock_data.data_ptr,
           params->simlock_data.data_len);
    qmi_request->simlock_data_len = params->simlock_data.data_len;
  }
  else if (params->simlock_data.data_len > QMI_UIM_SIMLOCK_DATA_MAX &&
           params->simlock_data.data_len <= QMI_UIM_SIMLOCK_DATA_EXT_MAX &&
           params->simlock_data.data_ptr != NULL)
  {
    qmi_request->simlock_data_ext_valid = TRUE;
    memcpy(qmi_request->simlock_data_ext,
           params->simlock_data.data_ptr,
           params->simlock_data.data_len);
    qmi_request->simlock_data_ext_len = params->simlock_data.data_len;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));

    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }

    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);
    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_REMOTE_UNLOCK_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(uim_remote_unlock_req_msg_v01),
                                    (void*) qmi_response,
                                    sizeof(uim_remote_unlock_resp_msg_v01),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_REMOTE_UNLOCK_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(uim_remote_unlock_req_msg_v01),
                                   (void*) qmi_response,
                                   sizeof(uim_remote_unlock_resp_msg_v01),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_remote_unlock_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_remote_unlock */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_terminal_capability
===========================================================================*/
/*!
@brief
  Issues the request for retrieving TLVs that the modem plans to send to
  the card as part of TERMINAL CAPABILITY, upon subsequent boot up.
  If the user_cb function pointer is set to NULL, this function will be invoked
  synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_get_terminal_capability
(
  qmi_client_type                                   client_handle,
  const qmi_uim_terminal_capability_params_type   * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                     rc;
  qmi_txn_handle                          txn_handle;
  uim_terminal_capability_req_msg_v01   * qmi_request   = NULL;
  uim_terminal_capability_resp_msg_v01  * qmi_response  = NULL;
  qmi_uim_cb_struct_type                * cb_data       = NULL;

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  qmi_request = (uim_terminal_capability_req_msg_v01*)qcril_malloc(sizeof(uim_terminal_capability_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_terminal_capability_resp_msg_v01*)qcril_malloc(
                   sizeof(uim_terminal_capability_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_terminal_capability_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_terminal_capability_resp_msg_v01));

  qmi_request->slot = (uim_slot_enum_v01)params->slot;
  qmi_request->terminal_capability_tlv_valid = (uint8_t) 0x00;

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_TERMINAL_CAPABILITY_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_TERMINAL_CAPABILITY_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_terminal_capability_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_get_terminal_capability */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_set_terminal_capability
===========================================================================*/
/*!
@brief
  Issues the request for setting/removing TLVs that the modem plans to send to
  the card as part of TERMINAL CAPABILITY, upon subsequent boot up.
  If the user_cb function pointer is set to NULL, this function will be invoked
  synchronously. Otherwise it will be invoked asynchronously.

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >=0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note

  - Dependencies
    - qcril_qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcril_qmi_uim_set_terminal_capability
(
  qmi_client_type                                   client_handle,
  const qmi_uim_terminal_capability_params_type   * params,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  int                                     rc;
  qmi_txn_handle                          txn_handle;
  uim_terminal_capability_req_msg_v01   * qmi_request   = NULL;
  uim_terminal_capability_resp_msg_v01  * qmi_response  = NULL;
  qmi_uim_cb_struct_type                * cb_data       = NULL;
  uint8_t                                 i             = 0;

  if (!params)
  {
    return QMI_SERVICE_ERR;
  }

  if (!user_cb)
  {
    if (!rsp_data)
    {
      return QMI_SERVICE_ERR;
    }
  }

  if(params->num_tlvs > QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX_V01 ||
     params->num_tlvs > QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_request = (uim_terminal_capability_req_msg_v01*)qcril_malloc(sizeof(uim_terminal_capability_req_msg_v01));
  if(qmi_request == NULL)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_response = (uim_terminal_capability_resp_msg_v01*)qcril_malloc(
                   sizeof(uim_terminal_capability_resp_msg_v01));
  if(qmi_response == NULL)
  {
    qcril_free(qmi_request);
    return QMI_SERVICE_ERR;
  }

  memset(qmi_request, 0, sizeof(uim_terminal_capability_req_msg_v01));
  memset(qmi_response, 0, sizeof(uim_terminal_capability_resp_msg_v01));

  qmi_request->slot = (uim_slot_enum_v01) params->slot;
  qmi_request->terminal_capability_tlv_valid = (uint8_t) 0x01;

  for (i = 0; i < params->num_tlvs && i < QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX; i++)
  {
    if(params->tlvs[i].value.data_ptr == NULL ||
       params->tlvs[i].value.data_len > sizeof(qmi_request->terminal_capability_tlv[i].value))
    {
      QCRIL_LOG_ERROR("%s", "Value of TLV invalid or larger than the max supported by QMIUIM");
        qcril_free(qmi_request);
        qcril_free(qmi_response);
        return QMI_SERVICE_ERR;
    }
    qmi_request->terminal_capability_tlv[i].tag = (uint8_t) params->tlvs[i].tag;
    qmi_request->terminal_capability_tlv[i].value_len = (uint32_t) params->tlvs[i].value.data_len;
    memcpy(qmi_request->terminal_capability_tlv[i].value,
           params->tlvs[i].value.data_ptr,
           params->tlvs[i].value.data_len);
    qmi_request->terminal_capability_tlv[i].remove_tlv = (params->tlvs[i].tlv_present ? 0x00 : 0x01);
  }
  qmi_request->terminal_capability_tlv_len = (uint8_t) params->num_tlvs;

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("%s", "failed to allocate cb_data");
      qcril_free(qmi_request);
      qcril_free(qmi_response);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_TERMINAL_CAPABILITY_REQ_V01,
                                    (void*) qmi_request,
                                    sizeof(*qmi_request),
                                    (void*) qmi_response,
                                    sizeof(*qmi_response),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( qmi_response );
      qcril_free( cb_data );
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_TERMINAL_CAPABILITY_REQ_V01,
                                   (void*) qmi_request,
                                   sizeof(*qmi_request),
                                   (void*) qmi_response,
                                   sizeof(*qmi_response),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qcril_uim_qmi_terminal_capability_resp(qmi_response, rsp_data);
    }
    qcril_free(qmi_response);
  }
  qcril_free(qmi_request);

  return rc;
} /* qcril_qmi_uim_set_terminal_capability */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_add_profile
===========================================================================*/
/*!
@brief
  LPA add profile

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_add_profile
(
  qmi_client_type                                   client_handle,
  const qmi_uim_add_profile_req_type              * params_ptr,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  qmi_txn_handle                        txn_handle;
  int                                   rc                     = 0;
  uim_add_profile_req_msg_v01           add_profile_req;
  uim_add_profile_resp_msg_v01        * add_profile_resp_ptr   = NULL;
  qmi_uim_cb_struct_type              * cb_data                = NULL;

  memset(&add_profile_req, 0x00, sizeof(add_profile_req));

  if (params_ptr == nullptr ||
      params_ptr->activation_code_ptr == nullptr ||
      strlen(params_ptr->activation_code_ptr) >= QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01)
  {
    QCRIL_LOG_ERROR("Invalid params_ptr or activation code");
    return QMI_SERVICE_ERR;
  }

  add_profile_req.slot = (uim_slot_enum_v01) params_ptr->slot;

  add_profile_req.activation_code_len = strlen(params_ptr->activation_code_ptr);
  memset(add_profile_req.activation_code,'\0', add_profile_req.activation_code_len);
  strlcpy(add_profile_req.activation_code,
          params_ptr->activation_code_ptr,
          sizeof(add_profile_req.activation_code));

  if (params_ptr->confirmation_code_ptr != NULL &&
      strlen(params_ptr->confirmation_code_ptr) != 0 &&
      strlen(params_ptr->confirmation_code_ptr) < QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01)
  {
    add_profile_req.confirmation_code_valid = TRUE;
    add_profile_req.confirmation_code_len   = strlen(params_ptr->confirmation_code_ptr);
    memset(add_profile_req.confirmation_code, '\0' , add_profile_req.confirmation_code_len);
    strlcpy(add_profile_req.confirmation_code,
            params_ptr->confirmation_code_ptr,
            sizeof(add_profile_req.confirmation_code));
  }
  else
  {
    add_profile_req.confirmation_code_valid = FALSE;
  }

  add_profile_req.user_consent_supported_valid = TRUE;
  add_profile_req.user_consent_supported = TRUE;

  /* Allocate response pointer since it is an async command */
  add_profile_resp_ptr = (uim_add_profile_resp_msg_v01 *)
                             qcril_malloc(sizeof(uim_add_profile_resp_msg_v01));
  if (add_profile_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("error allocating memory for add_profile_resp_ptr");
    return QMI_SERVICE_ERR;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("failed to allocate cb_data");
      qcril_free(add_profile_resp_ptr);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_ADD_PROFILE_REQ_V01,
                                    &add_profile_req,
                                    sizeof(uim_add_profile_req_msg_v01),
                                    (void *) add_profile_resp_ptr,
                                    sizeof(uim_add_profile_resp_msg_v01),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( cb_data );
      qcril_free(add_profile_resp_ptr);
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_ADD_PROFILE_REQ_V01,
                                   &add_profile_req,
                                   sizeof(uim_add_profile_req_msg_v01),
                                   (void *) add_profile_resp_ptr,
                                   sizeof(uim_add_profile_resp_msg_v01),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qmi_uim_add_profile_resp(add_profile_resp_ptr, rsp_data);
    }
    qcril_free(add_profile_resp_ptr);
  }

  return rc;
} /* qcril_qmi_uim_add_profile */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_set_sim_profile
===========================================================================*/
/*!
@brief
  LPA set SIM profile

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_set_sim_profile
(
  qmi_client_type                                   client_handle,
  const qmi_uim_set_sim_profile_req_type          * params_ptr,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  qmi_txn_handle                     txn_handle;
  int                                rc                      = 0;
  uim_set_sim_profile_req_msg_v01    en_dis_profile_req;
  uim_set_sim_profile_resp_msg_v01 * en_dis_profile_resp_ptr = NULL;
  qmi_uim_cb_struct_type           * cb_data                 = NULL;

  memset(&en_dis_profile_req, 0x00, sizeof(en_dis_profile_req));

  if (params_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid params_ptr or activation code");
    return QMI_SERVICE_ERR;
  }

  en_dis_profile_req.slot = (uim_slot_enum_v01)params_ptr->slot;
  en_dis_profile_req.profile_id = (uim_profile_id_enum_v01)params_ptr->profile_id;
  en_dis_profile_req.enable_profile_valid = TRUE;
  en_dis_profile_req.enable_profile = params_ptr->enable_profile;

  /* Allocate response pointer since it is an async command */
  en_dis_profile_resp_ptr = (uim_set_sim_profile_resp_msg_v01 *)
                              qcril_malloc(sizeof(uim_set_sim_profile_resp_msg_v01));
  if (en_dis_profile_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("error allocating memory for add_profile_resp_ptr");
    return QMI_SERVICE_ERR;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("failed to allocate cb_data");
      qcril_free(en_dis_profile_resp_ptr);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_SET_SIM_PROFILE_REQ_V01,
                                    &en_dis_profile_req,
                                    sizeof(uim_set_sim_profile_req_msg_v01),
                                    (void *) en_dis_profile_resp_ptr,
                                    sizeof(uim_set_sim_profile_resp_msg_v01),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( cb_data );
      qcril_free(en_dis_profile_resp_ptr);
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_SET_SIM_PROFILE_REQ_V01,
                                   &en_dis_profile_req,
                                   sizeof(uim_set_sim_profile_req_msg_v01),
                                   (void *) en_dis_profile_resp_ptr,
                                   sizeof(uim_set_sim_profile_resp_msg_v01),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qmi_uim_set_sim_profile_resp(en_dis_profile_resp_ptr, rsp_data);
    }
    qcril_free(en_dis_profile_resp_ptr);
  }

  return rc;
} /* qcril_qmi_uim_set_sim_profile */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_sim_profiles
===========================================================================*/
/*!
@brief
  LPA get SIM profiles

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_get_sim_profiles
(
  qmi_client_type                                   client_handle,
  const qmi_uim_get_sim_profiles_type             * params_ptr,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  qmi_txn_handle                     txn_handle;
  int                                rc                     = 0;
  uim_get_sim_profile_req_msg_v01    get_profiles_req;
  uim_get_sim_profile_resp_msg_v01 * get_profiles_resp_ptr  = NULL;
  qmi_uim_cb_struct_type           * cb_data                = NULL;

  memset(&get_profiles_req, 0x00, sizeof(get_profiles_req));

  if (params_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid params_ptr or activation code");
    return QMI_SERVICE_ERR;
  }

  get_profiles_req.slot = (uim_slot_enum_v01)params_ptr->slot;

  /* Allocate response pointer since it is an async command */
  get_profiles_resp_ptr = (uim_get_sim_profile_resp_msg_v01 *)
                              qcril_malloc(sizeof(uim_get_sim_profile_resp_msg_v01));
  if (get_profiles_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("error allocating memory for add_profile_resp_ptr");
    return QMI_SERVICE_ERR;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("failed to allocate cb_data");
      qcril_free(get_profiles_resp_ptr);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_GET_SIM_PROFILE_REQ_V01,
                                    &get_profiles_req,
                                    sizeof(uim_get_sim_profile_req_msg_v01),
                                    (void *) get_profiles_resp_ptr,
                                    sizeof(uim_get_sim_profile_resp_msg_v01),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( cb_data );
      qcril_free(get_profiles_resp_ptr);
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_GET_SIM_PROFILE_REQ_V01,
                                   &get_profiles_req,
                                   sizeof(uim_get_sim_profile_req_msg_v01),
                                   (void *) get_profiles_resp_ptr,
                                   sizeof(uim_get_sim_profile_resp_msg_v01),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qmi_uim_get_profiles_resp(get_profiles_resp_ptr, rsp_data);
    }
    qcril_free(get_profiles_resp_ptr);
  }

  return rc;
} /* qcril_qmi_uim_get_sim_profiles */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_delete_profile
===========================================================================*/
/*!
@brief
  LPA delete SIM profile

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_delete_profile
(
  qmi_client_type                                   client_handle,
  const qmi_uim_delete_profile_req_type           * params_ptr,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  qmi_txn_handle                     txn_handle;
  int                                rc                   = 0;
  uim_delete_profile_req_msg_v01     del_profile_req;
  uim_delete_profile_resp_msg_v01  * del_profile_resp_ptr = NULL;
  qmi_uim_cb_struct_type           * cb_data              = NULL;

  memset(&del_profile_req, 0x00, sizeof(del_profile_req));

  if (params_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid params_ptr or activation code");
    return QMI_SERVICE_ERR;
  }

  del_profile_req.slot = (uim_slot_enum_v01)params_ptr->slot;
  del_profile_req.profile_id = (uim_profile_id_enum_v01)params_ptr->profile_id;

  /* Allocate response pointer since it is an async command */
  del_profile_resp_ptr = (uim_delete_profile_resp_msg_v01 *)
                              qcril_malloc(sizeof(uim_delete_profile_resp_msg_v01));
  if (del_profile_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("error allocating memory for add_profile_resp_ptr");
    return QMI_SERVICE_ERR;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("failed to allocate cb_data");
      qcril_free(del_profile_resp_ptr);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_DELETE_PROFILE_REQ_V01,
                                    &del_profile_req,
                                    sizeof(uim_delete_profile_req_msg_v01),
                                    (void *) del_profile_resp_ptr,
                                    sizeof(uim_delete_profile_resp_msg_v01),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( cb_data );
      qcril_free(del_profile_resp_ptr);
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_DELETE_PROFILE_REQ_V01,
                                   &del_profile_req,
                                   sizeof(uim_delete_profile_req_msg_v01),
                                   (void *) del_profile_resp_ptr,
                                   sizeof(uim_delete_profile_resp_msg_v01),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qmi_uim_delete_profile_resp(del_profile_resp_ptr, rsp_data);
    }
    qcril_free(del_profile_resp_ptr);
  }

  return rc;
} /* qcril_qmi_uim_delete_profile */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_update_profile_nickname
===========================================================================*/
/*!
@brief
  LPA update profile nickname

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_update_profile_nickname
(
  qmi_client_type                                   client_handle,
  const qmi_uim_update_profile_nickname_req_type  * params_ptr,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  qmi_txn_handle                             txn_handle;
  int                                        rc                 = 0;
  uim_update_profile_nickname_req_msg_v01    nick_name_req;
  uim_update_profile_nickname_resp_msg_v01 * nick_name_resp_ptr = NULL;
  qmi_uim_cb_struct_type                   * cb_data            = NULL;

  memset(&nick_name_req, 0x00, sizeof(nick_name_req));

  if (params_ptr == nullptr ||
      params_ptr->nickname == nullptr ||
      strlen(params_ptr->nickname) > QMI_UIM_EUICC_NICKNAME_MAX_V01)
  {
    QCRIL_LOG_ERROR("Invalid params_ptr or activation code");
    return QMI_SERVICE_ERR;
  }

  nick_name_req.slot = (uim_slot_enum_v01) params_ptr->slot;
  nick_name_req.profile_id = (uim_profile_id_enum_v01)params_ptr->profile_id;

  if (strlen(params_ptr->nickname) > 0)
  {
    nick_name_req.nickname_valid = TRUE;
    nick_name_req.nickname_len = strlen(params_ptr->nickname);
    memset(nick_name_req.nickname, 0 , nick_name_req.nickname_len);
    memcpy(nick_name_req.nickname, params_ptr->nickname, nick_name_req.nickname_len);
  }

  /* Allocate response pointer since it is an async command */
  nick_name_resp_ptr = (uim_update_profile_nickname_resp_msg_v01 *)
                             qcril_malloc(sizeof(uim_update_profile_nickname_resp_msg_v01));
  if (nick_name_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("error allocating memory for add_profile_resp_ptr");
    return QMI_SERVICE_ERR;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("failed to allocate cb_data");
      qcril_free(nick_name_resp_ptr);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_UPDATE_PROFILE_NICKNAME_REQ_V01,
                                    &nick_name_req,
                                    sizeof(uim_update_profile_nickname_req_msg_v01),
                                    (void *) nick_name_resp_ptr,
                                    sizeof(uim_update_profile_nickname_resp_msg_v01),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( cb_data );
      qcril_free(nick_name_resp_ptr);
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_UPDATE_PROFILE_NICKNAME_REQ_V01,
                                   &nick_name_req,
                                   sizeof(uim_update_profile_nickname_req_msg_v01),
                                   (void *) nick_name_resp_ptr,
                                   sizeof(uim_update_profile_nickname_resp_msg_v01),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qmi_uim_update_profile_nickname_resp(nick_name_resp_ptr, rsp_data);
    }
    qcril_free(nick_name_resp_ptr);
  }

  return rc;
} /* qcril_qmi_uim_update_profile_nickname */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_euicc_memory_reset
===========================================================================*/
/*!
@brief
  LPA euicc memory reset

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_euicc_memory_reset
(
  qmi_client_type                                   client_handle,
  const qmi_uim_euicc_memory_reset_req_type       * params_ptr,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  qmi_txn_handle                        txn_handle;
  int                                   rc                     = 0;
  uim_euicc_memory_reset_req_msg_v01    memory_reset_req;
  uim_euicc_memory_reset_resp_msg_v01 * memory_reset_resp_ptr  = NULL;
  qmi_uim_cb_struct_type              * cb_data                = NULL;

  memset(&memory_reset_req, 0x00, sizeof(memory_reset_req));

  if (params_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid params_ptr or activation code");
    return QMI_SERVICE_ERR;
  }

  memory_reset_req.slot = (uim_slot_enum_v01)params_ptr->slot;
  memory_reset_req.reset_option_valid = TRUE;
  memory_reset_req.reset_option = params_ptr->reset_option;

  /* Allocate response pointer since it is an async command */
  memory_reset_resp_ptr = (uim_euicc_memory_reset_resp_msg_v01 *)
                              qcril_malloc(sizeof(uim_euicc_memory_reset_resp_msg_v01));
  if (memory_reset_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("error allocating memory for add_profile_resp_ptr");
    return QMI_SERVICE_ERR;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("failed to allocate cb_data");
      qcril_free(memory_reset_resp_ptr);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_EUICC_MEMORY_RESET_REQ_V01,
                                    &memory_reset_req,
                                    sizeof(uim_euicc_memory_reset_req_msg_v01),
                                    (void *) memory_reset_resp_ptr,
                                    sizeof(uim_euicc_memory_reset_resp_msg_v01),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( cb_data );
      qcril_free(memory_reset_resp_ptr);
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_EUICC_MEMORY_RESET_REQ_V01,
                                   &memory_reset_req,
                                   sizeof(uim_euicc_memory_reset_req_msg_v01),
                                   (void *) memory_reset_resp_ptr,
                                   sizeof(uim_euicc_memory_reset_resp_msg_v01),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qmi_uim_euicc_memory_reset_resp(memory_reset_resp_ptr, rsp_data);
    }
    qcril_free(memory_reset_resp_ptr);
  }

  return rc;
} /* qcril_qmi_uim_euicc_memory_reset */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_user_consent
===========================================================================*/
/*!
@brief
  LPA update profile nickname

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_user_consent
(
  qmi_client_type                                   client_handle,
  const qmi_uim_profile_user_consent_req_type     * params_ptr,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  qmi_txn_handle                          txn_handle;
  int                                     rc                     = 0;
  uim_profile_user_consent_req_msg_v01    user_consent_req;
  uim_profile_user_consent_resp_msg_v01 * user_consent_resp_ptr  = NULL;
  qmi_uim_cb_struct_type                * cb_data                = NULL;

  memset(&user_consent_req, 0x00, sizeof(user_consent_req));

  if (params_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid params_ptr or activation code");
    return QMI_SERVICE_ERR;
  }

  user_consent_req.slot = (uim_slot_enum_v01)params_ptr->slot;
  user_consent_req.user_ok = params_ptr->user_ok;

  /* Allocate response pointer since it is an async command */
  user_consent_resp_ptr = (uim_profile_user_consent_resp_msg_v01 *)
                              qcril_malloc(sizeof(uim_profile_user_consent_resp_msg_v01));
  if (user_consent_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("error allocating memory for add_profile_resp_ptr");
    return QMI_SERVICE_ERR;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("failed to allocate cb_data");
      qcril_free(user_consent_resp_ptr);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_PROFILE_USER_CONSENT_REQ_V01,
                                    &user_consent_req,
                                    sizeof(uim_profile_user_consent_req_msg_v01),
                                    (void *) user_consent_resp_ptr,
                                    sizeof(uim_profile_user_consent_resp_msg_v01),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( cb_data );
      qcril_free(user_consent_resp_ptr);
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_PROFILE_USER_CONSENT_REQ_V01,
                                   &user_consent_req,
                                   sizeof(uim_profile_user_consent_req_msg_v01),
                                   (void *) user_consent_resp_ptr,
                                   sizeof(uim_profile_user_consent_resp_msg_v01),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qmi_uim_user_consent_resp(user_consent_resp_ptr, rsp_data);
    }
    qcril_free(user_consent_resp_ptr);
  }

  return rc;
} /* qcril_qmi_uim_user_consent */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_eid
===========================================================================*/
/*!
@brief
  LPA get eid

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_get_eid
(
  qmi_client_type                                   client_handle,
  const qmi_uim_get_eid_req_type                  * params_ptr,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  qmi_txn_handle                        txn_handle;
  int                                   rc           = 0;
  uim_get_eid_req_msg_v01               eid_req;
  uim_get_eid_resp_msg_v01            * eid_resp_ptr = NULL;
  qmi_uim_cb_struct_type              * cb_data      = NULL;

  memset(&eid_req, 0x00, sizeof(eid_req));

  if (params_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid params_ptr or activation code");
    return QMI_SERVICE_ERR;
  }

  eid_req.slot = (uim_slot_enum_v01)params_ptr->slot;

  /* Allocate response pointer since it is an async command */
  eid_resp_ptr = (uim_get_eid_resp_msg_v01 *)
                     qcril_malloc(sizeof(uim_get_eid_resp_msg_v01));
  if (eid_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("error allocating memory for add_profile_resp_ptr");
    return QMI_SERVICE_ERR;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("failed to allocate cb_data");
      qcril_free(eid_resp_ptr);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_GET_EID_REQ_V01,
                                    &eid_req,
                                    sizeof(uim_get_eid_req_msg_v01),
                                    (void *) eid_resp_ptr,
                                    sizeof(uim_get_eid_resp_msg_v01),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( cb_data );
      qcril_free(eid_resp_ptr);
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_GET_EID_REQ_V01,
                                   &eid_req,
                                   sizeof(uim_get_eid_req_msg_v01),
                                   (void *) eid_resp_ptr,
                                   sizeof(uim_get_eid_resp_msg_v01),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qmi_uim_get_eid_resp(eid_resp_ptr, rsp_data);
    }
    qcril_free(eid_resp_ptr);
  }

  return rc;
} /* qcril_qmi_uim_get_eid */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_set_server_address
===========================================================================*/
/*!
@brief
  LPA get set server address

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_get_set_server_address
(
  qmi_client_type                                       client_handle,
  const qmi_uim_euicc_default_server_address_req_type * params_ptr,
  qmi_uim_user_async_cb_type                            user_cb,
  void                                                * user_data,
  qmi_uim_rsp_data_type                               * rsp_data
)
{
  qmi_txn_handle                                  txn_handle;
  int                                             rc                      = 0;
  uim_euicc_default_server_address_req_msg_v01    server_address_req;
  uim_euicc_default_server_address_resp_msg_v01 * server_address_resp_ptr = NULL;
  qmi_uim_cb_struct_type                        * cb_data                 = NULL;

  memset(&server_address_req, 0x00, sizeof(server_address_req));

  if (params_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid params_ptr or activation code");
    return QMI_SERVICE_ERR;
  }

  server_address_req.slot = (uim_slot_enum_v01) params_ptr->slot;

  if (params_ptr->default_smdp_address != nullptr)
  {  
    if (strlen(params_ptr->default_smdp_address) > QMI_UIM_FQDN_MAX_V01)
    {
      return QMI_SERVICE_ERR;
    }
    else
    {
      server_address_req.default_smdp_address_valid = TRUE;
      server_address_req.default_smdp_address_len = strlen(params_ptr->default_smdp_address);
      memcpy(server_address_req.default_smdp_address,
             params_ptr->default_smdp_address,
             server_address_req.default_smdp_address_len);
    }
  }

  /* Allocate response pointer since it is an async command */
  server_address_resp_ptr = (uim_euicc_default_server_address_resp_msg_v01 *)
                               qcril_malloc(sizeof(uim_euicc_default_server_address_resp_msg_v01));
  if (server_address_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("error allocating memory for add_profile_resp_ptr");
    return QMI_SERVICE_ERR;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("failed to allocate cb_data");
      qcril_free(server_address_resp_ptr);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_EUICC_DEFAULT_SERVER_ADDRESS_REQ_V01,
                                    &server_address_req,
                                    sizeof(uim_euicc_default_server_address_req_msg_v01),
                                    (void *) server_address_resp_ptr,
                                    sizeof(uim_euicc_default_server_address_resp_msg_v01),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( cb_data );
      qcril_free(server_address_resp_ptr);
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_EUICC_DEFAULT_SERVER_ADDRESS_REQ_V01,
                                   &server_address_req,
                                   sizeof(uim_euicc_default_server_address_req_msg_v01),
                                   (void *) server_address_resp_ptr,
                                   sizeof(uim_euicc_default_server_address_resp_msg_v01),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qmi_uim_get_set_server_address_resp(server_address_resp_ptr, rsp_data);
    }
    qcril_free(server_address_resp_ptr);
  }

  return rc;
} /* qcril_qmi_uim_get_set_server_address */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_get_profile_info
===========================================================================*/
/*!
@brief
  LPA get profile info

@return
  In the synchronous case, if return code < 0, the operation failed.  In
  the failure case, if the return code is QMI_SERVICE_ERR, then the
  qmi_err_code value will give you the QMI error reason.  Otherwise,
  qmi_err_code will have meaningless data.

  In the asynchronous case, if the return code < 0, the operation failed and
  you will not get an asynchronous response.  If the operation doesn't fail
  (return code >= 0), the returned value will be a transaction handle which
  can be used to cancel the transaction via the qmi_uim_abort() command.

@note
  - Dependencies
    - qmi_uim_srvc_init_client() must be called before calling this.

  - Side Effects
    - None
*/
/*=========================================================================*/
EXTERN int qcril_qmi_uim_get_profile_info
(
  qmi_client_type                                   client_handle,
  const qmi_uim_get_profile_info_req_type         * params_ptr,
  qmi_uim_user_async_cb_type                        user_cb,
  void                                            * user_data,
  qmi_uim_rsp_data_type                           * rsp_data
)
{
  qmi_txn_handle                       txn_handle;
  int                                  rc                    = 0;
  uim_get_profile_info_req_msg_v01     profile_info_req;
  uim_get_profile_info_resp_msg_v01  * profile_info_resp_ptr = NULL;
  qmi_uim_cb_struct_type             * cb_data               = NULL;

  memset(&profile_info_req, 0x00, sizeof(profile_info_req));

  if (params_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid params_ptr or activation code");
    return QMI_SERVICE_ERR;
  }

  profile_info_req.slot = (uim_slot_enum_v01)params_ptr->slot;
  profile_info_req.profile_id = (uim_profile_id_enum_v01)params_ptr->profile_id;

  /* Allocate response pointer since it is an async command */
  profile_info_resp_ptr = (uim_get_profile_info_resp_msg_v01 *)
                              qcril_malloc(sizeof(uim_get_profile_info_resp_msg_v01));
  if (profile_info_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("error allocating memory for add_profile_resp_ptr");
    return QMI_SERVICE_ERR;
  }

  if (user_cb)
  {
    cb_data = (qmi_uim_cb_struct_type*)qcril_malloc(sizeof(qmi_uim_cb_struct_type));
    if(cb_data == NULL)
    {
      QCRIL_LOG_ERROR("failed to allocate cb_data");
      qcril_free(profile_info_resp_ptr);
      return QMI_SERVICE_ERR;
    }
    QMI_UIM_MAKE_CB_STRUCT(cb_data, user_cb, user_data);

    rc = qmi_client_send_msg_async( client_handle,
                                    QMI_UIM_GET_PROFILE_INFO_REQ_V01,
                                    &profile_info_req,
                                    sizeof(uim_get_profile_info_req_msg_v01),
                                    (void *) profile_info_resp_ptr,
                                    sizeof(uim_get_profile_info_resp_msg_v01),
                                    qmi_uim_client_async_cb,
                                    cb_data,
                                    &txn_handle);
    if( rc != QMI_NO_ERR )
    {
      qcril_free( cb_data );
      qcril_free(profile_info_resp_ptr);
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync( client_handle,
                                   QMI_UIM_GET_PROFILE_INFO_REQ_V01,
                                   &profile_info_req,
                                   sizeof(uim_get_profile_info_req_msg_v01),
                                   (void *) profile_info_resp_ptr,
                                   sizeof(uim_get_profile_info_resp_msg_v01),
                                   QMI_UIM_DEFAULT_TIMEOUT);
    if(!rc)
    {
      qmi_uim_get_profile_info_resp(profile_info_resp_ptr, rsp_data);
    }
    qcril_free(profile_info_resp_ptr);
  }

  return rc;
} /* qcril_qmi_uim_get_profile_info */
