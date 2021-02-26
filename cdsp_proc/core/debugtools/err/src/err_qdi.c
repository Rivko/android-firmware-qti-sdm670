/*=============================================================================

                 err_qdi.c

GENERAL DESCRIPTION
   Implements QDI layer for Err that goes into main image

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2014,2018 by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_qdi.c#1 $ 
$DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/01/15   abh     Added changes for extended smem logging
08/27/14   din     Replaced snprintf with mutex free calls.
07/31/14   din     File created to support Error on User PD.

=============================================================================*/


/*****************************************************************************/
/*                           INCLUDE FILES                                   */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stringl/stringl.h>
#include "err_qdi.h"
#include "pd_mon_restart.h"
#include "err.h"
#include "tms_utils.h"
#include "tms_utils_msg.h"
#include "err_smem_log.h"
#include "timer.h"

/*****************************************************************************/
/*                         MACRO DECLARATIONS                                */
/*****************************************************************************/

/* Timeout value for pdtimeout timer, Unit in Msecs */
#define ERR_QDI_PDTIMEOUT_TIMER_PERIOD_DEFAULT  2000

#define ERR_FLUSH_ADDR(ADDR) \
  do { \
     asm volatile ("dccleana(%0)" : : "r" ((qurt_addr_t )( &ADDR )));\
   } while (0)

/*****************************************************************************/
/*                         DATA DECLARATIONS                                 */
/*****************************************************************************/
typedef struct err_qdi_opener_struct 
{
  /* Should be the first member */
  qurt_qdi_obj_t                 qdiobj;
  err_qdi_internal_s             user_pd_data;
  struct err_qdi_opener_struct   *next;
}err_qdi_opener_s;

typedef struct
{
  qurt_mutex_t     mutex;
  qurt_mutex_t     mutex_cb;  /* Mutex to serialize user PD callbacks */
  uint32           pd_callback_timeout_msec;
  uint32           pd_exit_timeout_msec;
  boolean          subsystem_pdr_enabled;
  err_qdi_opener_s *list;
}err_qdi_s;

static err_qdi_s err_qdi;


/* MACRO for locking/unlocking the qmutex*/
#define ERR_QDI_MUTEX_LOCK(x)  \
   qurt_pimutex_lock(x)
  
#define ERR_QDI_MUTEX_TRY_LOCK(x)  \
   qurt_pimutex_try_lock(x)

#define ERR_QDI_MUTEX_UNLOCK(x)  \
   qurt_pimutex_unlock(x)

#define ERR_QDI_MUTEX_INIT(x) \
   qurt_pimutex_init(x)

/*===========================================================================

                      Prototypes for external functions

===========================================================================*/
extern void err_mem_cache_clean(void);
extern void servreg_loc_send_pfr(char * pd_name, int pd_name_len, char * pfr, int pfr_len);

/*****************************************************************************/
/*                      FUNCTION DECLARATIONS                                */
/*****************************************************************************/
extern void *err_qdi_pdr_records_user_err_info(err_qdi_internal_s * dataobj);
extern void err_qdi_pdr_update_pd_exit_end_time
(
  void * temp_events,
  DalTimetickTime64Type   pd_exit_end_time
);
extern void err_qdi_pdr_records_init(void);
extern void err_qdi_dalcfg_get_pd_timeout
( 
  uint32 * pd_timeout_cb_msec, 
  uint32 * pd_timeout_exit_msec 
);

int err_qdi_invoke(int client_handle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9);

extern int pd_mon_private_proxy_attach(int client_asid, qurt_qdi_obj_t *obj_ptr);

/*=============================================================================

FUNCTION ERR_QDI_PROCESS_PDTIMEOUT_TIMER_EXPIRY

DESCRIPTION
  Crashes the system in the event of user PD error callbacks taking more than 
  expected time or during PD exit failure

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None.

=============================================================================*/

void err_qdi_process_pdtimeout_timer_expiry( timer_cb_data_type data )
{
  err_qdi_internal_s * dataobj;
  err_qdi_opener_s * clntobj = (err_qdi_opener_s *)(data);
  
  if( clntobj == NULL )
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0,0,0);
    return ;
  }
  
  /* CPU & DDR unvote for vote made in err_qdi_err_handling_done() */
  tms_utils_unvote_cpu_ddr();

  ERR_QDI_MUTEX_LOCK(&err_qdi.mutex);
  
  dataobj = &(clntobj->user_pd_data);
   
  if (dataobj->state == ERR_QDI_USERPD_ERR_INITIATED)
  {
    ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex); 
    /* This branch will not return */
    ERR_FATAL_AUX_MSG("USER PD crashed, it's callbacks are stuck, check coredump.err.aux_msg", 
      dataobj->pfr, ERR_QDI_PFR_LEN);
  }
  else
  {
    ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);  
    /* This branch will not return */
    ERR_FATAL_AUX_MSG("USER PD crashed, PD_exit is stuck, check coredump.err.aux_msg",
      dataobj->pfr, ERR_QDI_PFR_LEN);	
  }
} /* err_qdi_process_pdtimeout_timer_expiry */


/*=============================================================================

FUNCTION ERR_QDI_TERMINATE

DESCRIPTION
 Releases the Qdi object that corresponding to a process that got destroyed

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None.

=============================================================================*/
void err_qdi_terminate(qurt_qdi_obj_t *obj)
{
  err_qdi_opener_s *temp, *clntobj = (err_qdi_opener_s *)obj;
  err_qdi_internal_s * dataobj;
  DalTimetickTime64Type   pd_exit_end_time;

  if( clntobj == NULL )
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0,0,0);
    return ;
  }

  /* CPU & DDR unvote for PD error handling end, 
     vote made in err_qdi_err_handling_done() */
  tms_utils_unvote_cpu_ddr();
  
  ERR_QDI_MUTEX_LOCK(&err_qdi.mutex);
  
  dataobj = &(clntobj->user_pd_data);

  /* Clear the pdtimeout timer if set */
  if ( dataobj->pdtimeout_timer_set == TRUE )
  {
    (void)timer_clr_64( &(dataobj->pdtimeout_timer), T_MSEC );
    (void)timer_undef( &(dataobj->pdtimeout_timer) );
    dataobj->pdtimeout_timer_set = FALSE;
  }

  tms_get_timetick(&pd_exit_end_time);
 
  /* Update PDR records with PD exit time */
  err_qdi_pdr_update_pd_exit_end_time( dataobj->pdr_record_data, 
                                       pd_exit_end_time);

  if ( err_qdi.list == NULL  )
  {
    ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);
    return;
  }

  if ( err_qdi.list != NULL && err_qdi.list == clntobj )
  {
    err_qdi.list = clntobj->next;
    free(clntobj);
    ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);
    return;
  }
  
  for (temp = err_qdi.list; NULL != temp; temp = temp->next)
  {
    if ( temp->next == clntobj )
    {
      temp->next = clntobj->next;
      free(clntobj);
      ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);
      return;
    }
  }

  ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);

  /* Should not reach here */
  ERR_FATAL("Release Handle not found in the list", 0,0,0);

}/* err_qdi_terminate */


/*=============================================================================

FUNCTION ERR_QDI_SEND_PDR_INFO

DESCRIPTION
 Updates PDR policy to Root PD , if there is a change.

DEPENDENCIES
  None

RETURN VALUE
 ERR_QDI_SUCCESS - for success
 ERR_QDI_FAIL    - for failure

SIDE EFFECTS
  None

=============================================================================*/
err_qdi_e err_qdi_send_pdr_info
(
  err_qdi_opener_s * clntobj, 
  boolean            user_pdr_enabled
)
{
  if( clntobj == NULL )
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0,0,0);
    return ERR_QDI_NOT_INITIALIZED;
  }

  ERR_QDI_MUTEX_LOCK(&err_qdi.mutex); 

  clntobj->user_pd_data.pdr_enabled = user_pdr_enabled;

  ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);
  
  return ERR_QDI_SUCCESS;

} /* err_qdi_send_pdr_info */
	
/*=============================================================================

FUNCTION ERR_QDI_INFORM_PFR

DESCRIPTION
 Communicates PFR to Root PD in case of error/exceptions in User PD

DEPENDENCIES
  None

RETURN VALUE
 ERR_QDI_SUCCESS - for success
 ERR_QDI_FAIL    - for failure

SIDE EFFECTS
  None

=============================================================================*/
err_qdi_e err_qdi_inform_pfr
(
   int                client_handle,
   err_qdi_opener_s   *clntobj, 
   uint32             pfr_len, 
   char               *pfr
)
{
  err_qdi_internal_s * dataobj;
  int                  result;
  
  if( clntobj == NULL )
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0,0,0);
    /* Deliberate */
    return ERR_QDI_NOT_INITIALIZED;
  }

  ERR_QDI_MUTEX_LOCK(&err_qdi.mutex);
  
  dataobj = &(clntobj->user_pd_data);

  /* Not Checking for err_initialized. If this routine is called, 
   * pfr would be updated */

  dataobj->pfr_len = pfr_len > ERR_QDI_PFR_LEN ? ERR_QDI_PFR_LEN : pfr_len;

  result = qurt_qdi_copy_from_user(client_handle, dataobj->pfr, 
                                   pfr, dataobj->pfr_len);

  if (result)
  {
    ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);
    return ERR_QDI_INVALID_PARAMETERS;
  }

  /* Guarantee NULL termination, PFR buffer is expected to zeroed out during initialization */
  dataobj->pfr[ERR_QDI_PFR_LEN -1] =0;

  tms_get_timetick(&(dataobj->start_time));

  if ( err_qdi.pd_callback_timeout_msec != 0)
  {
    /* Set the timer to cover timeout cases for user PD 
       error callbacks failure */
    timer_set_64( &(dataobj->pdtimeout_timer), 
		  err_qdi.pd_callback_timeout_msec, 0, T_MSEC  );
    dataobj->pdtimeout_timer_set = TRUE;

  }

  dataobj->state = ERR_QDI_USERPD_ERR_INITIATED;

  ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);

  return ERR_QDI_SUCCESS; 

} /* err_qdi_inform_pfr */


/*=============================================================================

FUNCTION ERR_QDI_ERR_HANDLING_DONE

DESCRIPTION
 Communicates to Root PD that handling routine on User PD is complete.

DEPENDENCIES
  None

RETURN VALUE
 ERR_QDI_SUCCESS - for success
 ERR_QDI_FAIL    - for failure

SIDE EFFECTS
  None

=============================================================================*/
err_qdi_e err_qdi_err_handling_done
(
  err_qdi_opener_s   *clntobj
)
{
  err_qdi_internal_s * dataobj;

  if( clntobj == NULL )
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0,0,0);
    /* Deliberate */
    return ERR_QDI_NOT_INITIALIZED;
  }

  ERR_QDI_MUTEX_LOCK(&err_qdi.mutex);
  
  dataobj = &(clntobj->user_pd_data);
  
  /* State should be ERR_QDI_USERPD_INITIALIZED or ERR_QDI_USERPD_ERR_INITIATED */
  if(dataobj->state < ERR_QDI_USERPD_INITIALIZED)
  {
    ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);
    return ERR_QDI_NOT_INITIALIZED;
  } 

  /* Clear the pdtimeout timer if set */
  if ( dataobj->pdtimeout_timer_set == TRUE )
  {
    (void)timer_clr_64( &(dataobj->pdtimeout_timer), T_MSEC );
    dataobj->pdtimeout_timer_set = FALSE;
  }

  tms_get_timetick(&(dataobj->end_time));

  dataobj->state = ERR_QDI_USERPD_ERR_HANDLING_COMPLETED;

  if ( !( err_qdi.subsystem_pdr_enabled && dataobj->pdr_enabled) )
  {
    ERR_FATAL_AUX_MSG("PDR Disabled, USER PD crashed, check coredump.aux_msg for details", 
		            dataobj->pfr, ERR_QDI_PFR_LEN);
  }
  /* PDR Allowed */
  else
  {
    /* Vote for CPU & DDR during PD error handling start. This will unvote
       in err_qdi_terminate() or in case PD error handling stuck:
       unvote in err_qdi_process_pdtimeout_timer_expiry(). */
    tms_utils_vote_cpu_ddr();

    dataobj->state = ERR_QDI_USERPD_ERR_PD_EXIT_STARTED;
    
    /* Adding late process exit notification to clear the pdtimeout timer. */
    if ( -1 == pd_mon_private_proxy_attach((int)dataobj->pid, &clntobj->qdiobj))
    {
      ERR_FATAL("Registration of Late Notification for PD exit timeout timer failed",
		0, 0, 0);
    }
    
    /* Record PDR info */
    dataobj->pdr_record_data = err_qdi_pdr_records_user_err_info( dataobj);

    /*send PD failure reason to APSS*/
    servreg_loc_send_pfr(dataobj->pd_name, strlen(dataobj->pd_name), dataobj->pfr, strlen(dataobj->pfr));

    if ( err_qdi.pd_exit_timeout_msec != 0 )
    {
      /* Set the timer to cover timeout cases for PD exit pdtimeouts */
      timer_set_64( &(dataobj->pdtimeout_timer), 
		     err_qdi.pd_exit_timeout_msec, 0, T_MSEC  );
      dataobj->pdtimeout_timer_set = TRUE;
    }
    
    ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);
    
    if ( dataobj->pfr_len >= 1 )
    {
      TMS_MSG_SPRINTF_HIGH_2( "PD %u crashed: %s", 
                  (unsigned int )dataobj->pid, dataobj->pfr );
    }
  }

  return ERR_QDI_SUCCESS;

} /* err_qdi_err_handling_done */

/*=============================================================================

FUNCTION ERR_QDI_OPEN

DESCRIPTION
 Creates err driver object for the client and initializes required details.

DEPENDENCIES
  None

RETURN VALUE
 QURT_EOK if the driver obj is defined and set correctly.

SIDE EFFECTS
  None

=============================================================================*/
static int err_qdi_open(int client_handle)
{
   err_qdi_internal_s * dataobj;
   qurt_sysenv_procname_t pn={0};
   int rc = -1;
   err_qdi_opener_s *clntobj = calloc(1, sizeof(err_qdi_opener_s));
   
   if ( clntobj == NULL )
   {
     return -1;
   }

   /* QDI_OS_SYSENV is the standard method for requesting 
    * this type of information */
   /* QDI_HANDLE_LOCAL_CLIENT means that this request originated locally */
   /* QURT_SYSENV_PROCNAME_TYPE is the specific piece of data being requested 
    * -- to fill in a qurt_sysenv_procname_t structure */
   /* &pn is the structure we want filled in */

   if (0 > (rc = qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV, 
				        QDI_HANDLE_LOCAL_CLIENT, 
					QURT_SYSENV_PROCNAME_TYPE, &pn)) )
   {
      TMS_MSG_HIGH_2( "Cannot obtain client info %d %d", pn.asid, rc);
      free(clntobj);
      return rc;
   }

   ERR_QDI_MUTEX_LOCK(&err_qdi.mutex);
   
   clntobj->qdiobj.invoke    = err_qdi_invoke;
   clntobj->qdiobj.refcnt    = QDI_REFCNT_INIT;
   clntobj->qdiobj.release   = err_qdi_terminate;

   dataobj = &(clntobj->user_pd_data);
   
   /* Populate PD name and PID */
   strlcpy(dataobj->pd_name, pn.name, ERR_QDI_DEVICE_NAME_LEN);
   dataobj->pid = pn.asid;
   dataobj->state = ERR_QDI_USERPD_INITIALIZED;

   /* Insert at the head of the list */
   clntobj->next = err_qdi.list;
   err_qdi.list= clntobj;

   dataobj->client_handle = client_handle;

   timer_def_osal( &(dataobj->pdtimeout_timer), NULL, 
                   TIMER_FUNC1_CB_TYPE, 
		   &err_qdi_process_pdtimeout_timer_expiry, (uint32)clntobj );

   ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);
   
   return qurt_qdi_handle_create_from_obj_t(client_handle, &clntobj->qdiobj);

} /* err_qdi_open */

/*=============================================================================

FUNCTION ERR_QDI_CHECK_PDR_ENABLED

DESCRIPTION
 Checks if given PD Restart is enabled for given subsystem for given ASID 
 by walking list in err_qdi internal structure
 
DEPENDENCIES
  None

RETURN VALUE
 True if PDR is enabled for both subsystem and PD

SIDE EFFECTS
  None

=============================================================================*/
boolean err_qdi_check_pdr_enabled(int asid)
{
  err_qdi_opener_s *temp;
  err_qdi_internal_s * dataobj;
  boolean ret = FALSE;
  
  if (!err_qdi.subsystem_pdr_enabled || err_qdi.list == NULL)
  {
    return ret;
  }

  /* Walk the list, find matching PID and return pdr_enabled for that PID */
  for (temp = err_qdi.list; NULL != temp; temp = temp->next)
  {
    dataobj = &(temp->user_pd_data);

    if(dataobj->pid == asid)
    {
      ret = dataobj->pdr_enabled;
      break;
    }
  }
  
  return ret;
}

/*=============================================================================

FUNCTION ERR_QDI_COMMUNICATE_USER_CB

DESCRIPTION
 Communicates user callback addresses with root process err QDI layer.

DEPENDENCIES
  None

RETURN VALUE
 ERR_QDI_SUCCESS - for success
 ERR_QDI_FAIL    - for failure

SIDE EFFECTS
  None

=============================================================================*/
err_qdi_e err_qdi_communicate_user_cb
(
   err_qdi_opener_s   *clntobj,
   void               *pre_STM_cb,
   void               *post_STM_cb,
   void               *normal_cb,
   void               *late_cb,
   void               *last_cb,
   void               *stack_addr,
   uint32             stack_size
)
{
  err_qdi_internal_s * dataobj;

  if( clntobj == NULL )
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0,0,0);
    /* Deliberate */
    return ERR_QDI_NOT_INITIALIZED;
  }

  ERR_QDI_MUTEX_LOCK(&err_qdi.mutex);
  
  dataobj = &(clntobj->user_pd_data);
  
  /* State should be ERR_QDI_USERPD_INITIALIZED or ERR_QDI_USERPD_ERR_INITIATED */
  if(dataobj->state < ERR_QDI_USERPD_INITIALIZED)
  {
    ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);
    return ERR_QDI_NOT_INITIALIZED;
  }

  if( dataobj->user_cb.stack_addr == NULL )
  {
    dataobj->user_cb.stack_addr = (void*)(stack_addr);
    dataobj->user_cb.stack_size = stack_size;
  }

  dataobj->user_cb.user_cb[ERRCB_BUCKET_PRE_STM].cb = pre_STM_cb;
  dataobj->user_cb.user_cb[ERRCB_BUCKET_POST_STM].cb = post_STM_cb;
  dataobj->user_cb.user_cb[ERRCB_BUCKET_NORMAL].cb = normal_cb;
  dataobj->user_cb.user_cb[ERRCB_BUCKET_LATE].cb = late_cb;
  dataobj->user_cb.user_cb[ERRCB_BUCKET_LAST].cb = last_cb;

  ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex);
  
  return  ERR_QDI_SUCCESS;

} /* err_qdi_communicate_user_cb */

/*=============================================================================

FUNCTION ERR_QDI_INVOKE_USER_PROCESS_CB_HELPER

DESCRIPTION
  Callbacks belonging to user PDs are invoked

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
 None

=============================================================================*/
static void  err_qdi_invoke_user_process_cb_helper(err_qdi_internal_s * dataobj, err_cb_bucket_t bucket)
{
  /* Array boundary check*/
  if ( 0 > bucket || ERRCB_BUCKET_MAX <= bucket )
    return;

  /* Return early, if this callback was already attempted */
  if (dataobj->user_cb.user_cb[bucket].start_tick != 0 )
    return;

  /* Log start tick */
  tms_get_timetick(&(dataobj->user_cb.user_cb[bucket].start_tick));
  
  /* Clean cache to preserve the state of system before the callback */
  err_mem_cache_clean();

  if ( NULL == dataobj->user_cb.user_cb[bucket].cb || NULL == dataobj->user_cb.stack_addr || 0 == dataobj->user_cb.stack_size )
    return;

  qurt_qdi_handle_invoke(dataobj->client_handle, QDI_CLIENT_HANDLE_CALLBACK,
                         dataobj->user_cb.user_cb[bucket].cb,
                         dataobj->user_cb.stack_addr,
                         dataobj->user_cb.stack_size);

  /* Log end tick */
  tms_get_timetick(&(dataobj->user_cb.user_cb[bucket].end_tick));

  /* Clean cache to preserve the state of system after the callback */
  err_mem_cache_clean();
}

/*=============================================================================

FUNCTION ERR_QDI_INVOKE_ALL_USER_PROCESS_CBS

DESCRIPTION
  Callbacks belonging to user PDs are invoked

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
 None

=============================================================================*/
void  err_qdi_invoke_all_user_process_cbs(err_cb_bucket_t bucket)
{
  err_qdi_opener_s *temp;
  err_qdi_internal_s * dataobj;
  
  err_mem_cache_clean();
  
  if ( err_qdi.list == NULL  )
  {
     return;
  }

  for (temp = err_qdi.list; NULL != temp; temp = temp->next)
  {
    dataobj = &(temp->user_pd_data);

    /* State should be ERR_QDI_USERPD_INITIALIZED or ERR_QDI_USERPD_ERR_INITIATED */
    if(dataobj->state < ERR_QDI_USERPD_INITIALIZED)
    {
      continue;
    }
    
    /* Execute callbacks only if global ERR QDI mutex can be acquired.
     * P.S. Thread spawned by QDI_CLIENT_HANDLE_CALLBACK is one time 
     * global resource */
    if ( 0 == ERR_QDI_MUTEX_TRY_LOCK(&err_qdi.mutex_cb) )
    {
      err_qdi_invoke_user_process_cb_helper(dataobj,bucket);
      ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex_cb);
    }
   
  }
}

/*=============================================================================

FUNCTION ERR_QDI_INVOKE_USER_PROCESS_CBS

DESCRIPTION
  Callbacks belonging to user PDs are invoked

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
 None

=============================================================================*/
void err_qdi_invoke_user_process_cbs(uint32 pid)
{
  err_qdi_opener_s *temp;
  err_qdi_internal_s * dataobj;
  
  err_mem_cache_clean();
  
  if ( err_qdi.list == NULL  )
  {
     return;
  }

  for (temp = err_qdi.list; NULL != temp; temp = temp->next)
  {
    dataobj = &(temp->user_pd_data);

    if ( dataobj->pid != pid )
    {
      continue;
    }

    /* State should be ERR_QDI_USERPD_INITIALIZED or ERR_QDI_USERPD_ERR_INITIATED */
    if(dataobj->state < ERR_QDI_USERPD_INITIALIZED)
    {
      continue;
    }
    
    /* Get the global ERR QDI mutex_cb before executing user process callbacks. This will serialize
     * user callbacks for user process. P.S. Thread spawned by QDI_CLIENT_HANDLE_CALLBACK is one 
     * time global resource */
    ERR_QDI_MUTEX_LOCK(&err_qdi.mutex_cb);

    err_qdi_invoke_user_process_cb_helper(dataobj,ERRCB_BUCKET_POST_STM);
    err_qdi_invoke_user_process_cb_helper(dataobj,ERRCB_BUCKET_NORMAL);
    err_qdi_invoke_user_process_cb_helper(dataobj,ERRCB_BUCKET_LATE);
    err_qdi_invoke_user_process_cb_helper(dataobj,ERRCB_BUCKET_LAST);

    ERR_QDI_MUTEX_UNLOCK(&err_qdi.mutex_cb);

    return;
  }
}

/*=============================================================================

FUNCTION ERR_QDI_INVOKE

DESCRIPTION
 This function contains the switch which maps to all methods

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

=============================================================================*/
int err_qdi_invoke(int client_handle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9)
{
   /* Cannot be used for QDI_OPEN */
   err_qdi_opener_s* clntobj = (err_qdi_opener_s*)pobj;
   
   switch(method)
   {
      case QDI_OPEN: 
         return err_qdi_open(client_handle);
      
      case ERR_QDI_SEND_PDR_INFO:
	 return err_qdi_send_pdr_info(clntobj, a1.num);
			 
      case ERR_QDI_INFORM_PFR:  
         return err_qdi_inform_pfr(client_handle, clntobj, a1.num, a2.ptr );

      case ERR_QDI_ERR_HANDLING_DONE:  
         return err_qdi_err_handling_done(clntobj);

      case ERR_QDI_COMMUNICATE_USER_CB:
	 return err_qdi_communicate_user_cb(clntobj, a1.ptr, a2.ptr, a3.ptr, a4.ptr, a5.ptr, a6.ptr, a7.num );

      case QDI_CLOSE:
	 //err_qdi_terminate(pobj);
	 // No break;

      default:
         return qurt_qdi_method_default(client_handle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);
   }
} /* err_qdi_invoke */


/*=============================================================================

FUNCTION ERR_QDI_INIT

DESCRIPTION
 Registers with QDI Framework for ERR QDI
 
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

=============================================================================*/
void err_qdi_release(qurt_qdi_obj_t *obj)
{
  
}

/*=============================================================================

FUNCTION ERR_QDI_INIT

DESCRIPTION
 Registers with QDI Framework for ERR QDI
 
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

=============================================================================*/

static qurt_qdi_obj_t err_qdi_opener_obj;

void err_qdi_init 
(
  void
) 
{
   /* Initialize the mutex here */
   ERR_QDI_MUTEX_INIT(&err_qdi.mutex);
   ERR_QDI_MUTEX_INIT(&err_qdi.mutex_cb);

   err_qdi_opener_obj.invoke    = err_qdi_invoke;
   err_qdi_opener_obj.refcnt    = QDI_REFCNT_INIT;
   err_qdi_opener_obj.release   = err_qdi_release;
   
   qurt_qdi_register_devname(ERR_DRIVER_NAME, &err_qdi_opener_obj);

   /* Set Subsystem PDR policy */
   pd_mon_pd_restart_config( &err_qdi.subsystem_pdr_enabled );
   
   err_qdi.pd_callback_timeout_msec = ERR_QDI_PDTIMEOUT_TIMER_PERIOD_DEFAULT;
   err_qdi.pd_exit_timeout_msec = ERR_QDI_PDTIMEOUT_TIMER_PERIOD_DEFAULT;
	   
   err_qdi_dalcfg_get_pd_timeout(&err_qdi.pd_callback_timeout_msec,
		                 &err_qdi.pd_exit_timeout_msec );

   err_qdi_pdr_records_init();

} /* err_qdi_init */
