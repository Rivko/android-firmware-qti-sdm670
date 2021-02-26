/*===========================================================================

           S Y S _ M . C

DESCRIPTION

Copyright (c) 2015-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/debugtools/sys_m/src/sys_m_reset_non_mpss.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#include "sys_m.h"
#include "sys_m_smsm.h"
#include "qmi_ssreq_client.h"


/**
 *
 * @brief SYS_M_LOCK_INIT
 *
 * Initialize sys_m mutex
 *
*/
void SYS_M_LOCK_INIT(void)
{

}

/**
 *
 * @brief SYS_M_LOCK
 *
 * Lock sys_m mutex
 *
*/
void SYS_M_LOCK(void)
{

}

/**
 *
 * @brief SYS_M_TRY_LOCK
 *
 * Lock sys_m mutex
 *
*/
int SYS_M_TRY_LOCK(void)
{
   return 1;
}

/**
 *
 * @brief SYS_M_UNLOCK
 *
 * Unlock sys_m mutex
 *
*/
void SYS_M_UNLOCK(void)
{

}



/**
 *
 * @brief sys_m_init
 *
 * System monitor initialization function
 *
*/
void sys_m_init(void)
{
   
}


/**
 *
 * @brief sys_m_initiate_shutdown
 *
 * Request apps for a reset(restart) of the device
 *
 *
*/
void sys_m_initiate_shutdown(void)
{

}

/**
 *
 * @brief sys_m_initiate_poweroff
 *
 * Request apps for a poweroff(shutdown) of the device
 *
 *
 *
*/
void sys_m_initiate_poweroff(void)
{

}

/**
  Initiates reset of the complete system using SSREQ QMI
  And notify about the indication responce status to 
  the user of this API via its callback function (ind_cb_fn)  if provided

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_initiate_restart_ssreq(ssreq_qmi_ind_cb_fn_type *ind_cb_fn)
{
   return SSREQ_FAILURE;
}

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
ssreq_status_t sys_m_request_peripheral_restart_ssreq(ssreq_qmi_ind_cb_fn_type *ind_cb_fn)
{
   return SSREQ_FAILURE;
}


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
                                       ssreq_qmi_request_res_code_type res_code)
{
   return SSREQ_FAILURE;
}
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
                                          ssreq_qmi_request_res_code_type res_code)
{
   return SSREQ_FAILURE;
}


