/*===========================================================================

           SYS_M_RESET_MPSS. C

DESCRIPTION

Copyright (c) 2015-2017 by Qualcomm Technologies Incorporated. All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/sys_m/src/sys_m_reset_mpss.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#include "sys_m_reset.h"
#include "sys_m.h"
#include "qmi_ssreq_client.h"
#include "sys_m_smsm.h"
#include "qurt.h"
#include "subsystem_request_v01.h"
#include "rcecb.h"
#include "tms_utils_msg.h"

static qurt_mutex_t sys_m_lock_mutex;

static int sys_m_lock_inited = 0;

/**
 *
 * @brief SYS_M_LOCK_INIT
 *
 * Initialize sys_m mutex
 *
*/
void SYS_M_LOCK_INIT(void)
{
   if (!sys_m_lock_inited)
   {
      qurt_pimutex_init(&sys_m_lock_mutex);
      sys_m_lock_inited = 1;
   }
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
   if (sys_m_lock_inited)
   {
      qurt_pimutex_lock(&sys_m_lock_mutex);
   }
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
   int result = 1; /*since 0 is success , init with non-zero*/
   if (sys_m_lock_inited)
   {
      result =  qurt_pimutex_try_lock(&sys_m_lock_mutex);
   }
   return result;
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
   if (sys_m_lock_inited)
   {
      qurt_pimutex_unlock(&sys_m_lock_mutex);
   }
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
   SYS_M_LOCK_INIT();
}


/**
 *
 * @brief sys_m_initiate_shutdown
 *
 * Client Exposed API
 * Request apps for a reset(restart) of the device.
 *
 *
*/
void sys_m_initiate_shutdown(void)
{

   SYS_M_LOCK(); // Concurrency protection
   TMS_SHUTDOWN_MSG_HIGH_1("Initiate shutdown from 0x%x task context", rex_self());

   /*check if the shutdonw request can be forwarded to APSS(HLOS or TN ) using SSREQ QMI*/
   if((ssreq_ser_check_for_shutdown_support())&&(TRUE == get_ssreq_client_init_status()))
   {
       ssreq_status_t ret;
       ret = ssreq_process_request(QMI_SSREQ_SYSTEM_RESTART_REQ_V01, NULL,SSREQ_QMI_RES_NONE);
       TMS_SHUTDOWN_MSG_HIGH_1("SSREQ QMI ret: %d", ret);
   }
   // Check for the case where a forwarding callback has been registered. Specific use
   // case example is HLOS to MPSS has established a AT Command Forward channel that
   // is utilized to forward SHUTDOWN REQUEST back to HLOS.
   else if (0 < rcecb_getregistrants_name(SYS_M_RCECB_FWD_SHUTDOWN)) // count of registrants
   {
       TMS_SHUTDOWN_MSG_HIGH("Forwarding request");
       if(rcecb_signal_name(SYS_M_RCECB_FWD_SHUTDOWN)){} // signal executes callbacks for registrants
   }
   // Else the behavior is normal standalone behavior. Specific use case example
   // is that the APPS_TN gets the command request by SMP2P state bits.
   else
   {
      int32 ret;
      ret = sys_m_smsm_apps_set(SYS_M_SUBSYS2AP_SMSM_SHUT_DWN);
      TMS_SHUTDOWN_MSG_HIGH_1("SMP2P ret: %d", ret);
   }

   SYS_M_UNLOCK();
}

/**
 *
 * @brief sys_m_initiate_poweroff
 *
 * Client Exposed API
 * Request apps for a poweroff(shutdown) of the device
 *
 *
 *
*/
void sys_m_initiate_poweroff(void)
{

   SYS_M_LOCK(); // Concurrency protection

   TMS_SHUTDOWN_MSG_HIGH_1("Initiate Power off from 0x%x context", rex_self());

   /*check if the shutdonw request can be forwarded to APSS(HLOS or TN ) using SSREQ QMI*/
   if((ssreq_ser_check_for_shutdown_support())&&(TRUE == get_ssreq_client_init_status()))
   {
       ssreq_status_t ret;
       ret = ssreq_process_request(QMI_SSREQ_SYSTEM_SHUTDOWN_REQ_V01, NULL, SSREQ_QMI_RES_NONE);
       TMS_SHUTDOWN_MSG_HIGH_1("SSREQ QMI ret: %d", ret);
   }
   // Check for the case where a forwarding callback has been registered. Specific use
   // case example is HLOS to MPSS has established a AT Command Forward channel that
   // is utilized to forward SHUTDOWN REQUEST back to HLOS.
   else if (0 < rcecb_getregistrants_name(SYS_M_RCECB_FWD_POWEROFF)) // count of registrants
   {
       TMS_SHUTDOWN_MSG_HIGH("Forwarding request");
       if(rcecb_signal_name(SYS_M_RCECB_FWD_POWEROFF)){} // signal executes callbacks for registrants
   }
   // Else the behavior is normal standalone behavior. Specific use case example
   // is that the APPS_TN gets the command request by SMP2P state bits.
   else
   {
       int32 ret;
       ret = sys_m_smsm_apps_set(SYS_M_SUBSYS2AP_SMSM_PWR_OFF);
       TMS_SHUTDOWN_MSG_HIGH_1("SMP2P ret: %d", ret);
   }

   SYS_M_UNLOCK();
}

/**
  Initiates reset of the system using SSREQ QMI

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_initiate_restart_ssreq(ssreq_qmi_ind_cb_fn_type *ind_cb_fn)
{
   ssreq_status_t ssreq_ret = SSREQ_FAILURE;

   int res = SYS_M_TRY_LOCK(); // Concurrency protection
   if (res != 0)
      return SSREQ_IN_PROCESS;

   /*check if the shutdown request can be forwarded to APSS(HLOS or TN ) using SSREQ QMI*/
   if(TRUE == get_ssreq_client_init_status())
   {
       ssreq_ret = ssreq_process_request(QMI_SSREQ_SYSTEM_RESTART_REQ_V01, ind_cb_fn,SSREQ_QMI_RES_NONE);
       TMS_SHUTDOWN_MSG_HIGH_2("System restart SSREQ over QMI from 0x%x, ret: %d", rex_self(), ssreq_ret);
   }
   SYS_M_UNLOCK();
   return ssreq_ret;
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
   ssreq_status_t ssreq_ret = SSREQ_FAILURE;

   int res = SYS_M_TRY_LOCK(); // Concurrency protection
   if (res != 0)
      return SSREQ_IN_PROCESS;

   /*check if the shutdown request can be forwarded to APSS (HLOS or TN) using SSREQ QMI*/
   if(TRUE == get_ssreq_client_init_status())
   {
       ssreq_ret = ssreq_process_request(QMI_SSREQ_PERIPHERAL_RESTART_REQ_V01, ind_cb_fn,SSREQ_QMI_RES_NONE);
       TMS_SHUTDOWN_MSG_HIGH_2("Peripheral restart SSREQ over QMI from 0x%x, ret: %d", rex_self(), ssreq_ret);
   }
   SYS_M_UNLOCK();
   return ssreq_ret;
}

/**
  Initiates reset of the system using SSREQ QMI
  And notify about the indication responce status to 
  the user of this API via its callback function (ind_cb_fn)  if provided
  In addition provide the reason code(res_code) to the service for SSREQ request initiation

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_init_restart_ssreq_with_res_code(
   ssreq_qmi_ind_cb_fn_type *ind_cb_fn,
   ssreq_qmi_request_res_code_type res_code
)
{
   ssreq_status_t ssreq_ret = SSREQ_FAILURE;


   int res = SYS_M_TRY_LOCK(); // Concurrency protection
   if (res != 0)
      return SSREQ_IN_PROCESS;

   /*check if the shutdown request can be forwarded to APSS (HLOS or TN) using SSREQ QMI*/
   if(TRUE == get_ssreq_client_init_status())
   {
       ssreq_ret = ssreq_process_request(QMI_SSREQ_SYSTEM_RESTART_REQ_V01, ind_cb_fn, res_code);
       TMS_SHUTDOWN_MSG_HIGH_3("System restart reason: %d SSREQ over QMI from 0x%x, ret: %d", res_code, rex_self(), ssreq_ret);
   }
   SYS_M_UNLOCK();
   return ssreq_ret;
}
/**
  Initiates the peripheral restart using SSREQ QMI
  And notify about the indication responce status to 
  the user of this API via its callback function(ind_cb_fn) if provided
  In addition provide the reason code(res_code) to the service for SSREQ request initiation

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_req_pp_restart_ssreq_with_res_code(
   ssreq_qmi_ind_cb_fn_type *ind_cb_fn,
   ssreq_qmi_request_res_code_type res_code
)
{
   ssreq_status_t ssreq_ret = SSREQ_FAILURE;

   int res = SYS_M_TRY_LOCK(); // Concurrency protection
   if (res != 0)
      return SSREQ_IN_PROCESS;

   /*check if the shutdown request can be forwarded to APSS (HLOS or TN) using SSREQ QMI*/
   if(TRUE == get_ssreq_client_init_status())
   {
       ssreq_ret = ssreq_process_request(QMI_SSREQ_PERIPHERAL_RESTART_REQ_V01, ind_cb_fn, res_code);
       TMS_SHUTDOWN_MSG_HIGH_3("Peripheral restart reason: %d SSREQ over QMI from 0x%x, ret: %d", res_code, rex_self(), ssreq_ret);
   }
   SYS_M_UNLOCK();
   return ssreq_ret;
}

