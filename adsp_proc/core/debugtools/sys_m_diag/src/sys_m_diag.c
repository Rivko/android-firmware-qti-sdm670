/*===========================================================================

           S Y S _ M _ D I A G . C

DESCRIPTION
  The diag interface for system monitor

Copyright (c) 2014 - 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/16   nk      Added changes to introduce delay through QXDM command before entering into STM  
11/06/14   dav     File Created

===========================================================================*/

#include "sys_m_diag.h"
#include "sys_m_reset.h"
#include "tms_utils_msg.h"
#include "gen_cb_ctxt.h"

#define SYS_M_DIAG_DELAY_DEFAULT 1
static boolean  diag_pending = FALSE;
static diagpkt_user_table_entry_type sys_m_tbl_diag[2];
static timer_type       diagTimer;
static timer_group_type diag_timer_group = {0};
static diag_info_t diag_info;
extern void sys_m_shutdown_delay_secs_set(unsigned int secs);

/**
 *
 * @brief sys_m_diag_ind_cb
 *  
 * Indication callback for SSREQ 
 *
*/
static void sys_m_diag_ind_cb(ssreq_ind_result result)
{
  if(result == SSREQ_REQUEST_SERVED)
  {
     TMS_MSG_ERROR("MPSS:SSREQ_QMI_REQUEST_SERVICED");
  }
  else
  {
     TMS_MSG_ERROR("MPSS:SSREQ_QMI_REQUEST_NOT_SERVICED");
  }
}

/**
 *
 * @brief sys_m_restart_execute
 *  
 * Switch to the actual tasks that maps to the diag packet 
 *
*/
void sys_m_restart_execute(void* diag_info)
{
  diag_info_t *tmp_diag_info = (diag_info_t*) diag_info;

  switch(tmp_diag_info->type)
  {
    case SYS_M_PERIPHERAL_RESTART: 
      sys_m_request_peripheral_restart_ssreq(&sys_m_diag_ind_cb);
      break;
    case SYS_M_PERIPHERAL_RESTART_WITH_RES_CODE:
      sys_m_req_pp_restart_ssreq_with_res_code(&sys_m_diag_ind_cb, SSREQ_QMI_RES_SEGMENT_LOADING);
      break;
    case SYS_M_SYSTEM_RESTART: 
     sys_m_initiate_restart_ssreq(&sys_m_diag_ind_cb);
      break;
    case SYS_M_SYSTEM_RESTART_WITH_RES_CODE: 
      sys_m_init_restart_ssreq_with_res_code(&sys_m_diag_ind_cb, SSREQ_QMI_RES_MODEM_CONF_CHANGE);
      break;
    default:
      TMS_MSG_ERROR_1("Unhandled request ID %#x", 
	                                        tmp_diag_info->type);
  }
  diag_pending = FALSE;
}

/** =====================================================================
* Function:
*     sys_m_diag_generic_task_cb
*
* Description:
*     Timer driven function to offload and run from generic callback task 
*     context instead of timer context
*
* Parameters:
*     unused
*
* Returns:
*     none
* =====================================================================  */
static void sys_m_diag_generic_task_cb(timer_cb_data_type diag_info)
{
   gen_cb_ctxt_invoke_cb(sys_m_restart_execute, (void *)diag_info);
}

/**
 *
 * @brief sys_m_diag_initiate
 *  
 * Construct the diag_info and start the timer 
 *
*/
static void sys_m_diag_initiate(sys_m_restart_type req_type, uint32 req_delay)
{
  if (req_type >= SIZEOF_SYS_M_RESTART_TYPE) 
  {
    TMS_MSG_ERROR_1("Invalid ID %#x", req_type);
    diag_pending = FALSE;
    return;
  }

  diag_info.type   = req_type;
  diag_info.delay  = req_delay;
  
  //Even though req_delay is zero still throw it to timer task, out of diag context
  //since big executions will result to diag timeout
  //set cb timer to trigger actual task 
  timer_set_64(&diagTimer, (timetick_type)req_delay, 0, T_SEC);
}

/**
 *
 * @brief sys_m_request_restart_diag
 *
 * Diag handler when receiving the registered packet. Build the response message
 * and the necessary information for initialization from the retrievd diag packets.
 *
*/
PACK(void *) sys_m_request_restart_diag (PACK(void *) req_ptr, uint16 pkt_len)
{
  sys_m_restart_req_type *req;
  sys_m_restart_type type;
  uint32 delay;

  if (diag_pending)
  {
    TMS_MSG_HIGH("Diag pending, ignoring subsequent request");
    return(NULL);
  }
  else
  {
	//set before diag response to guarantee no reentrancy issues  
    diag_pending = TRUE;  
	/* build response */
    tms_diag_send_generic_response(req_ptr);
  }
  
  // decode request
  req = (sys_m_restart_req_type *) req_ptr;

  if (pkt_len <= (sizeof(req->hdr)+sizeof(req->restart_type))) 
  {
    type = req->restart_type;
    // default parameters
    delay = SYS_M_DIAG_DELAY_DEFAULT;
  }
  else
  {
	type = req->restart_type;
	delay = req->delay;
  }
  sys_m_diag_initiate(type, delay);

  return(NULL);
}

/*===========================================================================

FUNCTION SYS_M_SHUTDOWN_DELAY_DIAG

DESCRIPTION
  This callback is registered with diag (send_data 75 37 11 00 <delay>)
  When executed it will delay shutdown completion by <delay> seconds.
  This facilitates automated testing by allowing additional time for diag
  packets to be flushed.

===========================================================================*/
PACK(void *) sys_m_shutdown_delay_diag (PACK(void *) req_ptr, uint16 pkt_len)
{
  sys_m_shutdown_delay_req_type *req;

  /* build response */
  tms_diag_send_generic_response(req_ptr);

  // decode request
  req = (sys_m_shutdown_delay_req_type *) req_ptr;

    TMS_MSG_HIGH_1("Added shutdown delay: %d", req->delay_secs);
  // default parameters
  if (pkt_len <= sizeof(req->hdr)) 
  {
    // no delay given so clear it
    sys_m_shutdown_delay_secs_set(0);
  }
  else
  {
    sys_m_shutdown_delay_secs_set(req->delay_secs);
  }

  return(NULL);
}

/**
 *
 * @brief sys_m_diag_init
 *
 * System monitor diag support initialization function
 *
*/
void sys_m_diag_init(void)
{
  sys_m_tbl_diag[0] = tms_diag_build_table_entry(TMS_DIAGPKT_SYS_M_RESTART, sys_m_request_restart_diag);
  sys_m_tbl_diag[1] = tms_diag_build_table_entry(TMS_DIAGPKT_SYS_M_SHUTDOWN_DELAY, sys_m_shutdown_delay_diag);
  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC(SYS_M_DIAG_PROC_ID, DIAG_SUBSYS_DEBUG, sys_m_tbl_diag);

  //initialize timer group for delay  
  memset(&diag_timer_group, 0, sizeof(diag_timer_group)); 
  timer_def(&diagTimer, &diag_timer_group, NULL, 0, sys_m_diag_generic_task_cb, (timer_cb_data_type)&diag_info);
}

