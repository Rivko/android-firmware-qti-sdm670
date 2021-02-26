/*===========================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "comdef.h"
#include "diag_lsm.h"
#include "remote_simlock.h"
#include "user_identity_module_v01.h"
#include "common_v01.h"
#include "qmi_client.h"
#include "qmi_client_instance_defs.h"
#include "qmi_cci_target_ext.h"

/*===========================================================================

                           DEFINES

===========================================================================*/

#define REMOTE_SIMLOCK_LOG_ERROR(...) \
  printf(__VA_ARGS__); \
  printf("\n")

#define REMOTE_SIMLOCK_LOG_INFO(...) \
  printf(__VA_ARGS__); \
  printf("\n")

#define TRUE                                               1
#define FALSE                                              0

#define REMOTE_SIMLOCK_SUCCESS                             0x00
#define REMOTE_SIMLOCK_ERROR                               0x01

#define REMOTE_SIMLOCK_GET_MODEM_STATUS_BUFFER_MIN_LEN     1

#define REMOTE_SIMLOCK_QMI_MAX_TIMEOUT                     5000
#define REMOTE_SIMLOCK_QMI_CLIENT_SRV_REG_TIMEOUT          5000

static qmi_client_type  remote_simlock_client_handle = NULL;

/*===========================================================================

                           FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  remote_simlock_malloc

===========================================================================*/
/*!
    @brief
    Utility to allocate memory

    @return
    Pointer to the memory allocated
*/
/*=========================================================================*/
static void* remote_simlock_malloc(
  size_t size
)
{
  void *temp_ptr = NULL;

  if(0 < size)
  {
    temp_ptr = malloc(size);
    if(temp_ptr)
    {
      memset(temp_ptr, 0x00, size);
    }
    else
    {
      REMOTE_SIMLOCK_LOG_ERROR("Failed to allocate memory");
    }
  }

  return temp_ptr;
} /* remote_simlock_malloc */


/*===========================================================================

  FUNCTION:  remote_simlock_free

===========================================================================*/
/*!
    @brief
    Utility to free memory allocated with remote_simlock_malloc

    @return
    None
*/
/*=========================================================================*/
static void remote_simlock_free(
  void** to_be_freed_memory_ptr
)
{
  if(to_be_freed_memory_ptr)
  {
    if(NULL != *to_be_freed_memory_ptr)
    {
      free(*to_be_freed_memory_ptr);
      *to_be_freed_memory_ptr = NULL;
    }
  }
} /* remote_simlock_free */


/*===========================================================================

  FUNCTION:  Connect_to_Modem

===========================================================================*/
/*!
    @brief
    This function is to establish a connection with modem

    @return
    uint8_t
    REMOTE_SIMLOCK_ERROR     Could not successfully establish the connection
    REMOTE_SIMLOCK_SUCCESS   Successfully established connection
*/
/*=========================================================================*/
static uint8_t Connect_to_Modem(
  void
)
{
  qmi_client_error_type       qmi_status     = QMI_NO_ERR;
  boolean                     ret_val        = 0;
  qmi_idl_service_object_type service_object = uim_get_service_object_v01();
  qmi_client_os_params        os_params;

  memset(&os_params, 0x00, sizeof(os_params));

  if(remote_simlock_client_handle != NULL)
  {
    return REMOTE_SIMLOCK_SUCCESS;
  }

  /* Initialize Diag for QCRIL logging */
  ret_val = Diag_LSM_Init(NULL);
  if (!ret_val )
  {
    REMOTE_SIMLOCK_LOG_ERROR("Fail to initialize Diag");
  }

  qmi_status = qmi_client_init_instance(service_object,
                                        QMI_CLIENT_INSTANCE_ANY,
                                        NULL,
                                        NULL,
                                        &os_params,
                                        REMOTE_SIMLOCK_QMI_CLIENT_SRV_REG_TIMEOUT,
                                        &remote_simlock_client_handle);
  if (qmi_status != QMI_NO_ERR)
  {
    REMOTE_SIMLOCK_LOG_ERROR("Modem connect failure, qmi error %d", qmi_status);
    return REMOTE_SIMLOCK_ERROR;
  }

  return REMOTE_SIMLOCK_SUCCESS;
} /* Connect_to_Modem */


/*===========================================================================

  FUNCTION:  Disconnect_from_Modem

===========================================================================*/
/*!
    @brief
    This function is to disconnect from modem

    @return
    uint8_t
    REMOTE_SIMLOCK_ERROR     Could not successfully disconnect
    REMOTE_SIMLOCK_SUCCESS   Successfully disconnected
*/
/*=========================================================================*/
static uint8_t Disconnect_from_Modem(
  void
)
{
  qmi_client_error_type      qmi_status = QMI_NO_ERR;

  if(remote_simlock_client_handle == NULL)
  {
    return REMOTE_SIMLOCK_SUCCESS;
  }

  qmi_status = qmi_client_release(remote_simlock_client_handle);
  if (qmi_status != QMI_NO_ERR)
  {
    REMOTE_SIMLOCK_LOG_ERROR("Modem disconnect failure, qmi error %d", qmi_status);
    return REMOTE_SIMLOCK_ERROR;
  }

  remote_simlock_client_handle = NULL;

  return REMOTE_SIMLOCK_SUCCESS;
} /* Disconnect_from_Modem */


/*===========================================================================

  FUNCTION:  Modem_Remote_Simlock

===========================================================================*/
/*!
    @brief
    This function is to send a simlock setting request(lock or unlock)
    to modem

    @return
    uint8_t
    REMOTE_SIMLOCK_SUCCESS     Lock or Unlock is successfully applied
    REMOTE_SIMLOCK_ERROR       Could not successfully apply the lock or unlock
*/
/*=========================================================================*/
uint8_t Modem_Remote_Simlock(
  uint8_t           * buffer_ptr,
  uint32_t            buffer_len
)
{
  qmi_client_error_type            qmi_status = QMI_NO_ERR;
  uim_remote_unlock_req_msg_v01  * req_ptr    = NULL;
  uim_remote_unlock_resp_msg_v01 * resp_ptr   = NULL;
  uint8                            status     = REMOTE_SIMLOCK_SUCCESS;
  boolean                          disconnect = FALSE;

  if(buffer_ptr == NULL ||
     buffer_len == 0 ||
     buffer_len > QMI_UIM_SIMLOCK_DATA_MAX_V01)
  {
    return REMOTE_SIMLOCK_ERROR;
  }

  req_ptr = remote_simlock_malloc(sizeof(uim_remote_unlock_req_msg_v01));
  if(req_ptr == NULL)
  {
    return REMOTE_SIMLOCK_ERROR;
  }

  resp_ptr = remote_simlock_malloc(sizeof(uim_remote_unlock_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    remote_simlock_free((void**)&req_ptr);
    return REMOTE_SIMLOCK_ERROR;
  }

  if(remote_simlock_client_handle == NULL)
  {
    /* In the case where the daemon has not connected to the modem, attempt to
       connect to the modem before running the command. Also assume the daemon
       does not disconnect from the modem so disconnect from the modem after
       the command. */
    if(Connect_to_Modem() != REMOTE_SIMLOCK_SUCCESS)
    {
      remote_simlock_free((void**)&req_ptr);
      remote_simlock_free((void**)&resp_ptr);
      return REMOTE_SIMLOCK_ERROR;
    }
    disconnect = TRUE;
  }

  memset(req_ptr, 0x00, sizeof(uim_remote_unlock_req_msg_v01));
  memset(resp_ptr, 0x00, sizeof(uim_remote_unlock_resp_msg_v01));

  req_ptr->simlock_data_valid = TRUE;
  req_ptr->simlock_data_len = buffer_len;
  memcpy(req_ptr->simlock_data,
         buffer_ptr,
         buffer_len);

  qmi_status = qmi_client_send_msg_sync(remote_simlock_client_handle,
                                        QMI_UIM_REMOTE_UNLOCK_REQ_V01,
                                        req_ptr,
                                        sizeof(uim_remote_unlock_req_msg_v01),
                                        resp_ptr,
                                        sizeof(uim_remote_unlock_resp_msg_v01),
                                        REMOTE_SIMLOCK_QMI_MAX_TIMEOUT);

  if(disconnect)
  {
    (void)Disconnect_from_Modem();
  }

  if(qmi_status != QMI_NO_ERR ||
     resp_ptr->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    REMOTE_SIMLOCK_LOG_INFO("Remote_unlock_req error qmi: 0x%x, error: 0x%x",
                             qmi_status, resp_ptr->resp.error);
    status = REMOTE_SIMLOCK_ERROR;
  }

  remote_simlock_free((void**)&req_ptr);
  remote_simlock_free((void**)&resp_ptr);

  return status;
}/* Modem_Remote_Simlock */


/*===========================================================================

  FUNCTION:  Modem_Remote_Simlock_Get_Status

===========================================================================*/
/*!
    @brief
    This function is to know the status of simlock settings
    buffer_ptr would be filled inside this function
      0x00 : If the modem is not locked to any of the categories
      0x01 : If the modem is locked to at least one of the categories
    buffer_ptr should be checked only if this function returns REMOTE_SIMLOCK_SUCCESS

    @return
    uint8_t
    REMOTE_SIMLOCK_SUCCESS     Successful in getting current simlock settings
    REMOTE_SIMLOCK_ERROR       Error while getting current simlock settings
*/
/*=========================================================================*/
uint8_t Modem_Remote_Simlock_Get_Status(
  uint8_t           * buffer_ptr,
  uint32_t            buffer_len
)
{
  qmi_client_error_type                qmi_status      = QMI_NO_ERR;
  uim_get_configuration_req_msg_v01    config_req;
  uim_get_configuration_resp_msg_v01 * config_resp_ptr = NULL;
  boolean                              disconnect      = FALSE;
  uint8                                status          = REMOTE_SIMLOCK_SUCCESS;

  if(buffer_ptr == NULL ||
     buffer_len < REMOTE_SIMLOCK_GET_MODEM_STATUS_BUFFER_MIN_LEN)
  {
    return REMOTE_SIMLOCK_ERROR;
  }

  memset(buffer_ptr, 0x00, buffer_len);

  config_resp_ptr = remote_simlock_malloc(sizeof(uim_get_configuration_resp_msg_v01));
  if(config_resp_ptr == NULL)
  {
    return REMOTE_SIMLOCK_ERROR;
  }

  memset(&config_req, 0x00, sizeof(uim_get_configuration_req_msg_v01));
  memset(config_resp_ptr, 0x00, sizeof(uim_get_configuration_resp_msg_v01));

  if(remote_simlock_client_handle == NULL)
  {
    /* In the case where the daemon has not connected to the modem, attempt to
       connect to the modem before running the command. Also assume the daemon
       does not disconnect from the modem so disconnect from the modem after
       the command. */
    if(Connect_to_Modem() != REMOTE_SIMLOCK_SUCCESS)
    {
      remote_simlock_free((void**)&config_resp_ptr);
      return REMOTE_SIMLOCK_ERROR;
    }
    disconnect = TRUE;
  }

  config_req.configuration_mask_valid = TRUE;
  config_req.configuration_mask = UIM_GET_CONFIGURATION_PERSONALIZATION_STATUS_V01;

  /* Send get_configuration request to modem */
  qmi_status = qmi_client_send_msg_sync(remote_simlock_client_handle,
                                        QMI_UIM_GET_CONFIGURATION_REQ_V01,
                                        &config_req,
                                        sizeof(uim_get_configuration_req_msg_v01),
                                        config_resp_ptr,
                                        sizeof(uim_get_configuration_resp_msg_v01),
                                        REMOTE_SIMLOCK_QMI_MAX_TIMEOUT);

  if(disconnect)
  {
    (void)Disconnect_from_Modem();
  }

  /* Return error if
     a. Request to modem is not placed successfully
     b. Could not get successful response from modem
     c. No personalization status in response message */
  if (qmi_status != QMI_NO_ERR ||
      config_resp_ptr->resp.result != QMI_RESULT_SUCCESS_V01 ||
      !config_resp_ptr->personalization_status_valid)
  {
    REMOTE_SIMLOCK_LOG_INFO("Get_config error qmi: 0x%x, error: 0x%x personalization status 0x%x",
                             qmi_status,
                             config_resp_ptr->resp.error,
                             config_resp_ptr->personalization_status_valid);
    status = REMOTE_SIMLOCK_ERROR;
  }
  else if(config_resp_ptr->personalization_status_len > 0)
  {
    /* If the personalization status is greater than zero, at least one category is enabled */
    *buffer_ptr = 0x01;
  }
  else
  {
    /* No category is enabled */
    *buffer_ptr = 0x00;
  }

  remote_simlock_free((void**)&config_resp_ptr);
  return status;
}/* Modem_Remote_Simlock_Get_Status */


