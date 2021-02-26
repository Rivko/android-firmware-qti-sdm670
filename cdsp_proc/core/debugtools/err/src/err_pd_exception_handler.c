/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N   M O D U L E

GENERAL DESCRIPTION
  This module contains the AMSS exception handler

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2014 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_pd_exception_handler.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/15   abh     Added changes for extended smem logging
08/27/14   din     Replaced snprintf with mutex free calls.
07/31/14   din     File created to support Error on User PD.
===========================================================================*/

#include <stdio.h>
#include "comdef.h"
#include "stringl/stringl.h"
#include "qurt_event.h"
#include "erri.h"
#include "err_qdi_client.h"
#include "tms_utils.h"
#include "procinfo_qdi.h"

typedef enum {
  EXCP_UNKNOWN,
  EXCP_SWI,
  EXCP_UNDEF_INST,
  EXCP_MIS_ALIGN,
  EXCP_PAGE_FAULT,
  EXCP_EXE_FAULT,
  EXCP_DIV_BY_0,
  EXCP_MAX_NUM
} exception_type;

char  exception_info[EXCP_MAX_NUM][14] = {
  "    :Excep  ",
  "    :No SWI ",
  "    :Undef  ",
  "    :MisAlgn",
  "    :PFault ",
  "    :XFault ",
  "    :DivBy0 ",
};

char              qxdm_dbg_msg[LEN_QXDM_DBG_MSG];
char              exception_msg[ERR_LOG_MAX_MSG_LEN];

extern coredump_type coredump;
void err_exception_handler( void );
extern void err_fatal_post_exception_processing(void);
extern void err_fatal_jettison_core (unsigned int line, const char *file_name,
  const char *format, uint32 param1, uint32 param2, uint32 param3);
extern void err_emergency_error_recovery( void );
extern void err_update_coredump_tid_and_is_exception(uint32 tid, boolean is_exception);
extern void err_execute_external_cb_pre_STM(uint32 tid);
extern void err_execute_external_cb_post_STM(void);
extern void err_fatal_try_lock(void);
extern char* err_get_tcb_name(void);
extern void err_enter_stm_mode(void);
extern void err_fatal_unlock(void);

#define MAX_FAILURE_COUNT 10

/*===========================================================================
FUNCTION err_has_err_fatal_occured

DESCRIPTION
  Checks if exception is raised because of error fatal

===========================================================================*/

static boolean err_has_err_fatal_occured
(
  unsigned int badva,
  unsigned int cause
)
{
   /*  Test for BADVA is an actual address */
   if ( QURT_EXCEPT_EXIT == cause)
   {
     return ERR_TEST_FOR_ERR_FATAL(badva);
   }

   return FALSE;

} /* err_has_err_fatal_occured */


/*===========================================================================
FUNCTION err_exception_handler

DESCRIPTION
  Handle IPC from QURT Kernel when exceptions occur.

===========================================================================*/
void err_exception_handler(void)
{
  unsigned int     tid;
  qurt_sysevent_error_t sys_err;
  union arch_coredump_union *p_regs=&coredump.arch.regs;
  unsigned int failure_count=0;
  char pname[ERR_PDNAME_MAX_LEN] = {0};
  unsigned int pid;
  
  for (;;)
  {
    /* Register self as Exception_Handler. */
    tid = qurt_exception_wait2(&sys_err);

    /* Try acquiring err_fatal_mutex*/
    err_fatal_try_lock();

    if (-1==tid)
    {
      if ( sizeof(qxdm_dbg_msg) <= tms_utils_fmt(qxdm_dbg_msg, sizeof(qxdm_dbg_msg),
        "Failed to register with qurt_reg_error_handler: tid=%x", tid))
      {
        /* Record failure to coredump */
        strlcpy(coredump.err.int_msg, "Failed to copy failure result in qxdm_dbg_msg", 
          sizeof("Failed to copy exception in qxdm_dbg_msg"));
      }
      
      (void) tms_utils_fmt(coredump.err.int_msg, sizeof(coredump.err.int_msg),
        "Failed to register with qurt_reg_error_handler: tid=%x", tid);
        
      failure_count++;
      if(failure_count >= MAX_FAILURE_COUNT)
      {
        /* Failed to register with qurt_reg_error_handler */
        err_emergency_error_recovery();
      }
      err_fatal_unlock();
      continue;
    }

    /* Get the pid */
    qurt_thread_context_get_pid( tid, &pid ); 
    
    if (PROCINFO_QDI_FAILURE == procinfo_client_get_name(PROCINFO_OEM_NAME_TYPE, pname, sizeof(pname)))
    {
         /* Ignore return value: pname will revert to qurt provided name on failure */
         procinfo_client_get_name(PROCINFO_INTERNAL_NAME_TYPE, pname, sizeof(pname));
    }

    /* Executes Pre STM Callback */
    err_execute_external_cb_pre_STM(tid);

    /* Halt other HW threads for Root process or goto into sw freeze for user process*/
    err_enter_stm_mode();

    /* Update the registers */
    p_regs->name.pc    = sys_err.fault_pc;
    p_regs->name.sp    = sys_err.sp;
    p_regs->name.badva = sys_err.badva;
    p_regs->name.ssr   = sys_err.cause;
    p_regs->name.fp    = sys_err.fp;
    p_regs->name.lr    = sys_err.lr;	  

    /* There are various ways to reach here 
     * 1. Exception Caused by Main thread. ( Handled by if Part )
     * 2. Non recoverable exception raised by island exception handler. ( Handled by if Part )
     * 3. Error fatal in Main image. ( Handled by else Part )
     * 4. Error fatal in uImage. ( Handled by else Part )
     * 5. uImage exception redirected to fatal exception handler from island exception handler
     * 6. User Process ERR_FATAL handled through QDI layer
     * 7. User Process exception -> user process exception handler -> QDI layer -> ERR_FATAL
     * 8. Unhandled exceptions from user process
     * */

    if ( !err_has_err_fatal_occured(sys_err.badva, sys_err.cause) )
    {
      /* Update tid and is_exception to coredump */
      err_update_coredump_tid_and_is_exception(tid, TRUE);
    
      ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_EXCEPTION_PRE);
	      
      err_execute_external_cb_post_STM();

      if ( sizeof(qxdm_dbg_msg) <= tms_utils_fmt(qxdm_dbg_msg, sizeof(qxdm_dbg_msg),
        "ExIPC: Exception received tid=%x inst=%x", tid, sys_err.fault_pc) )
      {
        /* Record failure to coredump */
        strlcpy(coredump.err.int_msg, "Failed to copy exception in qxdm_dbg_msg", 
          sizeof("Failed to copy exception in qxdm_dbg_msg"));        
      }
      
      /* Exception message */
      if ( sizeof(exception_msg) <= tms_utils_fmt(exception_msg, sizeof(exception_msg),
        "EX:%s:%x:%s:%x:PC=%x", pname, pid, err_get_tcb_name(), tid, sys_err.fault_pc))
      {
        /* Record failure to coredump */
        strlcpy(coredump.err.int_msg, "Failed to copy exception_msg", 
          sizeof("Failed to copy exception_msg"));
      }
      
      /* Inform PFR reason to root PD */
      err_qdi_invoke_inform_pfr(sizeof(exception_msg), exception_msg);

      /* Perform higher level error logging - no return */
      err_fatal_jettison_core ( 0, exception_info[EXCP_UNKNOWN], exception_msg, 0, 0, 0);
    }
    else 
    {
      /* Update tid and is_exception to coredump */
      err_update_coredump_tid_and_is_exception(tid, FALSE);
      
      err_fatal_post_exception_processing();
    }
  }
} /* end of err_exception_handler */
