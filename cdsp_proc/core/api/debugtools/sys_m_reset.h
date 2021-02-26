#ifndef SYS_M_RESET_H
#define SYS_M_RESET_H
/*===========================================================================

           S Y S _ M _ R E S E T. H

DESCRIPTION

Copyright (c) 2011-16 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/api/debugtools/sys_m_reset.h#1 $
  when       who      what, where, why
--------     ---     ------------------------------------------------------------
11/04/14     rks      Added reason code support with SSREQ req
06/26/14     rks      SSREQ client implimentation
===========================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

/**
   RCECB Exposed Events

   @dependencies
   Requires client to include rcecb.h to use
*/

#define SYS_M_RCECB_FWD_POWEROFF    "SYSM:FWD:POWEROFF"
#define SYS_M_RCECB_FWD_SHUTDOWN    "SYSM:FWD:SHUTDOWN"

typedef enum _ssreq_ind_result{
   SSREQ_REQUEST_SERVED,
   SSREQ_REQUEST_NOT_SERVED,
}ssreq_ind_result;

typedef enum _ssreq_qmi_request_res_code_type{
   SSREQ_QMI_RES_NONE=0,
   SSREQ_QMI_RES_MODEM_CONF_CHANGE = 0x10,/*SSREQ_QMI_RES_MODEM_CONF_CHANGE_V01*/
   SSREQ_QMI_RES_SEGMENT_LOADING/*SSREQ_QMI_RES_SEGMENT_LOADING_V01*/
}ssreq_qmi_request_res_code_type;

typedef enum _ssreq_status_t
{
   SSREQ_SUCCESS      =  0, /*request was successful*/
   SSREQ_FAILURE      = -1, /*request failed*/
   SSREQ_IN_PROCESS   = -2, /*already a request is in process and cannot process a
                              new request until existing request completes */
}ssreq_status_t;

typedef void ssreq_qmi_ind_cb_fn_type(ssreq_ind_result res);

/**
  Initiates reset of the subsystem

  @return
  None.

  @dependencies
  None.
*/
void sys_m_initiate_shutdown(void);

/**
  Initiates full poweroff of the subsystem

  @return
  None.

  @dependencies
  None.
*/
void sys_m_initiate_poweroff(void);

/**
  Initiates restart of the complete system using SSREQ QMI
  And notify about the indication responce status to 
  the user of this API via its callback function (ind_cb_fn)  if provided

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_initiate_restart_ssreq(ssreq_qmi_ind_cb_fn_type *ind_cb_fn);

/**
  Initiates the peripheral restart using SSREQ QMI
  And notify about the indication responce status to 
  the user of this API via its callback function(ind_cb_fn) if provided


  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_request_peripheral_restart_ssreq(ssreq_qmi_ind_cb_fn_type *ind_cb_fn);

/**
  Initiates reset of the system using SSREQ QMI
  and notify about the indication responce status to 
  the user of this API via its callback function (ind_cb_fn)  if provided
  In addition provide the reason code(res_code) to the service for SSREQ request initiation

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_init_restart_ssreq_with_res_code(ssreq_qmi_ind_cb_fn_type *ind_cb_fn,
                                       ssreq_qmi_request_res_code_type res_code);
/**
  Initiates the peripheral restart using SSREQ QMI
  and notify about the indication responce status to 
  the user of this API via its callback function(ind_cb_fn) if provided
  In addition provide the reason code(res_code) to the service for SSREQ request initiation

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_req_pp_restart_ssreq_with_res_code(ssreq_qmi_ind_cb_fn_type *ind_cb_fn,
                                          ssreq_qmi_request_res_code_type res_code);
#if defined(__cplusplus)
}
#endif

#endif  /* SYS_M_RESET_H */
