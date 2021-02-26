/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides error reporting services for both fatal and
  non-fatal errors.  This module is not a task, but rather a set of
  callable procedures which run in the context of the calling task.

Copyright (c) 1992 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/15   din     Adapted from err.c

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "qurt.h"
#include "qurt_event.h"
#include "erri.h"
#include <stdlib.h>
#include <stringl.h>
#include "tms_utils.h"

/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/
void err_fatal_handler( void ) ;
void err_fatal_jettison_core (unsigned int line, const char *file_name,
const char *format, uint32 param1, uint32 param2, uint32 param3);

/*===========================================================================

                      Prototypes for external functions

===========================================================================*/
extern void dog_force_kick( void );
extern void err_update_image_versioning_info (void);
extern void err_emergency_error_recovery(void);
extern void err_raise_to_kernel(void);
extern void err_pd_init(void);
extern void err_mem_cache_clean(void);
extern void err_invoke_user_process_cbs(err_cb_bucket_t);
extern void  err_execute_external_cb_normal(void);
extern void  err_execute_external_cb_last(void);
extern void  err_execute_external_cb_late(void);

/*===========================================================================

                      External Data Vairables

===========================================================================*/

extern err_fatal_params_type err_fatal_params;

/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/
#define ERR_SET_AND_FLUSH_PTR(PTR, VAL) \
  do { \
     PTR = (void*)VAL; \
     asm volatile ("dccleana(%0)" : : "r" ((qurt_addr_t )( &PTR )));\
   } while (0)

#define ERR_FLUSH_ADDR(ADDR) \
  do { \
     asm volatile ("dccleana(%0)" : : "r" ((qurt_addr_t )( &ADDR )));\
   } while (0)

/* Macros for CB Mutex type - LOCK will initialize if not initialized */
#define ERR_CB_MUTEX_LOCK(x)         if(!err_internal.err_cb_mutex_init) {   \
                                       ERR_MUTEX_INIT(x);                    \
                                       err_internal.err_cb_mutex_init=TRUE;  \
                                     }                                       \
                                     ERR_MUTEX_LOCK(x);

#define ERR_CB_MUTEX_UNLOCK          ERR_MUTEX_UNLOCK

/* Struct used to hold coredump data */
coredump_type coredump;

typedef struct {
  ERR_MUTEX_TYPE err_cb_mutex; /* CB register/deregister protection */
  boolean err_cb_mutex_init;
} err_internal_s;

static err_internal_s err_internal;

/* The function below are processed by the error handler
 *   err_flush_internal    (one time - transitions to non-returning kernel code)
 */
static const err_cb_ptr err_flush_internal[] =
{
  err_raise_to_kernel,   /* Does not return */
  /* NULL must be last in the array */
  NULL
};

/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION ERR_INITIALIZE_COREDUMP

DESCRIPTION
  Initializes coredump

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void err_initialize_coredump (void)
{
  /* Set type and version values */
  coredump.version = ERR_COREDUMP_VERSION;
  coredump.arch.type = ERR_ARCH_COREDUMP_TYPE;
  coredump.arch.version = ERR_ARCH_COREDUMP_VER;  
  coredump.os.type = ERR_OS_COREDUMP_TYPE;
  coredump.os.version = ERR_OS_COREDUMP_VER;
  coredump.err.version = ERR_COREDUMP_VER;

} /* err_initialize_coredump */

/*===========================================================================

FUNCTION ERR_INIT

DESCRIPTION
  This function initializes error services and calls into target & process 
  based error intializing routine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  For Root PD on MPSS, Error log is loaded from NVM into RAM resident cache.

===========================================================================*/

DLL_API_GLOBAL void err_init (void)
{
  if(!err_fatal_params.err_fatal_mutex_init)
  {
    ERR_MUTEX_INIT(&err_fatal_params.err_fatal_mutex);
    err_fatal_params.err_fatal_mutex_init=TRUE;
  }

  if(!err_internal.err_cb_mutex_init)
  {
    ERR_MUTEX_INIT(&err_internal.err_cb_mutex);
    err_internal.err_cb_mutex_init=TRUE;
  }
  
  err_initialize_coredump();
  
  err_update_image_versioning_info();
} /* err_init */

/*===========================================================================

FUNCTION ERR_UPDATE_COREDUMP_TID_AND_IS_EXCEPTION

DESCRIPTION
 Updates tid information to coredump only the tid is not updated earlier.

DEPENDENCIES

RETURN VALUE
  No return.

SIDE EFFECTS

===========================================================================*/

void err_update_coredump_tid_and_is_exception(uint32 tid, boolean is_exception)
{
  qurt_thread_attr_t *attr = NULL;
  uint32 stack_base = 0;  /* lower start address of the stack */
  uint32 stack_end = 0;   /* higher adress, i.e. range of the stack */
	  
  if ( coredump.err.tid == 0 )
  {
    coredump.err.tid = tid;
  }
  coredump.err.is_exception = is_exception;

  /* Get tcb name from tid name. It uses k0lock which is internal to kernel. 
   * But that doesn’t preempt the caller task in any way */
  attr = qurt_system_stm_thread_attr_get(coredump.err.tid);

  if ( attr != NULL )
  {
    strlcpy(coredump.err.tcb_name, attr->name, QURT_THREAD_ATTR_NAME_MAXLEN );
    stack_base = (uint32)attr->stack_addr;      /* base address of the stack */
    stack_end = stack_base + attr->stack_size;  /* Range of the stack */
  }

  coredump.err.stack_error = STACK_NO_ERROR;

  /* Only check for stack_error if we got the stack addresses */
  if ( stack_base!= 0 && stack_end != 0 )
  {
    /* Check for stack overflow */
    if ( (coredump.arch.regs.name.fp < stack_base) ||
    	   (coredump.arch.regs.name.sp < stack_base) )
    {
      /* stack overflow detected */
  	  coredump.err.stack_error |= STACK_OVERFLOW;
    }

    /* Check for stack underflow */
    if ( (coredump.arch.regs.name.fp > stack_end) ||
    	   (coredump.arch.regs.name.sp > stack_end) )
    {
      /* stack underflow detected */
  	  coredump.err.stack_error |= STACK_UNDERFLOW;
    }
  }
  else
  {
    coredump.err.stack_error |= STACK_ERR_NO_INFO;
  }

} /* err_update_coredump_tid_and_is_exception */

/**
 * Execute external callbacks section
 */
static void err_execute_external_cb(err_cb_bucket_t bucket, uint32 data)
{
  int i = 0;

  err_mem_cache_clean();
  
  for (i=0; i<ERR_CB_MAX; i++)
  {
    if(coredump.err.err_cb[i].info.err_cb.cb == NULL
        || coredump.err.err_cb[i].info.bucket != bucket)
      continue;

    /* Continue, if this callback was already attempted */
    if ( coredump.err.err_cb[i].start_time != 0 )
      continue;

    tms_get_timetick(&(coredump.err.err_cb[i].start_time));
    ERR_FLUSH_ADDR(coredump.err.err_cb[i].start_time);

    coredump.err.err_cb_current = coredump.err.err_cb[i].info.err_cb.cb;
    ERR_FLUSH_ADDR(coredump.err.err_cb_current);

    dog_force_kick();
    if(ERRCB_TYPE_VOID == coredump.err.err_cb[i].info.type)
      coredump.err.err_cb[i].info.err_cb.cb();
    else
      coredump.err.err_cb[i].info.err_cb.cb_u32(data);
    dog_force_kick();

    if(ERRCB_BUCKET_LAST == coredump.err.err_cb[i].info.bucket)
      break; // Need to execute for loop only once (i.e. current one)
  }

  /* Set coredump.err.err_cb_current to NULL signifying completion to the current bucket callbacks */
  coredump.err.err_cb_current = NULL;
  ERR_FLUSH_ADDR(coredump.err.err_cb_current);
}

/*===========================================================================

FUNCTION err_execute_external_cb_pre_STM

DESCRIPTION
 Calls err pre STM cb

DEPENDENCIES

RETURN VALUE
  No return.

SIDE EFFECTS

===========================================================================*/
void err_execute_external_cb_pre_STM(uint32 tid)
{
  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_EXTERNEL_CBS_PRE_STM_PRE);
  err_execute_external_cb(ERRCB_BUCKET_PRE_STM, tid);
} /* err_execute_external_cb_pre_STM */

/*===========================================================================

FUNCTION err_execute_external_cb_post_STM_helper

DESCRIPTION
 Calls err next to STM cb

DEPENDENCIES

RETURN VALUE
  No return.

SIDE EFFECTS

===========================================================================*/
void err_execute_external_cb_post_STM_helper(void)
{
  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_EXTERNEL_CBS_POST_STM_PRE);
  err_execute_external_cb(ERRCB_BUCKET_POST_STM, coredump.err.tid);
}

/*===========================================================================

FUNCTION       err_execute_external_cb_normal_helper

DESCRIPTION
  Executes external callbacks of normal bucket

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void err_execute_external_cb_normal_helper(void)
{
  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_EXTERNEL_CBS_NORMAL_PRE);
  err_execute_external_cb(ERRCB_BUCKET_NORMAL, coredump.err.tid);
}

/*===========================================================================

FUNCTION       err_execute_external_cb_late_helper

DESCRIPTION
  Executes external callbacks of late bucket

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void err_execute_external_cb_late_helper(void)
{
  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_EXTERNEL_CBS_LATE_PRE);
  err_execute_external_cb(ERRCB_BUCKET_LATE, coredump.err.tid);
}

/*===========================================================================

FUNCTION       err_execute_external_cb_last_helper

DESCRIPTION
  Executes external callback of last bucket

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void err_execute_external_cb_last_helper(void)
{
  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_EXTERNEL_CB_LAST_PRE);
  err_execute_external_cb(ERRCB_BUCKET_LAST, coredump.err.tid );
}

/*===========================================================================

FUNCTION ERROR_FATAL_HANDLER

DESCRIPTION
  This function is invoked from err_fatal_jettison_core. When using JTAG,
  default breakpoint for ERR_FATAL should be placed at this function.
  Will log error to SMEM, kill the PA, and copy the coredump data into
  the err_data structure in unintialized memory.


DEPENDENCIES

RETURN VALUE
  No return.

SIDE EFFECTS
  **************************************************************
  ************ THERE IS NO RETURN FROM THIS FUNCTION ***********
  **************************************************************

===========================================================================*/
void err_fatal_handler ( void )
{
  int fptr_index;
  static uint32 err_count=0;

  /* Clean Cache */
  err_mem_cache_clean();

  err_count++;

  if((err_count>1) || (err_fatal_params.err_services_init_complete!=TRUE))
  {
    err_initialize_coredump();
  
    /* May not return */
    err_emergency_error_recovery();
  }

  err_execute_external_cb_normal();

  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_USER_PROCESS_CBS_PRE);
  
  err_invoke_user_process_cbs(ERRCB_BUCKET_NORMAL);

  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_USER_PROCESS_CBS_POST);

  err_execute_external_cb_late();
  err_invoke_user_process_cbs(ERRCB_BUCKET_LATE);

  err_execute_external_cb_last();
  err_invoke_user_process_cbs(ERRCB_BUCKET_LAST);
  
  /* Main loop (cache flush happens here, along with other
   * one-time post-flush operations */
  fptr_index=0;
  while(err_flush_internal[fptr_index] != NULL)
  {
    /* Cycle through internal functions */
    ERR_SET_AND_FLUSH_PTR(coredump.err.err_cb_current, err_flush_internal[fptr_index]); 
    err_flush_internal[fptr_index]();
    fptr_index++;
  }

} /* err_fatal_handler */

/*===========================================================================

FUNCTION ERR_FATAL_TRY_LOCK
DESCRIPTION
  Updates Error functionality at the entry of error. This is intentionally 
  used from exception handler.
============================================================================*/
void err_fatal_try_lock(void)
{
  if(err_fatal_params.err_fatal_mutex_init==TRUE)
  {
    ERR_MUTEX_TRY_LOCK(&err_fatal_params.err_fatal_mutex);
  }
  
  /* tms_get_timetick is not island mode safe */
  tms_get_timetick(&(coredump.err.err_handler_start_time));

  /* Kick Dog */
  dog_force_kick();

} /* err_fatal_try_lock */

/*===========================================================================

FUNCTION ERR_FATAL_UNLOCK
DESCRIPTION
  Unlocks Error mutex lock.
============================================================================*/
void err_fatal_unlock(void)
{
  if(err_fatal_params.err_fatal_mutex_init==TRUE)
  {
    ERR_MUTEX_UNLOCK(&err_fatal_params.err_fatal_mutex);
  }

} /* err_fatal_unlock */

/*===========================================================================

FUNCTION ERR_FATAL_CORE_DUMP
DESCRIPTION
  Logs fatal error information, including a core dump.

  NOTE: There is no return from this function.
============================================================================*/
void err_fatal_core_dump (
  unsigned int line,      /* From __LINE__ */
  const char   *file_name, /* From __FILE__ */
  const char   *format   /* format string */
)
{
  err_fatal_lock();
  err_fatal_jettison_core(line, file_name, format, 0, 0, 0);
}

/*===========================================================================

FUNCTION ERR_GET_TCB_NAME
DESCRIPTION
  Returns TCB NAME of the faulting thread.
  Should be used only after err_update_coredump_tid_and_is_exception()

  NOTE: There is no return from this function.
============================================================================*/

char * err_get_tcb_name (void)
{
  return coredump.err.tcb_name;
}


/*===========================================================================

FUNCTION ERR_FATAL_JETTISON_CORE
DESCRIPTION
  Logs fatal error information, including a core dump.
  Not to be called directly by outside code -- for that, use the function
  err_fatal_core_dump().

  NOTE: There is no return from this function.
============================================================================*/
#define ERR_FATAL_EXCEPTION_REENTRANCY "ERR_FATAL_EXCEPTION_REENTRANCY"

void err_fatal_jettison_core (
  unsigned int line,       /* From __LINE__ */
  const char   *file_name, /* From __FILE__ */
  const char   *format,    /* format string */
  uint32 param1,
  uint32 param2,
  uint32 param3
)
{

  /* NOTE: register information should already be saved prior to
   * calling this function.
   */

  /* Store line number */
  coredump.err.linenum = line;

  /* Copy file name */
  if(file_name != 0)
  {
    (void) strlcpy((char *)coredump.err.filename,
                       (char *)file_name,
                       ERR_LOG_MAX_FILE_LEN);
  }

  /* Copy message string */
  if(format != 0)
  {
    (void) strlcpy((char *)coredump.err.message,
                       (char *)format,
                       ERR_LOG_MAX_MSG_LEN);
  }

  coredump.err.param[0]=param1;
  coredump.err.param[1]=param2;
  coredump.err.param[2]=param3;

  if (err_fatal_params.aux_msg[0] != 0)
  {
    (void) strlcpy((char *)coredump.err.aux_msg,
                       (char *)err_fatal_params.aux_msg,
                       ERR_LOG_MAX_MSG_LEN);
  }

  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_ERR_FATAL_JETTISION_CORE_POST);

  /* Update the coredump pid */
  qurt_thread_context_get_pid( coredump.err.tid, 
		               &coredump.err.pd_id ); 

  /* Check if an user exceptions and ERR_FATAL arrived simultaneously */
  if ( err_fatal_params.crumb_trail_bmsk != 0 && 
        coredump.err.is_exception == TRUE)
  {
    /* Record secondary failure to coredump */
    strlcpy(coredump.err.int_msg, ERR_FATAL_EXCEPTION_REENTRANCY, 
	    sizeof(ERR_FATAL_EXCEPTION_REENTRANCY));   
  } 

  /* Call ERR_FATAL handler (no return) */
  err_fatal_handler();

}

/** err_cb_enable
 * This will allow clients to register for callback to be invoke in path
 * of error handling post-crash.
 *
 * @param cb_info err_cb_info_t structure having required info.
 *
 * @return ERRCB_E_SUCCESS in case of successful registration else
 *         err_cb_error_t error codes.
 */
DLL_API_GLOBAL err_cb_error_t err_cb_enable(err_cb_info_t * cb_info)
{
  int i = 0;

  /* input validation */
  if(cb_info->err_cb.cb == NULL && cb_info->err_cb.cb_u32 == NULL)
    return ERRCB_E_INVALID_PARAM;

  /* only normal order allowed for now */
  if(cb_info->order != ERRCB_ORDER_NORMAL)
    return ERRCB_E_NOT_ALLOWED;

  ERR_CB_MUTEX_LOCK(&err_internal.err_cb_mutex);

  /* only one cb is allowed in this bucket */
  if(cb_info->bucket == ERRCB_BUCKET_LAST)
  {
    for (i=0; i<ERR_CB_MAX; i++)
    {
      if(coredump.err.err_cb[i].info.err_cb.cb != NULL)
      {
        if (coredump.err.err_cb[i].info.bucket == ERRCB_BUCKET_LAST)
        {
          ERR_CB_MUTEX_UNLOCK(&err_internal.err_cb_mutex);
          return ERRCB_E_NOT_ALLOWED;
        }
      }
    }
  }

  /* register callback */
  for (i=0; i<ERR_CB_MAX; i++)
  {
    if(coredump.err.err_cb[i].info.err_cb.cb == NULL)
    {
      coredump.err.err_cb[i].info.type = cb_info->type;
      /* err_cb is a union of cb and cb_u32 and are of the same size */
      coredump.err.err_cb[i].info.err_cb.cb = cb_info->err_cb.cb;
      coredump.err.err_cb[i].info.bucket = cb_info->bucket;
      coredump.err.err_cb[i].info.order = cb_info->order;
      ERR_CB_MUTEX_UNLOCK(&err_internal.err_cb_mutex);
      return ERRCB_E_SUCCESS;
    }
  }

  ERR_CB_MUTEX_UNLOCK(&err_internal.err_cb_mutex);
  return ERRCB_E_MAX_LIMIT_REACHED;
}

/** err_cb_disable
 * This will allow clients to de-register callback, which was register via 
 * err_cb_enable.
 *
 * @param cb_info err_cb_info_t structure having required info.
 *
 * @return ERRCB_E_SUCCESS in case of successful de-registration else
 *         err_cb_error_t error codes.
 */
DLL_API_GLOBAL err_cb_error_t err_cb_disable(err_cb_info_t * cb_info)
{
  int i = 0;
  
  if(cb_info->err_cb.cb == NULL && cb_info->err_cb.cb_u32 == NULL)
    return ERRCB_E_INVALID_PARAM;

  ERR_CB_MUTEX_LOCK(&err_internal.err_cb_mutex);

  for (i=0; i<ERR_CB_MAX; i++)
  {
    if(coredump.err.err_cb[i].info.err_cb.cb == cb_info->err_cb.cb)
    {
      coredump.err.err_cb[i].info.type = 0;
      coredump.err.err_cb[i].info.err_cb.cb = NULL;
      coredump.err.err_cb[i].info.bucket = 0;
      coredump.err.err_cb[i].info.order = 0;
      ERR_CB_MUTEX_UNLOCK(&err_internal.err_cb_mutex);      
      return ERRCB_E_SUCCESS;
    }
  }

  ERR_CB_MUTEX_UNLOCK(&err_internal.err_cb_mutex);
  return ERRCB_E_NOT_FOUND;
}

/*=========================================================================

FUNCTION err_crash_cb_register

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called after an ERR_FATAL
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.

  !!!These functions MUST NOT call ERR_FATAL/ASSERT under ANY circumstances!!!

DEPENDENCIES
  None

RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  None

===========================================================================*/
DLL_API_GLOBAL boolean err_crash_cb_register(err_cb_ptr cb)
{
  err_cb_info_t cb_info;
  err_cb_error_t rv = ERRCB_E_FAILURE;

  cb_info.type = ERRCB_TYPE_VOID;
  cb_info.err_cb.cb = cb;
  cb_info.bucket = ERRCB_BUCKET_NORMAL;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_enable(&cb_info);
  if(ERRCB_E_SUCCESS == rv)
    return TRUE;
  else
    return FALSE;
}


/*=========================================================================

FUNCTION err_crash_cb_dereg

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
DLL_API_GLOBAL boolean err_crash_cb_dereg(err_cb_ptr cb)
{
  err_cb_info_t cb_info;
  err_cb_error_t rv = ERRCB_E_FAILURE;

  cb_info.type = ERRCB_TYPE_VOID;
  cb_info.err_cb.cb = cb;
  cb_info.bucket = ERRCB_BUCKET_NORMAL;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_disable(&cb_info);
  if(ERRCB_E_SUCCESS == rv)
    return TRUE;
  else
    return FALSE;
}

/*=========================================================================

FUNCTION err_crash_cb_reg_pre_STM

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called immediately before
  STM API call 
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.
 
  !!!These functions MUST NOT call ERR_FATAL under ANY circumstances!!!

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  Only one registration of such API is supported so if its used by more than one
  clients than it will overwrite the old registered callback.
  
===========================================================================*/
DLL_API_GLOBAL boolean err_crash_cb_reg_pre_STM(err_cb_ptr_u32 cb)
{
  err_cb_info_t cb_info;
  err_cb_error_t rv = ERRCB_E_FAILURE;

  cb_info.type = ERRCB_TYPE_UINT32;
  cb_info.err_cb.cb_u32 = cb;
  cb_info.bucket = ERRCB_BUCKET_PRE_STM;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_enable(&cb_info);
  if(ERRCB_E_SUCCESS == rv)
    return TRUE;
  else
     return FALSE;
} /* err_crash_cb_reg_pre_STM */

/*=========================================================================

FUNCTION err_crash_cb_dereg_pre_STM

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
DLL_API_GLOBAL boolean err_crash_cb_dereg_pre_STM(err_cb_ptr_u32 cb)
{
  err_cb_info_t cb_info;
  err_cb_error_t rv = ERRCB_E_FAILURE;

  cb_info.type = ERRCB_TYPE_UINT32;
  cb_info.err_cb.cb_u32 = cb;
  cb_info.bucket = ERRCB_BUCKET_PRE_STM;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_disable(&cb_info);
  if(ERRCB_E_SUCCESS == rv)
    return TRUE;
  else
    return FALSE;
} /* err_crash_cb_dereg_pre_STM */

/*=========================================================================

FUNCTION err_crash_cb_reg_next_to_STM

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called immediately after
  STM API call 
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.
 
  !!!These functions MUST NOT call ERR_FATAL under ANY circumstances!!!

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  Only one registration of such API is supported so if its used by more than one
  clients than it will overwrite the old registered callback,
  this API was provided only for special case handling to stop ULT Audio Core
  in DPM PL
===========================================================================*/
DLL_API_GLOBAL boolean err_crash_cb_reg_next_to_STM(err_cb_ptr cb)
{
  err_cb_info_t cb_info;
  err_cb_error_t rv = ERRCB_E_FAILURE;

  cb_info.type = ERRCB_TYPE_VOID;
  cb_info.err_cb.cb = cb;
  cb_info.bucket = ERRCB_BUCKET_POST_STM;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_enable(&cb_info);
  if(ERRCB_E_SUCCESS == rv)
    return TRUE;
  else
     return FALSE;
}
/*=========================================================================

FUNCTION err_crash_cb_dereg_next_to_STM

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
DLL_API_GLOBAL boolean err_crash_cb_dereg_next_to_STM(err_cb_ptr cb)
{
  err_cb_info_t cb_info;
  err_cb_error_t rv = ERRCB_E_FAILURE;

  cb_info.type = ERRCB_TYPE_VOID;
  cb_info.err_cb.cb = cb;
  cb_info.bucket = ERRCB_BUCKET_POST_STM;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_disable(&cb_info);
  if(ERRCB_E_SUCCESS == rv)
    return TRUE;
  else
    return FALSE;
}

/*===========================================================================
FUNCTION err_crash_cb_postflush_register
DESCRIPTION
  Register for callback function. The Callback function will be called after
  external functions are already called.
  It will be upto the callback to resolve its cache issues. 
============================================================================*/
DLL_API_GLOBAL boolean err_crash_cb_postflush_register (err_cb_ptr cb)
{
  err_cb_info_t cb_info;
  err_cb_error_t rv = ERRCB_E_FAILURE;

  cb_info.type = ERRCB_TYPE_VOID;
  cb_info.err_cb.cb = cb;
  cb_info.bucket = ERRCB_BUCKET_LATE;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_enable(&cb_info);
  if(ERRCB_E_SUCCESS == rv)
    return TRUE;
  else
    return FALSE;

} /* err_crash_cb_postflush_register*/

DLL_API_GLOBAL boolean err_crash_cb_postflush_deregister (err_cb_ptr cb)
{
  err_cb_info_t cb_info;
  err_cb_error_t rv = ERRCB_E_FAILURE;

  cb_info.type = ERRCB_TYPE_VOID;
  cb_info.err_cb.cb = cb;
  cb_info.bucket = ERRCB_BUCKET_LATE;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_disable(&cb_info);
  if(ERRCB_E_SUCCESS == rv)
    return TRUE;
  else
    return FALSE;

} /* err_crash_cb_postflush_deregister */
	

/*=========================================================================

FUNCTION err_update_crumb_trail_mask

DESCRIPTION
  Updates Crumb trail mask

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void err_update_crumb_trail_mask(uint32 bits)
{
  ERR_CRUMB_TRAIL_BMSK(bits);

} /* err_update_crumb_trail_mask*/

/*=========================================================================

FUNCTION err_update_reentracny_flag

DESCRIPTION
  Updates Crumb trail mask

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define ERR_REENTRANCY_STRING "ERR_FATAL reentrancy violation, remove cb until resolved"
void err_update_reentrancy_flag(void)
{
  /* Record secondary failure to coredump */
  strlcpy(coredump.err.int_msg, ERR_REENTRANCY_STRING, 
          sizeof(ERR_REENTRANCY_STRING));

} /* err_update_reentrancy_flag */
