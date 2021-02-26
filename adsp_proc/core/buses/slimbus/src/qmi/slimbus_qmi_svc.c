/*
===========================================================================

FILE:         slimbus_qmi_svc.c

DESCRIPTION:  This file implements the SLIMbus QMI service for QuRT.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/slimbus/src/qmi/slimbus_qmi_svc.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/04/17   RC      Decreased the slimbus qmi thread priority less than the 
                   audio threads.
11/18/16   MJS     Adding QMI deferred response command.
03/11/16   MJS     Return an error code for device open error.
01/19/16   MJS     Add support for second Slimbus instance.
03/18/15   MJS     Fix compiler warnings.
12/06/13   MJS     Fix compiler warning.
11/07/13   MJS     Support for check framer request.
01/25/13   MJS     Update QMI defines.
01/23/13   MJS     Increase stack size to 4K.
10/16/12   MJS     Initial revision.

===========================================================================
             Copyright (c) 2012 - 2017 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/*=====================================================================
                               INCLUDES
=======================================================================*/
#include "qmi_csi.h"
#include "slimbus_v01.h"
#include "qurt.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "ddislimbus.h"

/*=======================================================================
                  INTERNAL DEFINITIONS AND TYPES
========================================================================*/

#define SLIMBUS_QMI_SVC_WAIT_SIG 0x00010000
#define SLIMBUS_QMI_SERVICE_STACK_SIZE  (4096)

/* Structure to hold information about clients that have
   connected to the service */
typedef struct 
{
  qmi_client_handle client;
  DalDeviceHandle *hSB;
  uint32 uInst;
  uint32 bRespValid;
  qmi_response_type_v01 resp;
  qmi_req_handle req_handle;
} client_info_type;

/* Structure to hold service context */
typedef struct
{
  /** QMI service handle */
  qmi_csi_service_handle service_handle;
  /** Number of requests that the service has handled */
  uint32 num_requests;
} service_context_type;

extern uint32 SlimBus_SwitchMasterMode(DalDeviceHandle *h, uint32 bMaster);
extern void SlimBus_CheckFramerState(DalDeviceHandle *h);

/*========================================================================
                        GLOBAL VARIABLES
========================================================================*/

/* service thread structures */
static qurt_anysignal_t     slimbus_qmi_service_sig;
static qurt_thread_t        slimbus_qmi_service_tcb;
static qurt_thread_attr_t   slimbus_qmi_service_attr;

/* Supported Slimbus DAL device IDs */
static const uint32 uaDevIds[] = { DALDEVICEID_SLIMBUS_1, DALDEVICEID_SLIMBUS_2 };

/*===========================================================================
                               FUNCTIONS
===========================================================================*/


/**
 * @brief Callback when a client connects to the Slimbus service
 *
 * This function implements the callback for when a client 
 * connects to the Slimbus QMI service 
 * 
 * @param[in] client_handle  QMI client handle
 * @param[in] service_cookie  Pointer to the service cookie
 * @param[out] connection_handle  Pointer to the location to 
 *       store the client connection handle
 * 
 * @return QMI_CSI_CB_NO_ERR on success, error code on error
 */
static qmi_csi_cb_error slimbus_connect_cb
(
  qmi_client_handle         client_handle,
  void                      *service_cookie,
  void                      **connection_handle
)
{
  client_info_type *client_info = NULL;

 /* Assign client_handle pointer to connection_handle so that it can be
    used in the handle_req_cb to send indications if necessary */
  if(!connection_handle)
  {
    return QMI_CSI_CB_CONN_REFUSED;
  }

  DALSYS_Malloc(sizeof(*client_info), (void**)&client_info);
  if(!client_info)
  {
    return QMI_CSI_CB_CONN_REFUSED;
  }
  DALSYS_memset(client_info, 0, sizeof(*client_info));
  client_info->client = client_handle;
  
  *connection_handle = (void*)client_info;

  return QMI_CSI_CB_NO_ERR;
}


/**
 * @brief Callback when a client disconnects from the Slimbus 
 *        service
 *
 * This function implements the callback for when a client 
 * disconnects from the Slimbus QMI service 
 * 
 * @param[in] connection_handle  Pointer to the client 
 *       connection handle
 * @param[in] service_cookie  Pointer to the service cookie
 */
static void slimbus_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_cookie
)
{
  client_info_type *client_info = (client_info_type*)connection_handle;

  /* Free up memory for the client */
  if(NULL != client_info)
  {
    if(NULL != client_info->hSB)
    {
      DalDevice_Close(client_info->hSB);
      DAL_SlimBusDeviceDetach(client_info->hSB);
    }
    DALSYS_Free(client_info); /* Malloc in connect_cb */
  }

  return;
}

/**
 * @brief Callback when a sends a request to the Slimbus service
 *
 * This function implements the callback for when a client sends 
 * a request to the Slimbus QMI service 
 * 
 * @param[in] connection_handle  Pointer to the client 
 *       connection handle
 * @param[in] req_handle  The request handle
 * @param[in] msg_id  The message ID
 * @param[in] req_c_struct  Pointer to the request messages 
 *       structure
 * @param[in] req_c_struct_len  The length of the request 
 *       message structure
 * @param[in] service_cookie  Pointer to the service cookie
 * 
 * @return QMI_CSI_NO_ERR on success, error code on error 
 */
static qmi_csi_cb_error slimbus_handle_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
)
{
  client_info_type *client_info = (client_info_type*)connection_handle;
  qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;

  if( (NULL == client_info) || 
      (NULL == service_cookie) ||
      (NULL == req_c_struct) )
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  switch ( msg_id )
  {
    case QMI_SLIMBUS_POWER_REQ_V01:
    {
      slimbus_power_req_msg_v01  *pReqMsg;
      slimbus_power_resp_msg_v01  RespMsg;
      uint16_t                    msg_body_size = 0;

      RespMsg.resp.result = QMI_RESULT_SUCCESS_V01;
      RespMsg.resp.error = QMI_ERR_NONE_V01;
      msg_body_size = sizeof(slimbus_power_resp_msg_v01);

      pReqMsg = (slimbus_power_req_msg_v01*) req_c_struct;
      if( req_c_struct_len < sizeof(slimbus_power_req_msg_v01) )
      {
        RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
        RespMsg.resp.error = QMI_ERR_MALFORMED_MSG_V01;
      }
      else
      {
        if( NULL == client_info->hSB )
        {
          DALResult result = DAL_SlimBusDeviceAttach(uaDevIds[0], &client_info->hSB);
          client_info->uInst = 0;
          if(DAL_SUCCESS != result)
          {
            RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
            RespMsg.resp.error = QMI_ERR_INTERNAL_V01;
          }
        }
        
        if( NULL != client_info->hSB )
        {
          if (pReqMsg->pm_req == SLIMBUS_PM_ACTIVE_V01)
          {
            if (DAL_SUCCESS != DalDevice_Open(client_info->hSB, DAL_OPEN_SHARED))
            {
              RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
              RespMsg.resp.error = QMI_ERR_DEVICE_NOT_READY_V01;
            }
          }
          else if (pReqMsg->pm_req == SLIMBUS_PM_INACTIVE_V01)
          {
            DalDevice_Close(client_info->hSB);
          }
        }
      }

      if(pReqMsg->resp_type_valid &&
         SLIMBUS_RESP_DEFERRED_V01 == pReqMsg->resp_type)
      {
        if(client_info->bRespValid)
        {
          if(QMI_RESULT_SUCCESS_V01 == RespMsg.resp.result)
          {
            RespMsg.resp.error = QMI_ERR_INCOMPATIBLE_STATE_V01;
          }
        }
        else
        {
          client_info->bRespValid = TRUE;
          client_info->req_handle = req_handle;
          client_info->resp = RespMsg.resp;
          rc = QMI_CSI_NO_ERR;
          break;
        }
      }
      rc = qmi_csi_send_resp(req_handle, msg_id, (void *)&RespMsg, msg_body_size);
    }
    break;
    
    case QMI_SLIMBUS_SELECT_INSTANCE_REQ_V01:
    {
      slimbus_select_inst_req_msg_v01  *pReqMsg;
      slimbus_select_inst_resp_msg_v01  RespMsg;
      uint16_t                          msg_body_size = 0;

      RespMsg.resp.result = QMI_RESULT_SUCCESS_V01;
      RespMsg.resp.error = QMI_ERR_NONE_V01;
      msg_body_size = sizeof(slimbus_select_inst_resp_msg_v01);

      pReqMsg = (slimbus_select_inst_req_msg_v01*) req_c_struct;
      if( req_c_struct_len < sizeof(slimbus_select_inst_req_msg_v01) )
      {
        RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
        RespMsg.resp.error = QMI_ERR_MALFORMED_MSG_V01;
      }
      else if ( pReqMsg->instance >= sizeof(uaDevIds)/sizeof(uaDevIds[0]) )
      {
        RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
        RespMsg.resp.error = QMI_ERR_INVALID_ID_V01;
      }
      else if( NULL == client_info->hSB )
      {
        DALResult result = DAL_SlimBusDeviceAttach(uaDevIds[pReqMsg->instance], &client_info->hSB);
        client_info->uInst = pReqMsg->instance;
        if(DAL_SUCCESS != result)
        {
          RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
          RespMsg.resp.error = QMI_ERR_INTERNAL_V01;
        }
      }
      else
      {
        if( pReqMsg->instance != client_info->uInst )
        {
          RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
          RespMsg.resp.error = QMI_ERR_INVALID_ID_V01;
        }
      }
      
      if( QMI_RESULT_SUCCESS_V01 == RespMsg.resp.result &&
          FALSE != pReqMsg->mode_valid )
      {
        uint32 uRetVal = FALSE;
        
        if(SLIMBUS_MODE_MASTER_V01 == pReqMsg->mode)
        {
          uRetVal = SlimBus_SwitchMasterMode(client_info->hSB, TRUE);
        }
        else if(SLIMBUS_MODE_SATELLITE_V01 == pReqMsg->mode)
        {
          uRetVal = SlimBus_SwitchMasterMode(client_info->hSB, FALSE);
        }
        
        if( !uRetVal )
        {
          RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
          RespMsg.resp.error = QMI_ERR_INCOMPATIBLE_STATE_V01;
        }
      }

      rc = qmi_csi_send_resp(req_handle, msg_id, (void *)&RespMsg, msg_body_size);
    }
    break;

    case QMI_SLIMBUS_CHECK_FRAMER_REQ_V01:
    {
      slimbus_check_framer_resp_msg_v01  RespMsg;
      uint16_t                    msg_body_size = 0;

      RespMsg.resp.result = QMI_RESULT_SUCCESS_V01;
      RespMsg.resp.error = QMI_ERR_NONE_V01;
      msg_body_size = sizeof(slimbus_check_framer_resp_msg_v01);

      if( req_c_struct_len < sizeof(slimbus_check_framer_req_msg_v01) )
      {
        RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
        RespMsg.resp.error = QMI_ERR_MALFORMED_MSG_V01;
      }
      else
      {
        if( NULL == client_info->hSB )
        {
          DALResult result = DAL_SlimBusDeviceAttach(uaDevIds[0], &client_info->hSB);
          client_info->uInst = 0;
          if(DAL_SUCCESS != result)
          {
            RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
            RespMsg.resp.error = QMI_ERR_INTERNAL_V01;
          }
        }

        if( NULL != client_info->hSB )
        {
          SlimBus_CheckFramerState(client_info->hSB);
        }
      }

      rc = qmi_csi_send_resp(req_handle, msg_id, (void *)&RespMsg, msg_body_size);
    }
    break;

    case QMI_SLIMBUS_SEND_DEFERRED_RESPONSE_REQ_V01:
    {
      slimbus_send_deferred_response_resp_msg_v01  RespMsg;
      uint16_t                    msg_body_size = 0;

      RespMsg.resp.result = QMI_RESULT_SUCCESS_V01;
      RespMsg.resp.error = QMI_ERR_NONE_V01;
      msg_body_size = sizeof(slimbus_send_deferred_response_resp_msg_v01);

      if( req_c_struct_len < sizeof(slimbus_send_deferred_response_req_msg_v01) )
      {
        RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
        RespMsg.resp.error = QMI_ERR_MALFORMED_MSG_V01;
      }
      else if(!client_info->bRespValid)
      {
        RespMsg.resp.result = QMI_RESULT_FAILURE_V01;
        RespMsg.resp.error = QMI_ERR_INCOMPATIBLE_STATE_V01;
      }

      rc = qmi_csi_send_resp(req_handle, msg_id, (void *)&RespMsg, msg_body_size);
      
      if( QMI_CSI_NO_ERR == rc &&
          client_info->bRespValid )
      {
        client_info->bRespValid = FALSE;
        rc = qmi_csi_send_resp(client_info->req_handle, QMI_SLIMBUS_POWER_REQ_V01,
                               (void *)&client_info->resp, sizeof(client_info->resp));
      }
    }
    break;
  }
  
  if( QMI_CSI_NO_ERR == rc )
  {
    return QMI_CSI_CB_NO_ERR;
  }
  else
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }
}

/**
 * @brief Service thread for Slimbus QMI service
 *
 * This function implements the servie thread for the Slimbus 
 * QMI service 
 * 
 * @param[in] unused  Unused parameter
 */
static void slimbus_qmi_service_thread(void *unused)
{
  static service_context_type service_cookie; /* QMI service cookie */

  qmi_csi_os_params os_params;
  qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;
  qmi_idl_service_object_type slimbus_service_object;
  
  os_params.sig = SLIMBUS_QMI_SVC_WAIT_SIG;
  os_params.signal = &slimbus_qmi_service_sig;

  slimbus_service_object = slimbus_get_service_object_v01();

  if( NULL != slimbus_service_object )
  {
    rc = qmi_csi_register(slimbus_service_object, slimbus_connect_cb, 
                          slimbus_disconnect_cb, slimbus_handle_req_cb,
                          &service_cookie, &os_params,
                          &service_cookie.service_handle);
  }

  if(QMI_CSI_NO_ERR != rc)
  {
    DALSYS_LogEvent(DALDEVICEID_SLIMBUS_1, DALSYS_LOGEVENT_FATAL_ERROR,
                    "Unable to start slimbus QMI service!");
    return;
  }

  while(1)
  {
    qurt_anysignal_wait(os_params.signal, SLIMBUS_QMI_SVC_WAIT_SIG);
    qurt_anysignal_clear(os_params.signal, SLIMBUS_QMI_SVC_WAIT_SIG);
    qmi_csi_handle_event(service_cookie.service_handle, &os_params);
  }
}

/**
 * @brief Initialization function for the Slimbus QMI service
 *
 * This function implements the initialization code for the 
 * Slimbus QMI service 
 */
void slimbus_qmi_init(void)
{
  static char slimbus_qmi_service_stack[SLIMBUS_QMI_SERVICE_STACK_SIZE];

  qurt_anysignal_init(&slimbus_qmi_service_sig);
  qurt_thread_attr_init(&slimbus_qmi_service_attr);
  qurt_thread_attr_set_name(&slimbus_qmi_service_attr, "SlimBusQmiSvc");
  qurt_thread_attr_set_priority(&slimbus_qmi_service_attr, 39);
  qurt_thread_attr_set_stack_size(&slimbus_qmi_service_attr,
                                  SLIMBUS_QMI_SERVICE_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&slimbus_qmi_service_attr, slimbus_qmi_service_stack);
  qurt_thread_create(&slimbus_qmi_service_tcb, &slimbus_qmi_service_attr, 
                     slimbus_qmi_service_thread, NULL);
}

