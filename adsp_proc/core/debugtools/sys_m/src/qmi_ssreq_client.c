/*============================================================================
  @file qmi_ssreq_client.c

  Sub System Request(SSREQ) QMI cleint implementation

  Copyright (c) 2014-17 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/debugtools/sys_m/src/qmi_ssreq_client.c#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/04/14   rks      Added reason code support with SSREQ req
10/09/14   rks     added API ssreq_ser_check_for_shutdown_support()
06/26/14   rks     Initial Version

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "subsystem_request_v01.h"
#include "sys_m.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_ssreq_client.h"
#include "timer.h"
#include "tms_utils_msg.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/



#define SSREQ_REQ_TIMEOUT_MS 1000
#define SSREQ_SERVICE_MINOR_VER_FOR_SHUTDOWN_SUPPORT 99

extern qmi_idl_service_object_type   ssreq_service_object;
extern qmi_client_type               ssreq_client_handle;

boolean                       ssreq_req_in_process = FALSE;

/*store the function pointer to be called once the indication received 
  for the SSREQ QMI request*/
ssreq_qmi_ind_cb_fn_type      *ssreq_qmi_ind_cb = NULL;

static timer_type             ssreq_ind_timer;
static timer_group_type       ssreq_timer_group = {0};

/*===========================================================================

  FUNCTION:  ssreq_client_ind_cb

===========================================================================*/
/*!
    @brief
    Helper function for the client indication callback

    @detail
    This function send back the indication result to the 
    orignal request initiator client if registered the
    ssreq_qmi_ind_cb callback

    @return
    None

*/
/*=========================================================================*/

void ssreq_client_ind_cb
(
 qmi_client_type                client_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
   void *ind_msg;
   timetick_type responce_time = 0;
   qmi_client_error_type qmi_err;
   uint32_t decoded_size;
   qmi_ssreq_system_shutdown_ind_msg_v01* indication = NULL;
   ssreq_req_in_process = FALSE;
   responce_time = timer_clr(&ssreq_ind_timer,T_MSEC);

   TMS_SHUTDOWN_MSG_HIGH_3("QMI Indication for client: %d, msg_id: %d, response time: 0x%x",client_handle,msg_id,responce_time);
  
   qmi_err = qmi_idl_get_message_c_struct_len(
                ssreq_service_object,
                QMI_IDL_INDICATION, msg_id, &decoded_size
             );

   if( QMI_NO_ERR != qmi_err )
   {
      TMS_SHUTDOWN_MSG_ERROR_1("QMI get error: %d", qmi_err);
      return ;
   }

   ind_msg = malloc(decoded_size);
   if(!ind_msg) 
   {
      TMS_SHUTDOWN_MSG_HIGH("Indication buff allocation failed");
      return;
   }
   qmi_err = qmi_client_message_decode( client_handle, QMI_IDL_INDICATION,
               msg_id, ind_buf, ind_buf_len, ind_msg, decoded_size );

   if (qmi_err != QMI_NO_ERR)
   {
      TMS_SHUTDOWN_MSG_ERROR_1("QMI decode error: %d", qmi_err);
      free(ind_msg);
      return;
   }

   indication = (qmi_ssreq_system_shutdown_ind_msg_v01*)ind_msg;

   if(ssreq_qmi_ind_cb){
      ssreq_ind_result res = SSREQ_REQUEST_NOT_SERVED;

      if(indication->status == SSREQ_QMI_REQUEST_SERVICED_V01){
         res = SSREQ_REQUEST_SERVED;
      }

      ssreq_qmi_ind_cb(res);/*call the function registered by the client to indicate the result 
                                 of requested QMI command */
      ssreq_qmi_ind_cb = NULL; /*set the call back to NULL for this particlur client after the request is completed */
   }
        
   TMS_SHUTDOWN_MSG_HIGH_1("Request status: %d",indication->status);
   free(ind_msg);
   return;
}
/*===========================================================================

  FUNCTION:  ssreq_request_timout_fn

===========================================================================*/
/*!
    @brief
    Callback function called once the request timeout occure

    @detail
    Set ssreq_req_in_process to FALSE so that next requests can be processed
    And notify to the user via its registered callback.
    
    @return
    None

*/
/*=========================================================================*/
void ssreq_request_timout_fn(timer_cb_data_type data)
{
   TMS_SHUTDOWN_MSG_ERROR("Indication time-out");
   if(ssreq_qmi_ind_cb){
      ssreq_qmi_ind_cb(SSREQ_REQUEST_NOT_SERVED);/*call the function registered by the client to indicate the result 
                                                            of requested QMI command */
      ssreq_qmi_ind_cb = NULL; /*set the call back to NULL for this particlur client after the request is completed */

      ssreq_req_in_process = FALSE;
   }
}

/*===========================================================================

  FUNCTION:  ssreq_shutdown_system

===========================================================================*/
/*!
    @brief
    Send a request to the server to shutdown system

    @detail
    

    @return
    SSREQ_SUCCESS : request for memory was succesful
    SSREQ_FAILURE: request for memory failed

*/
/*=========================================================================*/
static ssreq_status_t ssreq_shutdown_system(ssreq_qmi_request_res_code_type res_code)
{
   int ret = 0;
   qmi_ssreq_system_shutdown_req_msg_v01 ssreq_shutdown_req;
   qmi_ssreq_system_shutdown_resp_msg_v01 ssreq_shutdown_resp;
   ssreq_shutdown_req.ss_client_id = SSREQ_QMI_CLIENT_INSTANCE_MPSS_V01;
   if(res_code!=SSREQ_QMI_RES_NONE)
   {
      ssreq_shutdown_req.res_code_valid = TRUE;
      ssreq_shutdown_req.res_code = (ssreq_qmi_request_res_code_enum_type_v01)res_code;/*typecasted to silent compiler warnings*/
   }else{
      ssreq_shutdown_req.res_code_valid = FALSE;
   }
   TMS_SHUTDOWN_MSG_HIGH_1("System shutdown reason: %d",res_code);
   
   /* QMI call to send shutdown request */
   ret = qmi_client_send_msg_sync(ssreq_client_handle, QMI_SSREQ_SYSTEM_SHUTDOWN_REQ_V01,
                                  (void*)&ssreq_shutdown_req,sizeof(ssreq_shutdown_req),
                                  (void*)&ssreq_shutdown_resp,sizeof(ssreq_shutdown_resp),0 );

   if(QMI_NO_ERR != ret)
   {
      TMS_SHUTDOWN_MSG_ERROR_1("QMI send error: %d",ret);
      return SSREQ_FAILURE;
   }

   if(ssreq_shutdown_resp.resp.result != QMI_RESULT_SUCCESS_V01)
   {
      TMS_SHUTDOWN_MSG_ERROR_1("QMI error, Response: %d",ssreq_shutdown_resp.resp.error);
      return SSREQ_FAILURE;
   }

   return SSREQ_SUCCESS;
}


/*===========================================================================

  FUNCTION:  ssreq_restart_system

===========================================================================*/
/*!
    @brief
    Send a request to the server to restart system

    @detail

    @return
    SSREQ_SUCCESS : request for memory was succesful
    SSREQ_FAILURE: request for memory failed

*/
/*=========================================================================*/
static ssreq_status_t ssreq_restart_system(ssreq_qmi_request_res_code_type res_code)
{
   int ret = 0;
   qmi_ssreq_system_restart_req_msg_v01 ssreq_restart_req;
   qmi_ssreq_system_restart_resp_msg_v01 ssreq_restart_resp;
   ssreq_restart_req.ss_client_id = SSREQ_QMI_CLIENT_INSTANCE_MPSS_V01;
   if(res_code!=SSREQ_QMI_RES_NONE){
      ssreq_restart_req.res_code_valid = TRUE;
      ssreq_restart_req.res_code = (ssreq_qmi_request_res_code_enum_type_v01)res_code;/*typecasted to silent compiler warnings*/
   }else{
      ssreq_restart_req.res_code_valid = FALSE;
   }
   TMS_SHUTDOWN_MSG_HIGH_1("System restart reason: %d", res_code);

   /* QMI call to send restart request */
   ret = qmi_client_send_msg_sync(ssreq_client_handle, QMI_SSREQ_SYSTEM_RESTART_REQ_V01,
                                  (void*)&ssreq_restart_req,sizeof(ssreq_restart_req),
                                  (void*)&ssreq_restart_resp,sizeof(ssreq_restart_resp),0 );
   if(QMI_NO_ERR != ret)
   {
      TMS_SHUTDOWN_MSG_ERROR_1("QMI send error: %d",ret);
      return SSREQ_FAILURE;
   }

   if(ssreq_restart_resp.resp.result != QMI_RESULT_SUCCESS_V01)
   {
      TMS_SHUTDOWN_MSG_ERROR_1("QMI error, Response: %d",ssreq_restart_resp.resp.error);
      return SSREQ_FAILURE;
   }

  return SSREQ_SUCCESS;
}

/*===========================================================================

  FUNCTION:  ssreq_restart_peripheral

===========================================================================*/
/*!
    @brief
    Send a request to the server to restart system

    @detail

    @return
    ssreq_SUCCESS : request for memory was succesful
    ssreq_FAILURE: request for memory failed

*/
/*=========================================================================*/
static ssreq_status_t ssreq_restart_peripheral(ssreq_qmi_request_res_code_type res_code)
{
   int ret = 0;
   qmi_ssreq_peripheral_restart_req_msg_v01 ssreq_p_restart_req;
   qmi_ssreq_peripheral_restart_resp_msg_v01 ssreq_p_restart_resp;
   ssreq_p_restart_req.ss_client_id = SSREQ_QMI_CLIENT_INSTANCE_MPSS_V01;
   if(res_code!=SSREQ_QMI_RES_NONE){
      ssreq_p_restart_req.res_code_valid = TRUE;
      ssreq_p_restart_req.res_code = (ssreq_qmi_request_res_code_enum_type_v01)res_code;/*typecasted to silent compiler warnings*/
   }else{
      ssreq_p_restart_req.res_code_valid = FALSE;
   }

   TMS_SHUTDOWN_MSG_HIGH_1("Peripheral restart reason: %d",res_code);
   /* QMI call to send restart request */
   ret = qmi_client_send_msg_sync(ssreq_client_handle, QMI_SSREQ_PERIPHERAL_RESTART_REQ_V01,
                                  (void*)&ssreq_p_restart_req,sizeof(ssreq_p_restart_req),
                                  (void*)&ssreq_p_restart_resp,sizeof(ssreq_p_restart_resp),0 );
   if(QMI_NO_ERR != ret)
   {
      TMS_SHUTDOWN_MSG_ERROR_1("QMI send error: %d",ret);
      return SSREQ_FAILURE;
   }

   if(ssreq_p_restart_resp.resp.result != QMI_RESULT_SUCCESS_V01)
   {
      TMS_SHUTDOWN_MSG_ERROR_1("QMI error, Response: %d",ssreq_p_restart_resp.resp.error);
      return SSREQ_FAILURE;
   }

   return SSREQ_SUCCESS;
}

/*===========================================================================

  FUNCTION:  ssreq_process_request

===========================================================================*/
/*!
    @brief
    Send a request to the ssreq server at APSS to process ssreq cmd

    @detail

    @return
    SSREQ_SUCCESS   : request for memory was succesful
    SSREQ_FAILURE   : request for memory failed
    SSREQ_IN_PROCESS: already a request is in process
    
    *Note*: This API is used only in sys_m.c and called inside SYS_M_LOCK lock
    So right now do not have its own mutex created for concurrency protection
*/
/*=========================================================================*/
ssreq_status_t ssreq_process_request(
             uint32 cmd, ssreq_qmi_ind_cb_fn_type *ind_cb_fn,
             ssreq_qmi_request_res_code_type res_code)
{
    static boolean ssreq_timer_def = FALSE;
    if(TRUE == ssreq_req_in_process)
    {
      return SSREQ_IN_PROCESS;
    }
    ssreq_req_in_process = TRUE;
    ssreq_qmi_ind_cb = ind_cb_fn;

    if(!ssreq_timer_def){
       timer_def(&ssreq_ind_timer, &ssreq_timer_group, NULL, 0, &ssreq_request_timout_fn, 0);
       ssreq_timer_def = TRUE;
    }

    timer_set(&ssreq_ind_timer, (timetick_type)SSREQ_REQ_TIMEOUT_MS, 0, T_MSEC);

    switch(cmd){
        case QMI_SSREQ_SYSTEM_SHUTDOWN_REQ_V01:
            return ssreq_shutdown_system(res_code);

        case QMI_SSREQ_SYSTEM_RESTART_REQ_V01:
            return ssreq_restart_system(res_code);

        case QMI_SSREQ_PERIPHERAL_RESTART_REQ_V01:
            return ssreq_restart_peripheral(res_code);

        default:
           TMS_SHUTDOWN_MSG_ERROR_1("Unhandled QMI Request: %d", cmd);
    }
    return SSREQ_FAILURE;
}

/*===========================================================================

  FUNCTION:  ssreq_ser_check_for_shutdown_support

===========================================================================*/
boolean ssreq_ser_check_for_shutdown_support(void)
{
   uint32 minor_vers = 0;
   qmi_client_error_type ret;
   ssreq_service_object = ssreq_get_service_object_v01();
   
   ret = qmi_idl_get_idl_minor_version(ssreq_service_object, &minor_vers);
   if(ret != QMI_IDL_LIB_NO_ERR)
   {
      minor_vers = 0;
   }

   TMS_SHUTDOWN_MSG_HIGH_1("Service minor version: %d", minor_vers);
   if (minor_vers < SSREQ_SERVICE_MINOR_VER_FOR_SHUTDOWN_SUPPORT)
   {
      return FALSE;
   }
   return TRUE;
}

